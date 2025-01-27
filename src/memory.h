#ifndef MEMORY_H
#define MEMORY_H

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
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

/**
 * Inserts a link entry (node index and channel) into a buffer
 * 
 * @param dest Destination buffer
 * @param insert_pos Position to insert at
 * @param node_index node index for the link
 * @param channel_index Channel index for the link
 * @param move_size Size of data to move
 * @return 1 if successful, 0 if failed
 */
int insert_link(unsigned char* dest, unsigned int insert_pos,
               unsigned int node_index, unsigned short channel_index,
               unsigned int move_size);

/**
 * Unloads node data from memory
 * 
 * @param node_index Index of the node to unload
 * @return 1 if successful, 0 if failed
 */
int unload_node_data(unsigned int node_index);

/**
 * Loads node data from file into memory
 * 
 * @param data_file File pointer to the data file
 * @param offset Offset in the file to read from
 * @param index Index of the node to load
 */
void load_node_from_file(FILE* data_file, long offset, unsigned int index);

int load_node_to_core(unsigned int node_index);

/**
 * Checks the consistency of all nodes in the Core array
 */
bool check_node_consistency(unsigned int node_index);

/**
 * Checks the consistency of all nodes in the Core array
 */ 
bool check_node_consistency_all();

#endif // MEMORY_H 