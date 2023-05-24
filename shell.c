#include "main.h"

/**
 * _strlen - computes string's length
 * @str: string
 * Return: string length
 */
size_t _strlen(char *str)
{
size_t i = 0;

while (str[i] != '\0')
i++;
return (i);
}

/**
 * main - Simple UNIX command line interpreter
 *
 * Return: Always 0.
 */

int main(void)
{char *command = NULL;
char *prompt = "Shell$ ";
size_t buff_size = 0;
ssize_t command_length;
bool pipe = false;
pid_t pid;
if (!isatty(STDIN_FILENO))
pipe = true;
while (1)
{
if (!pipe)
write(STDOUT_FILENO, prompt, _strlen(prompt));
command_length = getline(&command, &buff_size, stdin);
if (command_length == -1)
{
if (feof(stdin))
{write(STDOUT_FILENO, "\n", 1); /* Handle end of file (Ctrl+D) */
break;
}
else /* Error occurred while reading input */
{write(STDERR_FILENO, "Error: Failed to read input\n", 28);
exit(EXIT_FAILURE);
}
}
command[command_length - 1] = '\0'; /* Remove trailing newline */
pid = fork();
if (pid < 0)
{write(STDERR_FILENO, "Error: Fork failed\n", 19);  /* Fork error */
exit(EXIT_FAILURE);
}
else if (pid == 0)  /* Child process */
{char *args[2];
args[0] = command; args[1] = NULL;
if (execvp(args[0], args) == -1)  /* Executable not found */
{write(STDERR_FILENO, "Error: No such file or directory\n", 32);
exit(EXIT_FAILURE);
}
}
else
{
wait(NULL); /* Parent process */
}
}
free(command);
return (0);
}

