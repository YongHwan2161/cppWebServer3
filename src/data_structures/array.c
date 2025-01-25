#include "array.h"
#include <stdbool.h>
int remove_duplicates(unsigned int* arr, int count) {
    if (count <= 1) return count;
    
    int new_count = 1;  // Start with first element
    
    // Compare each element with previous unique elements
    for (int i = 1; i < count; i++) {
        bool is_duplicate = false;
        
        // Check if current element exists in unique elements
        for (int j = 0; j < new_count; j++) {
            if (arr[i] == arr[j]) {
                is_duplicate = true;
                break;
            }
        }
        
        // If not duplicate, add to unique elements
        if (!is_duplicate) {
            arr[new_count] = arr[i];
            new_count++;
        }
    }
    
    return new_count;
}