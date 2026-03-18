CC = gcc # Ccompiler
CFLAGS = -fPIC -Wall -Wextra# C++ flags
RM = rm -f   # rm command

LIBTARGET = libvector.so # target

SRC1 = number_interface.c
OBJ1 = $(SRC1:.c=.o)

SRC2 = string_interface.c
OBJ2 = $(SRC2:.c=.o)

SRC3 = vector.c
OBJ3 = $(SRC3:.c=.o)

.PHONY: all
all: ${LIBTARGET}  

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
	-${RM} $(LIBTARGET) $(OBJ3) $(OBJ2) $(OBJ1)