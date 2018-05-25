/* Natalie Rubin & Ziyao Yan
 * system class implementation
 */

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <stdlib.h>
#include "system.h"

System::System(int time,
         int tot_mem,
         int tot_dev,
         int quantum){
  this->time = time;
  this->tot_mem = tot_mem;
  this->avail_mem = tot_mem;
  this->tot_dev = tot_dev;
  this->avail_dev = tot_dev;
  this->quantum = quantum;

  this->sub_q = new std::list<Job*>();
  this->hold_q1 = new std::list<Job*>();
  this->hold_q2 = new std::list<Job*>();
  this->ready_q = new std::list<Process*>();
  this->wait_q = new std::list<Process*>();
  this->complete_q = new std::list<Process*>();

  this->cpu = NULL;
  this->remaining_quantum = 0;
}

int System::get_time(){return this->time;}
int System::get_tot_mem(){return this->tot_mem;}
int System::get_avail_mem(){return this->avail_mem;}
int System::get_tot_dev(){return this->tot_dev;}
int System::get_avail_dev(){return this->avail_dev;}
int System::get_quantum(){return this->quantum;}
int System::get_remaining_quantum(){return this->remaining_quantum;}
int System::get_running_job_num(){return this->cpu==NULL?0:this->cpu->get_job_num();}
float System::get_avg_turnaround_time(){
  int sum, n = 0;
  std::list<Process*>::iterator it;
  for(it = this->complete_q->begin();it != this->complete_q->end(); it++){
    sum += (*it)->get_turnaround_time();
    n++;
  }
  return n < 1
             ? -1
             : (float)sum / (float)n;
}
float System::get_avg_weighted_turnaround_time(){
  float sum, n = 0;
  std::list<Process*>::iterator it;
  for(it = this->complete_q->begin();it != this->complete_q->end(); it++){
    sum += (*it)->get_weighted_turnaround_time();
    n++;
  }
  return n < 1
             ? -1
             : sum / n;
}
void System::set_time(int time){this->time = time;}
void System::set_avail_mem(int memory){this->avail_mem = memory;}
void System::set_avail_dev(int devices){this->avail_dev = devices;}
void System::set_remaining_quantum(int remaining){this->remaining_quantum = remaining;}

bool sort_hold_q1(Job *job1, Job *job2){
  // sorting function for hold queue 1
  if(job1->get_run_time() == job2->get_run_time()){
  return job1->get_arr_time() < job2->get_arr_time();  // FIFO
  } else {
  return job1->get_run_time() < job2->get_run_time();  // SJF
  }
}

bool sort_hold_q2(Job *job1, Job *job2){
  // sorting function for hold queue 2
  return job1->get_arr_time() < job2->get_arr_time();  // FIFO
}

void System::submit(Job *job){
  if(job->get_mem_req() > this->get_tot_mem() ||
     job->get_max_dev() > this->get_tot_dev()){
    //std::cout << "job rejected: insufficient system resources" << std::endl;
  } else if(job->get_mem_req() > this->get_avail_mem()) {
    switch(job->get_priority()){
    case 1:
      this->hold_q1->push_back(job);
      this->hold_q1->sort(sort_hold_q1);
      break;
    case 2:
      this->hold_q2->push_back(job);
      this->hold_q2->sort(sort_hold_q2);
      break;
    }
  } else {
    this->ready_q->push_back(new Process(job));
    this->set_avail_mem(this->get_avail_mem() - job->get_mem_req());
    // subtract available memory from system if adding to ready queue
  }
}

void System::jump_to_time(int time){

  int length = time - this->get_time();
  
  if(length < this->get_remaining_quantum()
     && (this->cpu != NULL
         && this->cpu->get_remaining_time() > length)){
    // if the job isn't finishing, and the quantum isn't finishing
    // increment timers and leave system state the alone
    this->continue_quantum(length);
    return;
  }
  // end the current quantum by either running the current job
  // or finishing it
  this->end_quantum();

  length = time - this->get_time();

  this->swap_cpu_jobs();

  while(length > this->get_quantum()
        || (this->cpu != NULL
            && this->cpu->get_remaining_time() < length)){
    // run quantums until we're almost to time t
    this->run_quantum();
    length = time - this->get_time();
    this->swap_cpu_jobs();
  }

  this->begin_quantum(length); // begin the next quantum

  return; 
}

void System::end_quantum(){
  // run only the end of a quantum
  // whether that's running to the end of remaining_quantum or
  // finishing the current job
  if(this->cpu == NULL){
    // don't try to finish a quantum if nothing's running
    this->set_remaining_quantum(0);
    return;
  }

  if(this->cpu->get_remaining_time() < this->get_remaining_quantum()){
    // finish job
    this->set_time(this->get_time() + this->cpu->get_remaining_time());
    this->cpu->set_elap_time(this->cpu->get_run_time());
  } else {
    // finish quantum
    this->set_time(this->get_time() + this->get_remaining_quantum());
    this->cpu->set_elap_time(this->cpu->get_elap_time()
                             + this->get_remaining_quantum());
  }

  this->set_remaining_quantum(this->get_quantum());

}

