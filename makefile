CXX = g++
CXXFLAGS = -Wall -g
asm: asm.o kind.o lexer.o
	$(CXX) $(CXXFLAGS) -o asm asm.o kind.o lexer.o

asm.o: asm.cc kind.h lexer.h
	$(CXX) $(CXXFLAGS) -c asm.cc

kind.o: kind.h

lexer.o: lexer.h
