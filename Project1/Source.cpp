
//Names: Dhvanil Shah
//Due Date: 3/5/2018
//Class: CPSC-323-04
//Project 1

#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<stdlib.h>
#include<algorithm>
#include <stack>
#include<iomanip>
using namespace std;

//Create variables to store keywords, operators, and separators
const string oper = "+-*/";
const string sps = "[]{}():;,";
const string compound_ops = "=<>^";
const string compound_sps = "%";


vector<string> tokens;
vector<string> lexemes;

int instr_address = 1;
int mem_address = 2000;
vector<string> identifiers;
vector<int> location;
vector<string> idtype;

//Create an enumerated state transition type
enum state {
	first, //Starting State
	a,	//Initial State for recognizing Identifiers
	b,	//State for recognizing letters of Identifiers
	c,	//Final State for a Identifier ending with a letter, and declare a token
	d,	//Final State for a Identifier ending with a '$' sign, and declare a token
	e,	//State for recognizing digits within Identifiers
	f,  //Transition state for recognizing Integers, and declares a token if it's a valid Integer
	g,  //Transition state for recognizing Reals, and declares a token it it's a valid Real
	h	//Initial State for recognizing opearators and separators
};


//Declare a state type object called "go" and set it to the starting state
state go = first;
bool check_table(string id) {

	for (int i = 0; i < identifiers.size(); i++) {
		if (identifiers[i] == id) {
			return true;
		}
	}
	return false;
}

struct instr_table {
	vector<int> address;
	vector<string> op;
	vector<string> oprnd;
};


stack<int> jump_stack;

instr_table table;
int tab_val = 1;

