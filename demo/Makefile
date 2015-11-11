IFTFLAGS    = -L../lib -lift -llapack -lblas -lm -I ../include

all: $@.c

$@.c: $@.c 
	$(CC) $(FLAGS) $@.c -o ../bin/$@ $(IFTFLAGS)

clean:
	rm ../bin/*

