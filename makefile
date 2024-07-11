PROG = citysim
CC = g++
CFLAGS = -g -Wall

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.c=.o)


$(PROG): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm -f $(PROG)
	rm -f city_info.txt 
	rm -f city_distancetable.txt 
	rm -f city_timetable.txt 
	rm -f city_graph.txt 