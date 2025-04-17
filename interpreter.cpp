#include <iostream>
#include <map>
#include <string>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include "mikroC.h"

// Define the global root node for the syntax tree
Uzel *Koren = nullptr;

// Declare external variables for line and column tracking
extern unsigned Radek;
extern unsigned Sloupec;

// Global symbol table for storing variables
std::map<std::string, int> symbolTable;

// Generate a new AST node
Uzel* GenUzel(int typ, Uzel* prvni, Uzel* druhy, Uzel* treti, Uzel* ctvrty) {
    Uzel* node = new Uzel;
    node->Typ = typ;
    node->z.z.prvni = prvni;
    node->z.z.druhy = druhy;
    node->z.z.treti = treti;
    node->z.z.ctvrty = ctvrty;
    return node;
}

// Generate a node representing a number
Uzel* GenCislo(int value) {
    Uzel* node = new Uzel;
    node->Typ = CISLO;
    node->z.Cislo = value;
    return node;
}

// Generate a node representing a string
Uzel* GenRetez(const char* value) {
    Uzel* node = new Uzel;
    node->Typ = RETEZ;
    node->z.Retez = strdup(value); // Duplicate the string
    return node;
}

// Generate a node representing a variable
Uzel* GenPromen(const char* name) {
    Uzel* node = new Uzel;
    node->Typ = PROMENNA;
    node->z.Retez = strdup(name); // Duplicate the string
    return node;
}

// Implementation of yyerror for Bison
void yyerror(const char* message) {
    std::cerr << "Syntax error: " << message << " at line " << Radek << ", column " << Sloupec << std::endl;
}

// Error reporting function
void Chyba(const char* message, Pozice position) {
    switch (position) {
        case RADEK:
            std::cerr << "Error on line: " << Radek << " - " << message << std::endl;
            break;
        case SLOUPEC:
            std::cerr << "Error on line: " << Radek << ", column: " << Sloupec << " - " << message << std::endl;
            break;
        default:
            std::cerr << "Error: " << message << std::endl;
            break;
    }
    std::exit(EXIT_FAILURE);
}

// Function to evaluate the Abstract Syntax Tree (AST)
int evaluateAST(Uzel *node) {
    if (!node) return 0;

    switch (node->Typ) {
        case CISLO: // Integer constant
            return node->z.Cislo;

        case '+': // Addition
            return evaluateAST(node->z.z.prvni) + evaluateAST(node->z.z.druhy);

        case '-': // Subtraction
            return evaluateAST(node->z.z.prvni) - evaluateAST(node->z.z.druhy);

        case '*': // Multiplication
            return evaluateAST(node->z.z.prvni) * evaluateAST(node->z.z.druhy);

        case '/': { // Division
            int divisor = evaluateAST(node->z.z.druhy);
            if (divisor == 0) {
                Chyba("Division by zero");
                return 0;
            }
            return evaluateAST(node->z.z.prvni) / divisor;
        }

        case PROMENNA: { // Variable lookup
            std::string varName(node->z.Retez);
            if (symbolTable.find(varName) == symbolTable.end()) {
                Chyba("Undefined variable");
                return 0;
            }
            return symbolTable[varName];
        }

        case '=': { // Variable assignment
            std::string varName(node->z.z.prvni->z.Retez);
            int value = evaluateAST(node->z.z.druhy);
            symbolTable[varName] = value;
            return value;
        }

        case PRINT: {
            int result = evaluateAST(node->z.z.prvni);
            std::cout << result << std::endl; // Debugging print
            return 0;
        }

        case SCAN: { // Scan statement
            std::string varName(node->z.Retez);
            int value;
            std::cin >> value;
            symbolTable[varName] = value;
            return value;
        }

        case IF: { // If statement
            if (evaluateAST(node->z.z.prvni)) { // Condition
                evaluateAST(node->z.z.druhy); // Then branch
            } else if (node->z.z.treti) { // Else branch
                evaluateAST(node->z.z.treti);
            }
            return 0;
        }

        case FOR: { // For loop
            evaluateAST(node->z.z.prvni); // Initialization
            while (evaluateAST(node->z.z.druhy)) { // Condition
                evaluateAST(node->z.z.ctvrty);  // Body
                evaluateAST(node->z.z.treti);  // Increment
            }
            return 0;
        }

        case WHILE: { // While loop
            while (evaluateAST(node->z.z.prvni)) { // Condition
                evaluateAST(node->z.z.druhy);  // Body
            }
            return 0;
        }

        case DO: { // Do-while loop
            do {
                evaluateAST(node->z.z.prvni); // Body
            } while (evaluateAST(node->z.z.druhy)); // Condition
            return 0;
        }

        case 0: // End of program
            return 0;

        default:
            // Print node info
            std::cerr << "Unknown AST node type: " << node->Typ << std::endl;
            Chyba("Unknown AST node type");
            return 0;
    }
}

// Function to preprocess the input and remove carriage returns
std::string preprocessInput(std::istream &input) {
    std::ostringstream processedStream;
    char ch;

    // Read character by character
    while (input.get(ch)) {
        if (ch != '\r') { // Skip carriage return characters
            processedStream.put(ch);
        }
    }

    return processedStream.str();
}


int main() {
    // Initialize the lexer and parser
    LexInit();

    // Parse the input and generate the AST
    if (yyparse() == 0 && Koren) {
        // Evaluate the AST
        if (evaluateAST(Koren) == 0) {
            std::cout << "Program executed successfully." << std::endl;
        } else {
            std::cerr << "Execution failed." << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Parsing failed." << std::endl;
        return 1;
    }

    // Cleanup
    Flush();
    return 0;
}