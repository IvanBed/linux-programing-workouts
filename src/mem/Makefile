TARGET = myalloc
CFLAGS = -std=c11 -MP -MMD
SRCS = alloc.c alloc_utils.c
 
all: $(TARGET)

hello: alloc.o alloc_utils.o
	g++ alloc.o alloc_utils.o -o $(TARGET)

main.o: alloc.c
	g++ -c alloc.c

factorial.o: alloc_utils.c
	g++ -c alloc_utils.c

clean:
	rm -rf *.o myalloc