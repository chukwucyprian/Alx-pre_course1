#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	ssize_t command_length;
	pid_t pid;
	int i;
	int command_length_int;
 	char *args[MAX_ARGS];
 	char *token;
       	int arg_count = 0;
	char error_message[] = ": command not found\n";
	size_t error_length;
	char path[] = "/bin/";
       	char full_path[BUFFER_SIZE];
	int path_length;
	int command_len;
	int arg_length;
	int j;

	while (1)
	{
		write(STDOUT_FILENO, prompt, prompt_length); /* Display prompt */

		command_length = getline(&command, &buffer_size, stdin);

		if (command_length == -1)
		{
			/* Handle end of file (Ctrl+D) */
			write(STDOUT_FILENO, "\n", 1);
			break;
		}

		command[command_length - 1] = '\0'; /* Remove trailing newline */

		pid = fork();

		if (pid < 0)
		{
			/* Fork error */
			perror("Fork failed");
			_exit(1);
		}
		else if (pid == 0)
		{
			/* Child process */

			token = strtok(command, " ");
			while (token != NULL && arg_count < MAX_ARGS - 1)
			{
				args[arg_count] = token;
				arg_count++;
				token = strtok(NULL, " ");
			}
			args[arg_count] = NULL;
			
			path_length = sizeof(path) - 1;
			command_len = 0;
			j = 0;

            while (args[0][command_len] != '\0')
                command_len++;

            if (access(args[0], F_OK) == 0)
            {
                /* Command exists in current directory */
                execve(args[0], args, NULL);
            }
            else if (access("/bin", F_OK) == 0)
            {
                /* Check if command exists in /bin directory */
                if (path_length + command_len < BUFFER_SIZE)
                {
                    for (i = 0; i < path_length; i++)
                        full_path[i] = path[i];

                    for (j = 0; j < command_len; j++)
                        full_path[i++] = args[0][j];

                    full_path[i] = '\0';

                    execve(full_path, args, NULL);
                }
            }

            /* Command not found */
            error_length = sizeof(error_message) - 1;
            command_length_int = arg_count;
            for (i = 0; i < command_length_int; i++)
            {
                arg_length = 0;
                while (args[i][arg_length] != '\0')
                    arg_length++;

                write(STDOUT_FILENO, args[i], arg_length);
                write(STDOUT_FILENO, " ", 1);
            }
            write(STDOUT_FILENO, error_message, error_length);
            _exit(1);
        }
        else
        {
            /* Parent process */
            wait(NULL);
 }
}

free(command);

return (0);
}
