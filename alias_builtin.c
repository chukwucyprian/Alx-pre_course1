#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_ALIAS_COUNT 100
#define MAX_ALIAS_LENGTH 100

typedef struct Alias
{
    char name[MAX_ALIAS_LENGTH];
    char value[MAX_ALIAS_LENGTH];
} Alias;

Alias aliases[MAX_ALIAS_COUNT];
int aliasCount = 0;

/**
 * custom_strlen - Calculate the length of a string
 * @str: The input string
 *
 * Return: The length of the string
 */
size_t custom_strlen(const char *str)
{
    const char *s = str;

    while (*s)
        ++s;

    return s - str;
}

/**
 * custom_strncmp - Compare two strings up to a specified number of characters
 * @s1: The first string
 * @s2: The second string
 * @n: The maximum number of characters to compare
 *
 * Return: 0 if strings are equal, negative value if s1 < s2, positive value if s1 > s2
 */
int custom_strncmp(const char *s1, const char *s2, size_t n)
{
	  size_t i;
	  
	  for (i = 0; i < n; ++i)
	  {
		  if (s1[i] != s2[i])
			  return s1[i] - s2[i];
		  if (s1[i] == '\0')
			  return 0;
	  }
	  return 0;
}

/**
 * custom_strcpy - Copy a string
 * @dest: The destination string
 * @src: The source string
 */
void custom_strcpy(char *dest, const char *src)
{
    while (*src)
    {
        *dest = *src;
        ++dest;
        ++src;
    }

    *dest = '\0';
}

/**
 * printAliases - Print all aliases
 */
void printAliases(void)
{
	int i;
	
	for (i = 0; i < aliasCount; i++)
	{
		write(STDOUT_FILENO, aliases[i].name, custom_strlen(aliases[i].name));
		write(STDOUT_FILENO, "='", 2);
		write(STDOUT_FILENO, aliases[i].value, custom_strlen(aliases[i].value));
		write(STDOUT_FILENO, "'\n", 2);
	}
}

/**
 * printAlias - Print a specific alias
 * @name: The name of the alias
 */
void printAlias(const char *name)
{
	int i;
	
	for (i = 0; i < aliasCount; i++)
	{
		if (custom_strncmp(name, aliases[i].name, MAX_ALIAS_LENGTH) == 0)
		{
			write(STDOUT_FILENO, aliases[i].name, custom_strlen(aliases[i].name));
			write(STDOUT_FILENO, "='", 2);
			write(STDOUT_FILENO, aliases[i].value, custom_strlen(aliases[i].value));
			write(STDOUT_FILENO, "'\n", 2);
			return;
		}
	}
}

/**
 * defineAlias - Define or update an alias
 * @name: The name of the alias
 * @value: The value of the alias
 */
void defineAlias(const char *name, const char *value)
{
	int i;
	
	for (i = 0; i < aliasCount; i++)
	{
		if (custom_strncmp(name, aliases[i].name, MAX_ALIAS_LENGTH)  == 0)
		{
			custom_strcpy(aliases[i].value, value);
			return;
		}
	}

    if (aliasCount < MAX_ALIAS_COUNT)
    {
        custom_strcpy(aliases[aliasCount].name, name);
        custom_strcpy(aliases[aliasCount].value, value);
        aliasCount++;
    }
}

/**
 * processAliasCommand - Process an alias command
 * @command: The alias command to process
 */
void processAliasCommand(char *command)
{
    char *token;
    char *end_ptr;
    int token_count = 0;
    char *name;
    char *value;

    token = strtok_r(command, " ", &end_ptr);

    while (token != NULL)
    {
        if (token_count == 0)
        {
            name = token;
            token = strtok_r(NULL, " ", &end_ptr);
            token_count++;

            if (token == NULL)
            {
                if (custom_strncmp(name, "alias", MAX_ALIAS_LENGTH) == 0)
                {
                    printAliases();
                }
                else
                {
                    printAlias(name);
                }

                return;
            }
        }

        if (custom_strncmp(token, "=", 1) == 0)
        {
            value = token + 1;
            defineAlias(name, value);
        }

        token = strtok_r(NULL, " ", &end_ptr);
        token_count++;
    }
}

/**
 * main - Entry point of the program
 *
 * Return: 0 on success
 */
int main(void)
{
	char *command = NULL;
	size_t command_size = 0;
	char exit_command[] = "exit\n";
	char prompt[] = "alias> ";
	ssize_t prompt_length = custom_strlen(prompt);
	ssize_t command_len;

	write(STDOUT_FILENO, "Enter alias commands (type 'exit' to quit):\n", custom_strlen("Enter alias commands (type 'exit' to quit):\n"));

    while (1)
    {
        write(STDOUT_FILENO, prompt, prompt_length);

        command_len = getline(&command, &command_size, stdin);

        if (command_len == -1)
            break;

        if (custom_strncmp(command, exit_command, custom_strlen(exit_command)) == 0)
            break;

        command[command_len - 1] = '\0';

        processAliasCommand(command);
    }

    free(command);

    return 0;
}
