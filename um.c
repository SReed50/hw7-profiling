/*
 *              ** um.c **
 *    Authors: Adrien Lynch & Silas Reed
 *                 jlynch07 & sreed05
 *       Date: Nov 22, 2022
 * Assignment: HW6
 *    Summary: Implementation of the Um interface,
 *             with all relevant functions and libraries
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>
#include "seq.h"

#include "segment.h"
#include "instructions.h"


#define NUM_REGISTERS 8

typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


uint32_t *read_file(char *pathname, Segments all_segments);

uint32_t fetch(uint32_t *seg0, uint32_t *program_counter);

void decode(uint32_t instruction, 
            Um_opcode *opcode, 
            uint32_t *ra, uint32_t *rb, uint32_t *rc, 
            uint32_t *val);


/*      main
 * Purpose: drive the program by initializing structures, preparing
 *          the passed file, & running the instruction loop
 * Expectations: arguments have been specified appropriately
 * Input: number of command line arguments, content of arguments 
 * Output: 0, to signal program completed & exited successfully
 */
int main(int argc, char *argv[])
{
        /* confirm .um program provided */
        if (argc != 2) {
                fprintf(stderr, "Usage: ./um <input_file>\n");
                exit(EXIT_FAILURE);
        }
        
        /* initialize starting state */
        uint32_t registers[NUM_REGISTERS] = { 0 };
        uint32_t program_counter = 0;

        Segments all_segments = segments_initialize();
        uint32_t *segment_zero = read_file(argv[1], all_segments);

        /* variables for fetch, decode, execute loop */
        uint32_t next_instruction;
        Um_opcode opcode = 0;
        uint32_t ra, rb, rc, val;

        /* check for HALT command before entering loop */
        while (opcode != HALT) { 
                next_instruction = fetch(segment_zero, &program_counter);

                decode(next_instruction, &opcode, &ra, &rb, &rc, &val);

                /* 
                 * with instructions fetched & decoded,
                 * EXECUTE: 
                 */
                if (opcode == CMOV) {
                        conditional_move(registers, ra, rb, rc);
                } else if (opcode == SLOAD) {
                        segmented_load(all_segments, registers, ra, rb, rc);
                } else if (opcode == SSTORE) {
                        segmented_store(all_segments, registers, ra, rb, rc);
                } else if (opcode == ADD) {
                        addition(registers, ra, rb, rc);
                } else if (opcode == MUL) {
                        multiplication(registers, ra, rb, rc);
                } else if (opcode == DIV){
                        division(registers, ra, rb, rc);
                } else if (opcode == NAND) {
                        bitwise_NAND(registers, ra, rb, rc);
                } else if (opcode == ACTIVATE) { /* map_segment */
                        map_segment(all_segments, registers, rb, rc);
                } else if (opcode == INACTIVATE) { /* unmap_segment */
                        unmap_segment(all_segments, registers, rc);
                } else if (opcode == OUT) {
                        output(registers, rc);
                } else if (opcode == IN) {
                        input(registers, rc);
                } else if (opcode == LOADP) {
                        load_program(all_segments, 
                                     registers, 
                                     rb, rc, 
                                     &program_counter);
                } else if (opcode == LV) {
                        load_value(registers, ra, val);
                }
                /* update segment zero to be correct */
                segment_zero = Seq_get(all_segments->mapped, 0);
        }

        /* clean memory & return */
        segments_free(all_segments);
        return EXIT_SUCCESS;
}


/*      read_file
 * Purpose: use stat tool to get details of .um file, then set up
 *          program by instantiating segment 0
 * Expectations: provided file is valid (.um), instance of Segments
 *               struct exists & is valid
 * Input: string holding filename specified on command line, 
 *        struct holding Hanson sequences to all segment items
 * Output: uint32_t holding segment 0 (the full program)
*/
uint32_t *read_file(char *pathname, Segments all_segments)
{
        assert(pathname != NULL);
        assert(all_segments != NULL);
        
        /* 
         * confirm scan of provided program successful, then get info 
         * Note: size of file divided by 4 since 32-bit word == 4 bytes,
         * so this gives us total number of 32-bit words
         */
        struct stat program_info;
        assert(stat(pathname, &program_info) == 0); 

        /* Note: will only read up to the last complete word in file */
        const int total_words = program_info.st_size / 4;

        map_seg(all_segments, total_words);

        FILE *fp = fopen(pathname, "r");
        assert(fp != NULL);

        /* write each word in segment one byte at a time (big endian order) */
        uint32_t *segment_zero = (uint32_t *)Seq_get(all_segments->mapped, 0);
        
        for (int word = 0; word < total_words; word++) {
                for (int lsb = 24; lsb >= 0; lsb -= 8) {
                        uint32_t piece = getc(fp);
                        segment_zero[word] = Bitpack_newu(segment_zero[word], 
                                                          8,
                                                          lsb,
                                                          piece);
                }
        }

        fclose(fp);
        return segment_zero;
}


/*      fetch
 * Purpose: get the word within segment 0 specified by program 
 *          counter, & return it
 * Expectations: segment 0 exists & holds the valid contents of
 *               the full program, program counter >= 0, & also
 *               program counter < total number of words
 * Input: pointer to segment 0, pointer to program counter
 * Output: current word to be unpacked & executed (type uint32_t)
*/
uint32_t fetch(uint32_t *seg0, uint32_t *program_counter)
{
        assert(seg0 != NULL);
        assert(program_counter != NULL);

        /* identify our next word in segment */
        uint32_t curr_word = seg0[*program_counter];
        (*program_counter)++;
        return curr_word;
}

/*      decode
 * Purpose: break down a um instruction into its parts cosisting
 *          of opcodes, register(s) and possibly a value
 * Expectations: all the pointers (opcode, registers, value) exist
 * Input: uint32_t representing the um instruction, pointers to the
 *        variables to hold the opcode, registers, and value
 * Output: N/A, void - end result: opcode, at least one register,
 *         and possibly the value are updated
 */
void decode(uint32_t instruction, 
            Um_opcode *opcode, 
            uint32_t *ra, uint32_t *rb, uint32_t *rc, 
            uint32_t *val)
{
        assert(opcode != NULL);
        assert(ra != NULL);
        assert(rb != NULL);
        assert(rc != NULL);
        assert(val != NULL);

        /* get opcode and register/value info based on instruction type */
        *opcode = Bitpack_getu(instruction, 4, 28);

        if (*opcode == LV) {
                *ra  = Bitpack_getu(instruction, 3, 25);
                *val = Bitpack_getu(instruction, 25, 0);
        } else {
                *ra = Bitpack_getu(instruction, 3, 6);
                *rb = Bitpack_getu(instruction, 3, 3);
                *rc = Bitpack_getu(instruction, 3, 0);
        }
}