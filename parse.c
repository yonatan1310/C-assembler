#include "assembler.h"

int parse_word(char *, int);
int get_string(char[], char[], char *);

/**
 * Parses an assembly code line based on white spaces, commas and strings
 * @param line - the assembly code line
 * @return the number of operands or ERROR if there's an invalid string argument (missing ")
 */
int parse_line(char line[])
{
    char *token, *end_ptr, string[LINE_LEN] = "";
    int i = 0;

    data = (char **)malloc(sizeof(char *)); /* Allocating memory for the data array */

    /* If there's an illegal syntax regarding assembly language strings, we return an ERROR */
    if(get_string(line, string, strchr(line, '\"')))
        return ERROR;

    /* Getting the string's closing quote, if exists */
    end_ptr = strrchr(line, '\"');

    /* Getting the first argument, if exists */
    token = strtok(line, " \t\n");
    while(token)
    {
        if(*token == '\"' && *string)
        { /* If we reached an assembly language string, it is held, in its entirety, in the next cell of data */
            data[i] = (char *)malloc(strlen(string));
            if(!data[i])
                print_error("memory allocation failed");
            strcpy(data[i++], string); /* Allocating memory for the new cell and copying the string into it */

            data = (char **)realloc(data, (i + 1) * sizeof(char *));
            if(!data) /* Reallocating memory for a new space for the next cell */
                print_error("memory allocation failed");

            /* Skipping to the next argument after the end of the string */
            while(token != end_ptr && (&(token[strlen(token) - 1]) != end_ptr))
                token = strtok(NULL, " \t\n");
            token = strtok(NULL, " \t\n");
            continue;
        }
        /* If we found a comment (which ends the actual line) or we're in a blank line we return the current size */
        if((i = parse_word(token, i)) <= 0)
            return -i;
        token = strtok(NULL, " \t\n"); /* Gets the next argument */
    }
    return i;
}

/**
 * Parses an argument inside the line
 * @param word - the argument inside the line to be parsed
 * @param index - the current number of arguments
 * @return the updated number of arguments or it's negative value if a comment was found
 */
int parse_word(char *word, int index)
{
    char arg[MAX_ARG_SIZE];
    int arg_index, comment = 0;
    while(*word)
    {
        while(*word == ',') /* Commas are stored in a separate cell */
        {
            data[index++] = ",";
            data = (char **)realloc(data, (index + 1) * sizeof(char *));
            if(!data) /* Reallocating memory for a new space for the next cell */
                print_error("memory allocation failed");

            if(!(*(++word))) /* If the comma was at the end of the word, we finished parsing the word */
                return index;
        }

        arg_index = 0; /* Getting a non-comma argument */
        while(*word && *word != ',' && !comment)
        {
            if(*word == ';') /* Reached a comment */
                comment = -1;
            else arg[arg_index++] = *(word++);
        }
        arg[arg_index] = '\0'; /* Adding end of string character */
        if(*arg) /* If there's a new argument, we add it to the data array */
        {
            data[index] = (char *)malloc(strlen(arg));
            if(!data[index]) /* Allocating memory for the new cell and copying the argument into it */
                print_error("memory allocation failed");
            strcpy(data[index++], arg);

            data = (char **)realloc(data, (index + 1) * sizeof(char *));
            if(!data) /* Reallocating memory for a new space for the next cell */
                print_error("memory allocation failed");
        } /* If we found a comment the actual line is over. In oreder to difference the value returned by the
             function, we send the counter to the length until the comment */
        if(comment)
            return index * comment;
    }
    return index;
}

/**
 * Gets a string, if exists, and copies it to the string variable
 * @param line - the assembly code line
 * @param string - the variable that the string, if exists, will be copied to
 * @param ptr - a pointer to the beginning quote of the string
 * @return ERROR or OK, according to the correctness of the string's syntax
 */
int get_string(char line[], char string[], char *ptr)
{
    char *end_ptr;
    int i = 0;
    if(!ptr) /* If there's no string in the line */
        return OK;

    end_ptr = strrchr(line, '\"'); /* Getting the ending quote of the string, if exists */
    if(end_ptr == ptr) /* If it doesn't exist, the result will be the beginning quote */
        return print_error("Missing \" before or after the string");

    for(; i <= (end_ptr - ptr); i++) /* Copying the string into the variable */
        string[i] = ptr[i];
    string[i] = '\0'; /* Adding end of string character */

    return OK;
}
