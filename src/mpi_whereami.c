/*   mpi_mapper is a driver 
     1.) Get line arguments (optional):  help or number o seconds for load
     2.) Start MPI
          Option to EASILY set affinity here.
     3.) Call report_mask
     4.) Set a work load for each rank
     5.) Stop MPI
                                         Kent Milfeld 12/16/15

     Updated hybrid_report_mask to be called once.
     Previously it had to be called in the the pure MPI region, 
     and then in the hybri parallel region.
                                                                    Kent Milfeld
                                                                    2016/07/13
*/

/*

   Program mpi_whereami reports the mask for each MPI process,
   and works for nsec seconds (10). This allows one to inspect occupation through
   utilities like top  (e.g. execute top, then hit the 1 key).
   Uses maskeraid utilities  github.com/TACC/maskeraid
      mpi_report_mask() reports mask of MPI process.
      map_to_cpuid( cpuid );   sets thread affinity to cpu_id 
      (See /proc/cpuinfo, or hwloc)
      load_cpu_nsec(nsec) load the cpu for nsec (default 10)

*/
/*   mpi_whereami.c is a driver 
     1.) Get line arguments (optional):  help or number of seconds for load
     2.) Start MPI
           Affinity for MPI processes can be reset here.
           mpi_report_mask() reports MPI process masks
     3.) Set a work load on each MPI process (rank)
     4.) Stop MPI
                                         Kent Milfeld 12/16/15
*/

#include <stdio.h>
#include <mpi.h>

                                  // basic routes
void cmdln_get_nsec_or_help(int * nsec, int argc, char * argv[]);
void          load_cpu_nsec(int nsec );
int            hpc_affinity(int icore);
void        mpi_report_mask();
int           map_to_cpuid(int cpuid);          // set your own affinity here 


int main(int argc, char * argv[]){

int i,j;                  // General

int rank, nranks;         // MPI variables.

int nsec = 10;            // Default time for load

int cpuid;                // For mapping this process to cpuid in this variabl


// cmdln_get_nsec_or_help( &nsec, argc, argv); //optional, get nsec from cmd line


   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &nranks);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   // cpuid=rank;                     // set rank to cpuid
   // map_to_cpuid( cpuid );          // set your own affinity here 
                                      // (e.g. map this process to cpuid equal to this rank.

   mpi_report_mask();                 // Call mask reporter

   load_cpu_nsec( nsec );             // Load up rank process so user can watch top.

   MPI_Finalize();

}
