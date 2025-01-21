#ifndef CHANNEL_TESTS_H
#define CHANNEL_TESTS_H

#include "../../CGDB.h"

/**
 * Tests sequential channel creation in a node
 * 
 * @param node_index Target node index
 * @return Number of test failures (0 if all successful)
 */
int test_sequential_channel_creation(uint node_index);

#endif // CHANNEL_TESTS_H 