CC              = gcc
CC_FLAGS        = -Wall -m64 -std=c99 -pedantic -march=core2 -O3
CC_DEBUG_FLAGS  = -Wall -m64 -std=c99 -pedantic -march=core2 -O0 -ggdb
CC_PATHS        = 
CC_LIBS         = -lm 

all : matrix  

debug : matrix-debug

matrix : $(wildcard matrix.[ch] files.[ch] util.[ch] ccs.[ch] tree.[ch] 2d.[c])
	@${CC} ${CC_FLAGS} ${CC_PATHS} -o ${@} $(filter %.c, ${^}) ${CC_LIBS}

matrix-debug : $(wildcard matrix.[ch] files.[ch] util.[ch] ccs.[ch] tree.[ch] 2d.[ch])
	@${CC} ${CC_DEBUG_FLAGS} ${CC_PATHS} -o ${@} $(filter %.c, ${^}) ${CC_LIBS}
clean : 
	@rm -f core matrix matrix-debug
