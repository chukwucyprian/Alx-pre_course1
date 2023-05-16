#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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
	ssize_t prompt_length = strlen(prompt);
	ssize_t command_length;
	 pid_t pid;

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
			exit(1);
		}
		else if (pid == 0)
		{
			/* Child process */
			char *args[2];
			args[0] = command;
			args[1] = NULL;
			if (execve(command, args, NULL) == -1)
			{
				/* Executable not found */
				write(STDOUT_FILENO, command, strlen(command));
				write(STDOUT_FILENO, ": command not found\n", 20);
				exit(1);
			}
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

