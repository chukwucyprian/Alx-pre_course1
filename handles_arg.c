#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

#define BUFFER_SIZE 1024

/**
 * prompt - Display the shell prompt
 */
void prompt()
{
	write(STDOUT_FILENO, "MyShell$ ", 9);
	fflush(stdout);
}

/**
 * execute_command - Execute the given command with arguments
 * @args: Array of strings containing the command and arguments
 */
void execute_command(char **args)
{
	pid_t pid = fork();
	if (pid == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		execvp(args[0], args);
		perror("execvp");
		exit(EXIT_FAILURE);
	}
	else
	{
		waitpid(pid, NULL, 0);
	}
}

/**
 * main - Entry point of the shell
 *
 * Return: 0 on success
 */
int main(void)
{
	char *buffer = NULL;
	size_t buffer_size = 0;
	ssize_t line_length;
	char *token;
	int arg_count = 0;
	char **args = malloc(sizeof(char *) * (BUFFER_SIZE / 2 + 1));

	while (1)
	{
		prompt();

		line_length = getline(&buffer, &buffer_size, stdin);
		if (line_length == -1)
			break;

		/*Remove the newline character at the end of the input */
		buffer[line_length - 1] = '\0';

		token = strtok(buffer, " ");
		while (token != NULL)
		{
			args[arg_count] = token;
			arg_count++;
			token = strtok(NULL, " ");
		}
		args[arg_count] = NULL;

		execute_command(args);
	}

	free(buffer);
	free(args);
	return 0;
}

