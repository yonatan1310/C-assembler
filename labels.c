#include "ops_and_defs.h"
#include "assembler.h"
#include <ctype.h>
#define IS_NAME_IN(arr, ARR_SIZE)\
    for(i = 0; i < ARR_SIZE; i++)\
    {\
        if(!strcmp(name, arr[i].name))\
            return print_error("Invalid label");\
    }

/**
 * Checks if the given label is valid or not
 * @param name - the name of the possible label
 * @return ERROR if the label is invalid and OK if it's valid
 */
int invalid_label(char *name)
{
    int i = 0;
    for(; i < strlen(name); i++) /* Scanning the string */
    {
        /* The first character has to be a letter, and the other characters have to be letters or digits */
        if(!isalpha(name[i]) && ((i) ? !isdigit(name[i]) : 1))
            return print_error("Invalid label");
    }
    IS_NAME_IN(operations, NUM_OPS) /* The label cannot be an operation, or a register a definition */
    IS_NAME_IN(definitions, NUM_DEFS)
    if(strlen(name) == 2 && name[0] == 'r' && name[1] >= '0' && name[1] < '8')
        return print_error("Invalid label");
    return OK;
}

/**
 * Extracts a label (if it exists and it's valid) and inserts it to the label string
 * @param label - the string that will hold the label, if valid
 * @param is_second_pass - flag to check if we're in the second pass
 * @return ERROR if the label is invalid and OK if it's valid
 */
int get_label(char label[], int is_second_pass)
{
    /* The there's no operand, or the last character isn't ':' (if we're in the first pass, there's no label */
    if(data + data_index && (data[data_index][strlen(data[data_index]) - 1] == ':' || is_second_pass))
    {
        /* If we're in the first pass, we need to remove the ':' at the end of the label */
        if(!is_second_pass)
            data[data_index][strlen(data[data_index]) - 1] = '\0';

        if(invalid_label(data[data_index])) /* If the label isn't valid we return an error */
            return ERROR;

        strcpy(label, data[data_index++]); /* Copying the data to the label string */
    } else *label = '\0'; /* If there's no label, we put end of string character at the beginning of the string */
    return OK;
}

/**
 * Adding a label to the symbol table
 * @param name - the name of the label
 * @param type - the type of the label, according to the def_num enum
 * @param address - the address of the new symbol in the memory
 * @return ERROR if there's another definition of the label and OK otherwise
 */
int add_to_symb_table(char* name, int type, int address)
{
    ptr p, tail, node = (ptr)malloc(sizeof(symbol)); /* Allocating memory for the new symbol */
    if(!node)
        print_error("memory allocation failed");

    node->name = (char *)malloc(strlen(name) + 1); /* Allocating memory for the symbol's name */
    if(!(node->name))
        print_error("memory allocation failed");

    strcpy(node->name, name); /* Copying the symbols attributes */
    node->address = address;
    node->type = type;
    node->next = NULL; /* The node is now the tail of the symbol_table */

    if(!symbol_table_head) /* If the symbol_table is empty, the new node will be the head of the symbol_table */
    {
        symbol_table_head = node;
        return OK;
    }
    p = symbol_table_head;
    while(p) /* Otherwise, we scan the list until we reach it's tail */
    {
        if(!strcmp(p->name, node->name)) /* If we have another definition of the same symbol, we have an error */
        {
            if(p->type == EXTERN && node->type == EXTERN)
            {
                free(node->name);
                free(node);
                return OK;
            }
            return print_error("Multiple definitions of same label");
        }

        if(!(p->next))
            tail = p;
        p = p->next;
    }
    tail->next = node; /* The new tail will be the new symbol */
    return OK;
}

/**
 * Increases the address of each symbol with type of DATA by IC + 100
 */
void increase_address_to_data()
{
    ptr p = symbol_table_head;
    while(p) /* Scanning the symbol_table */
    {
        if(p->type == DATA) /* If the symbol's type is data, we increase its address by IC + 100 */
            p->address += IC + 100;
        p = p->next;
    }
}

/**
 * Adds entry to a given symbol
 * @param label - the name of the desired symbol
 * @return ERROR if the label isn't defined in the code and OK otherwise
 */
int add_entry_to(char *label)
{
    ptr p = symbol_table_head;
    while(p) /* Scanning the symbol_table */
    {
        if(!strcmp(p->name, label)) /* If we found the symbol, we change its type to ENTRY */
        {
            p->type = ENTRY;
            return OK;
        }
        p = p->next;
    } /* If we haven't found the symbol, the symbol isn't defined */
    return print_error("Label name in entry definition isn't defined");
}

/**
 * Getting the address of a given symbol
 * @param symbol - the symbol containing the desired address
 * @return the address of the symbol
 */
int get_address_from_symbol_table(char *symbol)
{
    ptr p = symbol_table_head;
    while(p) /* Scanning the symbol_table */
    {
        /* If we found the symbol, we insert its address to the certain location in the instruction_table */
        if(!strcmp(p->name, symbol))
        {
            set_val(instruction_table[IC], (p->type == EXTERN) ? 0 : p->address);
            set_are(instruction_table[IC], (p->type == EXTERN) ? E : R); /* Setting the are according the symbol's type */
            return p->address;
        }
        p = p->next;
    } /* If we haven't found the symbol, the symbol isn't defined */
    return print_error("Label name in operation isn't defined");
}
