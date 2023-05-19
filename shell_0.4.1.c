#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdbool.h>

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
	int command_length;
	char *args[MAX_ARGS];
	int arg_count;
	int in_word;
	int word_start;

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
            int status = 0;
            bool has_status = false;

            if (command[4] == ' ')
            {
                /* Extract status argument */
                char *status_str = &command[5];

                if (status_str[0] != '\0')
                {
                    status = atoi(status_str);
                    has_status = true;
                }
            }

            free(command);
            exit(has_status ? status : 0);
        }

        arg_count = 0;
        in_word = 0; /* Flag to track if currently inside a word */
        word_start = 0; /* Index of the start of the current word */

        for (i = 0; command[i] != '\0'; i++)
        {
            if (command[i] != ' ' && !in_word)
            {
                in_word = 1;
                word_start = i;
            }
            else if (command[i] == ' ' && in_word)
            {
                in_word = 0;
                args[arg_count] = &command[word_start];
                command[i] = '\0';
                arg_count++;

                if (arg_count >= MAX_ARGS - 1)
                    break;
            }
        }

        if (in_word)
        {
            args[arg_count] = &command[word_start];
            arg_count++;

            if (arg_count >= MAX_ARGS - 1)
                break;
        }

        args[arg_count] = NULL; /* Last element must be NULL */

        pid = fork();

        if (pid == -1)
        {
            /* Fork error */
            perror("fork");
            exit(1);
        }

        if (pid == 0)
        {
            /* Child process */
            execve(args[0], args, NULL);

            /* If execve fails, print error message and exit */
            command_length = write(STDERR_FILENO, args[0], 1);
            write(STDERR_FILENO, args[0] + 1, command_length - 1);
            write(STDERR_FILENO, ": command not found\n", sizeof(": command not found\n") - 1);
            exit(127);
        }
        else
        {
            /* Parent process */
            wait(NULL);
        }
    }

    free(command);

    return 0;
}

