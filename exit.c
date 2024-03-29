#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>

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
	pid_t pid;
	int i;
	int j;
	int command_length;
	char *args[MAX_ARGS];
	int arg_count;
	int arg_start;
        int in_arg;
	int arg_len;


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
            break;
        }


        arg_count = 0;
        arg_start = -1;
        in_arg = 0;

        for (i = 0; i < command_len; i++)
        {
            if (command[i] != ' ' && !in_arg)
            {
                arg_start = i;
                in_arg = 1;
            }
            else if ((command[i] == ' ' || i == command_len - 1) && in_arg)
            {
                arg_len = i - arg_start;
                args[arg_count] = malloc((arg_len + 1) * sizeof(char));
                
		for (j = 0; j < arg_len; j++)
                    args[arg_count][j] = command[arg_start + j];
                args[arg_count][j] = '\0';

                arg_count++;
                in_arg = 0;

                if (arg_count >= MAX_ARGS - 1)
                    break;
            }
        }

        args[arg_count] = NULL; /* Last element must be NULL */

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

            execve(args[0], args, NULL);

            /* If execve fails, print error message and exit */
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

        for (i = 0; i < arg_count; i++)
            free(args[i]);
    }

    free(command);

    return 0;
 }
