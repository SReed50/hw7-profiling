/*
 *              ** instructions.c **
 *    Authors: Adrien Lynch & Silas Reed
 *                 jlynch07 & sreed05
 *       Date: Nov 22, 2022
 * Assignment: HW6
 *    Summary: Implementation of the Instructions interface,
 *             with all relevant functions and libraries
 *
 */
#include "instructions.h"


/*      conditional_move
 * Purpose: move value in rb over to ra
 * Expectations: value in rc != 0, registers contain valid values
 * Input: pointer to array of 8 registers, 3-bit values indicating
 *        corresponding register for ra, rb, rc respectively
 * Output: N/A, void - end result: value moved if rc != 0
 */
void conditional_move(uint32_t *registers,
                      uint32_t ra,
                      uint32_t rb,
                      uint32_t rc)
{
        if (registers[rc] != 0) {
                registers[ra] = registers[rb];
        }
}

/*      segmented_load
 * Purpose: load the word at offset rc, within segment rb, into
 *          the register corresponding to ra
 * Expectations: registers contain valid values, segment exists
 * Input: struct holding Hanson sequences to all segment items,
 *        pointer to array of 8 registers, 3-bit values indicating
 *        corresponding register for ra, rb, rc respectively
 * Output: N/A, void - end result: register ra updated to reflect
 *         value of given word in given segment
 */
void segmented_load(Segments all_segments,
                    uint32_t *registers,
                    uint32_t ra,
                    uint32_t rb,
                    uint32_t rc)
{
        /* identify word within specific segment */
        uint32_t *segment = Seq_get(all_segments->mapped, registers[rb]);
        registers[ra] = segment[registers[rc]];
}

/*      segmented_store
 * Purpose: take the word within register rc, & store it within 
 *          segment ra, at the offset marked by rb
 * Expectations: registers contain valid values, segment exists
 * Input: struct holding Hanson sequences to all segment items,
 *        pointer to array of 8 registers, 3-bit values indicating
 *        corresponding register for ra, rb, rc respectively
 * Output: N/A, void - end result: word at given offset within
 *         given segment updated to reflect value in register 
 *         corresponding to rc
 */
void segmented_store(Segments all_segments,
                     uint32_t *registers,
                     uint32_t ra,
                     uint32_t rb,
                     uint32_t rc)
{
        /* update value of word within specific segment */
        uint32_t *segment = Seq_get(all_segments->mapped, registers[ra]);
        segment[registers[rb]] = registers[rc];
}

/*      addition
 * Purpose: add values in registers corresponding to rb & rc (mod 2^32), 
 *          then store result in register ra
 * Expectations: registers contain valid values
 * Input: pointer to array of 8 registers, 3-bit values indicating
 *        corresponding register for ra, rb, rc respectively
 * Output: N/A, void - end result: sum of rb & rc stored in ra
 */
void addition(uint32_t *registers, uint32_t ra, uint32_t rb, uint32_t rc)
{
        /* sum is mod 2^32 as a result of variable size */
        registers[ra] = registers[rb] + registers[rc];
}

/*      multiplication
 * Purpose: multiply values in registers corresponding to rb & rc 
 *          (mod 2^32), then store result in register ra
 * Expectations: registers contain valid values
 * Input: pointer to array of 8 registers, 3-bit values indicating
 *        corresponding register for ra, rb, rc respectively
 * Output: N/A, void - end result: product of rb & rc stored in ra
 */
void multiplication(uint32_t *registers, uint32_t ra, uint32_t rb, uint32_t rc)
{
        /* product is mod 2^32 as a result of variable size */
        registers[ra] = registers[rb] * registers[rc];
}

/*      division
 * Purpose: divide value in registers corresponding to rb by that
 *          of rc, then store result in register ra
 * Expectations: rc != 0, registers contain valid values
 * Input: pointer to array of 8 registers, 3-bit values indicating
 *        corresponding register for ra, rb, rc respectively
 * Output: N/A, void - end result: quotient of rb & rc stored in ra
 */
void division(uint32_t *registers, uint32_t ra, uint32_t rb, uint32_t rc)
{
        /* confirm division is possible */
        assert(registers[rc] != 0);
        registers[ra] = registers[rb] / registers[rc];
}

/*      bitwise_NAND
 * Purpose: get the bitwise and (&) of rb & rc, then invert it, &
 *          store resulting value in ra
 * Expectations: registers contain valid values
 * Input: pointer to array of 8 registers, 3-bit values indicating
 *        corresponding register for ra, rb, rc respectively
 * Output: N/A, void - end result: inverse of bitwise and of rb
 *         & rc stored in ra
 */
