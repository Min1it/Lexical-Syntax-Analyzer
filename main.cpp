/**
 * Authors: Lucas - Syntax Analyzer & EBNF, Hudson - Lexical Analyzer, 
 * Contributors: Smaran 
**/

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <string.h>
#include <functional>

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define STRING_OPEN 2
#define UNKNOWN 3

/* Token codes */
// TODO: HANDLE HEADER
#define KEYWORD 9			// things like int, for, if, else, sizeof, etc.
#define INT_LIT 10			// integer
#define INT_ARR 11			// integer array (not in use?)
#define STRING_LIT 12		// anything surrounded in double quotes
#define IDENT 15			// variable name
#define COMMENT 19			// a comment within code

// Operators
#define ASSIGN_OP 20		// =
#define ADD_OP 21			// +
#define SUB_OP 22			// -
#define MULT_OP 23			// *
#define DIV_OP 24			// /
#define LT_OP 25			// <
#define GT_OP 26			// >

// other syntax stuff
#define COMMA 27			// ,
#define END_STMT 28			// ;

// Nesting stuff
#define LEFT_PAREN 30		// (
#define RIGHT_PAREN 31		// )
#define LEFT_BRACKET 32		// [
#define RIGHT_BRACKET 33	// ]
#define LEFT_BRACE 34		// {
#define RIGHT_BRACE 35		// }

#define PREPROCESSOR 99		// #include <...>
#define LINE_END 100		// \n

/*Repetition Rules*/
#define ONE 0				// one.....
#define MINZERO  1			// {}  	(kleene closure)
#define MINONE  2			// {}- 	(positive kleene closure)
#define MAXONE  3			// []	(optional)

/* Function declarations */
//Lexical Analyzer
int lex();
void addChar();
void getChar();
void getNonBlank();
int lookup(char ch);
bool isKeyword(char lexeme[]);

//Syntax Analyzer
void nonterminalHelper(std::function<int()> rule, int reps); 
void terminalHelper(int tokenCode, int reps);
void terminalHelper(int tokenCode, int reps, const char keyword[]);

//ebnf nonterminal definitions
int program();
int preprocessor();
int block();
int statement();
int assignment();
int expression();
int expressionHelper();
int print();
int forloop();
int ifStatement();
int returnStatement();

/* global vars */
std::ifstream fileIn;
char nextChar;
char lexeme[100];

const char* keywords[] = {
	"int", "for", "if", "return", "sizeof", "main", "printf"
};

int charClass;
int nextToken;
int lexLen = 0;

int main(int argc, char* argv[]) {

	std::string fileName = "";

	// Check if no file name given
	if (argc != 2) {
		std::cout << "No file specified. Please enter one: " << std::endl;
		std::cin >> fileName;
	}
	else {
		//std::getline(std::cin, fileName);
		fileName = argv[1];
	}

	// Open file to read

	try {
		fileIn.open(fileName);
	}
	catch (const std::ifstream::failure& e) {
		std::cout << "Error: Could not open file (exception)." << std::endl;
		exit(-1);
	}

	// check if file is actually open
	if (!fileIn.is_open()) {
		std::cout << "Error: Could not open file." << std::endl;
		exit(-2);
	}

	// Get char to start off with
	getChar();
	lex();

	program();

	return 0;

}

/*program - ebnf rule:
	<program> ::= [<preprocessor>]  "int main()" {<block>}
*/
int program(){
	printf("Enter <program>\n");

	nonterminalHelper(preprocessor, MAXONE);
	terminalHelper(KEYWORD, ONE, "int");
	terminalHelper(KEYWORD, ONE, "main");
	terminalHelper(LEFT_PAREN, ONE);
	terminalHelper(RIGHT_PAREN, ONE);
	nonterminalHelper(block, ONE);
	//check for int main(){
	printf("Exit <program>\n");
	return 0;
}

/*block - ebnf rule:
	<block> ::= "{" {<statement>} "}"
*/
int block(){
	printf("Enter <block>\n");
	terminalHelper(LEFT_BRACE, ONE);
	nonterminalHelper(statement, MINZERO);

	terminalHelper(RIGHT_BRACE, ONE);
	printf("Exit <block>\n");
	return 0;
}

