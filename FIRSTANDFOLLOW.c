#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUCTIONS 100
#define MAX_SYMBOLS 128
#define MAX_RHS_LENGTH 50

typedef struct {
    char lhs;
    char rhs[MAX_RHS_LENGTH];
} Production;

Production grammar[MAX_PRODUCTIONS];
int num_productions = 0;
char start_symbol;
char symbols[MAX_SYMBOLS];
int num_symbols = 0;

int first[MAX_SYMBOLS][MAX_SYMBOLS];
int follow[MAX_SYMBOLS][MAX_SYMBOLS];

int get_symbol_index(char symbol) {
    for (int i = 0; i < num_symbols; i++) {
        if (symbols[i] == symbol) {
            return i;
        }
    }
    symbols[num_symbols] = symbol;
    return num_symbols++;
}

void add_production(char lhs, const char* rhs) {
    grammar[num_productions].lhs = lhs;
    strcpy(grammar[num_productions].rhs, rhs);
    num_productions++;
    get_symbol_index(lhs);
    for (int i = 0; rhs[i] != '\0'; i++) {
        get_symbol_index(rhs[i]);
    }
}

void compute_first() {
    int changes;
    do {
        changes = 0;
        for (int i = 0; i < num_productions; i++) {
            int lhs_index = get_symbol_index(grammar[i].lhs);
            char* rhs = grammar[i].rhs;

            for (int j = 0; rhs[j] != '\0'; j++) {
                int symbol_index = get_symbol_index(rhs[j]);
                if (!isupper(rhs[j])) {
                    if (!first[lhs_index][symbol_index]) {
                        first[lhs_index][symbol_index] = 1;
                        changes = 1;
                    }
                    break;
                } else {
                    for (int k = 0; k < num_symbols; k++) {
                        if (first[symbol_index][k] && k != get_symbol_index('#') && !first[lhs_index][k]) {
                            first[lhs_index][k] = 1;
                            changes = 1;
                        }
                    }
                    if (!first[symbol_index][get_symbol_index('#')]) {
                        break;
                    }
                }
            }
            if (rhs[0] == '\0' || (strlen(rhs) == 1 && rhs[0] == '#')) {
                if (!first[lhs_index][get_symbol_index('#')]) {
                    first[lhs_index][get_symbol_index('#')] = 1;
                    changes = 1;
                }
            }
        }
    } while (changes);
}

void compute_follow() {
    int start_index = get_symbol_index(start_symbol);
    follow[start_index][get_symbol_index('$')] = 1;

    int changes;
    do {
        changes = 0;
        for (int i = 0; i < num_productions; i++) {
            int lhs_index = get_symbol_index(grammar[i].lhs);
            char* rhs = grammar[i].rhs;

            for (int j = 0; rhs[j] != '\0'; j++) {
                if (isupper(rhs[j])) {
                    int curr_index = get_symbol_index(rhs[j]);
                    int k;
                    for (k = j + 1; rhs[k] != '\0'; k++) {
                        int next_index = get_symbol_index(rhs[k]);
                        if (!isupper(rhs[k])) {
                            if (!follow[curr_index][next_index]) {
                                follow[curr_index][next_index] = 1;
                                changes = 1;
                            }
                            break;
                        }
                        for (int l = 0; l < num_symbols; l++) {
                            if (first[next_index][l] && l != get_symbol_index('#') && !follow[curr_index][l]) {
                                follow[curr_index][l] = 1;
                                changes = 1;
                            }
                        }
                        if (!first[next_index][get_symbol_index('#')]) {
                            break;
                        }
                    }
                    if (rhs[k] == '\0') {
                        for (int l = 0; l < num_symbols; l++) {
                            if (follow[lhs_index][l] && !follow[curr_index][l]) {
                                follow[curr_index][l] = 1;
                                changes = 1;
                            }
                        }
                    }
                }
            }
        }
    } while (changes);
}

void print_set(int set[MAX_SYMBOLS]) {
    int first_symbol = 1;
    printf("{ ");
    for (int i = 0; i < num_symbols; i++) {
        if (set[i]) {
            if (!first_symbol) {
                printf(", ");
            }
            printf("%c", symbols[i]);
            first_symbol = 0;
        }
    }
    printf(" }");
}

int main() {
    char lhs, rhs[MAX_RHS_LENGTH];
    printf("Enter the productions (one per line, 'end' to finish):\n");
    printf("Format: S->ABC or S-># for epsilon\n");
    
    while (1) {
        if (scanf(" %c->%s", &lhs, rhs) != 2) {
            break;
        }
        if (strcmp(rhs, "end") == 0) {
            break;
        }
        add_production(lhs, rhs);
        if (num_productions == 1) {
            start_symbol = lhs;
        }
    }

    compute_first();
    compute_follow();

    printf("\nFirst sets:\n");
    for (int i = 0; i < num_symbols; i++) {
        if (isupper(symbols[i])) {
            printf("FIRST(%c) = ", symbols[i]);
            print_set(first[i]);
            printf("\n");
        }
    }

    printf("\nFollow sets:\n");
    for (int i = 0; i < num_symbols; i++) {
        if (isupper(symbols[i])) {
            printf("FOLLOW(%c) = ", symbols[i]);
            print_set(follow[i]);
            printf("\n");
        }
    }

    return 0;
}
