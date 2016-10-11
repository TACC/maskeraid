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

//

int i,j, isum;
int iters, base_iters=10000000;

float fsec, test_cost, startup_cost, test_factor, sec_p_base;

double t0,t1;
double tt0,tt1;

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


//                          Determine a base cost to run myspin.

                                            // this is just a warm up
      t0=gtod_timer();
         isum= myspin(1);                   // Make sure the instruction are in cache
         if(isum==0) printf("%d\n",isum);   // (if on isum) don't optimize  away myspin
      t1=gtod_timer();
      startup_cost = t1-t0;

      t0=gtod_timer();                      // run 10 samples to determine time for base_iters
      for(i=0;i<10;i++){
         isum= myspin(base_iters+i);        // (+i)don't optimize away
         if(isum==0) printf("%d\n",isum);   // (if on isum) don't optimize  away myspin
      }
      t1=gtod_timer();
      test_cost = t1-t0;

      fsec = (float)sec - startup_cost - test_cost;

      if(fsec < 0.0e0) return;   // Load will have to be the testing cost!

      sec_p_base = (t1-t0)/10.0e0;

      iters = fsec/sec_p_base;

//    if over 10K, use a base_iters of 1G (not 10M)
      if(iters > 10000){
         iters = iters/100;   // OK to be off by 1%
         base_iters = base_iters * 100;
      }


   t0=gtod_timer();
   for(i=0;i<iters;i++){
      isum= myspin(base_iters+i);       // (+i)don't optimize away; %error is noise
      if(isum==0) printf("%d\n",isum);   // (if on isum) don't optimize  away myspin
   }
   t1=gtod_timer();
   printf("TOTAL %f\n", startup_cost+test_cost+t1-t0);

}

int myspin(int n){
   long int i,sum=0;
   for(i=0;i<n;i++){ sum += i ; }
   return (int) sum;
}
