CC = gcc # Ccompiler
CFLAGS = -fPIC -Wall -Wextra# C++ flags
RM = rm -f   # rm command

LIBTARGET = libvector.so # target
TEST      = test
LIBPATH   = /home/ivan/Dev/linux-programing-workouts/src/utils/collections/vector

SRC1 = number_interface.c
OBJ1 = $(SRC1:.c=.o)

SRC2 = string_interface.c
OBJ2 = $(SRC2:.c=.o)

SRC3 = vector.c
OBJ3 = $(SRC3:.c=.o)

SRC4 = test.c
OBJ4 = $(SRC4:.c=.o)


.PHONY: all
all: ${LIBTARGET} ${TEST} clean

${TEST}: $(LIBTARGET) 
	$(CC) -L$(LIBPATH) -Wl,-rpath=$(LIBPATH) $(SRC4) -o ${TEST} -lvector

$(LIBTARGET): $(OBJ3) $(OBJ2) $(OBJ1)
	$(CC) -shared -o $@ $^

$(OBJS1): $(OBJS3) $(SRC1)
	$(CC) $(CFLAGS) -o

$(OBJS2): $(OBJS3) $(SRC2)
	$(CC) $(CFLAGS) -o

$(OBJS3): $(SRC3)
	$(CC) $(CFLAGS) -o


.PHONY: clean
clean-all:
	-${RM} $(LIBTARGET) ${TEST} $(OBJ4) $(OBJ3) $(OBJ2) $(OBJ1)

clean:
	-${RM} $(OBJ4) $(OBJ3) $(OBJ2) $(OBJ1)