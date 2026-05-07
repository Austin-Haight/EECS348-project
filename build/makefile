CC=g++
CXXFLAGS= -c -g -Wall 
OBJECTS=controller.o Error.o Evaluator.o Io.o Parser.o Token.o Tokenizer.o

all: evaluator

evaluator: main.o  $(OBJECTS)
	mkdir -p ../bin
	$(CC) $^ -o ../bin/evaluator

$(OBJECTS): %.o: ../src/%.cpp
	$(CC) $(CXXFLAGS) $^ -o $@

main.o: ../src/main.cpp
	$(CC) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf *.o
	rm -rf ../bin

run: 
	make
	../bin/evaluator