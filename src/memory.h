#ifndef MEMORY_H
#define MEMORY_H

#include <string.h>

/**
 * Inserts data into a memory buffer at a specified position
 * 
 * @param dest Destination buffer
 * @param insert_pos Position to insert at
 * @param src Source data to insert
 * @param src_size Size of data to insert
 * @param total_size Total size of destination buffer
 * @param move_size Size of data to move (from insert position to end)
 * @return 1 if successful, 0 if failed
 */
int insert_memory(unsigned char* dest, unsigned int insert_pos, 
                 const unsigned char* src, unsigned int src_size,
                 unsigned int total_size, unsigned int move_size);

/**
 * Inserts an axis entry (axis number and offset) into a buffer
 * 
 * @param dest Destination buffer
 * @param insert_pos Position to insert at
 * @param axis_number Axis number to insert
 * @param axis_offset Offset value for the axis
 * @param total_size Total size of destination buffer
 * @param move_size Size of data to move
 * @return 1 if successful, 0 if failed
 */
int insert_axis_entry(unsigned char* dest, unsigned int insert_pos,
                     unsigned short axis_number, unsigned int axis_offset,
                     unsigned int move_size);

/**
 * Inserts an unsigned short value into a buffer
 * 
 * @param dest Destination buffer
 * @param insert_pos Position to insert at
 * @param value Value to insert
 * @param move_size Size of data to move
 * @return 1 if successful, 0 if failed
 */
int insert_ushort(unsigned char* dest, unsigned int insert_pos,
                 unsigned short value, unsigned int move_size);

/**
 * Inserts an unsigned int value into a buffer
 * 
 * @param dest Destination buffer
 * @param insert_pos Position to insert at
 * @param value Value to insert
 * @param move_size Size of data to move
 * @return 1 if successful, 0 if failed
 */
int insert_uint(unsigned char* dest, unsigned int insert_pos,
               unsigned int value, unsigned int move_size);

#endif // MEMORY_H 