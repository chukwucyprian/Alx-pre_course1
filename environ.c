#include <unistd.h>

extern char **environ; /* Declare the external variable environ */

/**
 * main - Simple UNIX command line interpreter
 *
 * Return: Always 0.
 */
int main(void)
{
	int i;
	int j;
	char *env_str;

    /* Iterate through each environment string and print it */
    i = 0;
    while (environ[i] != NULL)
    {
        env_str = environ[i];
        j = 0;

        /* Print each character of the environment string */
        while (env_str[j] != '\0')
        {
            if (write(STDOUT_FILENO, &env_str[j], 1) == -1)
            {
                _exit(1); /* Exit with an error if write fails */
            }
            j++;
        }

        if (write(STDOUT_FILENO, "\n", 1) == -1)
        {
            _exit(1); /* Exit with an error if write fails */
        }

        i++;
    }

    return (0);
}