/*statement - ebnf rule:
	<statement> ::= assignmentStatement [";"]
              | forStatement 
              | printStatement [";"]
		      | returnStatement [";"]
	 	      | ifStatement
              | comment
*/
int statement(){
	switch(nextToken){
		case COMMENT:
			printf("Enter <statement>\n");
			terminalHelper(COMMENT, ONE);
			printf("Exit <statement>\n");
			return 0;
		case IDENT:
			printf("Enter <statement>\n");
			nonterminalHelper(assignment, ONE);
			terminalHelper(END_STMT, MAXONE);
			printf("Exit <statement>\n");
			return 0;
		case KEYWORD:
			if(strcmp(lexeme, "int") == 0){
				printf("Enter <statement>\n");
				nonterminalHelper(assignment, ONE);
				terminalHelper(END_STMT, MAXONE);
				printf("Exit <statement>\n");
				return 0;
			} else if (strcmp(lexeme, "printf") == 0){
				printf("Enter <statement>\n");
				nonterminalHelper(print, ONE);
				terminalHelper(END_STMT, MAXONE);
				printf("Exit <statement>\n");
				return 0;
			} else if (strcmp(lexeme, "for") == 0){
				printf("Enter <statement>\n");
				nonterminalHelper(forloop, ONE);
				printf("Exit <statement>\n");
				return 0;
			} else if (strcmp(lexeme, "if") == 0){
				printf("Enter <statement>\n");
				nonterminalHelper(ifStatement, ONE);
				printf("Exit <statement>\n");
				return 0;
			} else if (strcmp(lexeme, "return") == 0){
				printf("Enter <statement>\n");
				nonterminalHelper(returnStatement, ONE);
				terminalHelper(END_STMT, MAXONE);
				printf("Exit <statement>\n");
				return 0;
			}else{
				return -1;
			}
		default:
			return -1;
	}
	printf("Exit <statement>\n");
	return 0;

}

/*returnStatement - ebnf rule:
	<returnStatement> ::= "return" <expression>
*/
int returnStatement(){
	if(nextToken != KEYWORD || strcmp(lexeme, "return") != 0){
		return -1;
	}

	if(nextToken == KEYWORD || strcmp(lexeme, "return") == 0){
		printf("Enter <returnStatement>\n");
		terminalHelper(KEYWORD, ONE, "return");
		nonterminalHelper(expression, ONE);

		printf("Exit <returnStatement>\n");
		return 0;
	} else{
		return -1;
	}


}

/*ifStatement - ebnf rule:
	<ifStatement> ::= "if(" <expression> ")" <block>
*/
int ifStatement(){
	if(nextToken != KEYWORD || strcmp(lexeme, "if") != 0){
		return -1;
	}

	if(nextToken == KEYWORD || strcmp(lexeme, "if") == 0){
		printf("Enter <ifStatement>\n");
		terminalHelper(KEYWORD, ONE, "if");
		terminalHelper(LEFT_PAREN, ONE);
		nonterminalHelper(expression, ONE);
		terminalHelper(RIGHT_PAREN, ONE);
		nonterminalHelper(block, ONE);

		printf("Exit <ifStatement>\n");
		return 0;
	} else{
		return -1;
	}

}

/*forloop - ebnf rule:
	<forLoop> ::= "for(" <assignmentStatement> ";"  <expression> ";" <expression> ")" <block>
*/
int forloop(){
	if(nextToken != KEYWORD || strcmp(lexeme, "for") != 0)
		return -1;

	if(nextToken == KEYWORD || strcmp(lexeme, "for") == 0){
		printf("Enter <forLoop>\n");
		terminalHelper(KEYWORD, ONE, "for");
		terminalHelper(LEFT_PAREN, ONE);
		nonterminalHelper(assignment, ONE);
		terminalHelper(END_STMT, ONE);
		nonterminalHelper(expression, ONE);
		terminalHelper(END_STMT, ONE);
		nonterminalHelper(expression, ONE);
		terminalHelper(RIGHT_PAREN, ONE);
		nonterminalHelper(block, ONE);

		printf("Exit <forLoop>\n");
		return 0;
	} else{
		return -1;
	}
}

/*print - ebnf rule:
	<printstatement> ::= "printf(" <string> ["," <expression> ] ")" 
*/
int print(){
	if(nextToken != KEYWORD || strcmp(lexeme, "printf") != 0){
		return -1;
	}

	if(nextToken == KEYWORD || strcmp(lexeme, "printf") == 0){
		printf("Enter <print>\n");
		terminalHelper(KEYWORD, ONE, "printf");
		terminalHelper(LEFT_PAREN, ONE);
		terminalHelper(STRING_LIT, ONE);
		if(nextToken == COMMA){
			terminalHelper(COMMA, ONE);
			nonterminalHelper(expression, ONE);

		}
		terminalHelper(RIGHT_PAREN, ONE);

		printf("Exit <print>\n");
		return 0;
	} else{
		return -1;
	}
}

/*assignment - ebnf rule:
	<assignmentStatement> ::= ["int"] <identifier> "=" <expression>
*/
int assignment(){
	if(nextToken == IDENT){
		printf("Enter <assignment>\n");
		terminalHelper(IDENT, ONE);
		terminalHelper(ASSIGN_OP, ONE);
		nonterminalHelper(expression, ONE);
		printf("Exit <assignment>\n");
		return 0;
	} else if(nextToken == KEYWORD && strcmp(lexeme, "int") == 0){
		printf("Enter <assignment>\n");
		terminalHelper(KEYWORD, ONE, "int");
		terminalHelper(IDENT, ONE);
		terminalHelper(ASSIGN_OP, ONE);
		nonterminalHelper(expression, ONE);
		printf("Exit <assignment>\n");
		return 0;

	} else {
		return -1;
	}
}

