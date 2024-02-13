#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_FORMULA_LENGTH 1000
#define MAX_VARIABLES 26

// Stack structure for evaluation
typedef struct {
    int top;
    char items[MAX_FORMULA_LENGTH];
} Stack;

// Function prototypes
bool isValidFormula(char *formula);
void push(Stack *s, char c);
char pop(Stack *s);
bool evaluate(char *formula, int *variables, int num_variables);
bool evaluateExpression(char op, bool operand1, bool operand2);

int main(int argc, char *argv[]) {
    // Check the number of arguments
    if (argc != 3) {
        printf("Usage: %s <num_variables> <formula>\n", argv[0]);
        return 1;
    }

    // Parse arguments
    int num_variables = atoi(argv[1]);
    char *formula = argv[2];

    // Validate formula
    if (!isValidFormula(formula) || num_variables < 1 || num_variables > 26) {
        printf("Invalid formula or number of variables.\n");
        return 1;
    }
    if (strlen(formula) > MAX_FORMULA_LENGTH){
        return 1;
    }

    // Print header
    for (char var = 'a'; var < 'a' + num_variables; var++) {
        printf("%c ", var);
    }
    printf(": %s : Result\n", formula);
    int headerLength = 2 * num_variables; // Account for variable names
    headerLength += strlen(formula) + 11; // Account for ": ", " : ", and " Result"

    for (int i = 0; i < headerLength; i++) {
        printf("=");
    }
    printf("\n");

    // Initialize variables array
    int variables[MAX_VARIABLES];
    for (int i = 0; i < num_variables; i++) {
        variables[i] = 0;
    }

    // Generate truth table and evaluate formula
    for (int i = 0; i < (1 << num_variables); i++) {
        // Print current row values
        for (int j = num_variables - 1; j >= 0; j--) {
            printf("%d ", variables[j]);
        }
        printf(": ");

        // Evaluate formula for current row
        bool result = evaluate(formula, variables, num_variables);
        printf(" : %d\n", result);

        // Increment variables for next row
        for (int j = 0; j < num_variables; j++) {
            if (++variables[j] < 2) {
                break;
            }
            variables[j] = 0;
        }
    }

    return 0;
}

// Function to validate the formula syntax
bool isValidFormula(char *formula) {
    Stack s = { .top = -1 };

    for (int i = 0; formula[i] != '\0'; i++) {
        char c = formula[i];

        if ((c >= 'a' && c <= 'z') || c == '0' || c == '1') {
            push(&s, c);
        } else if (c == '-') {
            if (s.top == -1) {
                return false; // Invalid syntax: '-' without preceding operand
            }
            pop(&s);
            push(&s, '0' + ((s.items[s.top] == '0') ? 1 : 0)); // Negate top operand
        } else if (strchr("|&>#=", c) != NULL) {
            if (s.top < 1) {
                return false; // Invalid syntax: insufficient operands for operator
            }
            pop(&s); // Pop operand 2
            pop(&s); // Pop operand 1
            push(&s, '1'); // Push result placeholder
        } else {
            return false; // Invalid character
        }
    }

    return (s.top == 0); // Stack should have exactly 1 item at the end (result)
}

// Function to push an item onto the stack
void push(Stack *s, char c) {
    s->items[++s->top] = c;
}

// Function to pop an item from the stack
char pop(Stack *s) {
    return s->items[s->top--];
}

// Function to evaluate the formula
bool evaluate(char *formula, int *variables, int num_variables) {
    Stack s = { .top = -1 };

    // Reverse the order of variables for evaluation
    int reversed_variables[MAX_VARIABLES];
    for (int i = 0; i < num_variables; i++) {
        reversed_variables[i] = variables[num_variables - 1 - i];
    }

    for (int i = 0; formula[i] != '\0'; i++) {
        char c = formula[i];

        if (c >= 'a' && c <= 'z') {
            push(&s, reversed_variables[c - 'a'] ? '1' : '0');
            printf(" ");
        } else if (c == '0' || c == '1') {
            push(&s, c);
            printf(" ");
        } else if (c == '-') {
            if (s.top == -1) {
                return false; // Invalid syntax: '-' without preceding operand
            }
            char operand = pop(&s);
            char result = (operand == '0') ? '1' : '0';
            push(&s, result);
            printf("%c", result);
        } else if (strchr("|&>#=", c) != NULL) {
            if (s.top < 1) {
                return false; // Invalid syntax: insufficient operands for operator
            }
            char operand2 = pop(&s) - '0';
            char operand1 = pop(&s) - '0';
            bool expression_result = evaluateExpression(c, operand1, operand2);
            char result = expression_result ? '1' : '0';
            push(&s, result);
            printf("%c", result);
        }
    }

    if (s.top != 0) {
        return false; // Invalid syntax: stack should have exactly 1 item at the end
    }

    return (pop(&s) - '0');
}

// Function to evaluate binary expressions
bool evaluateExpression(char op, bool operand1, bool operand2) {
    switch (op) {
        case '|': return operand1 || operand2;
        case '&': return operand1 && operand2;
        case '#': return operand1 != operand2;
        case '>': return !operand1 || operand2;
        case '=': return operand1 == operand2;
        default: return false; // Invalid operator
    }
}