//Function to determine tokens and lexemes from
//Accepts a pointer to an array of char, inputted from a text file
void lex(char *buffer, ofstream &out) {

	//Declare Current character pointer, and a start pointer
	char *cc, *st;

	//Set those pointers to point to the location where the characters from text file are stored
	cc = st = buffer;

	//Initialize a variable that will concatenate characters once a token is found
	string toks = "";

	//Print the heading of the output
	cout << setw(20) << left << "Token" << "Lexeme\n" << setw(20) << right;
	//Send to output file
	out << setw(20) << left << "Token" << "Lexeme\n" << setw(20) << right;
	cout << std::string(35, '_') << endl;
	out << std::string(35, '_') << endl;


	//Loop that will iterate through the array, until the end of file character is read
	do {
		//Internal loop that will iterate through each character of array 
		//until an accepting state is found for each token

		switch (go) {

			//Case for the starting state
		case first:
			st = cc;
			//Statement that will ignore all whitespaces, horizontal tabs, and newline feeds and take the state back to initial state
			if (*cc == char(32) || *cc == char(9) || *cc == char(10)) {
				while (*cc == char(32) || *cc == char(9) || *cc == char(10)) {
					cc++;
				}
				st = cc;
				go = first;
				break;
			}
			//Statement that will recognize a comment, and annouce a token for comment
			if (*cc == char(33)) {
				cc++;
				while (*cc != char(33)) {
					cc++;
				}
				cc++;
				cout << setw(20) << left << "Comment" << "!...!\n" << setw(20) << right;;
				//Send to output file
				out << setw(20) << left << "Comment" << "!...!\n" << setw(20) << right;;
				st = cc;
				go = first;
				break;
			}

			//If a digit is read, the statement will transition the go object to f, which
			//is the state that will declare an integer token if valid integer is found
			if (isdigit(*cc)) {
				go = f;
				break;
			}
			//If a letter is read, the statement will transition the go object to a, which
			//is the starting state for recognizing identifiers/keywords
			if (isalpha(*cc)) {
				go = a;
				break;
			}
			//If an operator, or separator is read, the statement will transition the go object
			//to c, that will annouce opearators or separators
			if (oper.find(*cc) != std::string::npos || sps.find(*cc) != std::string::npos ||
				compound_ops.find(*cc) != std::string::npos || compound_sps.find(*cc) != std::string::npos) {
				go = h;
				break;
			}

			break;
			//End case first

			//Case a
		case a: {
			//If the character read is a letter, the statement will transition 'go' to state b
			if (isalpha(*cc)) {
				go = b;
				cc++;
			}
			break;
		}
				//End case a

				//Case b
		case b: {
			//If a '$' sign is read, go to state d
			if (*cc == char(36)) {
				go = d;
				cc++;
			}
			//Continue to be in stat b until letters are being read
			else if (isalpha(*cc)) {
				go = b;
				cc++;
			}
			//If a digit is read, go to state e
			else if (isdigit(*cc)) {
				go = e;
				cc++;
			}
			//Go to state c if a space, tab, new line is read, or if an opearator or separator is read
			else if (*cc == char(32) || oper.find(*cc) != std::string::npos ||
				sps.find(*cc) != std::string::npos || *cc == (10)
				|| *cc == char(9) || compound_ops.find(*cc) != std::string::npos ||
				compound_sps.find(*cc) != std::string::npos) {
				go = c;
			}
			break;
		}
				//End case b

				//Case c
		case c: {
			//Set the toks variable to empty
			toks = "";
			//Loop until the	start pointer is not equal to the current character pointer
			while (st != cc) {
				//Concatenate characters to toks variable
				toks += *st;
				st++;
			}

			//Announce a token identifer or keyword
			if (toks == "function" || toks == "int" || toks == "integer" || toks == "Keyword" || toks == "boolean" || toks == "real" ||
				toks == "if" || toks == "endif" || toks == "else" || toks == "return" ||
				toks == "put" || toks == "get" || toks == "while" || toks == "true" || toks == "false") {
				cout << setw(20) << left << "Keyword" << toks << setw(20) << right << endl;

				tokens.push_back("Keyword");
				if (toks == "true") {
					lexemes.push_back(to_string(1));
				}
				else if (toks == "false") {
					lexemes.push_back(to_string(0));
				}
				else {
					lexemes.push_back(toks);
				}

			}
			else {
				cout << setw(20) << left << "Identifier" << toks << setw(20) << right << endl;
				//Send to output file
				out << setw(20) << left << "Identifier" << toks << setw(20) << right << endl;


				tokens.push_back("Identifier");
				lexemes.push_back(toks);



			}
			go = first;
			break;
		}

				//End case c

				//Case d
		case d: {
			//Announce a token since a $ is marks an end of an identifier
			toks = "";
			while (st != cc) {
				toks += *st;
				st++;
			}
			cout << setw(20) << left << "Identifier" << toks << setw(20) << right << endl;
			//Send to output file
			out << setw(20) << left << "Identifier" << toks << setw(20) << right << endl;
			st = cc;
			go = first;

			tokens.push_back("Identifier");
			lexemes.push_back(toks);

			break;
		}	//End case d

			//Case e
		case e: {
			//If a digit is read inbetween identifer, continue to accept digits
			if (isdigit(*cc)) {
				cc++;
				go = e;
			}
			//If a letter is read, go to state b
			else if (isalpha(*cc)) {
				go = b;
				cc++;
			}
			//If a $ sign is read after a digit, go to state d
			else if (*cc == char(36)) {
				go = d;
				cc++;
			}
			//If anything except a letter or $ sign is read after a digit, announce an invalid identifier
			else if (*cc == char(32) || *cc == char(9) || *cc == char(10) ||
				oper.find(*cc) != std::string::npos || sps.find(*cc) != std::string::npos ||
				compound_ops.find(*cc) != std::string::npos || compound_sps.find(*cc) != std::string::npos) {
				toks = "";
				while (st != cc) {
					toks += *st;
					st++;
				}
				cout << setw(20) << left << toks << "Invalid Idenfier\n" << setw(20) << right;
				//Send to output file
				out << setw(20) << left << toks << "Invalid Idenfier\n" << setw(20) << right;
				go = first;
			}
			break;
			//End case e

			//Case f
		case f:
			//If a digit is read after the initial state, continue to read digits until a non-digit character is read
			st = cc;
			if (isdigit(*cc)) {
				toks = "";
				while (isdigit(*cc)) {
					toks += *cc;
					cc++;
				}
				//If a floating point is read after an integer, go to state g
				if (*cc == char(46)) {
					go = g;
					break;
				}
				//Announce a valid integer
				else {
					cout << setw(20) << left << "Integer" << toks << setw(20) << right << endl;
					//Send to output file
					out << setw(20) << left << "Integer" << toks << setw(20) << right << endl;
					st = cc;
					go = first;

					tokens.push_back("Integer");
					lexemes.push_back(toks);

					break;
				}
			}//End f

			 //Case g
		case g: {
			//Concatenate the digits before the floating point
			toks = "";
			while (st != cc) {
				toks += *st;
				st++;
			}
			cc++;
			//Concatenate the floating point and the digits after the floating point
			while (isdigit(*cc)) {
				cc++;
			}while (st != cc) {
				toks += *st;
				st++;
			}
			//Announe a Real
			cout << setw(20) << left << "Real" << toks << setw(20) << right << endl;
			//Send to output file
			out << setw(20) << left << "Real" << toks << setw(20) << right << endl;
			st = cc;
			go = first;

			tokens.push_back("Real");
			lexemes.push_back(toks);

			break;
		}
				//End case g

				//Case h
		case h: {
			//If an operator (+-*/) is read go to the following statement
			if (oper.find(*cc) != std::string::npos) {
				//Check if the variable is an operator, or a valid unary variable, or not
				char * unary = cc;
				switch (*cc) {
					//If the operator is +, /, *, check that its a valid operator, and not a unary 
				case '+': case '/': case '*': {
					unary--;
					if (*unary == char(32)) {
						while (*unary == char(32)) {
							unary--;
						}
					}
					//If the operator (+,/,*) turns out to be unary, announce an invalid unary operator 
					if (sps.find(*unary) != string::npos || compound_ops.find(*unary) != string::npos || compound_sps.find(*unary) != string::npos) {
						cout << setw(20) << left << "Invalid Unary" << *cc << setw(20) << right << endl;
						string temp = " ";
						temp += *cc;
						tokens.push_back("Operator");
						lexemes.push_back(temp);
						//Send to output file
						out << setw(20) << left << "Invalid Unary" << *cc << setw(20) << right << endl;
						cc++;
						go = first;
						break;
					}//Else announce a valid unary operator
					else {
						cout << setw(20) << left << "Operator" << *cc << setw(20) << right << endl;
						//Send to output file
						out << setw(20) << left << "Operator" << *cc << setw(20) << right << endl;

						string temp = " ";
						temp += *cc;
						tokens.push_back("Operator");
						lexemes.push_back(temp);

						cc++;
						go = first;

						break;
					}
				}
						  //If the operator read is a '-', check if its a unary operator, or a minus sign 
				case '-': {
					unary--;
					if (*unary == char(32)) {
						while (*unary == char(32)) {
							unary--;
						}
					}
					//If its a valid unary operator, announce a valid unary operator
					if (sps.find(*unary) != string::npos || compound_ops.find(*unary) != string::npos || compound_sps.find(*unary) != string::npos) {
						cout << setw(20) << left << "Unary Operator" << *cc << setw(20) << right << endl;
						//Send to output file
						out << setw(20) << left << "Unary Operator" << *cc << setw(20) << right << endl;

						string temp = " ";
						temp += *cc;
						tokens.push_back("Unary Operator");
						lexemes.push_back(temp);

						cc++;
						go = first;

						break;
					}
					//Else announce a minus sign operator 
					else {
						cout << setw(20) << left << "Operator" << *cc << setw(20) << right << endl;
						//Send to output file
						out << setw(20) << left << "Operator" << *cc << setw(20) << right << endl;

						string temp = " ";
						temp += *cc;
						tokens.push_back("Operator");
						lexemes.push_back(temp);

						cc++;
						go = first;

						break;
					}


				}
				}
			}//End Switch
			 //If the operator read is a separator, announce a separator token
			else if (sps.find(*cc) != std::string::npos) {

				cout << setw(20) << left << "Separator" << *cc << setw(20) << right << endl;
				//sendto output file
				out << setw(20) << left << "Separator" << *cc << setw(20) << right << endl;

				string temp = " ";
				temp += *cc;
				tokens.push_back("Separator");
				lexemes.push_back(temp);

				cc++;
				st = cc;
				go = first;
				break;
			}
			//Else if an operator that is read is a =,<,>,^, or %, check whether or not its a compound operator or compound separator
			else if (compound_ops.find(*cc) != std::string::npos || compound_sps.find(*cc) != std::string::npos) {
				char * f;
				f = cc;
				f++;
				st = f;
				//If it is a compound operator or compound separator, announce a token
				if (*f == char(9) || *f == char(10) || *f == char(32) || isalpha(*f) || isdigit(*f)
					|| oper.find(*f) != string::npos || sps.find(*f) != string::npos) {
					if (compound_ops.find(*cc) != std::string::npos) {
						cout << setw(20) << left << "Operator" << *cc << setw(20) << right << endl;
						//Send to output file
						out << setw(20) << left << "Operator" << *cc << setw(20) << right << endl;

						string temp = " ";
						temp += *cc;
						tokens.push_back("Operator");
						lexemes.push_back(temp);

					}

					else if (compound_sps.find(*cc) != std::string::npos) {
						cout << setw(20) << left << "Separator" << *cc << setw(20) << right << endl;
						//Send to output file
						out << setw(20) << left << "Separator" << *cc << setw(20) << right << endl;

						string temp = " ";
						temp += *cc;
						tokens.push_back("Separator");
						lexemes.push_back(temp);

					}
					st = cc = f;
					go = first;
					break;
				}
				//Else if it isn't a compound operator or separator, annouce a regualr operator token
				else if (compound_ops.find(*f) != std::string::npos || compound_sps.find(*f) != std::string::npos) {
					toks = "";
					f++;
					while (cc != f) {
						toks += *cc;
						cc++;
					}
					if (compound_ops.find(*st) != std::string::npos) {
						cout << setw(20) << left << "Operator" << toks << setw(20) << right << endl;
						//Sendto output file
						out << setw(20) << left << "Operator" << toks << setw(20) << right << endl;

						tokens.push_back("Operator");
						lexemes.push_back(toks);

					}
					else if (compound_sps.find(*st) != std::string::npos) {
						cout << setw(20) << left << "Separator" << toks << setw(20) << right << endl;
						//Send to output file
						out << setw(20) << left << "Separator" << toks << setw(20) << right << endl;

						tokens.push_back("Separator");
						lexemes.push_back(toks);

					}
					st = cc = f;
					go = first;
					break;
				}
				//If all the statements fail, announce an invalid operator
				else {
					cout << setw(20) << left << *cc << "Inavlid\n" << setw(20) << right << endl;
					//Send to output file
					out << setw(20) << left << *cc << "Inavlid\n" << setw(20) << right << endl;
					cc++;
					go = first;
					break;
				}
			}

			break;
		}
				//End case h

				//If all else fails, announce an error
		default:
			cout << "Error\n";
			//Send to output file
			out << "Error\n";
			break;
		}

		}//End Switch
		 //Loop until the End Of File character is not read
	} while (*cc != EOF);//End While

}//End Lex

