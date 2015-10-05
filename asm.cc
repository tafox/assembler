#include "kind.h"
#include "lexer.h"
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <string>
// Use only the neeeded aspects of each namespace
using std::string;
using std::vector;
using std::endl;
using std::cerr;
using std::cin;
using std::cout;
using std::getline;
using ASM::Token;
using ASM::Lexer;
using ASM::Kind;
using ASM::INT;
using ASM::HEXINT;
using ASM::DOTWORD;

class Assembler {
	public:
		Assembler(); 
		void AsmDotWord(Token* token);
		void OutputMachineCode();
	private:
		void AddInstruction(int instruct);
		vector<int> machine_code_;
		map<string, int> symbol_table_;
};

int main(int argc, char* argv[]){
  // Nested vector representing lines of Tokens
  // Needs to be used here to cleanup in the case
  // of an exception
  vector< vector<Token*> > tokLines;
  try{
    // Create a MIPS recognizer to tokenize
    // the input lines
    Lexer lexer;
	Assembler assembler;
    // Tokenize each line of the input
    string line;
    while(getline(cin,line)){
      tokLines.push_back(lexer.scan(line));
    }

    // Iterate over the lines of tokens and print them
    // to standard error
    vector<vector<Token*> >::iterator it;
    for(it = tokLines.begin(); it != tokLines.end(); ++it){
      vector<Token*>::iterator it2;
	  int i = 0;
      for(it2 = it->begin(); it2 != it->end(); ++it2, i++){
        cerr << "  Token: "  << *(*it2) << endl;
	  	if (i == 0 && (*it2)->getKind() == DOTWORD) {
			++it2;
        	cerr << "  Token: "  << *(*it2) << endl;
			assembler.AsmDotWord(*it2);
			break;
		}
      }
    }
	assembler.OutputMachineCode();

  } catch(const string& msg){
    // If an exception occurs print the message and end the program
    cerr << msg << endl;
  }
  // Delete the Tokens that have been made
  vector<vector<Token*> >::iterator it;
  for(it = tokLines.begin(); it != tokLines.end(); ++it){
    vector<Token*>::iterator it2;
    for(it2 = it->begin(); it2 != it->end(); ++it2){
      delete *it2;
    }
  }
}

Assembler::Assembler() {}

void Assembler::AsmDotWord(Token* token) {
	Kind kind = token->getKind();
	if (kind == HEXINT || kind == INT) {
		AddInstruction((int) token->toInt());
	}
}

void Assembler::OutputMachineCode() {
	vector<int>::iterator it;
	for (it = machine_code_.begin(); it != machine_code_.end(); ++it) {
		putchar(*it>>24 & 0xff);
		putchar(*it>>16 & 0xff);
		putchar(*it>>8 & 0xff);
		putchar(*it & 0xff);
	}
}

void Assembler::AddInstruction(int instruct) {
	machine_code_.push_back(instruct);
};