void System::begin_quantum(int length){
  // run only the beginning of a quantum

  if(this->cpu != NULL){
    // no jobs queued
    this->cpu->set_elap_time(this->cpu->get_elap_time() + length);
  }

  this->set_time(this->get_time() + length);
  this->set_remaining_quantum(this->get_quantum()
                              - (length % this->get_quantum()));
  // set how much remaining time we have
}

void System::continue_quantum(int length){
  // continue a quantum without finishing it 
  if(this->cpu != NULL){
    this->cpu->set_elap_time(this->cpu->get_elap_time() + length);
  }
  this->set_time(this->get_time()+length);
  this->set_remaining_quantum(this->get_remaining_quantum() - length);
  return;
}

void System::run_quantum(){
  // run for one quantum, does not modify system state
  if(this->cpu == NULL){
    this->set_time(this->get_time() + this->get_quantum());
    return;
  }
  this->end_quantum();
}

void System::complete_job(int time, int job_num){
  if(this->cpu->get_job_num() == job_num){
    // return devices
    this->set_avail_dev(this->cpu->get_alloc_dev() + this->get_avail_dev());
    this->cpu->set_alloc_dev(0);
    // return memory
    this->set_avail_mem(this->cpu->get_mem_req() + this->get_avail_mem());
    // set process completion time
    this->cpu->set_compl_time(this->get_time());
    this->complete_q->push_back(this->cpu); // add process to complete queue

    // check wait q
    std::list<Process*>::iterator it1;
    for(it1 = this->wait_q->begin(); it1 != this->wait_q->end();){
      // if we have the available devices
      if((*it1)->get_needed_dev() <= this->get_avail_dev()){
        // pretend to allocate devices
        (*it1)->set_alloc_dev((*it1)->get_alloc_dev() + (*it1)->get_needed_dev());
        this->set_avail_dev(this->get_avail_dev() - (*it1)->get_needed_dev());
  
        // check if in unsafe state
        if(!this->is_safe()){
          // return devices
          (*it1)->set_alloc_dev((*it1)->get_alloc_dev() - (*it1)->get_needed_dev());
          this->set_avail_dev(this->get_avail_dev() + (*it1)->get_needed_dev());
          // check next in wait queue
          it1++;
          continue; 
        } else {
          // set needed devices to 0
          (*it1)->set_needed_dev(0);
          // add to ready q
          this->ready_q->push_back(*it1);
          // remove from wait q
          it1 = this->wait_q->erase(it1);
        }
      }
    }

    std::list<Job*>::iterator it2;
    // check hold q 1
    if(!(this->hold_q1->empty())){
      for(it2 = this->hold_q1->begin(); it2 != this->hold_q1->end();){// check memory
        if((*it2)->get_mem_req() <= this->get_avail_mem()){
          // add process to ready q
          this->ready_q->push_back(new Process(*it2));
          this->set_avail_mem(this->get_avail_mem() - (*it2)->get_mem_req());
          it2 = this->hold_q1->erase(it2);
        }
        else{it2++;}
      }
    }

    if(!this->hold_q2->empty()){
      // check hold q 2
      for(it2 = this->hold_q2->begin(); it2 != this->hold_q2->end();){
        // check memory
        if((*it2)->get_mem_req() <= this->get_avail_mem()){
          // add process to ready q
          this->ready_q->push_back(new Process(*it2));
          this->set_avail_mem(this->get_avail_mem() - (*it2)->get_mem_req());
          it2 = this->hold_q2->erase(it2);
        }
        else{it2++;}
      }
    }
    
  } else {
    std::cout << "cannot complete job that is not running" << std::endl;
  }
  
}

void System::swap_cpu_jobs(){
  /* this function updates which job is being run on the cpu
   * it does not increment the timer
   */
  if(this->cpu != NULL){
    if(this->cpu->get_elap_time()>=this->cpu->get_run_time()){
      // if process is done
      this->complete_job(this->get_time(), this->cpu->get_job_num());
    } else {
      // otherwise
      this->ready_q->push_back(this->cpu); // add process to back of ready queue
    }
  }
  // move next job to cpu
  if(!this->ready_q->empty()){
    this->cpu=this->ready_q->front();
    this->ready_q->pop_front();
  } else {
    this->cpu=NULL;
  }
}

void System::request(int time, int job_num, int dev){
  //std::cout << "request at " << time << " by " << job_num << " for " << dev << " devices" << std::endl;
  if(this->cpu != NULL && this->cpu->get_job_num() == job_num){
    if(this->get_avail_dev() < dev){
      //std::cout << "cannot allocate devices, not enough resources" << std::endl;
      this->cpu->set_needed_dev(dev);
      this->wait_q->push_back(this->cpu);
      this->cpu = NULL; // remove job from cpu
      this->set_remaining_quantum(0);
    } else if (this->cpu->get_max_dev() < this->cpu->get_alloc_dev() + dev) {
      //std::cout << "process requesting more devices than declared" << std::endl;
    } else {
      // pretend to allocate devices
      this->set_avail_dev(this->get_avail_dev()-dev);
      this->cpu->set_alloc_dev(this->cpu->get_alloc_dev()+dev);
      if(this->is_safe()){
        this->ready_q->push_back(this->cpu); // remove current running process
        this->cpu = NULL;
        this->set_remaining_quantum(0);
        return;
      } else {
        // return devices
        this->set_avail_dev(this->get_avail_dev() + dev);
        this->cpu->set_alloc_dev(this->cpu->get_alloc_dev() - dev);
        // remember how many devices we need
        this->cpu->set_needed_dev(dev);
        // add job to back of wait queue
        this->wait_q->push_back(this->cpu);
        this->cpu = NULL; // remove job from cpu
        this->set_remaining_quantum(0);
      }
    }
    } else if (this->cpu == NULL){
    //std::cout << "no current running job" << std::endl;
  } else {
    //std::cout << "running job does not match request" << std::endl;
  }
}

