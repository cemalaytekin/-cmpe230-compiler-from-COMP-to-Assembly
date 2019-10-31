//============================================================================
// Name        : Assignment 1, CMPE230
// Author      : Berke Esmer & Cemal Aytekin
// Description : In this project, we implement a simple compiler called COMP 
//               that generates A86 code for a sequence of expressions and 
//               assignment statements that involve +,* and power operations.
// Run commands: g++ main.cpp -o output         // Creates the comp
//               ./output inputfile  // Arguments to run
//============================================================================

#include <iostream>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <string>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <fstream>
using namespace std;

/* Initializing the primitives and structures that will be used throughout the program.
 * queue <string> postfixQue    : It holds all the terms used in finding the postfix notation.
 * map <string, string> varsVal : It holds all the variables as keys and their representations in Assembly as values.
 * set <string> vars            : It holds all the variables existing in the program.
 * int errorCounter             : It counts the lines in Assembly code to print ERROR when found.
 * int forloopCounter           : It counts the forloop labels used in Assembly to print hexadecimal values
 * int printCounter             : It counts the print labels used in Assembly to print a result.
 * int counterCarry             : It counts the carry labels used in Assembly in addition process.
 * int undfVarCounter           : It counts the amount of similar variables in the program. E.g. 'abc', 'ABC', 'AbC'...
 */

queue <string> postfixQueue;
map <string, string> varsVal;
set <string> vars;
int errorCounter = 1;
int forloopCounter = 1;
int printCounter = 1;
int counterCarry = 1;
int undfVarCounter = 1;

/* Initializing the functions that will be used throughout the program.
 * void initializeVariables : params: none          --> It reads the whole input and defines the all variables used in the program into Assembly.
 * void printResult         : params: string var    --> It takes the given variable to print and determines whether it is an expression or just a variable.
 * void printHelper         : params: none          --> It assists the printResult method and adds the print codes into Assembly.
 * void calculator          : params: none          --> It does the arithmetic operation that is given in input and transforms its code into Assembly.
 * bool hasError            : params: string line   --> It reads the whole input and detects the ERROR if exists.
 * string expr              : params: string s      --> It takes the 'right-hand-side' of an input line and returns the remainder after processed.
 * string term              : params: string s      --> It takes the term to calculate and returns the remainder after processed.
 * string moreTerm          : params: string s      --> It takes the more terms part of an expression to calculate and returns the remainder after processed.
 * string factor            : params: string s      --> It takes the factor to calculate and returns the remainder after processed.
 * string moreFactor        : params: string s      --> It takes the more factor part of an expression to calculate and returns the remainder after processed.
 */

void initializeVariables();
void printResult(string var);
void printHelper();
void calculator();
bool hasError(string line);
string expr(string s);
string term(string s);
string moreTerm(string s);
string factor(string s);
string moreFactor(string s);

// Initializing the output variable here...
ofstream output("");

