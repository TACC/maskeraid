/*
  This needs to be simplified!
                               Kent Milfeld
                               2016/07/13
*/
double gtod_timer();
#include <stdio.h>

int  load_cpu_nsec_(int *sec);
void load_cpu_nsec(int);
//                          Fortran Version
int load_cpu_nsec_(int *sec){
    load_cpu_nsec(     *sec);
    return 0;
}
void load_cpu_nsec(int sec ){

// set sec to one, run, and then determine "arch_scale" needed to run for 1 second.
// That's all there is too it.
//
// load is controlled by outer and inner loop factors 
//                                                                             (large value ~ 1G)
//                                                                              faction        v
// inner loop: inner_iters =  between 0.5 x base_iters  to 1.5 x base_iters =  inner_factor x base_iters
// outer loop: outer_iters =  an integer                                     = outer_factor x sec
//                                                                              ~10
//                                                                             (small value )

// Implementation:  split arch_scale into two parts, an integer and a fraction.
//                  We don't want to scale the number of inner_iters too high
//                  or too low-- too high --> exceed int range, too high--> too little work.
//                  If arch_scale fraction < .5 --> outer_iters = floor arch_scale
//                                                  inner_iters = base_iters x arch_scale/floor(arc_scale)
//                                                  e.g.  arch_factor = 6.1 
//                                                        outer_iters = 6         x sec 
//                                                        inner_iters = 6.1/6.0   x base_iters
//                  If arch_scale fraction > .5 --> outer_iters = ceiling arch_scale
//                                                  inner_iters = base_iters x arch_scale/ceiling(arc_scale)
//                                                  e.g.  arch_factor = 6.9 
//                                                        outer_iters = 7         x sec 
//                                                        inner_iters = 1.9/7.0   x base_iters
//  load time is about .2% accurate.

float arch_factor = 0.0; // Generic -- Factor will be derived by code
#ifdef LS5
arch_factor = 3.992;  // jitter is much higher on ls5 than stampede
#endif
#ifdef SP1
arch_factor = 4.0635;
#endif
#ifdef SPKNL
arch_factor = 5.5608;
#endif

float fraction;
float              inner_factor;
int   inner_iters;
int   outer_iters, outer_factor;

int i,j, isum;
int base_iters=1000000000;
float test_cost   = 0.0;
float test_factor = 1.0;

double t0,t1;

//                                New, allow MASKERAID_LOAD_SECONDS to override argument value
char digits[10] = { "0" "1" "2" "3" "4" "5" "6" "7" "8" "9" };
int  slen, knt = 0;

const char* senv = getenv("MASKERAID_LOAD_SECONDS");


   if (senv!=NULL){
       slen=strlen(senv);
       for(j=0;j<slen;j++) for(i=0;i<10;i++)
          if(senv[j] == digits[i] ){knt++;};
       if(knt == slen){ sec = atoi((void *)senv); }  //if all are ints

       if(sec == 0 || knt != slen){
          printf("ERROR: ENV var MASKERAID_LOAD_SECONDS (%s) is invalid;"
                 " must be a non-zero int.\n", senv);
          exit(8);
       }
   }


//                          If machine factor not provided on compile line.
//                          This is probably safer, since it is determined at runtime.
   if(arch_factor == 0.0) {
                                         // this is just a warm up
      t0=gtod_timer();
      isum= myspin(base_iters/10+sec);       // (+i)don't optimize away
      if(isum==0) printf("%d\n",isum);   // (if on isum) don't optimize  away myspin
      t1=gtod_timer();
      test_cost = t1-t0;

      t0=gtod_timer();
      isum= myspin(base_iters/10+sec);       // (+i)don't optimize away
      if(isum==0) printf("%d\n",isum);   // (if on isum) don't optimize  away myspin
      t1=gtod_timer();
      test_cost += t1-t0;

      arch_factor = 0.1/(t1-t0);
      test_factor = (sec - test_cost)/sec;
//    printf("using arch_factor = %f  time=%f test_factor %f\n",arch_factor, test_cost, test_factor);
// fix this later for KNL vvvv
      if( (t1-t0) > 0.1 ) {
         printf("CANNOT set load time correctly ***** 0.1G int ops should take less than 0.1 (took %f)\n",t1-t0);
         exit(9);
      }
   }

   fraction = fabs( arch_factor - (int) arch_factor );

   if( fraction <  0.5){ outer_factor =       (int) floorf(arch_factor);
                         inner_factor = arch_factor/floorf(arch_factor); }
   if( fraction >= 0.5){ outer_factor =       (int) ceilf( arch_factor);
                         inner_factor = arch_factor/ceilf( arch_factor); }
   outer_iters = outer_factor * sec;
   inner_iters = inner_factor * base_iters * test_factor;

   //e.g. if 3.4 (arc_factor) is required to make 1,000,000,000 iterations of myspin
   //     have a 1 second execution time (3.4 x time of myspin(base_iters) = 1sec), 
   //     then total number of iteration (outer and inner) for nsec should be
   //     (3 x nsec ) * (3.4/3 x base_iters)
   //     outer_iters       inner_iters
   // --  Iterations of inner loop inside myspin is adjusted to allow 
   //     outer loop to be a small number of iteration.
   //           

   //printf("outer=%d   inner=%d\n",outer_iters, inner_iters);

   for(i=0;i<outer_iters;i++){
   // t0=gtod_timer();
      isum= myspin(inner_iters+i);       // (+i)don't optimize away; %error is noise
      if(isum==0) printf("%d\n",isum);   // (if on isum) don't optimize  away myspin
   // t1=gtod_timer();
   // printf("loop,time %d %f\n",i,t1-t0);
   }

}

int myspin(int n){
   long int i,sum=0, nn;
   nn = (long int) n;
   for(i=0;i<n;i++){ sum += i ; }
   return (int) sum;
}
