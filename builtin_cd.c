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
	char prev_dir[BUFFER_SIZE] = "";
	char *var_name;
	char *dir_path;
	pid_t pid;
	int command_length;
	char *args[MAX_ARGS];
	char *arg;
	int arg_count;

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
        else if (command[0] == 'c' && command[1] == 'd' && (command[2] == ' ' || command[2] == '\0'))
        {
            /* Change directory command entered */
           dir_path = NULL;

            if (command[2] == ' ')
                dir_path = &command[3]; /* Extract directory path from command */
            else
                dir_path = getenv("HOME"); /* No argument given, use HOME directory */

            if (dir_path != NULL)
            {
                if (dir_path[0] == '-' && dir_path[1] == '\0')
                {
                    /* Change to previous directory */
                    if (prev_dir[0] != '\0')
                    {
                        if (chdir(prev_dir) == -1)
                        {
                            perror("chdir");
                            write(STDERR_FILENO, "Failed to change directory\n", sizeof("Failed to change directory\n") - 1);
                        }
                    }
                    else
                    {
                        write(STDERR_FILENO, "No previous directory\n", sizeof("No previous directory\n") - 1);
                    }
                }
                else
                {
                    /* Change to specified directory */
                    if (getcwd(prev_dir, sizeof(prev_dir)) == NULL)
                    {
                        perror("getcwd");
                        write(STDERR_FILENO, "Failed to get current directory\n", sizeof("Failed to get current directory\n") - 1);
                    }

                    if (chdir(dir_path) == -1)
                    {
                        perror("chdir");
                        write(STDERR_FILENO, "Failed to change directory\n", sizeof("Failed to change directory\n") - 1);
                    }
                }
            }
            else
            {
                write(STDERR_FILENO, "Failed to get HOME directory\n", sizeof("Failed to get HOME directory\n") - 1);
            }
        }
        else
        {
            /* Execute command */
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
                            arg++;      /* Move to next argument */
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
    }

    free(command);

    return 0;
}
