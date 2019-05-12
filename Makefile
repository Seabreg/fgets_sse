CFLAGS  := -O3 -ansi -pipe -W -Wall -s -msse2

all: fgets_orig fgets_sse2

clean:
	rm -f core *.o fgets_orig fgets_sse2

fgets_orig: fgets_orig.c
	${CC} ${CFLAGS} -o $@ $<

fgets_sse2: fgets_sse2.o
	${CC} ${CFLAGS} -o $@ $< main.c

fgets_sse2.o: fgets_sse2.c
	${CC} ${CFLAGS} -o $@ $< -c
