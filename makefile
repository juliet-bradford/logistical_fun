
CC = g++
CFLAGS = -g -Wall
OBJ = citysim.o city_helper.o city_graph.o

citysim: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

citysim.o: 
city_helper.o: city_helper.cpp

clean:
	rm -f citysim *.o
	rm -f city_info.txt 
	rm -f city_distancetable.txt 
	rm -f city_timetable.txt 
	rm -f city_graph.txt 