int main(int argc, char* argv[]) {

    /* Initializing the file i/o variables.
     * ifstream infile      : params: argv[1]   --> It takes the input path from the terminal and reads it.
     * ifstream infile2     : params: argv[1]   --> It takes the input path from the terminal and again reads it.
     * ofstream output      : params: argv[2]   --> It takes the output path from the terminal and creates the file.
     * string line          : It is used to process lines in input in every iteration of the program.
     */

    ifstream infile(argv[1]);
    ifstream infile2(argv[1]);
    output.open((string(argv[1]).substr(0, string(argv[1]).length()-2)+"asm").c_str());
    string line;

    // The loop reads the input, detects error and defines all the variables..
    while(getline(infile,line)) {

        if (hasError(line)) {
            output << "Syntax Error:  " << errorCounter << endl;
            cout << "ASM file has been generated..." << endl;
            return 0;
        }

        errorCounter++;
        string var = "";

        // The loop iterates the line and defines the variable regarding of its uppercase/lowercase existence.
        for(int i=0; i<line.length(); i++) {
            if(line.at(i) == '=') {
                string tempVar = "";

                // The loop ignores the space characters in the left-hand-side. 
                for(int j=0; j<var.length(); j++) {
                    if(var.at(j) != ' ')
                        tempVar = tempVar + var.at(j);
                }

                string value = tempVar;
                string varUpp = tempVar;

                transform(varUpp.begin(), varUpp.end(), varUpp.begin(), ::toupper);
                set<string>::iterator it;

                // The loop iterates the variables in the set.
                for(it = vars.begin(); it != vars.end(); it++) {
                    string s = *it;
                    string str = s;
                    transform(str.begin(), str.end(),str.begin(), ::toupper);

                    // If the variable is already defined as uppercase/lowercase form, it changes the variable name avoiding duplicity.
                    if(str == varUpp){
                        stringstream ss;
                        ss << undfVarCounter;
                        string str = ss.str();
                        value = "" + value + str + "_Duple";
                        undfVarCounter++;
                        break;
                    }
                }

                varsVal[tempVar]=value;
                vars.insert(tempVar);
                break;      
            }
            else {
                var = var + line.at(i);
            }
        }            
    }

    // Calls to function to initialize all existing variables into Assembly.
    initializeVariables();

    // The loop reads the input for the second time and finds the LHS & RHS of an expression. It does all the work calling necessary functions.
    while(getline(infile2,line)) {

        if(line.length() == 0)
            continue;

        string LHS = "";
        string RHS = "";
        bool hasEq = false;

        // The loop seperates the sides of an assignment.
        for(int i=0; i<line.length(); i++){
            if(line.at(i)=='='){
                hasEq = true;
                RHS = line.substr(i+1);
                break;
            }
            else{
                LHS = LHS + line.at(i);
            }
        }       

        string tempLHS = "";
        string tempRHS = "";

        // The following two for loops ignores the space characters to avoid problems.
        for(int i=0; i<LHS.length(); i++){
            if(LHS.at(i)!=' ')
                tempLHS = tempLHS + LHS.at(i);
        }

        for(int i=0; i<RHS.length(); i++){
            if(RHS.at(i)!=' ')
                tempRHS = tempRHS + RHS.at(i);
        }

        // It follows to process print operation unless there is no right-hand-side in the read line.
        if(!hasEq)
            printResult(tempLHS);
        else {
            output << "   MOV BX, OFFSET " << varsVal[tempLHS] << "_low" << endl;
            output << "   PUSH BX" << endl;
            output << "   MOV BX, OFFSET " << varsVal[tempLHS] << "_high" << endl;
            output << "   PUSH BX" << endl;

            // It calls the expression function to transform the line into postfix notation and then calculates the result.
            expr(tempRHS);
            calculator();

            output << "   POP AX" << endl;
            output << "   POP CX" << endl;
            output << "   POP BP" << endl;
            output << "   MOV [BP], AX" << endl;
            output << "   POP BP" << endl;
            output << "   MOV [BP], CX" << endl;
        }
    }

    // Terminating the program...
    output << "   INT 20h";
    cout << "ASM file has been generated..." << endl;
    return 0;
}

void initializeVariables() {
    output << "JMP START" << endl;
    output << endl;

    map<string, string>::iterator it = varsVal.begin();

    // The loop iterates the map and defines them in the assembly code.
    while (it != varsVal.end())
    {
        string var = it->second;
        output <<"   "<< var << "_low" << " DW 0h"<< endl;
        output <<"   "<< var << "_high" << " DW 0h"<< endl;
        it++;
    }

    // The following variables are defined by the computer, not user.
    // They are used in several arithmetic calculations and therefore are important!
    output << "   loopvar DW 0h" << endl;
    output << "   exponentLow DW 0h" << endl;
    output << "   exponentHigh DW 0h" << endl;
    output << "   baseLow DW 0h" << endl;
    output << "   baseHigh DW 0h" << endl;
    output << "   powResultLow DW 0h" << endl;
    output << "   powResultHigh DW 0h" << endl;
    output << "   tempFirstLow DW 0h" << endl;
    output << "   tempFirstHigh DW 0h" << endl;
    output << "   tempSecondLow DW 0h" << endl;
    output << "   tempSecondHigh DW 0h" << endl;
    output << "   tempMulFirst DW 0h" << endl;
    output << "   tempMulSecond DW 0h" << endl;

    output << endl;
    output << "START:" << endl;
    output << endl;
}

