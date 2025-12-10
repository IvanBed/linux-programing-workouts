CC = g++  # C++ compiler
CFLAGS = -Wall -Wextra -O2 -c # C++ flags
RM = rm -f   # rm command
TARGET1 = shortest_path_mapper # target
TARGET2 = shortest_path_reducer

SRC1 = mapper.cpp
OBJ1 = $(SRC1:.cpp=.o)

SRC2 = reducer.cpp
OBJ2 = $(SRC2:.cpp=.o)

SRC3 = shortest_path_utils.cpp
OBJ3 = $(SRC3:.cpp=.o)

.PHONY: all
all: ${TARGET1}  ${TARGET2}

$(TARGET1): $(OBJ3) $(OBJ1)
	$(CC) -o $@ $^

$(TARGET2): $(OBJ3) $(OBJ2)
	$(CC) -o $@ $^

$(OBJS1): $(SRC1)
	$(CXX) $(CFLAGS) -o

$(OBJS2): $(SRC2)
	$(CXX) $(CFLAGS) -o

$(OBJS3): $(SRC3)
	$(CXX) $(CFLAGS) -o

.PHONY: clean
clean:
	-${RM} $(OBJ1) $(OBJ2) $(OBJ3)
