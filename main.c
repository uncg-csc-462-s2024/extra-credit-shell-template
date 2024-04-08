/**
 * This is a sample answer for assignment 1 in CSC 462/CSC 662 Spring 2024.
 *
 * Use this as the starting point to complete your assignment.
 * 
 * Compile via gcc -g -Wall -Werror main.c -o main.o
 * Execute via ./main.o
 * 
 * @author Regis Kopper
 * @version 04/08/2024
 */

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define DEFAULT_ARG_CAPACITY 10

// Default argument capacity.
static int args_capacity = 1;

/**
 * Executes a command (cmd) in the child process, using execvp.
 */
void execute_cmd(size_t num_args, char *args[]) {
    pid_t child;
    if ((child = fork()) < 0) {
        fprintf(stderr, "Failed to fork() to child\n");
        exit(EXIT_FAILURE);
    } else if (child == 0) {
        if (execvp(args[0], args) == -1) {
            fprintf(stderr, "Failed to execute command %s\n", args[0]);
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * Parses an input command. We tokenize on whitespace and allocate arguments.
 */
void parse_cmd(char *input) {
    // Remove the newline character.
    input[strcspn(input, "\n")] = '\0';

    // First, tokenize the input and copy the arguments over.
    char *token = strtok(input, " ");
    int bytes = sizeof(char *) * args_capacity;
    char **args = calloc(args_capacity, bytes);
    if (args == NULL) {
        fprintf(stderr, "Failed to calloc %d elements, each of size %d bytes\n", args_capacity, bytes);
        exit(errno);
    }    
    size_t num_args = 0;
    while (token != NULL) {
        args[num_args] = malloc(strlen(token) + 1);
        if (args[num_args] == NULL) {
            fprintf(stderr, "Failed to malloc %zu bytes\n", strlen(token) + 1);
            exit(errno);
        }
        strcpy(args[num_args], token);
        num_args++;
        // If we go over the argument buffer capacity, extend it by a factor of 2.
        //printf("%zu args, %d capacity\n", num_args, args_capacity);
        if (num_args >= args_capacity) {
            args_capacity *= 2;
            size_t new_size = sizeof(char *) * args_capacity;
            //printf("Will attempt to alloc %d elements, totaling %zu bytes\n", args_capacity, new_size);
            args = realloc(args, new_size);
            if (args == NULL) {
                fprintf(stderr, "Failed to alloc %d elements, totaling %zu bytes\n", args_capacity, new_size);
                exit(errno);
            }
        }

        token = strtok(NULL, " ");
    }

    // Now, execute the command.
    execute_cmd(num_args, args);
    args_capacity = DEFAULT_ARG_CAPACITY;

    // Wait for the child process to finish before free-ing the args.
    wait(NULL);
    for (int i = 0; i < num_args + 1; i++) {
        free(args[i]);
    }
    free(args);
}

int main(void) {
    char buf[LINE_MAX];

    while (true) {
        printf("> ");
        if (fgets(buf, sizeof buf, stdin) != NULL) {
            if(strcmp(buf, "quit\n") == 0){
                exit(0);
            }
            parse_cmd(buf);
        } else {
            break;
        }
    }
    return 0;
}