void printResult(string var) {
    bool singlePrint = true;

    // This loop iterates the understand whether the print operation is used for an expresssion or just a variable.
    for(int i=0; i<var.length(); i++) {
        if (var.at(i) == '*' || var.at(i) == '+' || var.at(i) == ',') {
            singlePrint = false;
            break;
        }
    }

    // If it is used for just printing a variable, bool singlePrint is TRUE
    if(singlePrint) {

	if(var.at(0)<='9' && var.at(0)>='0'){
		output << endl;
		output << "   MOV BX, " <<var<< endl;
		printHelper();
		output << "   MOV AH, 02" << endl;
	    	output << "   MOV DL, 0ah" << endl;
	    	output << "   INT 21h" << endl;
		return;	
		
        }
 	bool isNum = true;
            map<string, string>::iterator it = varsVal.begin();

            // The loop iterates to determine whether the next part is a number or not.
            while (it != varsVal.end())
            {
                if(it->first == var){
                    isNum = false;
                    break;
                }
                it++;
            }
	if(isNum){
		if(var.at(0)>'9' || var.at(0)<'0'){
			    var = "0h";
		output << endl;
		output << "   MOV BX, " <<var<< endl;
		printHelper();
		}
	else{
		output << endl;
		output << "   MOV BX, " << varsVal[var] << "_high" << endl;
		printHelper();

		output << endl;
		output << "   MOV BX, " << varsVal[var] << "_low" << endl;
		printHelper();
		}
	}
	else{
output << endl;
		output << "   MOV BX, " << varsVal[var] << "_high" << endl;
		printHelper();

		output << endl;
		output << "   MOV BX, " << varsVal[var] << "_low" << endl;
		printHelper();
	
	}
	
    }
    // If not, and the printing is used for an expression such as "variable_name * 2 + 3", bool singlePrint is FALSE
    else {
        var = expr(var);
        calculator();

        output << endl;
        output << "   POP BX" << endl;
        printHelper();

        output << endl;
        output << "   POP BX" << endl;
        printHelper();
    }

    output << "   MOV AH, 02" << endl;
    output << "   MOV DL, 0ah" << endl;
    output << "   INT 21h" << endl;
}

void printHelper() {
    // The following lines are to print the number in the hexadecimal format.
    output << "   MOV CX, 4h" << endl;
    output << "   MOV AH, 02" << endl;
    output << "PRINTLOOP" << printCounter << ":" << endl;
    output << "   MOV DX, 0fh" << endl;
    output << "   ROL BX, 4h" << endl;
    output << "   AND DX, BX" << endl;
    output << "   CMP DL, 0ah" << endl;
    output << "   JAE HEXDIGIT" << printCounter << endl;
    output << "   ADD DL, '0'" << endl;
    output << "   JMP PRINTOUT" << printCounter << endl;
    output << "HEXDIGIT" << printCounter << ":" << endl;
    output << "   ADD DL, 'A'" << endl;
    output << "   SUB DL, 0ah" << endl;
    output << "PRINTOUT" << printCounter << ":" << endl;
    output << "   INT 21h" << endl;
    output << "   DEC CX" << endl;
    output << "   JNZ PRINTLOOP" << printCounter << endl;

    printCounter++;
}

