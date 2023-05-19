#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

/**
 * custom_getline - Read a line of input from stdin
 *
 * Return: Pointer to the line read, or NULL on failure or EOF
 */
char *custom_getline(void)
{
	static char buffer[BUFFER_SIZE];
	static size_t buffer_pos = 0;
	static ssize_t num_read = 0;
	static int eof_flag = 0;
	size_t i;
        char *temp;
	char *line = NULL;
	size_t line_size = 0;
	int newline_found = 0;

	while (!newline_found)
	{
		if (buffer_pos == (size_t)num_read)
		{
			num_read = read(STDIN_FILENO, buffer, BUFFER_SIZE);
			if (num_read <= 0)
				break;
			buffer_pos = 0;
		}

       

        while (buffer_pos < (size_t)num_read)
        {
            if (buffer[buffer_pos] == '\n')
            {
                newline_found = 1;
                buffer_pos++;
                break;
            }

            temp = malloc((line_size + 1) * sizeof(char));
            if (temp == NULL)
                break;

            for (i = 0; i < line_size; i++)
                temp[i] = line[i];

            temp[line_size++] = buffer[buffer_pos++];

            free(line);
            line = temp;
        }

        if (buffer_pos == (size_t)num_read)
        {
            if (eof_flag)
                break;
            else
                eof_flag = 1;
        }
    }

    if (line != NULL)
    {
        temp = malloc((line_size + 1) * sizeof(char));
        if (temp != NULL)
        {
            
            for (i = 0; i < line_size; i++)
                temp[i] = line[i];
            temp[line_size] = '\0';

            free(line);
            line = temp;
        }
    }

    return line;
}

int main(void)
{
    char *line = NULL;

    write(STDOUT_FILENO, "Enter a line of text: ", sizeof("Enter a line of text: ") - 1);

    line = custom_getline();

    if (line != NULL)
    {
        write(STDOUT_FILENO, "Line read: ", sizeof("Line read: ") - 1);
        write(STDOUT_FILENO, line, strlen(line));
        write(STDOUT_FILENO, "\n", 1);
        free(line);
    }
    else
    {
        write(STDOUT_FILENO, "Error reading line.\n", sizeof("Error reading line.\n") - 1);
    }

    return 0;
}