/*expression - ebnf rule:
	<expression> ::= <identifier> 
		| <number> [++] 
		| "{" { <expressionHelper> } "}" 
		| "sizeof(" <identifier> ")" 
		| <expression> <operators> <expression>
*/
int expression(){
	switch(nextToken){
		case IDENT:
			printf("Enter <expression>\n");
			terminalHelper(IDENT, ONE);
			break;
		case INT_LIT:
			printf("Enter <expression>\n");
			terminalHelper(INT_LIT, ONE);
			break;
		case LEFT_BRACE:
			printf("Enter <expression>\n");
			terminalHelper(LEFT_BRACE, ONE);
			nonterminalHelper(expressionHelper, MINZERO);
			terminalHelper(RIGHT_BRACE, ONE);
			break;
		case KEYWORD:
			if(strcmp(lexeme, "sizeof") == 0){
				terminalHelper(KEYWORD, ONE, "sizeof");	
				terminalHelper(LEFT_PAREN, ONE);
				nonterminalHelper(expression, ONE);
				terminalHelper(RIGHT_PAREN, ONE);
				break;
			}
		default:
			return -1;
	}
	if(nextToken == ADD_OP){
		terminalHelper(ADD_OP, MAXONE);
		if(nextToken == ADD_OP){
			terminalHelper(ADD_OP, MAXONE);
			printf("Exit <expression>\n");
			return 0;
		} else {
			nonterminalHelper(expression, ONE);
		}
	} else if(nextToken >= 20 && nextToken <= 26){
		terminalHelper(nextToken, ONE);
		nonterminalHelper(expression, ONE);

	}

	printf("Exit <expression>\n");
	return 0;
}

/*expressionHelper - ebnf rule:
	<expressionHelper> ::= <number> | """",
	*/
int expressionHelper(){
	if(nextToken == INT_LIT){
		printf("Enter <expressionHelper>\n");
		terminalHelper(INT_LIT, ONE);
		printf("Exit <expressionHelper>\n");
		return 0;
	} else if(nextToken == COMMA){
		printf("Exit <expressionHelper>\n");
		terminalHelper(COMMA, ONE);
		printf("Exit <expressionHelper>\n");
		return 0;
	} else{
		return -1;
	}
}

/*preprocessor - ebnf rule: 
	<preprocessor> ::= "#include <" <string> ">"
*/
int preprocessor(){ //use __func__ ? just to be fancy lol
	if(nextToken != PREPROCESSOR)
		return -1;

	printf("Enter <preprocessor>\n");

	printf("Exit <preprocessor>\n");
	lex();
	return 0;
}

/* nonterminalHelper - used to handle logic around repetitions for nonterminals*/
void nonterminalHelper(std::function<int()> rule, int reps){
	if(rule() == -1){
		if(reps == ONE || reps == MINONE){
			printf("Exception in %s\n%d is not a valid token for\n","nonterminalHelper", nextToken);
		} else if(reps == MINZERO || reps == MAXONE){
			return;
		}
	} else{
		if(reps == ONE || reps == MAXONE){
			return;
		} else if(reps == MINZERO || reps == MINONE){
			//A+ == AA*, so reps will always be A*
			nonterminalHelper(rule, MINZERO);
		}
	}
}

/* terminalHelper - used to handle logic around repetitions for terminals*/

void terminalHelper(int tokenCode, int reps){
	if(nextToken != tokenCode){
		if(reps == ONE || reps == MINONE){
			printf("ERROR IN %s\nExpected Token:%d\nActualToken%d\n",__func__, tokenCode, nextToken);
		} else if(reps == MINZERO || reps == MAXONE){
			return;
		}
	}
	if(nextToken == tokenCode){
		lex();
		if(reps == ONE || reps == MAXONE){
			return;
		} else if(reps == MINZERO || reps == MINONE){
			terminalHelper(tokenCode, MINZERO);
		}
	}
}

/* terminalHelper - used to handle logic around repetitions for terminals with
	ambigous tokens (mainly KEYWORD)*/
void terminalHelper(int tokenCode, int reps, const char keyword[]){
	if(nextToken != tokenCode || strcmp(lexeme, keyword) != 0){
		if(reps == ONE || reps == MINONE){
			printf("ERROR IN %s\nExpected Token:%d\nActualToken%d\n",__func__, tokenCode, nextToken);
		} else if(reps == MINZERO || reps == MAXONE){
			return;
		}
	}
	if(nextToken == tokenCode && strcmp(lexeme, keyword) == 0){
		lex();
		if(reps == ONE || reps == MAXONE){
			return;
		} else if(reps == MINZERO || reps == MINONE){
			terminalHelper(tokenCode, MINZERO, keyword);
		}
	}
}



