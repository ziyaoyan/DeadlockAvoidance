/* Natalie Rubin & Ziyao Yan
 * process class
 */

#include "process.h"
#include "job.h"

Process::Process(Job *job){
  this->job = job;
  this->alloc_dev = 0;
  this->time_elapsed = 0;
  this->compile_time = -1;
}

int Process::get_turnaround_time(){
  return this->compile_time == -1  
    ? -1 
    : compile_time - arr_time;
}
float Process::get_weighted_turnaround_time(){
  return this->compile_time == -1
    ? -1
    : (float)turnaround_time / (float)run_time;
}
