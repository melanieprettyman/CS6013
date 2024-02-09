#Melanie Prettyman
#CS 6013 HW3
#Targets:
#clean deletes program, and all .o and .out files
#program compiles the source files of the program into an output executable
#run runs the program 

CXX=c++
CFLAGS= -std=c++11
CCSOURCE= main.cpp shelpers.cpp
HEADERS= shelpers.h

program: $(CCSOURCE) $(HEADERS)
	$(CXX) $(CFLAGS) -o mShell $(CCSOURCE)


clean: 
	rm -f *.o *.out program

run: program
	./mShell
