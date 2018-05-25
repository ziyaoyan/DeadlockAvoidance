/* Natalie Rubin, Ziyao Yan
 * job file
 */


#include <iostream>
#include "job.h"

using namespace std;

Job::Job(int arrival_time,
         int job_num,
         int mem_needed,
         int max_dev,
         int run_time,
         int priority){
  this->arrival_time = arrival_time;
  this->job_num = job_num;
  this->mem_needed = mem_needed;
  this->max_dev = max_dev;
  this->run_time = run_time;
  this->priority = priority;
}
