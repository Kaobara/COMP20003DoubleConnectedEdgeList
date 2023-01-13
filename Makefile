# -*- MakeFile -*-
voronoi1: voronoi1.o functions.o readDataset.o DCEL.o split.o watchtower.o
	gcc voronoi1.o functions.o readDataset.o DCEL.o split.o watchtower.o -o voronoi1 -g

voronoi1.o: voronoi1.c
	gcc -c voronoi1.c -g

functions.o: functions.c
	gcc -c functions.c -g
	
readDataset.o: readDataset.c
	gcc -c readDataset.c -g

DCEL.o: DCEL.c
	gcc -c DCEL.c -g

split.o: split.c
	gcc -c split.c -g

watchtower.o: watchtower.c
	gcc -c watchtower.c -g

clean: 
	rm -f *.o voronoi1 a.out
