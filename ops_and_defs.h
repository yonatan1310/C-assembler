#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NUM_OPS 16 /* Number of operation in the assembly language */
#define NUM_DEFS 4 /* Number of definitions in the assembly language */

enum op_num{MOV, CMP, ADD, SUB, LEA, CLR, NOT, INC,
    DEC, JMP, BNE, RED, PRN, JSR, RTS, STOP}; /* Matching the operations in the assembly language to numbers */
enum def_num{DATA, STRING, EXTERN, ENTRY, CODE}; /* Matching the definitions in the assembly language to numbers */

typedef struct{
    int num;
    char *name;
} operation_definition; /* Matching between a name to a number, for the following data structures */

extern operation_definition definitions[] /* Holds each definition name and the number that matches it */,
operations[] /* Holds each operation name and the number that matches it */,
**extern_appearances /* Holds the appearances of external variables in the machine code */;

typedef struct symbol *ptr;
typedef struct symbol{ /* Represents a symbol in a linked list (Dynamic data structure) */
    char* name; /* The name of the symbol */
    int address; /* The index in the data table in which the symbol data is held, or 0 if it's an external variable */
    int type; /* The type of the symbol (to which type of line it is matched), according to the def_num enum */
    ptr next; /* The pointer to the next symbol in the list (or to NULL) if the symbol is the last in the table */
}symbol;
ptr symbol_table_head; /* The head of the symbol table */

enum are{E = 1, R, A = 4}; /* Creating constants for the ARE section in a machine code word */
enum addressing_num{IMMEDIATE, DIRECT, INDIRECT_REGISTER, DIRECT_REGISTER}; /* Numbers of addressing methods,
                                                                               represented by their name */

/* In file operations.c */
/**
 * In charge of converting assembly instruction line to binary code in the instruction_table
 * @param label - holds the name of the label matched to the instruction, or NULL if there isn't a label
 * @return ERROR or OK, according to the correctness of the line's syntax
 */
int insert_instruction_to_table(char *);

/**
 * Adds the address of the symbol to the symbol in the symbol_table
 * @return ERROR or OK, according to the correctness of the line's syntax
 */
int add_address_to_symbols();

/**
 * Extracts the number of operation, according to the string operand in the current index in the data variable
 * @return the number of the operation, or ERROR if the operation is undefined
 */
int get_operation();

/**
 * Gets the addressing method of the operand, stored in the operand parameter
 * @param operand - the given operand in the instruction line
 * @return the number of the addressing method, according to the addressing_num enum, or ERROR if it's given an invalid string
 */
int get_address_method(char *);

/**
 * Calculates the operands value, according to it's addressing method. received by the get_address function
 * @param operand_index - the index of data in which the operand's name is stored
 * @param address_method - the addressing method of the operand in data[operand_index]
 * @return the value of the operand, according to it's addressing method
 */
int get_operand_value(int, int);

/* In file definitions.c */
/**
 * Gets the definition stored in data[data_index]
 * @return the number of the definition, according to the def_num enum, or ERROR if the definition is undefined
 */
int get_definition();

/**
 * Inserts the data in instructions to the data_table
 * @param label - if not empty, the label matched to the definition
 * @return ERROR or OK, according to the correctness of the line's syntax
 */
int insert_data_to_table(char *);

/**
 * Checks if the number is between a certain range (to be held in 12 or 15 bits)
 * @param s - the string that holds the number
 * @return 1 - if the value has between 0 and 15 bits, 2 - if it also has up to 12 bits and 0 otherwise
 */
int is_valid_value(char *);

/**
 * Adds the data, in data and string definitions, to the data table
 * @param type - the type of the definition: DATA or STRING
 * @return ERROR or OK, according to the correctness of the line's syntax
 */
int add_to_data_table(int);

/* In file labels.c */
/**
 * Checks if the given label is valid or not
 * @param name - the name of the possible label
 * @return ERROR if the label is invalid and OK if it's valid
 */
int invalid_label(char *);

/**
 * Extracts a label (if it exists and it's valid) and inserts it to the label string
 * @param label - the string that will hold the label, if valid
 * @param is_second_pass - flag to check if we're in the second pass
 * @return ERROR if the label is invalid and OK if it's valid
 */
int get_label(char[], int);

/**
 * Adding a label to the symbol table
 * @param name - the name of the label
 * @param type - the type of the label, according to the def_num enum
 * @param address - the address of the new symbol in the memory
 * @return ERROR if there's another definition of the label and OK otherwise
 */
int add_to_symb_table(char *, int, int);

/**
 * Increases the address of each symbol with type of DATA by IC + 100
 */
void increase_address_to_data();

/**
 * Adds entry to a given symbol
 * @param label - the name of the desired symbol
 * @return ERROR if the label isn't defined in the code and OK otherwise
 */
int add_entry_to(char *);

/**
 * Getting the address of a given symbol
 * @param symbol - the symbol containing the desired address
 * @return the address of the symbol
 */
int get_address_from_symbol_table(char *);