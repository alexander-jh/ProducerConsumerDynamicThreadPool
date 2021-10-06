# Source and lib files
PROGRAM = lab1_hoke62_hoke62
FILES.c = executor.c threads.c atomic_queue.c min_heap.c
FILES.h = threads.h atomic_queue.h min_heap.h
FILES.o = ${FILES.c:.c=.o} transformMat.o
TEST	= input/PCS_data_t00100
TEST1	= input/PCS_data_test
TEST2	= input/PCS_data_t00500

# Compiler options
CC 		= gcc
SFLAGS	= -std=c11
GFLAGS	= -g
OFLAGS  = -O3
PFLAGS	= -pthread
FFLAG1	= -no-pie
WFLAG1  = -Wall
WFLAG2  = -Wextra
WFLAG3  = -Wstrict-prototypes
WFLAG4  = -Wmissing-prototypes
FFLAGS	= ${FFLAG1}
WFLAGS	= ${WFLAG1} ${WFLAG2} ${WFLAG3} ${WFLAG4}
CFLAGS	= ${FFLAGS} ${SFLAGS} ${GFLAGS} ${OFLAGS} ${PFLAGS} ${WFLAGS}

all: ${PROGRAM}

${PROGRAM}: ${FILES.o}
	${CC} -o $@ ${CFLAGS} ${FILES.o}

executor.o:		${FILES.h}
atomic_queue.o:	${FILES.h}
min_heap.o:	${FILES.h}
threads.o:		${FILES.h}

test: ${PROGRAM} clean
	./lab1_hoke62_hoke62 <${TEST} 2> error.log

min-test: ${PROGRAM} clean
	./lab1_hoke62_hoke62 <${TEST1} 2> error.log

big-test: ${PROGRAM} clean
	./lab1_hoke62_hoke62 <${TEST2} 2> error.log

clean:
	rm -rf executor.o threads.o atomic_queue.o min_heap.o