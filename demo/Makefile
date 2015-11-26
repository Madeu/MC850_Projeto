IFTFLAGS    = -L../lib -L../libsvm -L /usr/lib64/atlas -lift -llapack -ltatlas -lm -lsvm -lstdc++ -I ../include -I../libsvm
#IFTFLAGS    = -L../lib -L../libsvm -L /usr/lib64/atlas -lift -llapack -lcblas -lm -lsvm -lstdc++ -I ../include -I../libsvm
FLAGS = -std=c11 -g -Wall 

all: $@.c

$@.c: $@.c 
	$(CC) $(FLAGS) $@.c -o ../bin/$@ $(IFTFLAGS)

clean:
	rm ../bin/*

