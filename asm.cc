#include "kind.h"
#include "lexer.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <stdio.h>
#include <string>
// Use only the neeeded aspects of each namespace
using std::string;
using std::vector;
using std::endl;
using std::map;
using std::cerr;
using std::cin;
using std::getline;
using ASM::Token;
using ASM::Lexer;
using ASM::Kind;
using ASM::INT;
using ASM::HEXINT;
using ASM::DOTWORD;
using ASM::LABEL;
using ASM::ID;
using ASM::ERR;

class Assembler {
	public:
		Assembler(); 
		void AsmDotWord(Token* token);
		void OutputMachineCode();
		void OutputSymbolTable();
		bool LookUpLabel(string label);
		void AddToSymbolTable(string label, int address);
		void InsertLabelValue(string label, int address);
		bool error_;
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
	int address = 0;
    // Tokenize each line of the input
    string line;
    while(getline(cin,line)){
      tokLines.push_back(lexer.scan(line));
    }

    // Iterate over the lines of tokens and print them
    // to standard error
	//PASS 1
    vector<vector<Token*> >::iterator it;
    for(it = tokLines.begin(); it != tokLines.end(); ++it){
		if (assembler.error_) {
			break;
		}
		vector<Token*>::iterator it2;
	  	int i = 0;
      	for(it2 = it->begin(); it2 != it->end(); ++it2, i++){
			if (assembler.error_) {
				break;
			}
      		//cerr << "  Token: "  << *(*it2) << endl;
			Kind kind = (*it2)->getKind();
			if ((kind == ERR) || (i == 0 && kind != LABEL && kind != DOTWORD)) {
				cerr << "ERROR Invalid Token." << endl;
				assembler.error_ = true;
				break;
			} 
			// deal with a label 
			if ((*it2)->getKind() == LABEL) {
				string label = (*it2)->getLexeme();
				label = label.substr(0, label.size()-1);
				if (i == 0) { 
					assembler.AddToSymbolTable(label, address);
				} else if (i > 0) {
					// if previous thing was a label add this label too
					if ((*(it2-1))->getKind() == LABEL) {
						assembler.AddToSymbolTable(label, address);
					}
				}
			} 
			// deal with a DOTWORD instruct
	  		if ((*it2)->getKind() == DOTWORD) {
				++it2;
				if (it2 != it->end()) {
					assembler.AsmDotWord(*it2);
				} else {
					cerr << "ERROR .word missing operand." << endl;
					assembler.error_ = true;
					break;
				}
				if ((it2+1) != it->end()) {
					cerr << "ERROR Extra Token." << endl;
					assembler.error_ = true;
				}
				address += 4;
				break;
			}
		}
    }
	// PASS 2
	address = 0;
    for(it = tokLines.begin(); it != tokLines.end(); ++it){
		if (assembler.error_) {
			break;
		}
		vector<Token*>::iterator it2;
	  	int i = 0;
      	for(it2 = it->begin(); it2 != it->end(); ++it2, i++){
			if ((*it2)->getKind() == DOTWORD) {
				++it2;
				if ((*it2)->getKind() == ID) {
					assembler.InsertLabelValue((*it2)->getLexeme(), address);
				}
				address += 4;
				break;
			}
		}
	}
	if (!assembler.error_) {
		assembler.OutputMachineCode();
		//assembler.OutputSymbolTable();
	}

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

Assembler::Assembler() : error_(false) {}

void Assembler::AsmDotWord(Token* token) {
	Kind kind = token->getKind();
	if (kind == HEXINT || kind == INT) {
		AddInstruction(token->toInt());
	} else if (kind == ID) {
		return;
	} else {
		cerr << "ERROR Invalid Operand to .word" << endl;
		error_ = true;
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

void Assembler::OutputSymbolTable() {
	map<string, int>::iterator it;
	for (it = symbol_table_.begin(); it != symbol_table_.end(); ++it) {
		cerr << it->first << " " << it->second << endl;
	}
}

void Assembler::AddInstruction(int instruct) {
	machine_code_.push_back(instruct);
};

bool Assembler::LookUpLabel(string label) {
	map<string, int>::iterator it;
	it = symbol_table_.find(label);
	if (it != symbol_table_.end()) {
		cerr << "ERROR Duplicate Label." << endl;
		error_ = true;
		return true;
	}
	return false;
}

void Assembler::AddToSymbolTable(string label, int address) {
	if (!LookUpLabel(label)) {
		symbol_table_[label] = address;
	}
}

void Assembler::InsertLabelValue(string label, int address) {
	map<string, int>::iterator it;
	it = symbol_table_.find(label);
	if (it != symbol_table_.end()) {
		int label_address = it->second;
		machine_code_.insert(machine_code_.begin() + (address/4), label_address); 
	} else {
		cerr << "ERROR Label undefined." << endl;
		error_ = true;
	}
}
