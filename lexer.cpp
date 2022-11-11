#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <string.h>
using namespace std;

string keywords[] = {"auto", "break", "case", "char", "const", "continue", "default",
                     "do", "double", "else", "enum", "extern", "float", "for", "goto",
                     "if", "int", "long", "register", "return", "short", "signed",
                     "sizeof", "static", "struct", "switch", "typedef", "union",
                     "unsigned", "void", "volatile", "while"};

char operators[] = {'+', '-', '*', '/', '%', '=', '<', '>'};

char seperators[] = {'(', ')', '{', '}', '[', ']', ';', ',', '\"', '\''};

bool isKeyword(string identifier)
{
    int length = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < length; i++)
    {
        if (identifier.compare(keywords[i]) == 0)
            return true;
    }
    return false;
}

bool isOperator(char c)
{
    int length = sizeof(operators) / sizeof(operators[0]);
    for (int i = 0; i < length; i++)
    {
        if (c == operators[i])
        {
            return true;
        }
    }
    return false;
}

bool isSeperator(char c)
{
    int length = sizeof(seperators) / sizeof(seperators[0]);
    for (int i = 0; i < length; i++)
    {
        if (c == seperators[i])
        {
            return true;
        }
    }
    return false;
}

/* Diagram of state table
                                Input
State   letter  digit   operators   seperators   whitespace  period
1          2      4         6           8            1          8
2          2      2         3           3            3          3
3  A       1      1         1           1            1          1
4          5      4         5           5            5          4
5  A       1      1         1           1            1          1
6          7      7         6           7            7          7
7  A       1      1         1           1            1          1
8  A       1      1         1           1            1          1

States
1. Starting state
2. In identifier
3. End of identifier
4. In integer
5. End of number (integer or double)
6. In operator
7. End of operator
8. Seperator
*/

int stateTable[8][6] = {
    2, 4, 6, 8, 1, 8,
    2, 2, 3, 3, 3, 3,
    1, 1, 1, 1, 1, 1,
    5, 4, 5, 5, 5, 4,
    1, 1, 1, 1, 1, 1,
    7, 7, 6, 7, 7, 7,
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1};

int main()
{
    ifstream inputFile;
    inputFile.open("input_scode.txt");
    ofstream outputFile;
    outputFile.open("output.txt");

    char c;                                   // current char
    string token;                             // string to hold token
    int state = 0;                            // current state set to initial state by defualt
    int col;                                  // column in state table

    while ((inputFile.get(c)) && c != EOF)
    {
        // Determine column in state table
        if (isalpha(c))
        {
            col = 0;
        }
        else if (isdigit(c))
        {
            col = 1;
        }
        else if (isOperator(c))
        {
            col = 2;
        }
        else if (isSeperator(c))
        {
            col = 3;
        }
        else if (c == ' ' || c == '\n')
        {
            col = 4;
        }
        else if (c == '.')
        {
            col = 5;
        }
        //Determine new state from input and current state
        state = stateTable[state][col] - 1;

        //To reach states 2 4 or 6 requires lexer to read 1 character past             the lexeme
        //So if states 2 4 or 6 is reached the last input read should not be added to token, white space should be ignored as well
        if(c != ' ' && state!= 2 && state != 4 && state!=6)
            token += c;

        // Switch case for states that are accepting
        //If the state is accepting, clear the token and set state back to starting state of 0
      //Also shift file pointer back for accepting states that read past the token
        switch (state)
        {
        // Accepting state for identifier
        case 2:
            // Check if identifier is a keyword
            if (isKeyword(token))
            {
                outputFile << setw(15) << left << "Keyword" << setw(15) << left << token << endl;
                state = 0;
                token.clear();
            }
            else
            {
                outputFile << setw(15) << left << "Identifier" << setw(15) << left << token << endl;
                state = 0;
                token.clear();
            }
            inputFile.seekg(-1, ios::cur);
            break;
        // Accepting state for number
        case 4:
            outputFile << setw(15) << left << "Integer" << setw(15) << left << token << endl;
            state = 0;
            token.clear();
            inputFile.seekg(-1, ios::cur);
            break;
        // Accepting state for operator
        case 6:
            outputFile << setw(15) << left << "Operator" << setw(15) << left << token << endl;
            state = 0;
            token.clear();
            inputFile.seekg(-1, ios::cur);
            break;
        // Accepting state for seperator
        case 7:
            outputFile << setw(15) << left << "Separator" << setw(15) << left << token << endl;
            state = 0;
            token.clear();
            break;
        default:
            break;
        }
    }

    // close files
    inputFile.close();
    outputFile.close();
    return 0;
}