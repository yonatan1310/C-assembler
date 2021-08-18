#include "ops_and_defs.h"
#include "assembler.h"
#define INSTRUCTION_TABLE_LEN 500

/* Holds each operation name and the number that matches it */
operation_definition operations[] = {{MOV, "mov"}, {CMP, "cmp"}, {ADD, "add"}, {SUB, "sub"},
                                     {LEA, "lea"}, {CLR, "clr"}, {NOT, "not"}, {INC, "inc"},
                                     {DEC, "dec"}, {JMP, "jmp"}, {BNE, "bne"}, {RED, "red"},
                                     {PRN, "prn"}, {JSR, "jsr"}, {RTS, "rts"}, {STOP, "stop"}};

word *instruction_table[INSTRUCTION_TABLE_LEN];

/**
 * Creating assembly code for an operation with 2 operands
 * @param op - the number of the operation
 * @return ERROR or OK, according to the correctness of the line's syntax
 */
int two_operands(int op)
{
    int address_method_src = get_address_method(data[data_index]), address_method_dest;
    int operand_value_src, operand_value_dest, both_operands_are_registers;
    if(address_method_src == ERROR) /* The source operand is invalid */
        return ERROR;

    address_method_dest = get_address_method(data[data_index + 2]);
    if(address_method_dest == ERROR) /* The destination operand is invalid */
        return ERROR;

    operand_value_src = get_operand_value(data_index, address_method_src); /* Getting the operands' values */
    operand_value_dest = get_operand_value(data_index + 2, address_method_dest);

    data_index += 3;
    if(extra_text_after_end_of_command()) /* Checking for extra text after the second operand, which is invalid */
        return ERROR;

    /* Checking if the addressing methods are valid, according to the operation */
    if((op == LEA && address_method_src != DIRECT) || (op != CMP && address_method_dest == IMMEDIATE))
        return print_error("Invalid addressing method for source or destination operand");

    instruction_table[IC] = (word *)calloc(1, sizeof(word)); /* Allocating memory for the machine code */
    instruction_table[IC + 1] = (word *)calloc(1, sizeof(word));
    both_operands_are_registers = (address_method_src >= INDIRECT_REGISTER && address_method_dest >= INDIRECT_REGISTER);
    if(!instruction_table[IC] || !instruction_table[IC + 1])
        print_error("memory allocation failed");
    if(!both_operands_are_registers) /* If both operands are registers, the instruction takes only 2 words */
    {
        instruction_table[IC + 2] = (word *)calloc(1, sizeof(word));
        if(!instruction_table[IC + 2])
            print_error("memory allocation failed");
    }

    set_opcode(instruction_table[IC], op); /* The first word holds the operation, the addressing methods and A in the are */
    set_src_in_op(instruction_table[IC], 1 << address_method_src);
    set_dest_in_op(instruction_table[IC], 1 << address_method_dest);
    set_are(instruction_table[IC++], A);

    if(address_method_src >= INDIRECT_REGISTER)
    {
        set_src_in_extra_word(instruction_table[IC], operand_value_src);
        set_are(instruction_table[IC], A); /* Register's extra word holds A in the are and the register number */
        if(both_operands_are_registers)
        { /* If both operands are registers, there's only one extra word that holds both registers' numbers */
            set_dest_in_extra_word(instruction_table[IC++], operand_value_dest);
            return OK;
        }
        if(address_method_dest == IMMEDIATE)
        { /* If the other operand is immediate, there's another extra word with A in the are and the number as the value */
            set_are(instruction_table[++IC], A);
            set_val(instruction_table[IC++], operand_value_dest);
            return OK;
        }
        IC += 2; /* Direct addressing method is treated in the second pass. For now the value in the extra word is 0 */
        return OK;
    }

    if(address_method_src == IMMEDIATE)
    { /* If the operand is immediate, there's another extra word with A in the are and the number as the value */
        set_val(instruction_table[IC], operand_value_src);
        set_are(instruction_table[IC++], A);
    } else IC++;
    if(address_method_dest == IMMEDIATE)
    { /* If the other operand is immediate, there's another extra word with A in the are and the number as the value */
        set_val(instruction_table[IC], operand_value_dest);
        set_are(instruction_table[IC++], A);
        return OK;
    }
    if(address_method_dest >= INDIRECT_REGISTER)
    { /* If the other operand is a register, there's another extra word with A in the are and the register's number */
        set_dest_in_extra_word(instruction_table[IC], operand_value_dest);
        set_are(instruction_table[IC++], A);
        return OK;
    }
    IC++; /* Direct addressing method is treated in the second pass. For now the value in the extra word is 0 */
    return OK;
}

