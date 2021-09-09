# Source and lib files
PROGRAM = executor
FILES.c = executor.c threads.c atomic_queue.c transform.c
FILES.h = threads.h atomic_queue.h transform.h transformMat.h
FILES.o = ${FILES.c:.c=.o} transformMat.o
TEST	= input/PCS_data_t00100

# Compiler options
CC 		= gcc
SFLAGS	= -std=c11
GFLAGS	= -g
OFLAGS  = -O3
PFLAGS	= -pthread
FFLAG1	= -no-pie
WFLAG1  = -Wall
WFLAG2  = -Wextra
WFLAG3  = -Werror
WFLAG4  = -Wstrict-prototypes
WFLAG5  = -Wmissing-prototypes
FFLAGS	= ${FFLAG1}
WFLAGS	= ${WFLAG1} ${WFLAG2} ${WFLAG3} ${WFLAG4} ${WFLAG5}
CFLAGS	= ${FFLAGS} ${SFLAGS} ${GFLAGS} ${OFLAGS} ${PFLAGS} ${WFLAGS}

all: ${PROGRAM}

${PROGRAM}: ${FILES.o}
	${CC} -o $@ ${CFLAGS} ${FILES.o}

executor.o:		${FILES.h}
transform.o:	${FILES.h}
atomic_queue.o:	${FILES.h}
threads.o:		${FILES.h}

test: ${PROGRAM} clean
	./executor <${TEST}

clean:
	rm -rf executor.o threads.o atomic_queue.o transform.o