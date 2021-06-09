#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILENAME "./input"

struct LogNode {
    unsigned int index;
    struct LogNode* prev;
};

void free_log(struct LogNode* log)
{
    if (log == NULL) {
        return;
    }

    if (log->prev != NULL) {
        free_log(log->prev);
    }

    free(log);
}

void swap(int rows, int instructions[rows][2], int index)
{
    if (instructions[index][0] == 'j') {
        instructions[index][0] = 'n';
    }
    else if (instructions[index][0] == 'n') {
        instructions[index][0] = 'j';
    }
}

int find_next_target(int rows, int instructions[rows][2], int current_try)
{
    int next_target = current_try + 1;

    while (next_target < rows) {
        if (instructions[next_target][0] == 'j'
            || instructions[next_target][0] == 'n') {
            return next_target;
        }
        next_target++;
    }
    return -1;
}

int main() {
    FILE* input_file = fopen(INPUT_FILENAME, "r");

    int instr_count = 0;
    while(!feof(input_file)) {
        char discard[256] = { '\0' };
        fgets(discard, 255, input_file);

        if (strlen(discard) >= 6) {
            instr_count++;
        }
    }

    int instructions[instr_count][2];

    for (int i = 0; i < sizeof(instructions)/sizeof(instructions[0]); i++) {
        for (int j = 0; j < sizeof(instructions[i])/sizeof(instructions[i][0]); j++) {
            instructions[i][j] = 0;
        }
    }

    fseek(input_file, 0, SEEK_SET);
    instr_count = 0;

    while(!feof(input_file)) {
        char buffer[256] = { '\0' };
        fgets(buffer, 255, input_file);

        if (strlen(buffer) < 6) {
            continue;
        }


        char* token =  strtok(buffer, " ");
        instructions[instr_count][0] = token[0];
        token = strtok(NULL, " ");
        instructions[instr_count][1] = atoi(token);

        instr_count++;
    }
    fclose(input_file);

    struct LogNode* current_node = NULL;

    unsigned int pc = 0;
    int acc = 0;

    int current_try = 0;

    while(1) {
        if (pc == sizeof(instructions) / sizeof(instructions[0]) - 1) {
            printf("Success - modified instruction %d - acc: %d\n", current_try, acc);
            break;
        }

        int halted = 0;

        struct LogNode* search_node = current_node;
        while(search_node != NULL) {
            if (search_node->index == pc) {
                swap(sizeof(instructions) / sizeof(instructions[0]), instructions, current_try);

                current_try = find_next_target(sizeof(instructions) / sizeof(instructions[0]), instructions, current_try);
                if (current_try == -1) {
                    printf("HALT - pc: %d  acc: %d\n", pc, acc);
                    halted = 1;
                    break;
                } 

                swap(sizeof(instructions) / sizeof(instructions[0]), instructions, current_try);

                pc = 0;
                acc = 0;
                free_log(current_node);
                current_node = NULL;
                search_node->prev = NULL;
            }
            search_node = search_node->prev;
        }

        if (halted) {
            break;
        }

        struct LogNode* new_entry = calloc(1, sizeof(struct LogNode));
        new_entry->index = pc;
        if (current_node != NULL) {
            new_entry->prev = current_node;
        }
        current_node = new_entry;

        switch (instructions[pc][0]) {
            case 'a':
                acc += instructions[pc][1];
                pc++;
                break;
            case 'j':
                pc += instructions[pc][1];
                break;
            default:
                pc++;
                break;
        }

    }

    free_log(current_node);
    return 0;
}