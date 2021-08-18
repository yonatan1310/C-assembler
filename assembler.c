#include "assembler.h"
#include "ops_and_defs.h"

void free_global_structures();

int main(int argc, char *argv[])
{
    open_files(argc, argv); /* Calls to the open file method to start the process */
    free_global_structures(); /* Frees the memory allocation of global structures */
    return 0;
}

/**
 * Prints an error, given by the parameter of the character pointer, to stderr
 * error - the error we want to print
 * return ERROR - there's an error in the line
 */
int print_error(char *error)
{
    /* Prints the error, including the file name and the number of the line in the file */
    fprintf(stdout, "Error - in file %s in line no. %d: %s\n", file_name, line_num, error);
    return ERROR;
}

/**
 * Checks if there's invalid text after the end of the command
 * @return ERROR if there's extra text, or OK otherwise
 */
int extra_text_after_end_of_command()
{
    /* The index of the last argument of the valid line should be 1 less than the data length */
    return (data_index < data_len - 1) ? print_error("Extra text after end of command") : 0;
}

/**
 * Frees the memory allocation of global structures
 */
void free_global_structures()
{
    int i = 0;
    ptr p = symbol_table_head, next;

    for(; i < IC; i++) /* Frees the instruction_table */
        free(instruction_table[i]);

    for(i = 0; i < DC; i++) /* Frees the data_table */
        free(data_table[i]);

    for(i = 0; i < EC; i++) /* Frees the extern_appearances table and the structure itself */
    {
        free(extern_appearances[i]->name);
        free(extern_appearances[i]);
    }
    free(extern_appearances);

    for(i = 0; i < data_len; i++) /* Frees the data arguments and the data itself */
        free(data[i]);
    free(data);
    while(p) /* Frees the symbol table */
    {
        next = p->next;
        free(p);
        p = next;
    }
}
