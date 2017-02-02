/* Routine sets affinity to a single cpu_id (cpuid)
                                           Kent Milfeld
                                           206/07/13

*/

#include <sched.h>
#include <unistd.h>
#include <cstdlib>

int map_to_cpuid( int cpuid){
    cpu_set_t        mask;
    CPU_ZERO(       &mask);
    CPU_SET(cpuid,  &mask);
    return( sched_setaffinity( (pid_t) 0 , sizeof(mask), &mask ) );
}

void  map_to_cpuid_(int *cpuid){
int i;
    i=map_to_cpuid(     *cpuid);
    if( i!=0 ) exit(1);
}