int one_operand(int op)
{
    int address_method = get_address_method(data[data_index]), operand_value;
    if(address_method == ERROR) /* The operand is invalid */
        return ERROR;
    operand_value = get_operand_value(data_index++, address_method); /* Getting the operand's value */

    if(extra_text_after_end_of_command()) /* Checking for extra text after the operand, which is invalid */
        return ERROR;

    if(op != PRN) /* Checking if the addressing method is valid, according to the operation */
    {
        if(((op == JMP || op == BNE || op == JSR) && address_method == DIRECT_REGISTER) || address_method == IMMEDIATE)
            return print_error("Invalid addressing method for destination operand");
    }

    instruction_table[IC] = (word *)calloc(1, sizeof(word)); /* Allocating memory for the machine code */
    instruction_table[IC + 1] = (word *)calloc(1, sizeof(word));
    if(!instruction_table[IC] || !instruction_table[IC + 1])
        print_error("memory allocation failed");

    set_opcode(instruction_table[IC], op); /* The first word holds the operation, the addressing method and A in the are */
    set_dest_in_op(instruction_table[IC], 1 << address_method);
    set_are(instruction_table[IC++], A);

    if(address_method != DIRECT)
    { /* The extra word includes A in the are and the the value (The immediate number or the register's number) */
        set_are(instruction_table[IC], A);
        set_val(instruction_table[IC++], operand_value);
        return OK;
    }
    IC++; /* Direct addressing method is treated in the second pass. For now the value in the extra word is 0 */
    return OK;
}

int zero_operands(int op)
{
    if(extra_text_after_end_of_command()) /* Checking for extra text after the operation, which is invalid */
        return ERROR;

    instruction_table[IC] = (word *)calloc(1, sizeof(word)); /* Allocating memory for the machine code */
    if(!instruction_table[IC])
        print_error("memory allocation failed");

    set_opcode(instruction_table[IC], op); /* The only word holds the operation and A in the are */
    set_are(instruction_table[IC++], A);
    return OK;
}

/**
 * Extracts the number of operation, according to the string operand in the current index in the data variable
 * @return the number of the operation, or ERROR if the operation is undefined
 */
int get_operation()
{
    int i = 0;
    for(; i < NUM_OPS; i++)
    { /* If the string matches one of the operation names, we return the operation's number */
        if(!strcmp(data[data_index], operations[i].name))
            return i;
    } /* If the current operation doesn't match any of the operation names, the operation is undefined */
    return print_error("Undefined operation or definition name");
}

/**
 * In charge of converting assembly instruction line to binary code in the instruction_table
 * @param label - holds the name of the label matched to the instruction, or NULL if there isn't a label
 * @return ERROR or OK, according to the correctness of the line's syntax
 */
int insert_instruction_to_table(char *label)
{
    int op = get_operation(); /* Getting the operation number */
    if (op == ERROR)
        return ERROR;

    /* Incrementing data_index to get to the first operand or to the end of the valid line */
    if (*label) /* If there's a label matched to the current line, we'll add it to the symbol table */
        add_to_symb_table(label, CODE, IC + START_OF_CODE);

    if (op < 14) /* Operations 5 - 13 have 1 operand */
    {
        data_index++;
        if (op < 5) /* Operations 0 - 4 have 2 operands */
            return two_operands(op);
        return one_operand(op);
    }
    return zero_operands(op); /* Operations 15 and 16 have no operand */
}

/**
 * Gets the addressing method of the operand, stored in the operand parameter
 * @param operand - the given operand in the instruction line
 * @return the number of the addressing method, according to the addressing_num enum, or ERROR if it's given an invalid string
 */
int get_address_method(char *operand)
{
    char *invalid; /* To check if received a valid number, in case of immediate addressing method */
    int asterisk_flag, register_num;
    if(!data_len)
        return print_error("Missing operand");

    asterisk_flag = (*operand == '*'); /* In order to difference between direct and indirect registers */
    if(operand[(asterisk_flag) ? 1 : 0] == 'r')
    {
        if(strlen(operand) < ((asterisk_flag) ? 3 : 2))
            return ERROR; /* If the character holding the number is missing, the operand is invalid */
        /* Getting the number of the register */
        register_num = strtol(operand + ((asterisk_flag) ? 2 : 1), &invalid, 10);
        if(!invalid[0] && 0 <= register_num && register_num < 8) /* The register number is from 0 to 7 */
            /* Returns indirect register if there's an astrix, or direct register otherwise */
            return (asterisk_flag) ? INDIRECT_REGISTER : DIRECT_REGISTER;
    }

    if(*operand == '#') /* If the first character is '#', a valid operand will be a number */
    {
        strtol(operand + 1, &invalid, 10); /* Invalid holds the invalid part */
        if(!invalid[0]) /* If there's no invalid part, we check if the number fits 12 bits, including MSB as sign flag */
            return (is_valid_value(operand + 1) == 2) ? IMMEDIATE : print_error("Number overflow or underflow");
        return print_error("Not an integer"); /* If there's an invalid part, the operand isn't an integer */
    }

    /* The only option left is direct addressing. We check if the label is valid end return an appropriate value */
    return (invalid_label(operand)) ? ERROR : DIRECT;
}