int index = 0;

void rat();
void qualifier();
void opt_dec_list();
void dec_list();
void dec_list_();
void declaration();
void ids();
void ids_();
void stat_list();
void stat_list_();
void stat();
void compound();
void assign();
void iff();
void iff_();
void returns();
void returns_();
void print();
void scan();
void whiles();
void condition();
void relop();
void exp();
void exp_();
void term();
void term_();
void fact();
void prim();
void back_patch(int ad);


int label_jump_loc = 0;
int jumpz = 0;

void gen(string oper, string oprnd) {
	table.address.push_back(tab_val);
	table.op.push_back(oper);
	table.oprnd.push_back(oprnd);
	tab_val++;
}

int jumpaddr = 0;



int main() {

	//Create a ifstream variable
	char ans;

	do {
		cout << "Choose from the following files\n";
		cout << "testcase1.txt = Less than 10 lines of code\ntestcase2.txt = More tan 10 lines of code\ntestcase3.txt = More than 20 lines of code\n";
		cout << "Enter the name the file: ";
		string filename;
		ofstream out;
		ifstream in;
		cin >> filename;
		cout << endl;
		if (filename == "testcase1.txt") {
			out.open("outputcase1.txt");

		}
		else if (filename == "testcase2.txt") {
			out.open("outputcase2.txt");
		}
		else {
			out.open("outputcase3.txt");
		}
		in.open(filename.c_str());
		//Create a string variable
		string message;
		while (in) {
			//Read in the character of the source file into a string
			message.push_back(in.get());

		}
		//create a pointer to the string of characters
		char* strp;
		strp = &message[0];
		out << message << endl << endl;
		//Pass the pointer to the lex function
		lex(strp, out);
		cout << endl << "Would you like to test another file (y = yes/n = no): ";

		cin >> ans;
	} while (ans != 'n');

	int x = lexemes.size();
	cout << endl;
	lexemes.push_back("$");
	tokens.push_back("End");
	cout << "----------------------------------------------------------" << endl;
	for (int i = 0; i < lexemes.size(); i++) {
		cout << i << "\t" << "'" << tokens[i] << "'" << "   " << "'" << lexemes[i] << "'" << endl;
	}
	cout << endl << endl;

	rat();



	if (lexemes[index] == "$") {
		cout << "SUCCESSFULLY PARSED\n";
	}
	else {
		cout << "Parse Unsuccessful\n";
	}

	cout << endl << endl;

	for (int i = 0; i < table.address.size(); i++) {
		cout << table.address[i] << "\t" << table.op[i] << "\t" << table.oprnd[i] << endl;
	}

	cout << endl << endl;
	cout << "\t" << "SYMBOL TABLE" << endl;

	for (int i = 0; i < identifiers.size(); i++) {
		cout << setw(10) << left << identifiers[i] << setw(10) << location[i] << setw(10) << idtype[i] << endl;
	}
	cout << endl;

	system("Pause");
	return 0;
}
void back_patch(int loc) {
	int addrs = jump_stack.top();
	table.oprnd[addrs - 1] = to_string(loc);

}