void bitwise_NAND(uint32_t *registers, uint32_t ra, uint32_t rb, uint32_t rc)
{
        registers[ra] = ~(registers[rb] & registers[rc]);
}

/*      map_segment
 * Purpose: create a new segment based on number of words specified 
 *          by value in rc, which each word initialized to 0, then
 *          stored segment ID in register corresponding to rb
 * Expectations: registers contain valid values, number of words > 0
 * Input: struct holding Hanson sequences to all segment items,
 *        pointer to array of 8 registers, 3-bit values indicating
 *        corresponding register for rb, rc respectively
 * Output: N/A, void - end result: new segment allocated, & its 
 *         ID is stored for later reference
 */
void map_segment(Segments all_segments,
                 uint32_t *registers, 
                 uint32_t rb, 
                 uint32_t rc)
{
        /* Note: registers[rc] represents number of words in segment */
        registers[rb] = map_seg(all_segments, registers[rc]);
}

/*      unmap_segment
 * Purpose: deallocate an existing segment based on the identifier
 *          (segment ID) specified in register corresponding to rc
 * Expectations: registers contain valid values, segment already exists
 * Input: struct holding Hanson sequences to all segment items,
 *        pointer to array of 8 registers, 3-bit value indicating
 *        corresponding register for rc 
 * Output: N/A, void - end result: existing segment is deallocated,
 *         & its ID is stored for reuse by a new segment
 */
void unmap_segment(Segments all_segments, uint32_t *registers, uint32_t rc)
{
        /* Note: registers[rc] represents segment ID */
        unmap_seg(all_segments, registers[rc]);
}

/*      output
 * Purpose: write the value held by rc to I/O
 * Expectations: registers contain valid values (char from 0-255)
 * Input: pointer to array of 8 registers, 3-bit value indicating
 *        corresponding register for rc 
 * Output: N/A, void - end result: value between 0-255 is outputted
 */
void output(uint32_t *registers, uint32_t rc)
{
        if (registers[rc] <= 255) {
                putchar(registers[rc]);
        } 
}

/*      input
 * Purpose: get input from I/O, then load value into rc .. if EOF,
 *          then fill rc with 32-bit word of all 1s
 * Expectations: input is valid (char from 0-255)
 * Input: pointer to array of 8 registers, 3-bit value indicating
 *        corresponding register for rc 
 * Output: N/A, void - end result: value between 0-255 is inputted 
 */
void input(uint32_t *registers, uint32_t rc) {
        registers[rc] = getchar();
}

/*      load_program
 * Purpose: duplicate the segment indicated by rb, and replace segment
 *          zero with it, set the program counter to the offset indicated
 *          by rc
 * Expectations: registers contain valid values, segment exists
 * Input: struct holding Hanson sequences to all segment items,
 *        pointer to array of 8 registers, 3-bit values indicating
 *        corresponding register for rb and rc respectively, pointer
 *        to the program_counter
 * Output: N/A, void - end result: segment 0 is replaced by the segment
 *         indicated by rb and the program counter is set to the offset
 *         given by rc
 */
void load_program(Segments all_segments,
                  uint32_t *registers,
                  uint32_t rb,
                  uint32_t rc,
                  uint32_t *program_counter)
{
        /* get segment at ID stored in rb (and its length) & duplicate it */
        uint32_t *new_segment = Seq_get(all_segments->mapped, registers[rb]);
        uint32_t num_words = *(uint32_t *)Seq_get(all_segments->lengths, 
                                                  registers[rb]);

        uint32_t *new_duplicate = malloc(sizeof(num_words) * num_words);
        assert(new_duplicate != NULL);

        for (uint32_t word = 0; word < num_words; word++) {
                new_duplicate[word] = new_segment[word];
        }

        /* set segment 0 to be the new segment with the correct length */
        uint32_t *segment_0 = Seq_put(all_segments->mapped, 0, new_duplicate);

        uint32_t *num_words_p = malloc(sizeof(num_words));
        assert(num_words_p != NULL);
        
        *num_words_p = num_words;
        uint32_t *old_length = Seq_put(all_segments->lengths, 0, num_words_p);

        /* free the old segment 0 */
        free(segment_0);
        free(old_length);

        /* update program counter */
        *program_counter = registers[rc];
}

/*      load_value
 * Purpose: take the value at the end of the instruction and put it in
 *          the given register
 * Expectations: ra corresponds to one of the 8 registers
 * Input: pointer to array of 8 registers, 3-bit value indicating
 *        corresponding register for ra, 25-bit value to be stored
 *        in the register
 * Output: N/A, void - end result: given register is updated to hold
 *         the given value
 */
void load_value(uint32_t *registers, uint32_t ra, uint32_t val)
{
        registers[ra] = val;
}
