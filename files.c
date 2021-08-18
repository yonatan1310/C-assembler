#include "assembler.h"
#include "ops_and_defs.h"

void write_output_files(char *);

/**
 * Opens the assembly files, calls the assembler programs (the passes), and the method that writes the output files
 * @param argc - the number of command line arguments
 * @param argv - the command line arguments
 */
void open_files(int argc, char *argv[])
{
    FILE *fptr;
    int i = 1;
    for(; i < argc; i++) /* Scanning argv from the second argument, the first file name to be checked */
    {
        /* Assigns memory for the name of the file and copies the name of the file, including the ".as" ending, to it */
        file_name = (char *)malloc(strlen(argv[i]) + strlen(".as"));
        if(!file_name)
            print_error("memory allocation failed");
        strcpy(file_name, argv[i]);
        strcat(file_name,".as");

        /* Opens the file, if it exists, for reading */
        fptr = fopen(file_name, "r");
        if(!fptr) /* If the wasn't found, or it isn't allowed for reading, the file pointer is NULL */
        {
            fprintf(stderr, "Couldn't open file %s\n", file_name);
            continue;
        }

        /* Starts the first and second passes, and if they are finished successfully, writes the output files */
        if(!first_pass(fptr) && !second_pass(fptr))
            write_output_files(argv[i]);

        fclose(fptr); /* Closes the file after reading and frees the file_name string for the next file name */
        free(file_name);
    }
}

/**
 * Gets a file pointer (with reading permission with the given name end ending
 * @param name - the file's name
 * @param ending - the file's ending
 * @return a file pointer to the new file
 */
FILE *get_file(char *name, char *ending)
{
    FILE *fptr;
    char *out_name = (char *)malloc(strlen(name) + strlen(ending));
    if(!out_name) /* Allocates memory for the file's name and copies the name and the ending into it */
        print_error("memory allocation failed");
    strcpy(out_name, name);
    strcat(out_name, ending);

    fptr = fopen(out_name, "w"); /* Opens the file, with writing permission */
    free(out_name); /* The file's name isn't needed anymore */
    return fptr;
}

/**
 * Writes the ".ob" file
 * @param name - the name of the given file, without ".as" ending
 */
void write_ob_file(char *name)
{
    FILE *fptr = get_file(name, ".ob"); /* Gets the file with ".ob" ending and writing permission */
    int i = 0;
    fprintf(fptr, "\t%d %d\n", IC, DC); /* Prints the number of instruction and data words */
    for(; i < IC; i++) /* First, prints each instruction word */
        fprintf(fptr, "%04d %05o\n", i + START_OF_CODE, instruction_table[i]->val);
    for(i = 0; i < DC; i++) /* Prints each data word */
        fprintf(fptr, "%04d %05o\n", i + IC + START_OF_CODE, data_table[i]->val);
    fclose(fptr); /* Frees the file pointer after use */
}

/**
 * Writes the ".ent" file
 * @param name - the name of the given file, without ".as" ending
 */
void write_ent_file(char *name)
{
    int first = 1;
    FILE *fptr;
    ptr p = symbol_table_head;
    while (p) /* Scanning the symbol_table */
    {
        if (p->type == ENTRY)
        {
            if(first) /* We have a first entry variable */
            {
                first = 0; /* To not enter this condition again */
                fptr = get_file(name, ".ent"); /* Gets the file with ".ent" ending and writing permission */
            } /* Prints every entry symbol, and its address, to the file */
            fprintf(fptr, "%s %d\n", p->name, p->address);
        }
        p = p->next;
    }
    fclose(fptr); /* Frees the file pointer after use */
}

/**
 * Writes the ".ext" file
 * @param name - the name of the given file, without ".as" ending
 */
void write_ext_file(char *name)
{
    FILE *fptr;
    int i = 0;
    if(!EC) /* There's no external variables appearances */
        return;
    fptr = get_file(name, ".ext"); /* Gets the file with ".ext" ending and writing permission */
    for(; i < EC; i++) /* Scanning the extern_appearances table and printing each appearance to the file */
        fprintf(fptr, "%s\t%04d\n", extern_appearances[i]->name, extern_appearances[i]->num);
    fclose(fptr); /* Frees the file pointer after use */
}

/**
 * Writes the output files for a given file
 * @param name - the name of the given file, without ".as" ending
 */
void write_output_files(char *name)
{ /* Calling to the methods that write each output file, with the name of the given file */
    write_ob_file(name);
    write_ent_file(name);
    write_ext_file(name);
}
