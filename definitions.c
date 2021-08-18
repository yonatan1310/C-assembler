#include "ops_and_defs.h"
#include "assembler.h"
#include <math.h>
#define DATA_TABLE_LEN 100

/* Holds each definition name and the number that matches it */
operation_definition definitions[] = {{DATA, ".data"}, {STRING, ".string"},
                                      {EXTERN, ".extern"}, {ENTRY, ".entry"}};
word *data_table[DATA_TABLE_LEN];

/**
 * Gets the definition stored in data[data_index]
 * @return the number of the definition, according to the def_num enum, or ERROR if the definition is undefined
 */
int get_definition()
{
    int i = 0;
    for(; i < NUM_DEFS; i++)
    { /* If the string matches one of the definition names, we return the definition's number */
        if(!strcmp(data[data_index], definitions[i].name))
            return i;
    } /* If the current definition doesn't match any of the definition names, the definition is undefined */
    return ERROR;
}

/**
 * Inserts the data in instructions to the data_table
 * @param label - if not empty, the label matched to the definition
 * @return ERROR or OK, according to the correctness of the line's syntax
 */
int insert_data_to_table(char *label)
{
    int def = get_definition(); /* Getting the definition number */
    if (def == ERROR)
        return 2 * def;

    data_index++; /* Incrementing data_index to get to the data or the symbol name */
    if(*label)
    {
        if(def == ENTRY || def == EXTERN) /* Label cannot be matched to entry or extern definitions */
            fprintf(stdout, "Warning - in file %s in line no. %d: Label before entry or extern definitions\n", file_name, line_num);
        else
        { /* Adding the label matched to data or string definitions to the symbol table */
            if (add_to_symb_table(label, DATA, DC))
                return ERROR;
        }
    }
    if(def != ENTRY) /* Entry definitions will be treated in the second pass */
    {
        if(def != EXTERN) /* If the instruction is data or string, we add the data of the instruction to the data table */
            return add_to_data_table(def);
        else
        { /* If the instruction is extern, we add the following symbol to the symbol table */
            if(add_to_symb_table(data[data_index], EXTERN, 0))
                return ERROR;
        }
    }
    return OK;
}

/**
 * Checks if the number is between a certain range (to be held in 12 or 15 bits)
 * @param s - the string that holds the number
 * @return 1 - if the value has between 0 and 15 bits, 2 - if it also has up to 12 bits and 0 otherwise
 */
int is_valid_value(char *s)
{
    int num = atoi(s) /* The number in the string (which was checked for validity) */, num_bits;
    if(num == 0 || num == -1)
        num_bits = 2;
    else /* Calculating, using log to base 2, the number of bits the number takes */
        num_bits = ((num > 0) ? (int)(log(num)/log(2)) : (int)(log(-num - 1)/log(2))) + 2;

    /* Returns the corresponding value */
    return (num_bits >= 0 && num_bits <= 15) ? ((num_bits <= 12) ? 2 : 1) : 0;
}

/**
 * Adds the data, in data and string definitions, to the data table
 * @param type - the type of the definition: DATA or STRING
 * @return ERROR or OK, according to the correctness of the line's syntax
 */
int add_to_data_table(int type)
{
    char *ptr, *invalid;
    if(data_index == data_len) /* We already reached the end of the line */
        return print_error("Missing values");
    switch(type)
    {
        case DATA:
            for (; data_index < data_len; data_index += 2) /* Every other operand is a comma */
            {
                strtol(data[data_index], &invalid, 10); /* Invalid holds the invalid part */
                if(!invalid[0]) /* If there's no invalid part, we check if the number fits 12 bits, including MSB as sign flag */
                {
                    if(!is_valid_value(data[data_index]))
                        print_error("Number overflow or underflow");
                } else return print_error("Not an integer"); /* If there's an invalid part, the operand isn't an integer */

                data_table[DC] = (word *)calloc(1, sizeof(word)); /* Allocating memory for each integer */
                if(!data_table[DC])
                    print_error("memory allocation failed");
                data_table[DC++]->val = atoi(data[data_index]);
            }
            data_index -= 2; /* In order to check for extra text */
            break;
        case STRING:
            if(*data[data_index] != '\"') /* The first character after the definition has to be " */
                return print_error("Invalid code before string");
            ptr = data[data_index];
            ptr++;
            while(*(ptr + 1)) /* Scanning the characters in the string */
            {
                data_table[DC] = (word *)calloc(1, sizeof(word));
                if(!data_table[DC])
                    print_error("memory allocation failed");
                data_table[DC++]->val = *(ptr++); /* Allocating memory for each character and inserting the current one */
            }
            data_table[DC] = (word *)calloc(1, sizeof(word));
            if(!data_table[DC])
                print_error("memory allocation failed");
            data_table[DC++]->val = 0; /* Allocating memory for each character and inserting end of string */
    }
    return extra_text_after_end_of_command(); /* Checking for text after the end of the definition */
}