/* getChar - a function to get the next character of
input and determine its character class */
void getChar() {
	if (fileIn.get(nextChar)) {
		if (isalpha(nextChar))
			charClass = LETTER;
		else if (isdigit(nextChar))
			charClass = DIGIT;
		else if (nextChar == '"')
			charClass = STRING_OPEN;
		else charClass = UNKNOWN;
	}
	else
		charClass = EOF;
}

/* lex - a simple lexical analyzer for arithmetic
expressions */
int lex() {
	lexLen = 0;
	getNonBlank();
	switch (charClass) {
		/* Parse identifiers */
	case LETTER:
		addChar();
		getChar();
		while (charClass == LETTER || charClass == DIGIT || nextChar == '[' || nextChar == ']') {
			addChar();
			getChar();
		}
		// MAKE DETERMINATION IF KEYWORD IDENT
		nextToken = (isKeyword(lexeme) ? KEYWORD : IDENT);
		break;
		/* Parse integer literals */
	case DIGIT:
		addChar();
		getChar();
		while (charClass == DIGIT) {
			addChar();
			getChar();
		}
		nextToken = INT_LIT;
		break;
	case STRING_OPEN:
		do {
			addChar();
			getChar();
		} while (nextChar != '"');
		addChar();
		getChar();
		nextToken = STRING_LIT;
		break;
		/* Parentheses and operators */
	case UNKNOWN:
		lookup(nextChar);
		getChar();
		break;
		/* EOF */
	case EOF:
		nextToken = EOF;
		lexeme[0] = 'E';
		lexeme[1] = 'O';
		lexeme[2] = 'F';
		lexeme[3] = 0;
		break;
	} /* End of switch */

	// special case for comments (since they conflict with division symbols)
	if (nextToken == DIV_OP && nextChar == '/') {
		do {
			addChar();
			getChar();
		} while (nextChar != '\n');
		nextToken = COMMENT;
	}
	// special case for preprocessor
	else if (nextToken == PREPROCESSOR) {
		do {
			addChar();
			getChar();		//is preprocessor supposed to be in the bnf?
		} while (nextChar != '\n');
	}

	printf("Next token is: %d, Next lexeme is %s\n",
		nextToken, lexeme);
	return nextToken;
} /* End of function lex */

/* lookup - a function to lookup operators and parentheses
and return the token */
int lookup(char ch) {
	switch (ch) {
	case '(':
		addChar();
		nextToken = LEFT_PAREN;
		break;
	case ')':
		addChar();
		nextToken = RIGHT_PAREN;
		break;
	case '[':
		addChar();
		nextToken = LEFT_BRACKET;
		break;
	case ']':
		addChar();
		nextToken = RIGHT_BRACKET;
		break;
	case '{':
		addChar();
		nextToken = LEFT_BRACE;
		break;
	case '}':
		addChar();
		nextToken = RIGHT_BRACE;
		break;
	case '=':
		addChar();
		nextToken = ASSIGN_OP;
		break;
	case '+':
		addChar();
		nextToken = ADD_OP;
		break;
	case '-':
		addChar();
		nextToken = SUB_OP;
		break;
	case '*':
		addChar();
		nextToken = MULT_OP;
		break;
	case '/':
		addChar();
		nextToken = DIV_OP;
		break;
	case '<':
		addChar();
		nextToken = LT_OP;
		break;
	case '>':
		addChar();
		nextToken = GT_OP;
		break;
	case ',':
		addChar();
		nextToken = COMMA;
		break;
	case ';':
		addChar();
		nextToken = END_STMT;
		break;
	case '#':
		addChar();
		nextToken = PREPROCESSOR;
		break;
	case '"':
		addChar();
		nextToken = STRING_LIT;
		break;
	default:                    // UNKNOWN CHARS ARE CONSIDERED END OF FILE
		addChar();
		nextToken = EOF;
		break;
	}
	return nextToken;
}

/* checks if given lexeme is a keyword */
bool isKeyword(char lexeme[]) {
	for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i) {
		if (strcmp(lexeme, keywords[i]) == 0)
			return true;
	}

	return false;
}

/* addChar - a function to add nextChar to lexeme */
void addChar() {
	if (lexLen <= 98) {
		lexeme[lexLen++] = nextChar;
		lexeme[lexLen] = 0;
	}
	else
		printf("Error - lexeme is too long \n");
}

/* getNonBlank - a function to call getChar until it
returns a non-whitespace character */
void getNonBlank() {
	for (int i = 0; isspace(nextChar) && i < 1000; ++i)
		getChar();
}
