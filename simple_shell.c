#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#define BUFFER_SIZE 1024

/**
 * prompt - Display shell prompt
 */
void prompt() {
    write(STDOUT_FILENO, "MyShell$ ", 9);
}

/**
 * main - Simple shell program
 *
 * Return: Always 0
 */
int main(void) {
    char *buffer = NULL;
    size_t buffer_size = 0;
    ssize_t input_size;

    while (1) {
        prompt();

        input_size = getline(&buffer, &buffer_size, stdin);

        if (input_size == -1) {
            /* Handle Ctrl+D */
            write(STDOUT_FILENO, "\n", 1);
            break;
        }

        if (buffer[input_size - 1] == '\n') {
            buffer[input_size - 1] = '\0';
        }

        if (strlen(buffer) > 0) {
            pid_t pid = fork();

            if (pid == -1) {
                perror("Fork failed");
                exit(1);
            }

            if (pid == 0) {
                /* Child process */
                char **argv = malloc(2 * sizeof(char *));
                if (argv == NULL) {
                    perror("Malloc failed");
                    exit(1);
                }
                argv[0] = buffer;
                argv[1] = NULL;

                if (execvp(argv[0], argv) == -1) {
                    perror("Error");
                    free(argv);
		    exit(1);
                }
            } else {
                /* Parent process */
                waitpid(pid, NULL, 0);
            }
        }
    }

    free(buffer);
    return 0;
}