/**
 * Calculates the operands value, according to it's addressing method. received by the get_address function
 * @param operand_index - the index of data in which the operand's name is stored
 * @param address_method - the addressing method of the operand in data[operand_index]
 * @return the value of the operand, according to it's addressing method
 */
int get_operand_value(int operand_index, int address_method)
{
    switch(address_method)
    {
        /* If the address method is immediate or direct register, we have only one character before the value */
        case IMMEDIATE:
        case DIRECT_REGISTER:
            return atoi(data[operand_index] + 1);

        case DIRECT:
            return 0; /* Unknown value in the first pass */

        case INDIRECT_REGISTER: /* If the address method is indirect register, we have two characters before the value */
            return atoi(data[operand_index] + 2);
    }
    return -1;
}

operation_definition **extern_appearances; /* Holds the appearances of external variables in the machine code */

/**
 * Adds an appearance of an extern symbol to the extern_appearances structure
 * @param name - the symbol's name
 * @param address - the address of the word in which the symbol appears
 */
void add_to_extern_appearances(char *name, int address)
{
    if(!extern_appearances) /* Allocating (or reallocating) memory for extern_appearances */
        extern_appearances = (operation_definition **)malloc(sizeof(operation_definition *));
    else
        extern_appearances = (operation_definition **)realloc(extern_appearances, (IC + 1) * sizeof(operation_definition *));
    if(!extern_appearances)
        print_error("memory allocation failed");

    /* Allocating memory for the current symbol and for it's name */
    extern_appearances[EC] = (operation_definition *)malloc(sizeof(operation_definition));
    if(!extern_appearances[EC])
        print_error("memory allocation failed");
    extern_appearances[EC]->name = (char *)malloc(strlen(data[data_index]) + 1);

    strcpy(extern_appearances[EC]->name, name); /* Copying the name an address to the structure */
    if(!(extern_appearances[EC]->name))
        print_error("memory allocation failed");
    extern_appearances[EC++]->num = address;
}

/**
 * Adds the address of the symbol to the symbol in the symbol_table
 * @return ERROR or OK, according to the correctness of the line's syntax
 */
int add_address_to_symbols()
{
    int op = get_operation() /* Gets the operation */, is_first_register, address;
    data_index++; /* Incrementing the data_index in order to get the first operand */
    if(op < 14) /* If the operation number is less then 14, there's at least one operand */
    {
        IC++; /* Gets the first extra word */
        is_first_register = (get_address_method(data[data_index]) >= INDIRECT_REGISTER); /* Checks if the first operand is a register */

        if(get_address_method(data[data_index]) == DIRECT) /* If the addressing method is direct, we found a symbol */
        {
            if((address = get_address_from_symbol_table(data[data_index])) < 0) /* The symbol isn't defined */
                return ERROR;

            if(!address) /* If the address is 0 - the symbol is extern, we'll add the appearance to the extern_appearances structure */
                add_to_extern_appearances(data[data_index], IC + 100);
        }

        if(op < 5) /* If the operation number is also less then 5, there are 2 operands */
        {
            data_index += 2; /* Gets the second operand */
            IC++; /* Gets the next word */

            if(is_first_register && get_address_method(data[data_index]) >= INDIRECT_REGISTER)
                 return OK; /* If both operands are registers, there isn't another extra word */

            if(get_address_method(data[data_index]) == DIRECT) /* If the addressing method is direct, we found a symbol */
            {
                if((address = get_address_from_symbol_table(data[data_index])) < 0) /* The symbol isn't defined */
                    return ERROR;

                if(!address) /* If the address is 0 - the symbol is extern, we'll add the appearance to the extern_appearances structure */
                    add_to_extern_appearances(data[data_index], IC + 100);
            }
        }
    }
    IC++; /* Gets the next word */
    return OK;
}