// Rat18s
void rat() {
	if (lexemes[index] == "%%") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions>  -> " << lexemes[index] << endl;
		cout << endl;
		index++;
		opt_dec_list();
		stat_list();
	}
	else if (lexemes[index] == "$") {
		cout << "End of Progam" << endl;
	}
	else {
		cout << "Error" << endl;
	}
}

// Qualifier
void qualifier() {
	if (lexemes[index] == "integer" || lexemes[index] == "int" || tokens[index] == "Integer")
	{
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> "
			"<Opt Param List> -> <Parameter List> -> <Parameter> -> <Qualifier> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
	}
	else if (lexemes[index] == "boolean") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> "
			"<Opt Param List> -> <Parameter List> -> <Parameter> -> <Qualifier> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
	}
	else if (lexemes[index] == "real") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> "
			"<Opt Param List> -> <Parameter List> -> <Parameter> -> <Qualifier> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
	}
	else {
		cout << "Error" << endl;
	}
}



// Opt Declaration List
void opt_dec_list() {
	if (lexemes[index] == "integer" || lexemes[index] == "int" || tokens[index] == "Integer") {
		dec_list();
	}
	else if (lexemes[index] == "boolean") {
		dec_list();

	}
	else if (lexemes[index] == "real") {
		dec_list();
	}
	else if (lexemes[index] == " {" || tokens[index] == "Identifier" || lexemes[index] == "if" || lexemes[index] == "return"
		|| lexemes[index] == "put" || lexemes[index] == "get" || lexemes[index] == "while") {
		return;
	}
	else {
		cout << "Error" << endl;
	}

}


