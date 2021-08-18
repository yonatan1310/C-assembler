#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define START_OF_CODE 100 /* The address of the first word in the code */
#define LINE_LEN 82 /* The maximal length of an assembly code line */
#define MAX_ARG_SIZE 75 /* The maximal length of an argument (including strings) */

enum status{ERROR = -1, OK = 0};

int IC /* Instruction counter */, DC /* Data counter */, EC /* Extern variables appearances counter */;

char *file_name /* The name of the current file */;
int line_num /* The line number in the current file */;

char **data /* Holds the array of string arguments, parsed by function parse_line, in order to make the analysis easier */;
int data_index /* The index of the current parsed argument in data */ , data_len /* The length of the data array */;


/**
 * Preforms the first pass on the given file, searching for most of the syntax errors,
 * and translating most of the .as file into machine code
 * @param fptr - a pointer to the given file
 * @return ERROR or OK, according to the correctness of the file's syntax
 */
int first_pass(FILE *);

/**
 * Preforms the second pass on the given file, searching for the rest of syntax errors.
 * mainly responsible for extracting label information from the symbol table and inserting it into the machine code
 * @param fptr - a pointer to the given file
 * @return ERROR or OK, according to the correctness of the file's syntax
 */
int second_pass(FILE *);

/**
 * Prints an error, given by the parameter of the character pointer, to stderr
 * error - the error we want to print
 * return ERROR - there's an error in the line
 */
int print_error(char *);

/**
 * Checks if there's invalid text after the end of the command
 * @return ERROR if there's extra text, or OK otherwise
 */
int extra_text_after_end_of_command();

/* In file files.c */
/**
 * Opens the assembly file, calls the assembler programs (the passes), and the method that writes the output files
 * @param argc - the number of command line arguments
 * @param argv - the command line arguments
 */
void open_files(int argc, char *argv[]);

/* In file parse.c */
/**
 * Parses an assembly code line based on white spaces, commas and strings
 * @param line - the assembly code line
 * @return the number of operands or ERROR if there's an invalid string argument (missing ")
 */
int parse_line(char[]);

/* In file commas.c */
/**
 * Checks if there are multiple consecutive commas. If there are, it prints an error message
 * @return ERROR or OK, according to the correctness of the file's syntax
 */
int multiple_commas();

/**
 * Checks if there are illegal or missing commas. If there are, it prints an error message
 * @return ERROR or OK, according to the correctness of the file's syntax
 */
int illegal_and_missing_commas();

/* In file word.c */
typedef struct{ /* This struct represents a word in the assembly code */
    unsigned int val : 15;
}word;
extern word *instruction_table[], *data_table[];

/**
 * Gets a pointer to a word, and sets the ARE
 * @param w - the pointer to the word that its are will be changed
 * @param num - the number that will the word's new are
 */
void set_are(word *, int);

/**
 * Sets the destination in the word's opcode according to the given number
 * @param w - the pointer to the word that its destination will be changed
 * @param num - the number that will the word's new destination
 */
void set_dest_in_op(word *, int);

/**
 * Sets the destination in the extra word according to the given number
 * @param w - the pointer to the word that its destination will be changed
 * @param num - the number that will the word's new destination
 */
void set_dest_in_extra_word(word *, int);

/**
 * Sets the source in the word's opcode according to the given number
 * @param w - the pointer to the word that its source will be changed
 * @param num - the number that will the word's new source
 */
void set_src_in_op(word *, int);

/**
 * Sets the source in the extra word according to the number
 * @param w - the pointer to the word that its source will be changed
 * @param num - the number that will the word's new source
 */
void set_src_in_extra_word(word *, int);

/**
 * Sets the word's opcode according to the number
 * @param w - the pointer to the word that its opcode will be changed
 * @param num - the number that will the word's new opcode
 */
void set_opcode(word *, int);

/**
 * Sets the value in the word according to the number
 * @param w - the pointer to the word that its value will be changed
 * @param num - the number that will the word's new value
 */
void set_val(word *, int);
