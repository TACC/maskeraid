CC_SER  =   icc
CC_MPI  = mpicc
CC_OMP  =   icc -qopenmp
CC_HYB  = mpicc -qopenmp
LD_MPI  = mpicc
LD_OMP  =   icc -qopenmp
LD_HYB  = mpicc -qopenmp

AR      = ar

ifeq ($(TACC_SYSTEM),stampede)
   LOAD_X=-xhost
   LOAD_DEF=-DSP1
endif
ifeq ($(TACC_SYSTEM),stampedeknl)
   LOAD_X=-xMIC-AVX512
   LOAD_DEF=-DSPKNL
endif
ifeq ($(TACC_SYSTEM),ls5)
   LOAD_X=-xhost
   LOAD_DEF=-DLS5
endif

#                               new load code auto adjusts for units of seconds.
#CFLAGS	= -D _GNU_SOURCE -D LS5 no longer required
#CFLAGS	= -D _GNU_SOURCE -D SP1 no longer required
#CFLAGS	= -D _GNU_SOURCE        Required only for timers, boundto and mapto_cpuid  added separate rule for timers
 CFLAGS	= 
LDFLAGS	= 

DRIVER_SOURCES = mpi_whereami.c omp_whereami.c hybrid_whereami.c 
DRIVER_OBJECTS = $(DRIVER_SOURCES:.c=.o)

LIB_SOURCES=boundto.c cmdln_args.c load.c map_to_cpuid.c print_mask.c timers.c mpi_report_mask.c omp_report_mask.c hybrid_report_mask.c
LIB_OBJECTS = $(LIB_SOURCES:.c=.o)


LIB     = maskeraid.a
EXES	= $(DRIVER_SOURCES:.c=)
#EXES = mpi_whereami omp_whereami hybrid_whereami 

all: $(EXES)

mpi_whereami    :            mpi_whereami.o                       $(LIB)
	$(LD_MPI) $(LDFLAGS_MPI)  mpi_whereami.o    -o mpi_whereami    $(LIB)
	mkdir -p ../bin
	cp -p  mpi_whereami ../bin
omp_whereami    :            omp_whereami.o                       $(LIB)
	$(LD_OMP) $(LDFLAGS_OMP)  omp_whereami.o    -o omp_whereami    $(LIB)
	mkdir -p ../bin
	cp -p  omp_whereami ../bin
hybrid_whereami :            hybrid_whereami.o                    $(LIB)
	$(LD_HYB) $(LDFLAGS_HYB)  hybrid_whereami.o -o hybrid_whereami $(LIB)
	mkdir -p ../bin
	cp -p  hybrid_whereami ../bin


$(LIB) : $(LIB_OBJECTS)
	$(AR) -csr  $@  $^
	mkdir -p ../lib
	cp -p $(LIB) ../lib

mpi_whereami.o    : mpi_whereami.c 
	$(CC_MPI) $(CFLAGS) -c $< -o $@
mpi_report_mask.o : mpi_report_mask.c
	$(CC_MPI) $(CFLAGS) -c $< -o $@

omp_whereami.o    :  omp_whereami.c
	$(CC_OMP) $(CFLAGS) -c $< -o $@
omp_report_mask.o :  omp_report_mask.c
	$(CC_OMP) $(CFLAGS) -c $< -o $@

hybrid_whereami.o    :  hybrid_whereami.c
	$(CC_HYB) $(CFLAGS) -c $< -o $@
hybrid_report_mask.o :  hybrid_report_mask.c
	$(CC_HYB) $(CFLAGS) -c $< -o $@


load.o  : load.c
	$(CC_SER) $(LOAD_X) -O3 $(LOAD_DEF)  -c $< -o $@

boundto.o  : boundto.c
	$(CC_SER)  -D _GNU_SOURCE -c $< -o $@
map_to_cpuid.o  : map_to_cpuid.c
	$(CC_SER)  -D _GNU_SOURCE -c $< -o $@
timers.o  : timers.c
	$(CC_SER)  -D _GNU_SOURCE -c $< -o $@


%.o : %.c
	$(CC_SER) $(CFLAGS) -c $< -o $@


.PHONY: install clean cleanall
install:
	mkdir -p -v ../bin ../lib
	cp -p $(LIB)  ../lib
	cp -p $(EXES) ../bin

clean :
	rm -fr $(EXES) $(DRIVER_OBJECTS) $(LIB_OBJECTS) $(LIB) *.mod

cleanall :
	rm -fr $(EXES) $(DRIVER_OBJECTS) $(LIB_OBJECTS) $(LIB) *.mod
	rm -fr $(addprefix ../bin/, $(EXES))  ../lib/$(LIB)
	rmdir  ../bin/        ../lib/


#http://stackoverflow.com/questions/15195087/how-to-make-static-library-in-makefile
#other information
