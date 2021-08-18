#include "assembler.h"
#include "ops_and_defs.h"

enum length{ARE_LEN = 3, DEST_LEN_IN_OP = 4, SRC_LEN_IN_OP = 4, OPCODE_LEN = 4, DEST_LEN_IN_EXTRA_WORD = 3,
            SRC_LEN_IN_EXTRA_WORD = 3, VAL_LEN = 12}; /* The length of each part in the machine code word */

/* The first_bit of each part in the machine code word */
enum first_bit{ARE_FIRST_BIT, DEST_FIRST_BIT = 3, SRC_FIRST_BIT_IN_OP = 7, OPCODE_FIRST_BIT = 11,
               SRC_FIRST_BIT_IN_EXTRA_WORD = 6, VAL_FIRST_BIT = 3};

void set(word *w, int num, int first_bit, int length)
{
    w->val &= ~(((1 << length) - 1) << first_bit); /* Using bitwise operations to change the specific bits */
    w->val += num * (1 << first_bit);
}

/**
 * Gets a pointer to a word, and sets the ARE
 * @param w - the pointer to the word that its are will be changed
 * @param num - the number that will the word's new are
 */
void set_are(word *w, int num) {set(w, num, ARE_FIRST_BIT, ARE_LEN);}

/**
 * Sets the destination in the word's opcode according to the given number
 * @param w - the pointer to the word that its destination will be changed
 * @param num - the number that will the word's new destination
 */
void set_dest_in_op(word *w, int num) {set(w, num, DEST_FIRST_BIT, DEST_LEN_IN_OP);}

/**
 * Sets the destination in the extra word according to the given number
 * @param w - the pointer to the word that its destination will be changed
 * @param num - the number that will the word's new destination
 */
void set_dest_in_extra_word(word *w, int num) {set(w, num, DEST_FIRST_BIT, DEST_LEN_IN_EXTRA_WORD);}

/**
 * Sets the source in the word's opcode according to the given number
 * @param w - the pointer to the word that its source will be changed
 * @param num - the number that will the word's new source
 */
void set_src_in_op(word *w, int num) {set(w, num, SRC_FIRST_BIT_IN_OP, SRC_LEN_IN_OP);}

/**
 * Sets the source in the extra word according to the number
 * @param w - the pointer to the word that its source will be changed
 * @param num - the number that will the word's new source
 */
void set_src_in_extra_word(word *w, int num) {set(w, num, SRC_FIRST_BIT_IN_EXTRA_WORD, SRC_LEN_IN_EXTRA_WORD);}

/**
 * Sets the word's opcode according to the number
 * @param w - the pointer to the word that its opcode will be changed
 * @param num - the number that will the word's new opcode
 */
void set_opcode(word *w, int num) {set(w, num, OPCODE_FIRST_BIT, OPCODE_LEN);}

/**
 * Sets the value in the word according to the number
 * @param w - the pointer to the word that its value will be changed
 * @param num - the number that will the word's new value
 */
void set_val(word *w, int num) {set(w, num, VAL_FIRST_BIT, VAL_LEN);}
