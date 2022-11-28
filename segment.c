/*
 *              ** Segment.c **
 *    Authors: Adrien Lynch & Silas Reed
 *                 jlynch07 & sreed05
 *       Date: Nov 22, 2022
 * Assignment: HW6
 *    Summary: Implementation of the Segment interface,
 *             with all relevant functions and libraries
 *
 */

#include "segment.h"


/*      segments_initialize
 * Purpose: create Hanson sequences to hold mapped IDs, their segment's 
 *          respective lengths, & unmapped/recycled IDs
 * Expectations: structures do not yet exist, we're at start of program
 * Input: N/A, none
 * Output: instance of Segments struct, containing 3 Hanson sequences
 */
Segments segments_initialize()
{
        Segments all_segments = malloc(sizeof(*all_segments));
        assert(all_segments);

        /* create Hanson sequence to hold the segments */
        all_segments->mapped = Seq_new(0);

        /* create Hanson sequence for unmapped/recycled IDs */
        all_segments->unmapped = Seq_new(0);

        /* create Hanson sequence for lengths of segments */
        all_segments->lengths = Seq_new(0);

        return all_segments;
}


/*      map_seg
 * Purpose: handle the finer details of allocating space on the heap
 *          for a new segment, based on number of words specified, 
 *          while taking advantage of any recycled IDs
 * Expectations: instance of Segments struct exists & is valid, 
 *               number of words > 0
 * Input: struct holding Hanson sequences to all segment items,
 *        uint32_t specifying number of words
 * Output: N/A, void - end result: new segment allocated, & its 
 *         ID is stored for later reference
 */
uint32_t map_seg(Segments all_segments, uint32_t num_words)
{
        assert(all_segments != NULL);

        /* allocate space for new segment based on length */
        uint32_t *segment = malloc(sizeof(num_words) * num_words);
        assert(segment != NULL);

        uint32_t *seg_length = malloc(sizeof(num_words));
        assert(seg_length != NULL);
        *seg_length = num_words;
        
        /* if recycled ID exists, then use it, otherwise add to the end */
        if (Seq_length(all_segments->unmapped) > 0) {
                /* Seq_remlo returns pointer, so dereference for int ID */
                uint32_t *open_address_p = Seq_remlo(all_segments->unmapped);
                uint32_t open_address = *open_address_p;
                free(open_address_p);
                Seq_put(all_segments->mapped, open_address, segment);

                uint32_t *old_length = Seq_put(all_segments->lengths,
                                              open_address,
                                              seg_length);
                free(old_length);

                return open_address;
        } else {
                /* recycled ID unavailable, so just create new ID */
                Seq_addhi(all_segments->mapped, segment);
                Seq_addhi(all_segments->lengths, seg_length);
                return Seq_length(all_segments->mapped) - 1;
        }
        
}


/*      unmap_seg
 * Purpose: handle the finer details of deallocating heap space for
 *          an existing segment, & recycling its ID for later use
 * Expectations: instance of Segments struct exists & is valid, 
 *               segment ID < total number of allocated segments
 * Input: struct holding Hanson sequences to all segment items,
 *        uint32_t specifying ID of segment to recycle/unmap
 * Output: N/A, void - end result: existing segment deallocated, 
 *         & its ID is stored/recycled for later reuse
 */
void unmap_seg(Segments all_segments, uint32_t seg_ID)
{
        assert(all_segments != NULL);
        assert(seg_ID > 0);

        /* make segment memory available for another mapping */
        uint32_t *segment = Seq_get(all_segments->mapped, seg_ID);
        //uint32_t *length = Seq_get(all_segments->lengths, seg_ID);
        free(segment);
        //free(length);
        
        Seq_put(all_segments->mapped, seg_ID, NULL);

        /* keep track of newly unmapped/recycled ID */
        uint32_t *hold_ID = malloc(sizeof(seg_ID));
        *hold_ID = seg_ID;
        Seq_addhi(all_segments->unmapped, (void *)hold_ID);
}

/*      segments_free
 * Purpose: free all the memory that has been allocated by our
 *          universal machine
 * Expectations: instance of Segments struct exists & is valid, 
 *               number of words > 0
 * Input: struct holding Hanson sequences to all segment items
 * Output: N/A, void - end result: all segments emptied and freed
*/
void segments_free(Segments all_segments)
{
        assert(all_segments != NULL);

        Seq_T mapped = all_segments->mapped;
        Seq_T unmapped = all_segments->unmapped;
        Seq_T lengths = all_segments->lengths;

        /* free the elements of the 3 sequences */
        while (Seq_length(mapped) > 0) {
                uint32_t *word = (uint32_t *)Seq_remlo(mapped);
                free(word);
        }

        while (Seq_length(unmapped) > 0) {
                uint32_t *word = (uint32_t *)Seq_remlo(unmapped);
                free(word);
        }
        
        while (Seq_length(lengths) > 0) {
                uint32_t *length = (uint32_t *)Seq_remlo(lengths);
                free(length);
        }

        Seq_free(&mapped);
        Seq_free(&unmapped);
        Seq_free(&lengths);

        free(all_segments);
}
