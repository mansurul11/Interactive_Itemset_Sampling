interactive_sampling.out : StringTokenizer.o random.o interactive_pattern_mining.cpp database.h pattern.h pattern_factory.h random_walk_manager_freq_randomedge.h  
	g++ -O3 -g -o interactive_sampling.out StringTokenizer.o random.o interactive_pattern_mining.cpp

StringTokenizer.o : StringTokenizer.cpp StringTokenizer.h
	g++ -c StringTokenizer.cpp StringTokenizer.h

random.o: random.h random.cpp
	$(CC) -c random.cpp

clean:
	rm -f *.o
	rm -f *.out