// Declaration List
void dec_list() {
	if (lexemes[index] == "integer" || lexemes[index] == "int" || lexemes[index] == "boolean" || lexemes[index] == "real" || tokens[index] == "Integer") {
		declaration();
		if (lexemes[index] == " ;") {
			cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
			cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Opt Declaration List> ->"
				" <Declaration List> -> <Declaration> ->" << lexemes[index] << endl;
			cout << endl;
			index++;
			dec_list_();
		}
	}

	else {
		cout << "Error" << endl;
	}
}


// Declaration List'
void dec_list_() {
	if (lexemes[index] == "integer" || lexemes[index] == "int" || tokens[index] == "Integer" || tokens[index] == "Real"
		|| tokens[index] == "Boolean" || lexemes[index] == "boolean" || lexemes[index] == "real") {
		dec_list();
	}
	else if (lexemes[index] == " {" || tokens[index] == "Identifier" || lexemes[index] == "if" || lexemes[index] == "return"
		|| lexemes[index] == "put" || lexemes[index] == "get" || lexemes[index] == "while") {
		return;
	}
	else {
		cout << "Error" << endl;
	}
}
string from = "";
// Declaration
void declaration() {
	if (lexemes[index] == "integer" || lexemes[index] == "int" || tokens[index] == "Integer") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Opt Declaratin List> -> <Declaration List> -> "
			" <Declaration> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
		from = "integer";
		ids();
		from = "";
	}
	else if (lexemes[index] == "boolean") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Opt Declaratin List> -> <Declaration List> -> "
			" <Declaration> -> " << lexemes[index] << endl;
		cout << endl;
		from = "boolean";
		index++;
		ids();
		from = "";
	}
	else if (lexemes[index] == "real") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Opt Declaratin List> -> <Declaration List> -> "
			" <Declaration> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
		ids();
	}
	else {
		cout << "Error" << endl;
	}
}

// IDs
void ids() {
	if (tokens[index] == "Identifier") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> function -> <Identifier> -> [ -> "
			"<Opt Param List> -> <Parameter List> -> <Parameter> -> <IDs> ->" << lexemes[index] << endl;
		cout << endl;
		if (from == "integer") {
			if (check_table(lexemes[index]) == true) {
				cout << "---ERROR---\n Redefining Identifier.\n PROGRAM TERMINATED\n";
				system("Pause");
				exit(1);
			}
			else {
				identifiers.push_back(lexemes[index]);
				idtype.push_back("integer");
				location.push_back(mem_address);
				mem_address++;
			}

		}
		else if (from == "boolean") {
			if (check_table(lexemes[index]) == true) {
				cout << "---ERROR---\n Redefining Identifier.\n PROGRAM TERMINATED\n";
				system("Pause");
				exit(1);
			}
			else {
				identifiers.push_back(lexemes[index]);
				idtype.push_back("boolean");
				location.push_back(mem_address);
				mem_address++;
			}
		}
		index++;
		ids_();
	}
	else {
		cout << "Error" << endl;
	}
}

// IDs'
void ids_() {
	if (lexemes[index] == " ,") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> function -> <Identifier> -> [ -> "
			"<Opt Param List> -> <Parameter List> -> <Parameter> -> <IDs> -> <IDs'> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
		ids();
	}
	else if (lexemes[index] == " ;" || " :" || " )") {
		return;
	}
	else {
		cout << "Error" << endl;
	}
}


// Statement List
void stat_list() {
	if (lexemes[index] == " {" || tokens[index] == "Identifier" || lexemes[index] == "if" ||
		lexemes[index] == "return" || lexemes[index] == "put" || lexemes[index] == "get" ||
		lexemes[index] == "while") {
		stat();
		stat_list_();
	}
	else {
		cout << "Error" << endl;
	}

}