void System::release(int time, int job_num, int dev){
  //std::cout << "release at " << time << " by " << job_num << " of " << dev << " devices" << std::endl;
  if(this->cpu != NULL && this->cpu->get_job_num() == job_num
     && this->cpu->get_alloc_dev() >= dev){
    this->set_avail_dev(this->get_avail_dev()+dev);
    this->cpu->set_alloc_dev(this->cpu->get_alloc_dev()-dev);
    // check wait queue

    this->ready_q->push_back(this->cpu); // remove process from cpu on release
    this->cpu = NULL;
    this->set_remaining_quantum(0);
    
    std::list<Process*>::iterator it1;
    for(it1 = this->wait_q->begin(); it1 != this->wait_q->end();){
      // if we have the available devices
      if((*it1)->get_needed_dev() <= this->get_avail_dev()){
        // pretend to allocate devices
        (*it1)->set_alloc_dev((*it1)->get_alloc_dev() + (*it1)->get_needed_dev());
        this->set_avail_dev(this->get_avail_dev() - (*it1)->get_needed_dev());
  
        // check if in unsafe state
        if(!this->is_safe()){
          // return devices
          (*it1)->set_alloc_dev((*it1)->get_alloc_dev() - (*it1)->get_needed_dev());
          this->set_avail_dev(this->get_avail_dev() + (*it1)->get_needed_dev());
          // check next in wait queue
          it1++;
          continue; 
        } else {
          // set needed devices to 0
          (*it1)->set_needed_dev(0);
          // add to ready q
          this->ready_q->push_back(*it1);
          // remove from wait q
          it1 = this->wait_q->erase(it1);
        }
      }
    }
  } else if (this->cpu != NULL && this->cpu->get_job_num() == job_num) {
    //std::cout << "not enough devices to release" << std::endl;
  } else if (this->cpu == NULL){
   // std::cout << "no current running job" << std::endl;
  } else {
    //std::cout << "running job does not match request" << std::endl;
  }
}

bool System::is_safe(){
  int P = this->ready_q->size() + this->wait_q->size();
  if(this->cpu != NULL){
    P++;
  }

  std::list<Process*>::iterator iter;
  int* need = (int*) calloc(P, sizeof(int));
  int* alloc = (int*) calloc(P, sizeof(int));
  int* j_nums = (int*) calloc(P, sizeof(int));
  int i = 0;
  for (iter=this->ready_q->begin();iter!=this->ready_q->end();iter++){
      need[i] = (*iter)->get_max_dev() - (*iter)->get_alloc_dev();
      alloc[i] = (*iter)->get_alloc_dev();
      j_nums[i] = (*iter)->get_job_num();
      i++;
  }
  for (iter=this->wait_q->begin();iter!=this->wait_q->end();iter++){
      need[i] = (*iter)->get_max_dev() - (*iter)->get_alloc_dev();
      alloc[i] = (*iter)->get_alloc_dev();
      j_nums[i] = (*iter)->get_job_num();
      i++;
  }
  if(this->cpu != NULL){
    need[i] = this->cpu->get_max_dev() - this->cpu->get_alloc_dev();
    alloc[i] = this->cpu->get_alloc_dev();
    j_nums[i] = this->cpu->get_job_num();
  }

  bool* finish = (bool*) calloc(P, sizeof(bool));

  int* safeSeq = (int*) calloc(P, sizeof(int));

  int work = this->get_avail_dev();

  int count = 0;
  while (count < P){
    bool found = false;
    for (int p = 0; p < P; p++){
      if (finish[p] == 0){
        if (need[p] > work){
          continue;
        }
        work += alloc[p];
        // Add this process to safe sequence.
        safeSeq[count++] = p;
        // Mark this p as finished
        finish[p] = 1;
        found = true;
      }
    }

    if (found == false){
      //std::cout << "System is not in safe state" << std::endl;
      free(need);
      free(alloc);
      free(j_nums);
      free(finish);
      free(safeSeq);
      return false;
    }
  }
  // std::cout << "System is in safe state.\nSafe"
  //    " the job numbers of the sequence are: ";
  // for (int i = 0; i < P ; i++){
  //   std::cout << j_nums[safeSeq[i]] << " ";
  // }
  // std::cout << std::endl;
  free(need);
  free(alloc);
  free(j_nums);
  free(finish);
  free(safeSeq);
  return true;
}
