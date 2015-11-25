IFTFLAGS    = -std=c99 -L../lib -lift -llapack -lblas -lm -I ../include
#IFTFLAGS    = -std=c99 -L /usr/lib64/atlas -L../lib -lift -llapack -ltatlas -lm -I ../include

all: $@.c

$@.c: $@.c 
	$(CC) $(FLAGS) $@.c -o ../bin/$@ $(IFTFLAGS)

clean:
	rm ../bin/*

