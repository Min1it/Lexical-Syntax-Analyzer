# Lexical-Syntax-Analyzer

Basic Lexical and Syntax Analyzer for the following program:

![image](https://github.com/user-attachments/assets/ec241fcb-b288-473a-adcc-a3b2335d24cb)


EBNF Rules:
<program> ::= [<preprocessor>]  "int main()" {<block>}

<preprocessor> ::= "#include <" <string> ">"

<statement> ::= assignmentStatement [";"]
              | forStatement 
              | printStatement [";"]
	      | returnStatement [";"]
	      | ifStatement
              | comment

<ifStatement> ::= "if(" <expression> ")" <block>

<assignmentStatement> ::= ["int"] <identifier> "=" <expression>

<expression> ::= <identifier> 
| <number> [++] 
| "{" { <expressionHelper> } "}" 
| "sizeof(" <identifier> ")" 
| <expression> <operators> <expression>

<expressionHelper> ::= <number> | ,

<printstatement> ::= "printf(" <string> ["," <expression> ] ")" 

<forStatement> ::= "for(" <assignmentStatement> ";"  <expression> ";" <expression> ")" <block>

<returnStatement> ::= "return" <expression>

<identifier> ::= <letter> {<letter>|<digit>} ["[]"]


<operators> ::= = | + | - | * | / | < | >

<string> ::= {<char>}-

<number> ::= {<digit>}-

<char> ::= <letter> | <digit>

<letter> ::= "a"..."z" | "A"..."Z"

<digit> ::= "0"..."9"






