/* Natalie Rubin & Ziyao Yan
 * system class
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
int System::get_running_job_num(){return this->cpu==NULL?0:this->cpu->job_num();}
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
  //hold queue 1 sort
  if(job1.run_time == job2.run_time){
  return job1.arrival_time < job2.arrival_time;  // FIFO
  } else {
  return job1.run_time < job2.run_time;  // SJF
  }
}

bool sort_hold_q2(Job *job1, Job *job2){
  //hold queue 2 sort
  return job1.arrival_time < job2.arrival_time;  // FIFO
}

void System::submit(Job *job){
  if(job.mem_needed > this->get_tot_mem() ||
     job.max_dev > this->get_tot_dev()){
  } else if(job.mem_needed > this->get_avail_mem()) {
    switch(job.priority{
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
    this->set_avail_mem(this->get_avail_mem() - job.mem_needed);
  }
}

void System::jump_to_time(int time){

  int length = time - this->get_time();
  
  if(length < this->get_remaining_quantum()
     && (this->cpu != NULL
         && this->cpu->get_remaining_time() > length)){
    this->continue_quantum(length);
    return;
  }
  this->end_quantum();

  length = time - this->get_time();

  this->swap_cpu_jobs();

  while(length > this->get_quantum()
        || (this->cpu != NULL
            && this->cpu->get_remaining_time() < length)){
    this->run_quantum();
    length = time - this->get_time();
    this->swap_cpu_jobs();
  }

  this->begin_quantum(length);

  return; 
}

void System::end_quantum(){
  if(this->cpu == NULL){
    this->set_remaining_quantum(0);
    return;
  }

  if(this->cpu->get_remaining_time() < this->get_remaining_quantum()){
    this->set_time(this->get_time() + this->cpu->get_remaining_time());
    this->cpu->set_elap_time(this->cpu->get_run_time());
  } else {
    this->set_time(this->get_time() + this->get_remaining_quantum());
    this->cpu->set_elap_time(this->cpu->get_elap_time()
                             + this->get_remaining_quantum());
  }

  this->set_remaining_quantum(this->get_quantum());

}

void System::begin_quantum(int length){

  if(this->cpu != NULL){
    this->cpu->set_elap_time(this->cpu->get_elap_time() + length);
  }

  this->set_time(this->get_time() + length);
  this->set_remaining_quantum(this->get_quantum()
                              - (length % this->get_quantum()));
}

void System::continue_quantum(int length){
  if(this->cpu != NULL){
    this->cpu->set_elap_time(this->cpu->get_elap_time() + length);
  }
  this->set_time(this->get_time()+length);
  this->set_remaining_quantum(this->get_remaining_quantum() - length);
  return;
}

void System::run_quantum(){
  if(this->cpu == NULL){
    this->set_time(this->get_time() + this->get_quantum());
    return;
  }
  this->end_quantum();
}

void System::complete_job(int time, int job_num){
  if(this->cpu.job_num == job_num){
    this->set_avail_dev(this->cpu->get_alloc_dev() + this->get_avail_dev());
    this->cpu->set_alloc_dev(0);
    this->set_avail_mem(this->cpu.mem_needed + this->get_avail_mem());
    this->cpu->set_compl_time(this->get_time());
    this->complete_q->push_back(this->cpu);

    std::list<Process*>::iterator it1;
    for(it1 = this->wait_q->begin(); it1 != this->wait_q->end();){
      if((*it1)->get_needed_dev() <= this->get_avail_dev()){
        (*it1)->set_alloc_dev((*it1)->get_alloc_dev() + (*it1)->get_needed_dev());
        this->set_avail_dev(this->get_avail_dev() - (*it1)->get_needed_dev());
  
        if(!this->is_safe()){
          (*it1)->set_alloc_dev((*it1)->get_alloc_dev() - (*it1)->get_needed_dev());
          this->set_avail_dev(this->get_avail_dev() + (*it1)->get_needed_dev());
          it1++;
          continue; 
        } else {
          (*it1)->set_needed_dev(0);
          this->ready_q->push_back(*it1);
          it1 = this->wait_q->erase(it1);
        }
      }
    }

    std::list<Job*>::iterator it2;
   if(!(this->hold_q1->empty())){
      for(it2 = this->hold_q1->begin(); it2 != this->hold_q1->end();){
        if((*it2).mem_needed <= this->get_avail_mem()){
          this->ready_q->push_back(new Process(*it2));
          this->set_avail_mem(this->get_avail_mem() - (*it2).mem_needed);
          it2 = this->hold_q1->erase(it2);
        }
        else{it2++;}
      }
    }

    if(!this->hold_q2->empty()){
      for(it2 = this->hold_q2->begin(); it2 != this->hold_q2->end();){
        if((*it2).mem_needed <= this->get_avail_mem()){
          this->ready_q->push_back(new Process(*it2));
          this->set_avail_mem(this->get_avail_mem() - (*it2).mem_needed);
          it2 = this->hold_q2->erase(it2);
        }
        else{it2++;}
      }
    }
    
  }
  
}

void System::swap_cpu_jobs(){
  if(this->cpu != NULL){
    if(this->cpu->get_elap_time()>=this->cpu.run_time){
      this->complete_job(this->get_time(), this->cpu.job_num);
    } else {
      this->ready_q->push_back(this->cpu);
    }
  }
  if(!this->ready_q->empty()){
    this->cpu=this->ready_q->front();
    this->ready_q->pop_front();
  } else {
    this->cpu=NULL;
  }
}

void System::request(int time, int job_num, int dev){
  if(this->cpu != NULL && this->cpu.job_num == job_num){
    if(this->get_avail_dev() < dev){
      this->cpu->set_needed_dev(dev);
      this->wait_q->push_back(this->cpu);
      this->cpu = NULL;
      this->set_remaining_quantum(0);
    } else if (this->cpu.max_dev < this->cpu->get_alloc_dev() + dev) {
     
    } else {
      this->set_avail_dev(this->get_avail_dev()-dev);
      this->cpu->set_alloc_dev(this->cpu->get_alloc_dev()+dev);
      if(this->is_safe()){
        this->ready_q->push_back(this->cpu);
        this->cpu = NULL;
        this->set_remaining_quantum(0);
        return;
      } else {
        this->set_avail_dev(this->get_avail_dev() + dev);
        this->cpu->set_alloc_dev(this->cpu->get_alloc_dev() - dev);
        this->cpu->set_needed_dev(dev);
        this->wait_q->push_back(this->cpu);
        this->cpu = NULL;
        this->set_remaining_quantum(0);
      }
    }
    }
}

void System::release(int time, int job_num, int dev){
  if(this->cpu != NULL && this->cpu.job_num == job_num
     && this->cpu->get_alloc_dev() >= dev){
    this->set_avail_dev(this->get_avail_dev()+dev);
    this->cpu->set_alloc_dev(this->cpu->get_alloc_dev()-dev);

    this->ready_q->push_back(this->cpu);
    this->cpu = NULL;
    this->set_remaining_quantum(0);
    
    std::list<Process*>::iterator it1;
    for(it1 = this->wait_q->begin(); it1 != this->wait_q->end();){
      if((*it1)->get_needed_dev() <= this->get_avail_dev()){
        (*it1)->set_alloc_dev((*it1)->get_alloc_dev() + (*it1)->get_needed_dev());
        this->set_avail_dev(this->get_avail_dev() - (*it1)->get_needed_dev());
  

        if(!this->is_safe()){
          (*it1)->set_alloc_dev((*it1)->get_alloc_dev() - (*it1)->get_needed_dev());
          this->set_avail_dev(this->get_avail_dev() + (*it1)->get_needed_dev());
   
          it1++;
          continue; 
        } else {
          (*it1)->set_needed_dev(0);
          this->ready_q->push_back(*it1);
          it1 = this->wait_q->erase(it1);
        }
      }
    }
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
      need[i] = (*iter).max_dev - (*iter)->get_alloc_dev();
      alloc[i] = (*iter)->get_alloc_dev();
      j_nums[i] = (*iter).job_num;
      i++;
  }
  for (iter=this->wait_q->begin();iter!=this->wait_q->end();iter++){
      need[i] = (*iter).max_dev - (*iter)->get_alloc_dev();
      alloc[i] = (*iter)->get_alloc_dev();
      j_nums[i] = (*iter).job_num;
      i++;
  }
  if(this->cpu != NULL){
    need[i] = this->cpu.max_dev - this->cpu->get_alloc_dev();
    alloc[i] = this->cpu->get_alloc_dev();
    j_nums[i] = this->cpu.job_num;
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
        safeSeq[count++] = p;
        finish[p] = 1;
        found = true;
      }
    }

    if (found == false){
      free(need);
      free(alloc);
      free(j_nums);
      free(finish);
      free(safeSeq);
      return false;
    }
  }
  
  free(need);
  free(alloc);
  free(j_nums);
  free(finish);
  free(safeSeq);
  return true;
}
