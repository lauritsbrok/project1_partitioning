CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -pthread -lm
DEPS = data_tuple.h partitioner.h utils.h throughput_test.h
OBJ = data_tuple.o partitioner.o utils.o throughput_test.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

partition_test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o partition_test