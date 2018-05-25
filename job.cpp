/* Natalie Rubin, Ziyao Yan
 * job class
 */


#include <iostream>
#include "job.h"

using namespace std;

Job::Job(public int arrival_time,
         public int job_num,
         public int mem_needed,
         public int max_dev,
         public int run_time,
         public int priority){
  
  this->arrival_time = arrival_time;
  this->job_num = job_num;
  this->mem_needed = mem_needed;
  this->max_dev = max_dev;
  this->run_time = run_time;
  this->priority = priority;
}
