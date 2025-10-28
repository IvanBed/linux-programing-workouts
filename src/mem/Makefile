TARGET = myalloc
 
all: $(TARGET)

myalloc: alloc.o alloc_utils.o
	g++ alloc.o alloc_utils.o -o $(TARGET)

alloc.o: alloc.c
	g++ -c alloc.c

alloc_utils.o: alloc_utils.c
	g++ -c alloc_utils.c

clean:
	rm -rf *.o myalloc
