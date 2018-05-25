/* Natalie Rubin & Ziyao Yan
 * process class header
 */

#ifndef _PROCESS_H
#define _PROCESS_H

#include "job.h"

class Process{
  Job *job;
  public:
    int alloc_dev,
    time_elapsed,
    compile_time,
    needed_dev;
};

#endif
