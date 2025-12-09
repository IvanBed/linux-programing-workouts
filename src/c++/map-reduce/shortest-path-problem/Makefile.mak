CC = g++  # C++ compiler
CFLAGS = -Wall -Wextra -O2 -c # C++ flags
RM = rm -f   # rm command
TARGET = shortest_path_mapper # target

SRC1 = mapper.cpp
OBJ1 = $(SRC1:.cpp=.o)
SRC2 = shortest_path_utils.cpp
OBJ2 = $(SRC2:.cpp=.o)

.PHONY: all
all: ${TARGET}

$(TARGET): $(OBJ2) $(OBJ1)
        $(CC) -o $@ $^

$(OBJS1): $(SRC1)
        $(CXX) $(CFLAGS) -o

$(OBJS2): $(SRC2)
        $(CXX) $(CFLAGS) -o

.PHONY: clean
clean:
        -${RM} $(OBJ1) $(OBJ2)