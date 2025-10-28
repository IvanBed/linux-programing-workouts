TARGET = myalloc
 
all: $(TARGET)

myalloc: alloc.o alloc_utils.o
	gcc alloc.o alloc_utils.o -o $(TARGET)

alloc.o: alloc.c
	gcc -c alloc.c

alloc_utils.o: alloc_utils.c
	gcc -c alloc_utils.c

clean:
	rm -rf *.o myalloc
