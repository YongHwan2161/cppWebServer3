#ifndef AXIS_TESTS_H
#define AXIS_TESTS_H

// Test function declarations
int test_axis_creation(void);
int test_resize_vertex_space(void);
int test_axis_create_delete(int vertex_index, int channel_index, int max_axis);
int test_free_block_offsets(void);

#endif // AXIS_TESTS_H 