void calculator() {

    // The loop iterates the postFixNotation and makes the calculation.
    while(!postfixQueue.empty()) {
        string pfq = postfixQueue.front();
        postfixQueue.pop();

        // This produces the assembly code of addition process.
        if(pfq == "+") {
            output << "   POP tempFirstHigh" << endl;
            output << "   POP tempFirstLow" << endl;
            output << "   POP tempSecondHigh" << endl;
            output << "   POP tempSecondLow" << endl;
            output << "   MOV AX, tempFirstLow" << endl;
            output << "   MOV BX, tempSecondLow" << endl;
            output << "   ADD AX, BX" << endl;
            output << "   PUSH AX" << endl;
            output << "   JNC NOTCARRY" << counterCarry << endl;
            output << "   INC tempFirstHigh" << endl;
            output << "NOTCARRY" << counterCarry << ":" << endl;
            output << "   MOV AX, tempFirstHigh" << endl;
            output << "   MOV BX, tempSecondHigh" << endl;
            output << "   ADD AX, BX" << endl;
            output << "   PUSH AX" << endl;

            counterCarry++;
        }
        // This produces the assembly code of multiplication process.
        else if(pfq == "*") {
            output << "   POP tempFirstHigh" << endl;
            output << "   POP tempFirstLow" << endl;
            output << "   POP tempSecondHigh" << endl;
            output << "   POP tempSecondLow" << endl;
            output << "   MOV AX, tempFirstHigh" << endl;
            output << "   MOV CX, tempSecondLow" << endl;
            output << "   MUL CX" << endl;
            output << "   MOV tempMulFirst, AX" << endl;
            output << "   MOV AX, tempFirstLow" << endl;
            output << "   MOV CX, tempSecondHigh" << endl;
            output << "   MUL CX" << endl;
            output << "   ADD AX, tempMulFirst" << endl;
            output << "   MOV tempMulFirst, AX" << endl;
            output << "   MOV AX, tempFirstLow" << endl;
            output << "   MOV CX, tempSecondLow" << endl;
            output << "   MUL CX" << endl;
            output << "   MOV tempMulSecond, AX" << endl;
            output << "   MOV AX, tempMulFirst" << endl;
            output << "   ADD AX, DX" << endl;
            output << "   PUSH tempMulSecond" << endl;
            output << "   PUSH AX" << endl;
        }
        // This produce the assembly code of pow operation process.
        else if(pfq == "pow") {
            output << "   POP exponentHigh" << endl;
            output << "   POP exponentLow" << endl;
            output << "   POP baseHigh" << endl;
            output << "   POP baseLow" << endl;
            output << "   MOV powResultHigh, 0000h" << endl;
            output << "   MOV powResultLow, 0001h" << endl;
            output << "FORLOOP" << forloopCounter << ":" << endl;
            output << "   MOV BX, exponentLow" << endl;
            output << "   AND BX, 1" << endl;
            output << "   CMP BL, 1" << endl;
            output << "   JNZ NESTEDLOOP" << forloopCounter << endl;
            output << "   MOV AX, powResultHigh" << endl;
            output << "   MOV CX, baseLow" << endl;
            output << "   MUL CX" << endl;
            output << "   MOV tempMulFirst, AX" << endl;
            output << "   MOV AX, powResultLow" << endl;
            output << "   MOV CX, baseHigh" << endl;
            output << "   MUL CX" << endl;
            output << "   ADD AX, tempMulFirst" << endl;
            output << "   MOV tempMulFirst, AX" << endl;
            output << "   MOV AX, powResultLow" << endl;
            output << "   MOV CX, baseLow" << endl;
            output << "   MUL CX" << endl;
            output << "   MOV tempMulSecond, AX" << endl;
            output << "   MOV AX, tempMulFirst" << endl;
            output << "   ADD AX, DX" << endl;
            output << "   MOV powResultHigh, AX" << endl;
            output << "   MOV powResultLow, tempMulSecond" << endl;
            output << "NESTEDLOOP" << forloopCounter << ":" << endl;
            output << "   SHR exponentLow, 1" << endl;
            output << "   CMP exponentLow, 0" << endl;
            output << "   JZ EXITFORLOOP" << forloopCounter << endl;
            output << "   MOV AX, baseHigh" << endl;
            output << "   MOV CX, baseLow" << endl;
            output << "   MUL CX" << endl;
            output << "   MOV tempMulFirst, AX" << endl;
            output << "   MOV AX, baseLow" << endl;
            output << "   MOV CX, baseHigh" << endl;
            output << "   MUL CX" << endl;
            output << "   ADD AX, tempMulFirst" << endl;
            output << "   MOV tempMulFirst, AX" << endl;
            output << "   MOV AX, baseLow" << endl;
            output << "   MOV CX, baseLow" << endl;
            output << "   MUL CX" << endl;
            output << "   MOV tempMulSecond, AX" << endl;
            output << "   MOV AX, tempMulFirst" << endl;
            output << "   ADD AX, DX" << endl;
            output << "   MOV baseHigh, AX" << endl;
            output << "   MOV baseLow, tempMulSecond" << endl;
            output << "   JMP FORLOOP"<<forloopCounter<<endl;
            output << "EXITFORLOOP" << forloopCounter << ":" << endl;

            forloopCounter++;

            output << "FORLOOP" << forloopCounter << ":" << endl;
            output << "   MOV BX, exponentHigh" << endl;
            output << "   AND BX, 1" << endl;
            output << "   CMP BL, 1" << endl;
            output << "   JNZ NESTEDLOOP" << forloopCounter << endl;
            output << "   MOV AX, powResultHigh" << endl;
            output << "   MOV CX, baseLow" << endl;
            output << "   MUL CX" << endl;
            output << "   MOV tempMulFirst, AX" << endl;
            output << "   MOV AX, powResultLow" << endl;
            output << "   MOV CX, baseHigh" << endl;
            output << "   MUL CX" << endl;
            output << "   ADD AX, tempMulFirst" << endl;
            output << "   MOV tempMulFirst, AX" << endl;
            output << "   MOV AX, powResultLow" << endl;
            output << "   MOV CX, baseLow" << endl;
            output << "   MUL CX" << endl;
            output << "   MOV tempMulSecond, AX" << endl;
            output << "   MOV AX, tempMulFirst" << endl;
            output << "   ADD AX, DX" << endl;
            output << "   MOV powResultHigh, AX" << endl;
            output << "   MOV powResultLow, tempMulSecond" << endl;
            output << "NESTEDLOOP" << forloopCounter << ":" << endl;
            output << "   SHR exponentHigh, 1" << endl;
            output << "   CMP exponentHigh, 0" << endl;
            output << "   JZ EXITFORLOOP" << forloopCounter << endl;
            output << "   MOV AX, baseHigh" << endl;
            output << "   MOV CX, baseLow" << endl;
            output << "   MUL CX" << endl;
            output << "   MOV tempMulFirst, AX" << endl;
            output << "   MOV AX, baseLow" << endl;
            output << "   MOV CX, baseHigh" << endl;
            output << "   MUL CX" << endl;
            output << "   ADD AX, tempMulFirst" << endl;
            output << "   MOV tempMulFirst, AX" << endl;
            output << "   MOV AX, baseLow" << endl;
            output << "   MOV CX, baseLow" << endl;
            output << "   MUL CX" << endl;
            output << "   MOV tempMulSecond, AX" << endl;
            output << "   MOV AX, tempMulFirst" << endl;
            output << "   ADD AX, DX" << endl;
            output << "   MOV baseHigh, AX" << endl;
            output << "   MOV baseLow, tempMulSecond" << endl;
            output << "   JMP FORLOOP"<<forloopCounter<<endl;
            output << "EXITFORLOOP" << forloopCounter << ":" << endl;

            forloopCounter++;

            output << "   PUSH powResultLow" << endl;
            output << "   PUSH powResultHigh" << endl;
        }

        else {
            bool isNum = true;
            map<string, string>::iterator it = varsVal.begin();

            // The loop iterates to determine whether the next part is a number or not.
            while (it != varsVal.end())
            {
                if(it->first == pfq){
                    output << "   PUSH " << it->second << "_low" << endl;
                    output << "   PUSH " << it->second << "_high" << endl;
                    isNum = false;
                    break;
                }
                it++;
            }
            // If the right hand side is just a number, it transforms the number into the assembly code.
            if(isNum) {
	    	if(pfq.at(0)>'9' || pfq.at(0)<'0'){
		    pfq = "0";
            	}
                string high = "";
                string low = "";
                int resultLength = pfq.length();

                if (resultLength <= 4) {
                    low = pfq;
                    for (int k = resultLength; k < 4; k++) {
                        low = "0" + low;
                    }
                    high = "0000";
                }
                else {
                    for (int k = 0; k < 8 - resultLength; k++) {
                        pfq = "0" + pfq;
                    }
                    high = pfq.substr(0, 4);
                    low = pfq.substr(4);
                }

                output << "   PUSH 0" << low << "h" << endl;
                output << "   PUSH 0" << high << "h" << endl;
            }
        }
    }
}