// Statement List'
void stat_list_() {
	if (lexemes[index] == " {" || tokens[index] == "Identifier" || lexemes[index] == "if" ||
		lexemes[index] == "return" || lexemes[index] == "put" || lexemes[index] == "get" || lexemes[index] == "while") {
		stat_list();
	}
	else if (lexemes[index] == "$" || " }") {
		return;
	}
	else {
		cout << "Error" << endl;
	}

}
// Statement 
void stat() {
	if (lexemes[index] == " {") {
		compound();
	}
	else if (tokens[index] == "Identifier") {
		assign();
	}
	else if (lexemes[index] == "if") {
		iff();
	}
	else if (lexemes[index] == "return") {
		returns();
	}
	else if (lexemes[index] == "put") {
		print();
	}
	else if (lexemes[index] == "get") {
		scan();
	}
	else if (lexemes[index] == "while") {
		whiles();
	}
	else {
		cout << "Error" << endl;
	}
}
//Compound

void compound() {
	if (lexemes[index] == " {") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
		stat_list();
		if (lexemes[index] == " }") {
			cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
			cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
				"<Statement List> -> <Statement> -> <If> -> " << lexemes[index] << endl;
			cout << endl;
			index++;
		}
	}
	else {
		cout << "Error" << endl;
	}

}


//Assign
void assign() {
	if (tokens[index] == "Identifier") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Assign> -> " << lexemes[index] << endl;
		cout << endl;
		if (check_table(lexemes[index]) == false) {
			cout << "---ERROR---\n Using undeclared Identifier.\n PROGRAM TERMINATED\n";
			system("Pause");
			exit(1);
		}

		else {
			int token_location = 0;
			for (int i = 0; i < identifiers.size(); i++) {
				if (identifiers[i] == lexemes[index]) {
					token_location = location[i];
				}
			}
			index++;
			if (lexemes[index] == " =") {
				cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
				cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
					"<Statement List> -> <Statement> -> <Assign> -> " << lexemes[index] << endl;
				cout << endl;
				index++;
				exp();
				gen("POPM", to_string(token_location));
				if (lexemes[index] == " ;") {
					cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
					cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
						"<Statement List> -> <Statement> -> <If> -> " << lexemes[index] << endl;
					cout << endl;

					index++;
				}
			}
		}
	}
	else {
		cout << "Error" << endl;
	}

}

// If
void iff() {
	if (lexemes[index] == "if") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <If> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
		if (lexemes[index] == " (") {
			cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
			cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
				"<Statement List> -> <Statement> -> <If> -> " << lexemes[index] << endl;
			cout << endl;
			index++;
			condition();
			if (lexemes[index] == " )") {
				cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
				cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
					"<Statement List> -> <Statement> -> <If> -> " << lexemes[index] << endl;
				cout << endl;
				index++;
				stat();
				int adders = tab_val;
				back_patch(adders);
				iff_();
				gen(" ", " ");

			}
		}

	}


	else {
		cout << "Error" << endl;
	}


}

// If'
void iff_() {
	if (lexemes[index] == "endif") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <If> ->  <If'> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
	}
	else if (lexemes[index] == "else") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <If> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
		stat();
		if (lexemes[index] == "endif") {
			cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
			cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
				"<Statement List> -> <Statement> -> <If> -> " << lexemes[index] << endl;
			cout << endl;
			index++;
		}
	}
	else {
		cout << "Error" << endl;
	}

}


// Return
void returns() {
	if (lexemes[index] == "return") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body> -> "
			"<Statement List> -> <Statement> -> <Return> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
		returns_();
	}
	else {
		cout << "Error" << endl;
	}

}

// Return'
void returns_() {
	if (lexemes[index] == " ;") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Return> -> <Return'> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
	}
	else if (lexemes[index] == " - " || tokens[index] == "Identifier" ||
		tokens[index] == "Integer" || lexemes[index] == "int" || lexemes[index] == " (" ||
		lexemes[index] == "1" || lexemes[index] == "0" || lexemes[index] == "real") {
		exp();
		if (lexemes[index] == " ;") {
			cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
			cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
				"<Statement List> -> <Statement> -> <Return> -> <Return'> -> <Expression> -> " << lexemes[index] << endl;
			cout << endl;
			index++;
		}
	}
	else {
		cout << "Error" << endl;
	}

}

// Print
void print() {
	if (lexemes[index] == "put") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Print> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
		if (lexemes[index] == " (") {
			cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
			cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
				"<Statement List> -> <Statement> -> <Print> -> " << lexemes[index] << endl;
			cout << endl;
			index++;
			exp();
		}
		if (lexemes[index] == " )") {
			cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
			cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
				"<Statement List> -> <Statement> -> <Print> -> " << lexemes[index] << endl;
			cout << endl;
			index++;
		}
		if (lexemes[index] == " ;") {
			cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
			cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
				"<Statement List> -> <Statement> -> <Print> -> " << lexemes[index] << endl;
			cout << endl;
			index++;
			gen("STDOUT", " ");

		}
	}



	else {
		cout << "Error" << endl;
	}
}

