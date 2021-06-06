NAME = copy

COMPILER = gcc

FLAGS = -Wall

compile: copy.o
	$(COMPILER) $(FLAGS) -o $(NAME) copy.o

copy.o: copy.c

clean:
	$(RM) copy *.o *~