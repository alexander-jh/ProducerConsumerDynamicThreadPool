# Source and lib files
PROGRAM = lab1_hoke62_hoke62
FILES.c = executor.c atomic_queue.c atomic_task.c atomic_thread.c atomic_transform.c min_heap.c
FILES.h = executor.h atomic_queue.h atomic_task.h atomic_thread.h atomic_transform.h min_heap.h
FILES.o = ${FILES.c:.c=.o} transformMat.o
DEL.o	= ${FILES.c:.c=.o}
TEST	= input/PCS_data_t00100
TEST1	= input/PCS_data_test
TEST2	= input/PCS_data_t00500

# Compiler options
CC 		= gcc
SFLAGS	= -std=c99
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

executor.o:			${FILES.h}
atomic_queue.o:		${FILES.h}
atomic_task.o:		${FILES.h}
atomic_thread.o:	${FILES.h}
atomic_transform.o:	${FILES.h}
min_heap.o:			${FILES.h}

test: clean ${PROGRAM} clean
	./lab1_hoke62_hoke62 <${TEST} 2> error.log

min-test: clean ${PROGRAM} clean
	./lab1_hoke62_hoke62 <${TEST1} 2> error.log

big-test: clean ${PROGRAM} clean
	./lab1_hoke62_hoke62 <${TEST2} 2> error.log

clean:
	rm -rf ${DEL.o}