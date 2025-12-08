CC = g++  # C++ compiler
CFLAGS = -Wall -Wextra -O2 -g # C++ flags
RM = rm -f   # rm command
TARGET = shortest_path_mapper # target

SRCS = mapper.cpp shortest_path_utils.cpp # source files
OBJS = $(SRCS:.cpp=.o)

.PHONY: all
all: ${TARGET}

$(TARGET): $(OBJS)
        $(CC) -o $@ $^

$(SRCS:.c=.d):%.d:%.c
        $(CC) $(CFLAGS) -MM $< >$@

include $(SRCS:.cpp=.d)

.PHONY: clean
clean:
        -${RM} ${TARGET} ${OBJS} $(SRCS:.cpp=.d)