/* Routine print mask with node-name, rank #, and thread number prefix when appropriate.

   UPDATE:
     Now allows up to 999 for max cpu_id value (should hold for a few years)
                                                                    Kent Milfeld
                                                                    2016/07/13
*/
#include <stdio.h>
#define MAX_NAME 4096
                      //!!! If you change NODE_SIZE from 20, change %20s in 2 multi-node format statements.
#define NODE_SIZE  20

void print_mask(int hd_prnt, char *name, int multi_node, int rank, int thrd, int ncpus, int nranks, int nthrds, int *icpus){ 

int i, lsdigit, n10, n100;
char node_header[MAX_NAME];

//                              prints mask acquired for particular rank and thread
//                              ncpus = number of cpu_id's on a node
//                              nranks, nthreads = number of ranks and thread on node
//                              icpus is an int array for mask: 1=occupied 0= not occ.
//                              print/not print header  hd_prnt = 1/0
//    Print Format                          
//    Header:  rank thrd  |        |   10    |   20    | ...
//    printed:   1   1    --234---------------123-------
//
//    icpus               001110000000000000001110000000

//    meaning: rank 1, thread 1 mask allows occupation on cpu_id 2,3,4 and 21,22,23
//    If nranks = 1, rank   number is not printed.
//    If nthrds = 1, thread number is not printed.

                          // If node names are different, then list them in output
  if(multi_node && hd_prnt){

     for(i=0; i<=NODE_SIZE;i++)      node_header[i]           = ' ';
     node_header[NODE_SIZE-4] = 'n'; node_header[NODE_SIZE-3] = 'o'; 
     node_header[NODE_SIZE-2] = 'd'; node_header[NODE_SIZE-1] = 'e';
     node_header[NODE_SIZE+1] = ' '; node_header[NODE_SIZE+2] = '\0';

  }

  if(hd_prnt){   

// Do this the easy way!
//                              HEADER (ranks and threads part)
//                              First part of Header, rank/thrd title

    
    if(hd_prnt == 1)                 printf("\n      Each row of matrix is an Affinity "
                                                    "mask. A set mask bit = matrix digit "
                                                    "+ column # in |...|\n");
    if(multi_node)                   printf(" %20s",node_header); // Multi-node

    if(nranks == 1 &&  nthrds == 1 ) printf(" ");                 // serial
    if(nranks != 1 &&  nthrds == 1 ) printf("rank ");             // mpi only
    if(nranks == 1 &&  nthrds != 1 ) printf("thrd ");             // threads only
    if(nranks != 1 &&  nthrds != 1 ) printf("rank thrd ");        // hybrid

//                              HEADER (Groups of 10's)
//                              Print out Header
                                printf("|         |");                                
     if(ncpus < 100){
//                                                          # of header groups < 100 
        n10  = ( (int) ceilf( (float)ncpus/10 ) );
        for(i = 1; i<n10;  i++) printf("   %2d    |",i*10);
     }else{
//                                                           # of header groups > 100
        n100 = ( (int) ceilf( (float)ncpus/10 ) ) - 10 ;
        for(i = 1; i<10  ; i++) printf("   %2d    |",i*10    );
        for(i = 0; i<n100; i++) printf("   %3d   |", i*10+100);
     }
                                printf("\n");
   
  }     
  else {
//                              
//                              Print out rank/thrd value
    if(multi_node)                   printf(" %20.20s  ", name); // Multi-node
    if(nranks == 1 &&  nthrds == 1 ) printf(" "          );
    if(nranks != 1 &&  nthrds == 1 ) printf("%4.4d ",rank);
    if(nranks == 1 &&  nthrds != 1 ) printf("%4.4d ",thrd);
    if(nranks != 1 &&  nthrds != 1 ) printf("%4.4d %4.4d ", rank, thrd);

//                      MASK (just print first digit, header group indicates 10's digit)
//                      Print out mask (if 0, print - , == not set)
//                      Mask index == head group + least significant digit (sdigit)

    for(i=0;i<ncpus;i++){ 
      lsdigit=i;
      if(i>9){lsdigit= i - (i/10)*10; }
      if(icpus[i] == 1) printf("%1.1d",lsdigit);
      else              printf("-"            );
      if(i == ncpus-1)  printf("\n");
    }

  }
  fflush(stdout);
}