bool hasError(string line) {
    // There is no necessary to check blank lines.
    if(line.length() == 0)
        return false;

    stack<char> s;

    // The loop determines whether the paranthesis are balanced.
    for(int i=0; i<line.length(); i++) {
        if(line.at(i)=='(') {
            s.push('(');
        }
        else if(line.at(i)==')') {
            if(s.size()==0)
                return true;
            s.pop();
        }
    }

    // If paranthesis are not balanced, returns ERROR
    if(s.size() != 0)
        return true;

    string LHS = "";
    string RHS = "";
    bool hasEq = false;

    // The loop determines the both sides of an operation.
    for(int i=0; i<line.length(); i++){
        if(line.at(i)=='='){
            hasEq = true;
            RHS = line.substr(i+1);
            break;
        }
        else{
            LHS = LHS + line.at(i);
        }
    }      

    int m = 0;
    int finalIndex = 0;

    // The loop determines the end of a line and check if it ends with an arithmetic operator.
    for(int m = 0; m < line.length(); m++){
        if(line.at(m)=='*' || line.at(m)=='+')
            finalIndex = m;
    }

    bool hasNothing = true;
    if(finalIndex!=0){
        for(int i = finalIndex+1; i<line.length(); i++){
            if(line.at(i)==' ')
                continue;
            else{
                hasNothing = false;
            }
        }
        if(hasNothing) { 
            return true;    //if ends with an operator
        }
    }

    // Determining error of two LHS variables. E.g. a b c = 3
    if(hasEq) {
        int k=0;

        while(LHS.at(k)==' '){
            k++;
            if(LHS.length()==k)
            	return true;
        }

        while(LHS.at(k)!=' '){
            k++;
            if(LHS.length()==k)
            	break;
        }

        for(int j = k; j<LHS.length(); j++){
            if(LHS.at(j)!=' ')
                return true;
        }
    }    

    // Determining error of consequent arithmetic operators. E.g. ++ , *+++
    int i = 0;

    while(line.at(i)!='+' && line.at(i)!='*'){
        i++;
        if(line.length() == i)
            break;
    }  

    for(int j=i+1; j<line.length(); j++){
        if(line.at(j)==' ')
            continue;
        else if(line.at(j)=='+' || line.at(j)=='*')
            return true;
        else
            break;
    }
    return false;
}

