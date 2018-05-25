/* Natalie Rubin & Ziyao Yan
 * job scheduler header
 */

#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <list>
#include "job.h"
#include "process.h"

class System {
  int time,
    total_mem,
    available_mem,
    total_dev,
    available_dev,
    quantum,
    remaining_quantum;

  std::list<Job*> *sub_q;
  std::list<Job*> *hold_q1;
  std::list<Job*> *hold_q2;

  std::list<Process*> *ready_q;
  std::list<Process*> *wait_q;
  std::list<Process*> *complete_q;
  Process* cpu;

  void swap_cpu_jobs(); // moves current process to a queue,
                        // moves next job to process

 public:
  System(int time,
         int total_mem,
         int total_dev,
         int quantum);
  int get_time();
  int get_tot_mem();
  int get_avail_mem();
  int get_tot_dev();
  int get_avail_dev();
  int get_quantum();
  int get_remaining_quantum();
  int get_running_job_num();
  float get_avg_turnaround_time();
  float get_avg_weighted_turnaround_time();
  void set_time(int time);
  void set_avail_dev(int devices);
  void set_avail_mem(int memory);
  void set_remaining_quantum(int remaining);
  bool is_safe();

  void submit(Job *job);
  void update();
  void status();
  void dump_json();
  void request(int time, int job_num, int dev);
  void release(int time, int job_num, int dev);
  void run_quantum();
  void jump_to_time(int time); 
  void end_quantum();
  void begin_quantum(int length);
  void complete_job(int time, int job_num);
  void continue_quantum(int length);
};

#endif
