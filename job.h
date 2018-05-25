/* Natalie Rubin, Ziyao Yan
 * job header file
 */

#ifndef _JOB_H
#define _JOB_H

class Job {
  int arrival_time,
    job_num,
    mem_needed,
    max_dev,
    run_time,
    priority;
  
 public:
  Job(int arrival_time,
      int job_num,
      int mem_needed,
      int max_dev,
      int run_time,
      int priority);
};


#endif
