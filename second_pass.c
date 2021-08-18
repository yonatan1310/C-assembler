#include "assembler.h"
#include "ops_and_defs.h"

/**
 * Preforms the second pass on the given file, searching for the rest of syntax errors.
 * mainly responsible for extracting label information from the symbol table and inserting it into the machine code
 * @param fptr - a pointer to the given file
 * @return ERROR or OK, according to the correctness of the file's syntax
 */
int second_pass(FILE *fptr)
{
    char line[LINE_LEN], label[MAX_ARG_SIZE];
    int error = OK, def;
    IC = 0; EC = 0;

    for(line_num = 1; fgets(line, LINE_LEN, fptr); line_num++) /* Scanning through each line of the file */
    {
        data_index = 0;
        data_len = parse_line(line); /* Getting the line arguments after parsing */
        if(data_len == 0) /* If the line is empty, we continue to the next line */
            continue;

        if(get_label(label, 0)) /* Gets the label of this line, if exists. If the label is invalid, there's an error */
        {
            error = ERROR;
            continue;
        }


        if((def = get_definition()) != ERROR) /* Checks if there's any definition identified */
        {
            if(def != ENTRY) /* Checks if the line has entry definition in it */
                continue;
            data_index++; /* Incrementing data_index to get the entry label */

            /* Gets the label in the entry definition. If the label is invalid, there's an error */
            if(get_label(label, 1))
            {
                error = ERROR;
                continue;
            }

            if(add_entry_to(label)) /* If there ins't a symbol with the given name, there's an error */
                error = ERROR;
        }
        else
        { /* If one of the symbol isn't defined, we have an error
             Otherwise, we add the address of each symbol to its appearance in the instruction table */
            if(add_address_to_symbols())
                error = ERROR;
        }
    }
    return error; /* Returns a flag regarding an error in the file */
}
