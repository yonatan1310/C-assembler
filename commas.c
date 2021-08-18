#include "assembler.h"
#include "ops_and_defs.h"

/**
 * Checks if there are multiple consecutive commas. If there are, it prints an error message
 * @return ERROR or OK, according to the correctness of the file's syntax
 */
int multiple_commas()
{
    int i = 1;
    for(; i < data_len; i++) /* Scanning the argument array */
    { /* If there are subsequent cells that contain comma, there are multiple consecutive commas */
        if(!strcmp(data[i], ",") && !strcmp(data[i - 1], ","))
            return print_error("Multiple consecutive commas");
    }
    return OK;
}

/**
 * Checks if there given string is a label
 * @param name - the string that is checked for being a label
 * @return 0 if it isn't and a non-zero value otherwise
 */
int is_label(char *name) { return name[strlen(name) - 1] == ':'; }

/**
 * Checks if there are illegal or missing commas. If there are, it prints an error message
 * @return ERROR or OK, according to the correctness of the file's syntax
 */
int illegal_and_missing_commas()
{
    int i;
    /* If there's a comma before the first argument (after the operation or definition), there's an illegal comma */
    if(!strcmp(data[0], ",") || (data_len > 2) ? !strcmp(data[1], ",") : 0 || ((is_label(*data) && data_len > 2) ? !strcmp(data[2], ",") : 0))
        return print_error("Illegal comma");

    for(i = (is_label(*data)) ? 3 : 2; i < data_len; i += 2)
    { /* If there aren't multiple consecutive commas, every other cell has to hold a comma in it */
        if(strcmp(data[i], ","))
            return print_error("Missing comma");
    }
    return OK;
}
