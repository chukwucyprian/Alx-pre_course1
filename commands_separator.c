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

void execute_command(char *command);

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
	char *token;
	char *command_token;
	char *end_ptr;
	pid_t pid;
	int command_length;
	char *args[MAX_ARGS];
	char *token;
	int arg_count;
	char *arg;

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

        /* Execute commands separated by semicolons */
        command_token = command;
       
        while ((token = strtok_r(command_token, ";", &end_ptr)) != NULL)
        {
            execute_command(token); /* Execute the command */
            command_token = NULL;   /* Reset command_token for subsequent tokens */
        }
    }

    free(command);

    return 0;
}

void execute_command(char *command)
{
    /* Execute single command */
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
        arg = command;

        while (*arg != '\0' && arg_count < MAX_ARGS - 1)
        {
            /* Skip leading spaces */
            while (*arg == ' ')
                arg++;

            if (*arg != '\0')
            {
                /* Store argument */
                args[arg_count++] = arg;

                /* Find end of argument */
                while (*arg != ' ' && *arg != '\0')
                    arg++;

                if (*arg != '\0')
                {
                    *arg = '\0'; /* Terminate argument */
                    arg++;       /* Move to next argument */
                }
            }
        }

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

