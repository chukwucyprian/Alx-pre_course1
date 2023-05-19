#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#define MAX_ARGS 10
#define BUFFER_SIZE 1024

/**
 * main - Simple UNIX command line interpreter
 *
 * Return: Always 0.
 */
int main(void)
{
	char *command = NULL;
	size_t buffer_size = 0;
	char prompt[] = "myShell> ";
	ssize_t prompt_length = sizeof(prompt) - 1;
	ssize_t command_len;
	int status;
	int has_status;
	char *status_str;
	char *var_value;
	char *var_name;
	pid_t pid;
	int command_length;
	char *args[MAX_ARGS];
	int arg_count;
	char *token;
	char *next_token;


 	while (1)
    	{
        write(STDOUT_FILENO, prompt, prompt_length); /* Display prompt */

        command_len = getline(&command, &buffer_size, stdin);

        if (command_len == -1)
        {
            /* Handle end of file (Ctrl+D) */
            write(STDOUT_FILENO, "\n", 1);
            break;
        }

        command[command_len - 1] = '\0'; /* Remove trailing newline */

        if (command[0] == 'e' && command[1] == 'x' && command[2] == 'i' && command[3] == 't' && (command[4] == ' ' || command[4] == '\0'))
        {
            /* Exit command entered */
             status = 0;
             has_status = 0;

            if (command[4] == ' ')
            {
                /* Extract status argument */
                status_str = &command[5];

                if (status_str[0] != '\0')
                {
                    status = atoi(status_str);
                    has_status = 1;
                }
            }

            free(command);
            exit(has_status ? status : 0);
        }
        else if (command[0] == 's' && command[1] == 'e' && command[2] == 't' && command[3] == 'e' && command[4] == 'n' && command[5] == 'v' && command[6] == ' ')
        {
            /* Set environment variable command entered */
            var_value = command + 7;
            var_name = var_value;

            while (*var_name != ' ' && *var_name != '\0')
                var_name++;

            if (*var_name != '\0')
            {
                *var_name = '\0';
                var_name++;

                if (*var_name != '\0')
                {
                    if (setenv(var_value, var_name, 1) == -1)
                    {
                        perror("setenv");
                        write(STDERR_FILENO, "Failed to set environment variable\n", sizeof("Failed to set environment variable\n") - 1);
                    }
                }
                else
                {
                    write(STDERR_FILENO, "Invalid syntax. Usage: setenv VARIABLE VALUE\n", sizeof("Invalid syntax. Usage: setenv VARIABLE VALUE\n") - 1);
                }
            }
            else
            {
                write(STDERR_FILENO, "Invalid syntax. Usage: setenv VARIABLE VALUE\n", sizeof("Invalid syntax. Usage: setenv VARIABLE VALUE\n") - 1);
            }
        }
        else if (command[0] == 'u' && command[1] == 'n' && command[2] == 's' && command[3] == 'e' && command[4] == 't' && command[5] == 'e' && command[6] == 'n' && command[7] == 'v' && command[8] == ' ')
        {
            /* Unset environment variable command entered */
            var_name = command + 9;

            if (*var_name != '\0')
            {
                if (unsetenv(var_name) == -1)
                {
                    perror("unsetenv");
                    write(STDERR_FILENO, "Failed to unset environment variable\n", sizeof("Failed to unset environment variable\n") - 1);
                }
            }
            else
            {
                write(STDERR_FILENO, "Invalid syntax. Usage: unsetenv VARIABLE\n", sizeof("Invalid syntax. Usage: unsetenv VARIABLE\n") - 1);
            }
        }
        else
        {
            /* Execute external command */

            arg_count = 0;

            pid = fork();

            if (pid == -1)
            {
                /* Fork error */
                perror("fork");
                exit(1);
            }
            else if (pid == 0)
            {
                /* Child process */

                token = command;
                next_token = NULL;

                while (*token != '\0')
                {
                    next_token = token;

                    while (*next_token != ' ' && *next_token != '\0')
                        next_token++;

                    if (*next_token != '\0')
                    {
                        *next_token = '\0';
                        next_token++;
                    }

                    args[arg_count++] = token;

                    if (arg_count >= MAX_ARGS - 1)
                        break;

                    token = next_token;
                }

                if (arg_count < MAX_ARGS - 1)
                    args[arg_count++] = token;

                args[arg_count] = NULL; /* Last element must be NULL */

                execvp(args[0], args);

                /* If execvp fails, print error message and exit */
                command_length = write(STDERR_FILENO, args[0], 1);
                write(STDERR_FILENO, args[0] + 1, command_length - 1);
                write(STDERR_FILENO, ": command not found\n", sizeof(": command not found\n") - 1);
                _exit(127);
            }
            else
            {
                /* Parent process */
                wait(NULL);
            }
        }
    }

    free(command);

    return 0;
}
