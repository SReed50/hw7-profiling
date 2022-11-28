/*
 *              ** instructions.h **
 *    Authors: Adrien Lynch & Silas Reed
 *                 jlynch07 & sreed05
 *       Date: Nov 22, 2022
 * Assignment: HW6
 *    Summary: The Instructions interface, with all relevant functions 
 *             and libraries
 * 
 */

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "assert.h"
#include "segment.h"
#include "bitpack.h"

extern uint32_t map_seg(Segments all_segments, uint32_t num_words);
extern void unmap_seg(Segments all_segments, uint32_t seg_ID);

void conditional_move(uint32_t *registers,
                      uint32_t ra,
                      uint32_t rb,
                      uint32_t rc);

void segmented_load(Segments all_segments,
                    uint32_t *registers,
                    uint32_t ra,
                    uint32_t rb,
                    uint32_t rc);

void segmented_store(Segments all_segments, 
                     uint32_t *registers,
                     uint32_t ra,
                     uint32_t rb,
                     uint32_t rc);

void addition(uint32_t *registers, uint32_t ra, uint32_t rb, uint32_t rc);

void multiplication(uint32_t *registers, uint32_t ra, uint32_t rb, uint32_t rc);

void division(uint32_t *registers, uint32_t ra, uint32_t rb, uint32_t rc);

void bitwise_NAND(uint32_t *registers, uint32_t ra, uint32_t rb, uint32_t rc);

/* no halt function */

void map_segment(Segments all_segments, 
                 uint32_t *registers, 
                 uint32_t rb, 
                 uint32_t rc);

void unmap_segment(Segments all_segments, uint32_t *registers, uint32_t rc);

void output(uint32_t *registers, uint32_t rc);

void input(uint32_t *registers, uint32_t rc);

void load_program(Segments all_segments, 
                  uint32_t *registers, 
                  uint32_t rb, 
                  uint32_t rc,
                  uint32_t *program_counter);

void load_value(uint32_t *registers, uint32_t ra, uint32_t val);


#endif /* INSTRUCTIONS_H */