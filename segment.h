/*
 *              ** segment.h **
 *    Authors: Adrien Lynch & Silas Reed
 *                 jlynch07 & sreed05
 *       Date: Nov 22, 2022
 * Assignment: HW6
 *    Summary: The Segment interface, with all relevant functions and libraries
 * 
 */

#ifndef SEGMENT_H
#define SEGMENT_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "assert.h"
#include "seq.h"


typedef struct Segments {
        /* 
         * Hanson sequence of mapped IDs
         * Note: index == emulator's 32-bit address representation,
         *       value at index == 64-bit (pointer) place in memory
         *       holding the segments themselves, from our computers
         */ 
        Seq_T mapped; 

        /* Hanson sequence of lengths of segments */
        Seq_T lengths;

        /* 
         * Hanson sequence of unmapped IDs 
         * Note: value at given index in sequence == available ID
         */
        Seq_T unmapped; 
} *Segments;


Segments segments_initialize();

uint32_t map_seg(Segments all_segments, uint32_t num_words);

void unmap_seg(Segments all_segments, uint32_t seg_ID);

void segments_free(Segments all_segments);



#endif /* SEGMENT_H */