// Scan
void scan() {
	if (lexemes[index] == "get") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Scan> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
		gen("STDIN", " ");
		if (lexemes[index] == " (") {
			cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
			cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
				"<Statement List> -> <Statement> -> <Scan> -> " << lexemes[index] << endl;
			cout << endl;
			index++;
			for (int i = 0; i < identifiers.size(); i++) {
				if (identifiers[i] == lexemes[index]) {
					gen("POPM", to_string(location[i]));
					break;
				}
			}
			ids();

		}
		if (lexemes[index] == " )") {
			cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
			cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
				"<Statement List> -> <Statement> -> <Scan> -> " << lexemes[index] << endl;
			cout << endl;
			index++;
		}
		if (lexemes[index] == " ;") {
			cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
			cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
				"<Statement List> -> <Statement> -> <Scan> -> " << lexemes[index] << endl;
			cout << endl;
			index++;
		}
	}

	else {
		cout << "Error" << endl;
	}
}

// While
void whiles() {
	if (lexemes[index] == "while") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <While> -> " << lexemes[index] << endl;
		cout << endl;
		label_jump_loc = tab_val;
		gen("LABEL", " ");
		index++;
		if (lexemes[index] == " (") {
			cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
			cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
				"<Statement List> -> <Statement> -> <While> -> " << lexemes[index] << endl;
			cout << endl;
			index++;
			condition();
			if (lexemes[index] == " )") {
				cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
				cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
					"<Statement List> -> <Statement> -> <While> -> " << lexemes[index] << endl;
				cout << endl;
				index++;
				stat();
				gen("JUMP", to_string(label_jump_loc));
				back_patch(tab_val);

			}

		}
	}


	else {
		cout << "Error" << endl;
	}

}

// Condition
void condition() {
	if (lexemes[index] == " -" || tokens[index] == "Identifier" || tokens[index] == "Integer" || lexemes[index] == "integer" ||
		lexemes[index] == "int" || lexemes[index] == " (" || lexemes[index] == "real" || lexemes[index] == "1" ||
		lexemes[index] == "0") {
		exp();

		int x = index;
		relop();
		exp();

		if (lexemes[x] == " <") {
			gen("LES", " ");
			jump_stack.push(tab_val);
			gen("JUMPZ", " ");

		}
		else if (lexemes[x] == " >") {
			gen("GRT", " ");
			jump_stack.push(tab_val);
			gen("JUMPZ", " ");
		}
		else if (lexemes[x] == "==") {

			gen("EQU", " ");
			jump_stack.push(tab_val);
			gen("JUMPZ", " ");
		}
		else if (lexemes[x] == "^=") {
			gen("NEQ", " ");
			jump_stack.push(tab_val);
			gen("JUMPZ", " ");
		}
		else if (lexemes[x] == "=>") {
			gen("GEQ", " ");
			jump_stack.push(tab_val);
			gen("JUMPZ", " ");
		}
		else if (lexemes[x] == "=<") {
			gen("LEQ", " ");
			jump_stack.push(tab_val);
			gen("JUMPZ", " ");
		}

	}
	else {
		cout << "Error" << endl;
	}
}
// Relop
void relop() {

	if (lexemes[index] == "==") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
	}
	else if (lexemes[index] == " =") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
	}
	else if (lexemes[index] == " >") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
	}
	else if (lexemes[index] == " <") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
	}
	else if (lexemes[index] == "^=") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
	}
	else if (lexemes[index] == "=>") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
	}
	else if (lexemes[index] == "=<") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
	}
	else {
		cout << "Error" << endl;
	}
}
// Expression
void exp() {
	if (lexemes[index] == " -" || tokens[index] == "Identifier" || lexemes[index] == "integer" ||
		tokens[index] == "Integer" || lexemes[index] == "int" ||
		lexemes[index] == " (" || lexemes[index] == "real" || lexemes[index] == "1" || lexemes[index] == "0") {

		term();
		exp_();


	}
	else {
		cout << "Error " << endl;
	}

}

// Expression'
void exp_() {
	if (lexemes[index] == " +") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> <Expression> ->"
			"<Experssion'> -> " << lexemes[index] << endl;
		cout << endl;

		index++;
		term();

		gen("ADD", " ");
		exp_();

	}
	else if (lexemes[index] == " -") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> <Expression> ->"
			"<Experssion'> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
		term();
		exp_();
	}
	else if (lexemes[index] == " ;" || lexemes[index] == " )" || lexemes[index] == "==" || lexemes[index] == "^="
		|| lexemes[index] == " >" || lexemes[index] == " <" || lexemes[index] == "=<" || lexemes[index] == "=>") {
		return;
	}
	else {
		cout << "Error" << endl;
	}

}

