# Melanie Prettyman
# CS 6013 HW5
# Targets:
# clean deletes program, and all .o and .out files
# program compiles the source files of the program into an output executable
# run runs the program

CXX=c++
CFLAGS= -std=c++11
CCSOURCE= ConcurrentQueueTests.cpp
HEADERS= ConcurrentQueue.hpp

hw5: $(CCSOURCE) $(HEADERS)
	$(CXX) $(CFLAGS) -o hw5 $(CCSOURCE)

clean:
	rm -f *.o *.out hw5

run: hw5
	./hw5