string expr(string s) {
    // Split the expression into two parts, term and moreTerm.
    s = term(s);
    s = moreTerm(s);

    return s;
}

string term(string s) {
    // Split the term into two parts, factor and moreFactor.
    s = factor(s);
    s = moreFactor(s);

    return s;
}


string moreTerm(string s) {
    // If there is no term, returns blank. Else processes the term and returns the remainder.
    if(s == "")
        return s;
    
    if (s.at(0) == '+') {
        s = s.substr(1);
        s = term(s);
        postfixQueue.push("+");
        s = moreTerm(s);

        return s;
    }
    else
        return s;
}

string moreFactor(string s) {
    // If there is no factor, returns blank. Else processes the factor and returns the remainder.
    if(s=="")
        return s;
    if(s.at(0) == '*'){
        s = s.substr(1);
        s = factor(s);
        postfixQueue.push("*");
        s = moreFactor(s);
        return s;
    }
    else{
        return s;
    }
}

string factor(string s) {
    // Processes the factor, calls the necessary functions to do arithmetic operations.
    int count = 0;
    string temp = "";

    // Realizing the expression contains paranthesis..
    if(s.at(0)=='(') {
        count++;

        for(int i=1; i<s.length(); i++) {
            if (s.at(i)==')' && count == 1) {
                temp = s.substr(1, i-1);
                expr(temp);

                if(s.length() != i)
                    s = s.substr(i+1);
                else
                    s = "";

                return s;
            }
            else if(s.at(i)=='(')
                count++;
            else if(s.at(i)==')') {
                count--;
            }
        }
    }
    else {
        string temp = "";
        for (int i = 0; i < s.length(); i++) {
            if (s.at(i) == '+' || s.at(i) == '*') {
                postfixQueue.push(temp);
                s = s.substr(i);

                return s;
            }
            else {
                temp = temp + s.at(i);
                if(temp == "pow") {
                    string exp1 = "";
                    string exp2 = "";
                    int j = i+2;
                    int countComma = 0;

                    while(s.at(j) != ',' || countComma != 0) {
                        exp1 += s.at(j);
                        if(exp1.length()>=3) {
                            if(exp1.substr(exp1.length()-3) == "pow")
                                countComma++;
                        }
                        if(s.at(j) == ',')
                            countComma--;
                        j++;
                    }

                    int count2 = 0;

                    j++;

                    for(; j<s.length(); j++) {
                        if(s.at(j)=='(')
                            count2++;
                        else if (s.at(j) == ')') {
                            if(count2==0) {
                                exp1 = expr(exp1);
                                exp2 = expr(exp2);
                                break;
                            }

                            count2--;
                        }
                        exp2 += s.at(j);
                    }

                    postfixQueue.push("pow");

                    if(s.length() == j)
                        return "";

                    s = s.substr(j+1);
                    return s;
                }
            }
        }

        postfixQueue.push(temp);
        s = "";

        return s;
    }
}

// END OF PROGRAM.