// Term
void term() {
	if (lexemes[index] == " -" || tokens[index] == "Identifier" || lexemes[index] == "integer" ||
		lexemes[index] == "int" || tokens[index] == "Integer"
		|| lexemes[index] == " (" || lexemes[index] == "real" || lexemes[index] == "1" || lexemes[index] == "0") {

		fact();

		term_();
	}
	else {
		cout << "Error" << endl;
	}

}
// Term'
void term_() {
	if (lexemes[index] == " *") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> <Expression> ->"
			"<Term> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
		fact();
		term_();
	}
	else if (lexemes[index] == " /") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> <Expression> ->"
			"<Term> -> " << lexemes[index] << endl;
		cout << endl;
		index++;
		fact();
		term_();
	}
	else if (lexemes[index] == " +" || lexemes[index] == " -" || lexemes[index] == " ;" || lexemes[index] == " )"
		|| lexemes[index] == "==" || lexemes[index] == "^="
		|| lexemes[index] == " >" || lexemes[index] == " <" || lexemes[index] == "=<" || lexemes[index] == "=>") {
		return;
	}
	else {
		cout << "Error" << endl;
	}

}

// Factor
void fact() {
	if (lexemes[index] == " -") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> <Expression> ->"
			"<Term> -> <Factor> " << lexemes[index] << endl;
		cout << endl;
		index++;
		prim();
	}
	else if (tokens[index] == "Identifier" || lexemes[index] == "int" || tokens[index] == "Integer" || lexemes[index] == "integer" ||
		lexemes[index] == " (" || lexemes[index] == "real" || lexemes[index] == "1" || lexemes[index] == "0") {
		prim();
	}
	else {
		cout << "Error" << endl;
	}

}
// Primary
void prim() {

	if (tokens[index] == "Identifier") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> <Expression> ->"
			"<Term> -> -> <Factor> -> <Primary> ->  " << lexemes[index] << endl;
		cout << endl;
		for (int i = 0; i < identifiers.size(); i++) {
			if (identifiers[i] == lexemes[index]) {
				gen("PUSHM", to_string(location[i]));
			}
		}
		index++;
		if (lexemes[index] == " (") {
			cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
			cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
				"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> <Expression> ->"
				"<Term> -> -> <Factor> -> <Primary> ->  " << lexemes[index] << endl;
			cout << endl;
			index++;
			ids();
			if (lexemes[index] == " )") {
				cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
				cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
					"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> <Expression> ->"
					"<Term> -> -> <Factor> -> <Primary> ->  " << lexemes[index] << endl;
				cout << endl;
				index++;
			}
		}

	}
	else if (lexemes[index] == "integer" || lexemes[index] == "int" || tokens[index] == "Integer") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> <Expression> ->"
			"<Term> -> -> <Factor> -> <Primary> ->  " << lexemes[index] << endl;
		cout << endl;

		gen("PUSHI", (lexemes[index]));

		index++;


	}

	else if (lexemes[index] == " (") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> <Expression> ->"
			"<Term> -> -> <Factor> -> <Primary> ->  " << lexemes[index] << endl;
		cout << endl;
		index++;
		exp();
		if (lexemes[index] == " )") {
			cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
			cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
				"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> <Expression> ->"
				"<Term> -> -> <Factor> -> <Primary> ->  " << lexemes[index] << endl;
			cout << endl;

			index++;
		}
	}
	else if (lexemes[index] == "real") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> <Expression> ->"
			"<Term> -> -> <Factor> -> <Primary> ->  " << lexemes[index] << endl;
		cout << endl;
		index++;
	}
	else if (lexemes[index] == "1") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> <Expression> ->"
			"<Term> -> -> <Factor> -> <Primary> ->  " << lexemes[index] << endl;
		cout << endl;
		gen("PUSHI", (lexemes[index]));
		index++;
	}
	else if (lexemes[index] == "0") {
		cout << setw(10) << left << "Token:" << setw(15) << tokens[index] << setw(10) << "Lexeme:" << setw(10) << lexemes[index] << endl;
		cout << "<Rat18s> -> <Opt Funcion Definitions> -> <Function Definitions> -> <Function> -> <Body>"
			"<Statement List> -> <Statement> -> <Compound> -> <If> -> <Condition> -> <Relop> -> <Expression> ->"
			"<Term> -> -> <Factor> -> <Primary> ->  " << lexemes[index] << endl;
		cout << endl;

		gen("PUSHI", (lexemes[index]));

		index++;


	}
	else {
		cout << "Error" << endl;
	}
}