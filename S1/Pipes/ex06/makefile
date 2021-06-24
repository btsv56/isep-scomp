NAME = ex06
INCLUDES =
SOURCES = ${NAME}.c
OBJFILES = ${NAME}.o
EXE = ${NAME}

MSG = $(msg)

.SUFFIXES : .c .o
${OBJFILES}: ${SOURCES} ${INCLUDES}

.c.o:
	gcc -g -ansi -Wall -c $<

all: ${EXE}

${EXE}: ${OBJFILES}
	gcc -g -o ${EXE} ${OBJFILES}

run: ${EXE}
	./${EXE}

clean:
	rm ${OBJFILES} ${EXE}