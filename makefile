cc = g++
src = src
inc = -I incl
out = out
lib = -lpthread

OBJECTS = main.o tcp_pthread.o gobang.o

main:$(OBJECTS) 
	$(cc) $(out)/*.o $(lib)	-o main

main.o:$(src)/main.cpp
	$(cc) -c $(src)/main.cpp $(inc) -o $(out)/main.o

tcp_pthread.o:$(src)/tcp_pthread.cpp
	$(cc) -c $(src)/tcp_pthread.cpp $(inc) -o $(out)/tcp_pthread.o

gobang.o:$(src)/gobang.cpp
	$(cc) -c $(src)/gobang.cpp $(inc) -o $(out)/gobang.o

clean:
	rm $(out)/*.o main