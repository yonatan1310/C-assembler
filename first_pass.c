#include "assembler.h"
#include "ops_and_defs.h"

/**
 * Preforms the first pass on the given file, searching for most of the syntax errors,
 * and translating most of the .as file into machine code
 * @param fptr - a pointer to the given file
 * @return ERROR or OK, according to the correctness of the file's syntax
 */
int first_pass(FILE *fptr)
{
    char line[LINE_LEN], label[MAX_ARG_SIZE];
    int error = 0, def_error = 0;
    IC = 0, DC = 0;

    for(line_num = 1; fgets(line, LINE_LEN, fptr); line_num++) /* Scanning through each line of the file */
    {
        data_index = 0;
        data_len = parse_line(line); /* Getting the line arguments after parsing */
        if(data_len <= 0) /* If the line is empty, or there's an error, we continue to the next line */
        {
            if(data_len == ERROR)
                error = ERROR; /* Updating if there's an error */
            continue;
        }

        if(multiple_commas() || illegal_and_missing_commas()) /* There's an issue with the commas in the line */
        {
            error = ERROR;
            continue;
        }

        if(get_label(label, 0)) /* Gets the label of this line, if exists. If the label is invalid, there's an error */
        {
            error = ERROR;
            continue;
        } if(data_index == data_len) continue;

        if((def_error = insert_data_to_table(label)) == ERROR)
        { /* If there's a syntax error in a directive sentence, we continue to the next line */
            error = ERROR;
            continue;
        }

        /* If there wasn't a directive sentence, a we found a syntax error in an instruction sentence, we continue to the next line */
        if(def_error && insert_instruction_to_table(label))
            error = ERROR;
    }

    if(!error) /* If the code in the file valid so far, we increase the addresses of the data variables */
        increase_address_to_data();

    rewind(fptr); /* Rewinds the file for the second pass and returns a flag regarding a spotted error in the file */
    return error;
}
