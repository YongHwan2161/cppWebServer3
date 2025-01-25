#include "../Graph_structure/axis.h"
#include "../Graph_structure/channel.h"
#include "../Graph_structure/link.h"
#include "../Graph_structure/node.h"
#include "../Graph_structure/cycle.h"
#include "../Graph_structure/vertex.h"
#include "../free_space.h"
#include "../tests/axis_tests.h"
#include "../tests/link_tests.h"
#include "../tests/channel_tests.h"
#include "../map.h"
#include "../memory.h"
#include "command_handler.h"
#include "test_command_handler.h"
#include "validate_command_handler.h"
#include "../../CGDB.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Common error handling utilities
void print_command_usage(const char* command, const char* args_desc) {
    printf("Usage: %s %s\n", command, args_desc);
    printf("Example: %s\n", command);
}

void print_argument_error(const char* command, const char* args_desc, bool is_missing) {
    printf("Error: %s arguments\n", is_missing ? "Missing" : "Invalid");
    print_command_usage(command, args_desc);
    
    // Add example based on command type
    if (strcmp(command, "create-axis") == 0) {
        printf("Example: create-axis 0 0 0\n");
    } else if (strcmp(command, "check-axis") == 0) {
        printf("Example: check-axis 0 0 0\n");
    } else if (strcmp(command, "list-axes") == 0) {
        printf("Example: list-axes 0 0\n");
    } else if (strcmp(command, "delete-axis") == 0) {
        printf("Example: delete-axis 0 0 0\n");
    } else if (strcmp(command, "create-link") == 0) {
        printf("Example: create-link 0 0 1 0 0\n");
    }
}

// Modified command handlers
int handle_create_axis(char* args) {
    int node_index, channel_index, axis_number;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &node_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("create-axis", "<node_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (node_index < 0 || node_index >= 256) {
        printf("Error: node index must be between 0 and 255\n");
        return CMD_ERROR;
    }
    
    // Create the axis
    int result = create_axis(node_index, channel_index, axis_number);
    if (result == AXIS_SUCCESS) {
        printf("Successfully created axis %d in node %d, channel %d\n",
               axis_number, node_index, channel_index);
        return CMD_SUCCESS;
    } else {
        printf("Failed to create axis\n");
        return CMD_ERROR;
    }
}

int handle_check_axis(char* args) {
    int node_index, channel_index, axis_number;

    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &node_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("check-axis", "<node_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    
    uint node_position = CoreMap[node_index].core_position;
    // Get node and check if it exists
    if (!Core[node_position]) {
        printf("Error: node %d does not exist\n", node_index);
        return CMD_ERROR;
    }
    
    // Check if axis exists
    bool exists = has_axis(Core[node_position], channel_index, axis_number);
    printf("Axis %d %s in node %d, channel %d\n",
           axis_number,
           exists ? "exists" : "does not exist",
           node_index,
           channel_index);
           
    return CMD_SUCCESS;
}

int handle_list_axes(char* args) {
    int node_index, channel_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d", &node_index, &channel_index);
    if (parsed != 2) {
        print_argument_error("list-axes", "<node_index> <channel_index>", false);
        return CMD_ERROR;
    }
    uint node_position = CoreMap[node_index].core_position;
    // Get node and check if it exists
    if (!Core[node_position]) {
        printf("Error: node %d does not exist\n", node_index);
        return CMD_ERROR;
    }
    
    // Get channel offset
    uint channel_offset = get_channel_offset(Core[node_position], channel_index);
    
    // Get axis count
    ushort axis_count = *(ushort*)(Core[node_position] + channel_offset);
    
    printf("\nAxes in node %d, channel %d:\n", node_index, channel_index);
    if (axis_count == 0) {
        printf("No axes found\n");
        return CMD_SUCCESS;
    }
    
    printf("Total axes: %d\n", axis_count);
    
    // List all axes by scanning through axis data
    printf("\nAxis numbers:\n");
    int axis_data_offset = channel_offset + 2;  // Skip axis count
    
    // Create an array to store axis numbers for sorting (optional)
    ushort* axis_numbers = (ushort*)malloc(axis_count * sizeof(ushort));
    
    // Read all axis numbers
    for (int i = 0; i < axis_count; i++) {
        axis_numbers[i] = *(ushort*)(Core[node_position] + axis_data_offset + (i * 6));
    }
    
    // Optional: Sort axis numbers for better readability
    for (int i = 0; i < axis_count - 1; i++) {
        for (int j = 0; j < axis_count - i - 1; j++) {
            if (axis_numbers[j] > axis_numbers[j + 1]) {
                ushort temp = axis_numbers[j];
                axis_numbers[j] = axis_numbers[j + 1];
                axis_numbers[j + 1] = temp;
            }
        }
    }
    
    // Print axis numbers with special labels for known types
    for (int i = 0; i < axis_count; i++) {
        const char* axis_type = "";
        switch(axis_numbers[i]) {
            case TOKEN_SEARCH_AXIS:
                axis_type = "(Token search)";
                break;
            case TOKEN_DATA_AXIS:
                axis_type = "(Token data)";
                break;
            case STRING_AXIS:
                axis_type = "(string)";
                break;
        }
        printf("- Axis %d %s\n", axis_numbers[i], axis_type);
    }
    
    free(axis_numbers);
    return CMD_SUCCESS;
}

int handle_delete_axis(char* args) {
    int node_index, channel_index, axis_number;
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d", &node_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("delete-axis", "<node_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }
    // Delete the axis
    int result = delete_axis(node_index, channel_index, axis_number);
    return (result == AXIS_SUCCESS) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_create_link(char* args) {
    int source_node, source_ch, dest_node, dest_ch, axis_number;
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d %d %d", 
                       &source_node, &source_ch, 
                       &dest_node, &dest_ch, 
                       &axis_number);
    if (parsed != 5) {
        print_argument_error("create-link", 
            "<source_node> <source_ch> <dest_node> <dest_ch> <axis_number>", 
            false);
        return CMD_ERROR;
    }
    // Create the link
    int result = create_link(source_node, source_ch, 
                           dest_node, dest_ch, 
                           axis_number);
    return (result == LINK_SUCCESS) ? CMD_SUCCESS : CMD_ERROR;
}
int handle_create_loop(char* args) {
    int node_index, channel_index, axis_number;
    int parsed = sscanf(args, "%d %d %d", &node_index, &channel_index, &axis_number);
    if (parsed != 3) {
        print_argument_error("create-loop", "<node_index> <channel_index> <axis_number>", false);
        return CMD_ERROR;
    }    
    // Validate input
    if (!validate_node(node_index)) {
        return CMD_ERROR;
    }
    return create_loop(node_index, channel_index, axis_number);
}
int handle_create_bidirectional_link(char* args) {
    int source_node, source_ch, dest_node, dest_ch;
    int parsed = sscanf(args, "%d %d %d %d", 
                       &source_node, &source_ch, 
                       &dest_node, &dest_ch);
    if (parsed != 4) {
        print_argument_error("create-bidirectional-link", 
            "<source_node> <source_ch> <dest_node> <dest_ch>", 
            false);
        return CMD_ERROR;
    }
    Vertex source_vertex = {source_node, source_ch};
    Vertex dest_vertex = {dest_node, dest_ch};
    return create_bidirectional_link(source_vertex, dest_vertex);
}
int handle_delete_link(char* args) {
    int source_node, source_ch, dest_node, dest_ch, axis_number;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d %d %d %d", 
                       &source_node, &source_ch, 
                       &dest_node, &dest_ch, 
                       &axis_number);
    if (parsed != 5) {
        print_argument_error("delete-link", 
            "<source_node> <source_ch> <dest_node> <dest_ch> <axis_number>", 
            false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (!validate_node(source_node) ||
        !validate_node(dest_node)) {
        return CMD_ERROR;
    }
    if (!has_axis(Core[source_node], source_ch, axis_number)) {
        printf("Error: Axis %d does not exist in node %d, channel %d\n",
               axis_number, source_node, source_ch);
        return CMD_ERROR;
    }
    
    // Delete the link
    int result = delete_link(source_node, source_ch, 
                           dest_node, dest_ch, 
                           axis_number);
    return (result == LINK_SUCCESS) ? CMD_SUCCESS : CMD_ERROR;
}

int handle_print_node(char* args) {
    int node_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d", &node_index);
    if (parsed != 1) {
        print_argument_error("print-node", "<node_index>", false);
        return CMD_ERROR;
    }
    long node_position = get_node_position(node_index);
    if (node_position == -1) {
        printf("Error: node %d is not loaded in memory\n", node_index);
        return CMD_ERROR;
    }
    printf("node %d is at Core position %d\n", node_index, (unsigned int)node_position);
    // Check if node exists
    if (!Core[(unsigned int)node_position]) {
        printf("Error: node %d does not exist\n", node_index);
        return CMD_ERROR;
    }
        // Print raw memory contents
    printf("\nMemory Contents:\n");
    printf("Offset    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F    ASCII\n");
    printf("--------  -----------------------------------------------    ----------------\n");
    
    ushort node_size = 1 << (*(ushort*)Core[(unsigned int)node_position]);
    printf("node_size: %d\n", node_size);
    for (int i = 0; i < node_size; i += 16) {
        // Print offset
        printf("%08X  ", i);
        
        // Print hex values
        for (int j = 0; j < 16; j++) {
            if (i + j < node_size) {
                printf("%02X ", Core[(unsigned int)node_position][i + j]);
            } else {
                printf("   ");
            }
        }
        
        // Print ASCII representation
        printf("   ");
        for (int j = 0; j < 16 && i + j < node_size; j++) {
            char c = Core[(unsigned int)node_position][i + j];
            printf("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        printf("\n");
    }
    // Print node metadata
    printf("\nnode %d Information:\n", node_index);
    printf("Size: %d bytes\n", 1 << (*(ushort*)Core[(unsigned int)node_position]));
    printf("Actual Size: %d bytes\n", *(uint*)(Core[(unsigned int)node_position] + 2));
    printf("Core Position: %d\n", CoreMap[node_index].core_position);
    printf("File Offset: 0x%08lX\n", CoreMap[node_index].file_offset);
    printf("Load Status: %s\n", CoreMap[node_index].is_loaded ? "Loaded" : "Not loaded");
    
    // Get channel count
    ushort channel_count = get_channel_count(Core[node_position]);
    printf("\nChannel Count: %d\n", channel_count);

    // Print channel information
    for (int ch = 0; ch < channel_count; ch++) {
        uint channel_offset = get_channel_offset(Core[node_position], ch);
        printf("\nChannel %d (offset: 0x%04X):\n", ch, channel_offset);
        
        // Get axis count for this channel
        ushort axis_count = *(ushort*)(Core[node_position] + channel_offset);
        printf("  Axis Count: %d\n", axis_count);
        
        // Print axis information
        for (int i = 0; i < axis_count; i++) {
            // Get axis entry
            int axis_entry_offset = channel_offset + 2 + (i * 6);
            ushort axis_number = *(ushort*)(Core[node_position] + axis_entry_offset);
            uint axis_offset = *(uint*)(Core[node_position] + axis_entry_offset + 2);
            
            // Get axis type label
            const char* axis_type = "";
            switch(axis_number) {
                case TOKEN_SEARCH_AXIS: axis_type = "(Token search)"; break;
                case TOKEN_DATA_AXIS: axis_type = "(Token data)"; break;
                case STRING_AXIS: axis_type = "(string)"; break;
            }
            
            printf("  Axis %d %s (offset: 0x%04X):\n", axis_number, axis_type, axis_offset);
            
            // Get link count for this axis
            ushort link_count = *(ushort*)(Core[node_position] + channel_offset + axis_offset);
            printf("    Link Count: %d\n", link_count);
            
            // Print link information
            for (int j = 0; j < link_count; j++) {
                int link_offset = channel_offset + axis_offset + 2 + (j * 6);
                if (link_offset >= 1 << (*(ushort*)Core[node_position])) {
                    printf("    error: link offset %d is greater than node size %d\n", link_offset, 1 << (*(ushort*)Core[node_position]));
                    break;
                }
                uint dest_node = *(uint*)(Core[node_position] + link_offset);
                ushort dest_channel = *(ushort*)(Core[node_position] + link_offset + 4);
                printf("    Link %d: node %d, Channel %d\n", 
                       j, dest_node, dest_channel);
            }
        }
    }
    
    
    return CMD_SUCCESS;
}

int handle_print_free_space(char* args) {
    if (args) {
        print_argument_error("print-free-space", "", false);
        return CMD_ERROR;
    }
    
    printf("\nFree Space Information:\n");
    printf("Total free blocks: %u\n\n", free_space->count);
    
    if (free_space->count > 0) {
        printf("Free Blocks:\n");
        printf("Size (bytes)    Offset\n");
        printf("------------    ------\n");
        
        for (uint i = 0; i < free_space->count; i++) {
            printf("%-14u0x%08lx\n", 
                   free_space->blocks[i].size,
                   free_space->blocks[i].offset);
        }
    }
    
    return CMD_SUCCESS;
}

int handle_create_channel(char* args) {
    int node_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d", &node_index);
    if (parsed != 1) {
        print_argument_error("create-channel", "<node_index>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (!validate_node(node_index)) {
        printf("Error: node %d does not exist\n", node_index);
        return CMD_ERROR;
    }
    
    // Create the channel
    int result = create_channel(node_index);
    if (result == CHANNEL_SUCCESS) {
        printf("Successfully created new channel in node %d\n", node_index);
        return CMD_SUCCESS;
    } else {
        printf("Failed to create channel in node %d\n", node_index);
        return CMD_ERROR;
    }
}

int handle_clear_channel(char* args) {
    int node_index, channel_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d", &node_index, &channel_index);
    if (parsed != 2) {
        print_argument_error("clear-channel", "<node_index> <channel_index>", false);
        return CMD_ERROR;
    }
    
    // Validate input
    if (!validate_node(node_index)) {
        printf("Error: node %d does not exist\n", node_index);
        return CMD_ERROR;
    }
    
    // Clear the channel
    int result = clear_channel(node_index, channel_index);
    if (result == CHANNEL_SUCCESS) {
        printf("Successfully cleared channel %d in node %d\n", channel_index, node_index);
        return CMD_SUCCESS;
    } else {
        printf("Failed to clear channel %d in node %d\n", channel_index, node_index);
        return CMD_ERROR;
    }
}

int handle_get_channel_offset(char* args) {
    int node_index, channel_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d %d", &node_index, &channel_index);
    if (parsed != 2) {
        print_argument_error("get-channel-offset", "<node_index> <channel_index>", false);
        return CMD_ERROR;
    }
    long node_position = get_node_position(node_index);
    // Get node and check if it exists
    if (node_position == -1) {
        printf("Error: node %d does not exist\n", node_index);
        return CMD_ERROR;
    }
    
    // Get channel offset
    uint channel_offset = get_channel_offset(Core[node_position], channel_index);
    printf("Channel %d offset in node %d: 0x%04X\n", 
           channel_index, node_index, channel_offset);
           
    return CMD_SUCCESS;
}

int handle_get_node_position(char* args) {
    int node_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d", &node_index);
    if (parsed != 1) {
        print_argument_error("get-node-position", "<node_index>", false);
        return CMD_ERROR;
    }
    
    // Get node position
    long position = get_node_position(node_index);
    if (position >= 0) {
        printf("node %d is at Core position %d\n", node_index, (unsigned int)position);
        printf("Memory address: %p\n", (void*)Core[(unsigned int)position]);
        return CMD_SUCCESS;
    }
    
    return CMD_ERROR;
}

int handle_unload_node(char* args) {
    int node_index;
    
    // Parse arguments
    int parsed = sscanf(args, "%d", &node_index);
    if (parsed != 1) {
        print_argument_error("unload-node", "<node_index>", false);
        return CMD_ERROR;
    }
    // Unload the node
    if (unload_node_data(node_index)) {
        printf("Successfully unloaded node %d from memory\n", node_index);
        return CMD_SUCCESS;
    }
    
    return CMD_ERROR;
}


int handle_print_coremap(char* args) {
    int node_index = -1;
    
    if (args) {
        // Parse node index
        if (sscanf(args, "%d", &node_index) != 1) {
            print_argument_error("print-coremap", "<node_index>", false);
            return CMD_ERROR;
        }
        
        // Validate node index
        if (node_index < 0 ) {
            printf("Error: node index must be between 0 and 255\n");
            return CMD_ERROR;
        }
        
        // Print single node info
        printf("\nCoreMap Status for node %d:\n", node_index);
        printf("%-8s %-15s %-15s %-15s\n", 
               "node", "Core Position", "Is Loaded", "File Offset");
        printf("--------------------------------------------------------\n");
        printf("%-8d %-15d %-15s 0x%08lX\n", 
               node_index,
               CoreMap[node_index].core_position,
               CoreMap[node_index].is_loaded ? "Yes" : "No",
               CoreMap[node_index].file_offset);
    } else {
        // Print summary of loaded vertices
        printf("\nCoreMap Status:\n");
        printf("Total Loaded vertices: %d\n\n", CoreSize);
        printf("%-8s %-15s %-15s %-15s\n", 
               "node", "Core Position", "Is Loaded", "File Offset");
        printf("--------------------------------------------------------\n");
        
        for (int i = 0; i < 256; i++) {
            if (CoreMap[i].is_loaded) {
                printf("%-8d %-15d %-15s 0x%08lX\n", 
                       i,
                       CoreMap[i].core_position,
                       "Yes",
                       CoreMap[i].file_offset);
            }
        }
    }
    printf("\n");
    return CMD_SUCCESS;
}

int handle_check_core_size() {
    printf("\nCore Memory Status:\n");
    printf("Current node Count: %d\n", CurrentnodeCount);
    printf("Current Core Size: %d\n", CoreSize);
    printf("Maximum Core Size: %d\n", MaxCoreSize);
    printf("Available Slots: %d\n", MaxCoreSize - CoreSize);
    
    // Show utilization percentage
    float utilization = ((float)CoreSize / MaxCoreSize) * 100;
    printf("Memory Utilization: %.1f%%\n\n", utilization);
    
    return CMD_SUCCESS;
}

void print_help() {
    printf("\nAvailable commands:\n");

    // Node Management
    printf("\nNode Management:\n");
    printf("  unload-node <node>                  Unload node from memory\n");
    printf("  load-node <node>                    Load node into memory\n");
    printf("  create-node                         Create a new node\n");
    printf("  delete-node <node_index>            Delete a node and add to garbage chain\n");
    printf("  print-node <node_index>             Print node data in hexadecimal format\n");

    // Axis Management
    printf("\nAxis Management:\n");
    printf("  create-axis <node> <channel> <axis>  Create a new axis\n");
    printf("  check-axis <node> <channel> <axis>   Check if specific axis exists\n");
    printf("  list-axes <node> <channel>           List all axes in channel\n");
    printf("  delete-axis <node> <channel> <axis>  Delete an existing axis\n");

    // Link Management
    printf("\nLink Management:\n");
    printf("  create-link <src_node> <src_ch> <dst_node> <dst_ch> <axis>  Create a link\n");
    printf("  create-loop <node> <ch> <axis>      Create a loop\n");
    printf("  delete-link <src_node> <src_ch> <dst_node> <dst_ch> <axis>  Delete a link\n");

    // Channel Management
    printf("\nChannel Management:\n");
    printf("  create-channel <node>               Create a new channel in node\n");
    printf("  clear-channel <node> <channel>      Clear all data in a channel\n");
    printf("  get-channel-offset <node> <channel> Get channel offset\n");

    // System Information
    printf("\nSystem Information:\n");
    printf("  print-free-space                    Print free space information\n");
    printf("  get-node-position <node>            Get node's position in Core array\n");
    printf("  print-coremap [node_index]          Print CoreMap status (with optional node index)\n");
    printf("  check-core-size                     Show Core memory usage statistics\n");

    // Token and String Management
    printf("\nToken and String Management:\n");
    printf("  create-string <v1> <v2> ...         Create string from token vertices\n");
    printf("  create-string-str <text>            Create string from ASCII text (0-255)\n");
    printf("  get-string <node> <channel>         Get string data from cycle\n");
    printf("  search-token <text>                 Search for matching token sequence\n");
    printf("  integrate-tokens <node>             Integrate matching tokens in node\n");
    printf("  optimize-string <node> <ch> <axis>   Optimize string cycle by combining tokens\n");

    // Path Operations
    printf("\nPath Operations:\n");
    printf("  insert-path <node> <ch> <axis> <path>  Insert path into existing cycle\n");
    printf("  delete-path <node> <ch> <axis> <length>  Delete path from existing cycle\n");

    // Cycle Management
    printf("\nCycle Management:\n");
    printf("  validate-cycle <node> <ch> <axis>   Check if path forms a cycle\n");
    printf("  print-cycle <node> <ch> <axis>      Print cycle information\n");
    printf("  print-garbage                       Print garbage cycle information\n");

    // Testing Commands
    printf("\nTesting Commands:\n");
    printf("  run-tests                           Run all test cases\n");
    printf("  test-resize                         Run resize node space tests\n");
    printf("  test-axis-create-delete <node> <ch> <max>  Test axis creation/deletion\n");
    printf("  test-multiple-link <node> <ch> <axis>  Test multiple link creation\n");
    printf("  test-create-delete-links <node> <ch> <axis>  Test link creation/deletion cycle\n");
    printf("  test-multi-channel-links <node>     Test link creation/deletion across multiple channels\n");
    printf("  test-channel-creation <node>        Test sequential channel creation\n");
    printf("  validate-free-offsets               Validate free block offsets\n");

    // General Commands
    printf("\nGeneral Commands:\n");
    printf("  help                                Show this help message\n");
    printf("  exit                                Exit the program\n");

    // Additional Information
    printf("\nAxis Types:\n");
    printf("  0: Forward link\n");
    printf("  1: Backward link\n");
    printf("  3: Time axis\n");

    printf("\nExample Usage:\n");
    printf("  create-string 65 66 67              Create string from token vertices\n");
    printf("  create-string-str ABC               Same as above, using ASCII text\n");
    printf("  search-token AABBCCDD              Search for token starting with AABBCCDD\n");
    printf("  integrate-tokens 5                  Integrate matching tokens in node 5\n");
    printf("\n");
}

int handle_command(char* command) {
    char* cmd = strtok(command, " \n");
    if (!cmd) return CMD_SUCCESS;
    
    char* args = strtok(NULL, "\n");
    
    // Common argument validation
    if (strcmp(cmd, "help") == 0 || strcmp(cmd, "exit") == 0 || 
        strcmp(cmd, "print-free-space") == 0 || strcmp(cmd, "run-tests") == 0 ||
        strcmp(cmd, "test-resize") == 0 || strcmp(cmd, "validate-free-offsets") == 0 ||
        strcmp(cmd, "create-node") == 0) {
        // These commands don't need arguments
        if (strcmp(cmd, "help") == 0) {
            print_help();
            return CMD_SUCCESS;
        }
        else if (strcmp(cmd, "print-free-space") == 0) {
            return handle_print_free_space(args);
        }
        else if (strcmp(cmd, "run-tests") == 0) {
            return handle_run_tests(args);
        }
        else if (strcmp(cmd, "test-resize") == 0) {
            return handle_test_resize(args);
        }
        else if (strcmp(cmd, "validate-free-offsets") == 0) {
            return handle_validate_free_offsets(args);
        }
        else if (strcmp(cmd, "create-node") == 0) {
            return handle_create_node(args);
        }
        return CMD_EXIT;
    }
    
    // Check if arguments are required but missing
    if (!args) {
        if (strcmp(cmd, "create-axis") == 0) {
            print_argument_error(cmd, "<node_index> <channel_index> <axis_number>", true);
        }
        else if (strcmp(cmd, "check-axis") == 0) {
            print_argument_error(cmd, "<node_index> <channel_index> <axis_number>", true);
        }
        else if (strcmp(cmd, "list-axes") == 0) {
            print_argument_error(cmd, "<node_index> <channel_index>", true);
        }
        else if (strcmp(cmd, "delete-axis") == 0) {
            print_argument_error(cmd, "<node_index> <channel_index> <axis_number>", true);
        }
        else if (strcmp(cmd, "create-link") == 0) {
            print_argument_error(cmd, "<source_node> <source_ch> <dest_node> <dest_ch> <axis_number>", true);
        }
        else if (strcmp(cmd, "create-loop") == 0) {
            print_argument_error(cmd, "<node_index> <channel_index> <axis_number>", true);
        }
        else if (strcmp(cmd, "create-bidirectional-link") == 0) {
            print_argument_error(cmd, "<source_node> <source_ch> <dest_node> <dest_ch>", true);
        }
        else if (strcmp(cmd, "print-node") == 0) {
            print_argument_error(cmd, "<node_index>", true);
        }
        else if (strcmp(cmd, "delete-link") == 0) {
            print_argument_error(cmd, 
                "<source_node> <source_ch> <dest_node> <dest_ch> <axis_number>", 
                true);
        }
        else if (strcmp(cmd, "create-channel") == 0) {
            print_argument_error(cmd, "<node_index>", true);
        }
        else if (strcmp(cmd, "test-multi-channel-links") == 0) {
            print_argument_error(cmd, "<node_index>", true);
        }
        else if (strcmp(cmd, "test-channel-creation") == 0) {
            print_argument_error(cmd, "<node_index>", true);
        }
        else if (strcmp(cmd, "test-axis-create-delete") == 0) {
            print_argument_error(cmd, "<node_index> <channel_index> <max_axis>", true);
        }
        else if (strcmp(cmd, "clear-channel") == 0) {
            print_argument_error(cmd, "<node_index> <channel_index>", true);
        }
        else if (strcmp(cmd, "get-channel-offset") == 0) {
            print_argument_error(cmd, "<node_index> <channel_index>", true);
        }
        else if (strcmp(cmd, "get-node-position") == 0) {
            print_argument_error(cmd, "<node_index>", true);
        }
        else if (strcmp(cmd, "unload-node") == 0) {
            print_argument_error(cmd, "<node_index>", true);
        }
        else if (strcmp(cmd, "load-node") == 0) {
            print_argument_error(cmd, "<node_index>", true);
        }
        else if (strcmp(cmd, "print-coremap") == 0) {
            return handle_print_coremap(args);
        }
        else if (strcmp(cmd, "check-core-size") == 0) {
            return handle_check_core_size(args);
        }
        else if (strcmp(cmd, "delete-node") == 0) {
            return handle_delete_node(args);
        }
        else if (strcmp(cmd, "print-garbage") == 0)
        {
            return handle_print_garbage();
        }

        else if (strcmp(cmd, "test-sequential-token") == 0)
        {
            return handle_test_sequential_token_creation(args);
        }
        else if (strcmp(cmd, "test-repeating-string") == 0)
        {
            return handle_test_repeating_string(args);
        }
        else if (strcmp(cmd, "c") == 0)
        {
            return handle_print_vertex(args);
        }
        else if (strcmp(cmd, "check-node-consistency-all") == 0)
        {
            if (check_node_consistency_all() == 0)
            {
                printf("All nodes are consistent\n");
                return CMD_SUCCESS;
            }
        }
        else if (strcmp(cmd, "get-current") == 0){
            return handle_get_current_vertex();
        }
        else if (strcmp(cmd, "get-root") == 0){
            return handle_get_root_vertex();
        }
        else
        {
            printf("Unknown command. Type 'help' for available commands.\n");
        }
        return CMD_ERROR;
    }
    
    // Execute command with arguments
    if (strcmp(cmd, "create-axis") == 0) {
        return handle_create_axis(args);
    }
    else if (strcmp(cmd, "check-axis") == 0) {
        return handle_check_axis(args);
    }
    else if (strcmp(cmd, "list-axes") == 0) {
        return handle_list_axes(args);
    }
    else if (strcmp(cmd, "delete-axis") == 0) {
        return handle_delete_axis(args);
    }
    else if (strcmp(cmd, "create-link") == 0) {
        return handle_create_link(args);
    }
    else if (strcmp(cmd, "create-loop") == 0) {
        return handle_create_loop(args);
    }
    else if (strcmp(cmd, "create-bidirectional-link") == 0) {
        return handle_create_bidirectional_link(args);
    }
    else if (strcmp(cmd, "print-node") == 0) {
        return handle_print_node(args);
    }
    else if (strcmp(cmd, "delete-link") == 0) {
        return handle_delete_link(args);
    }
    else if (strcmp(cmd, "test-axis-create-delete") == 0) {
        return handle_test_axis_create_delete(args);
    }
    else if (strcmp(cmd, "test-multiple-link") == 0) {
        return handle_test_multiple_link_creation(args);
    }
    else if (strcmp(cmd, "test-create-delete-links") == 0) {
        return handle_test_create_delete_links(args);
    }
    else if (strcmp(cmd, "create-channel") == 0) {
        return handle_create_channel(args);
    }
    else if (strcmp(cmd, "test-multi-channel-links") == 0) {
        return handle_test_multi_channel_links(args);
    }
    else if (strcmp(cmd, "test-channel-creation") == 0) {
        return handle_test_channel_creation(args);
    }
    else if (strcmp(cmd, "clear-channel") == 0) {
        return handle_clear_channel(args);
    }
    else if (strcmp(cmd, "get-channel-offset") == 0) {
        return handle_get_channel_offset(args);
    }
    else if (strcmp(cmd, "get-node-position") == 0) {
        return handle_get_node_position(args);
    }
    else if (strcmp(cmd, "unload-node") == 0) {
        return handle_unload_node(args);
    }
    else if (strcmp(cmd, "load-node") == 0) {
        return handle_load_node(args);
    }
    else if (strcmp(cmd, "print-coremap") == 0) {
        return handle_print_coremap(args);
    }
    else if (strcmp(cmd, "check-core-size") == 0) {
        return handle_check_core_size(args);
    }
    else if (strcmp(cmd, "create-node") == 0) {
        return handle_create_node(args);
    }
    else if (strcmp(cmd, "delete-node") == 0) {
        return handle_delete_node(args);
    }
    // Update handle_command to include new commands:
    else if (strcmp(cmd, "validate-free-offsets") == 0)
    {
        return handle_validate_free_offsets(args);
    }
    else if (strcmp(cmd, "validate-cycle") == 0)
    {
        return handle_validate_cycle(args);
    }
    else if (strcmp(cmd, "print-cycle") == 0)
    {
        return handle_print_cycle(args);
    }
    else if (strcmp(cmd, "create-cycle") == 0) {
        return handle_create_cycle(args);
    }
    else if (strcmp(cmd, "get-token") == 0) {
        return handle_get_token_data(args);
    }
    else if (strcmp(cmd, "create-token") == 0) {
        return handle_create_token(args);
    }
    else if (strcmp(cmd, "/") == 0) {
        uint start_node;
        ushort start_channel;
        int result = handle_create_string(args, &start_node, &start_channel, false, false);
        if (result == SUCCESS) {
            printf("Successfully created string starting at node %u, channel %u\n", 
                   start_node, start_channel);
        }
        return result;
    }
    else if (strcmp(cmd, "//") == 0) {
        uint start_node;
        ushort start_channel;
        int result = handle_create_string(args, &start_node, &start_channel, false, true);
        if (result == SUCCESS) {
            printf("Successfully created string starting at node %u, channel %u\n", 
                   start_node, start_channel);
        }
        return result;
    }
    else if (strcmp(cmd, "get-string") == 0) {
        return handle_get_string(args);
    }
    else if (strcmp(cmd, "search-token") == 0) {
        return handle_search_token(args);
    }
    else if (strcmp(cmd, "insert-path") == 0) {
        return handle_insert_path(args);
    }
    else if (strcmp(cmd, "delete-path") == 0) {
        return handle_delete_path(args);
    }
    else if (strcmp(cmd, "integrate-tokens") == 0) {
        return handle_integrate_tokens(args);
    }
    else if (strcmp(cmd, "optimize-string") == 0) {
        return handle_optimize_string(args);
    }
    else if (strcmp(cmd, "save-node") == 0)
    {
        return handle_save_node(args);
    }
    else if (strcmp(cmd, "move") == 0){
        return handle_move_current_vertex(args);
    }
    else if (strcmp(cmd, "upload-text") == 0) {
        return handle_upload_text(args);
    }
    else
    {
        printf("Unknown command. Type 'help' for available commands.\n");
        return CMD_ERROR;
    }
}


int handle_validate_free_offsets(char* args) {
    if (args) {
        print_argument_error("validate-free-offsets", "", false);
        return CMD_ERROR;
    }
    return validate_free_offsets(args) ? CMD_SUCCESS : CMD_ERROR;
}


int handle_print_garbage() {
    // Get cycle information
    cycleInfo* info = get_cycle_info(GarbagenodeIndex, 0, 0);
    
    if (info->count == 0) {
        printf("No cycle found starting from node %d, channel %d, axis %d\n",
               GarbagenodeIndex, 0, 0);
    } else {
        printf("Found cycle with %d vertices:\n", info->count);
        printf("Path: ");
        for (int i = 0; i < info->count; i++) {
            printf("(node %u, Ch %u)", info->vertices[i], info->channels[i]);
            if (i < info->count - 1) {
                printf(" -> ");
            }
        }
        printf(" -> (node %u, Ch %u)\n", info->vertices[0], info->channels[0]);
    }
    
    free_cycle_info(info);
    return CMD_SUCCESS;
}

int handle_load_node(char* args) {
    unsigned int node_index;
    
    // Parse arguments
    if (!args || sscanf(args, "%u", &node_index) != 1) {
        print_argument_error("load-node", "<node_index>", false);
        return CMD_ERROR;
    }
    
    // Validate node index
    if (node_index >= CurrentnodeCount) {
        printf("Error: Invalid node index %u\n", node_index);
        return CMD_ERROR;
    }

    // Check if node is already loaded
    if (CoreMap[node_index].is_loaded) {
        printf("node %u is already loaded at Core position %u\n", 
               node_index, CoreMap[node_index].core_position);
        return CMD_SUCCESS;
    }

    // Check if we have space in Core array
    if (CoreSize >= (int)MaxCoreSize) {
        printf("Error: Core array is full (max size: %u)\n", MaxCoreSize);
        return CMD_ERROR;
    }

    // Try to load node data
    FILE* data_file = fopen(DATA_FILE, "rb");
    if (!data_file) {
        printf("Error: Could not open data.bin\n");
        return CMD_ERROR;
    }

    // Seek to node data
    if (fseek(data_file, CoreMap[node_index].file_offset, SEEK_SET) != 0) {
        printf("Error: Failed to seek to node data\n");
        fclose(data_file);
        return CMD_ERROR;
    }

    // Read size power first to allocate buffer
    ushort size_power;
    if (fread(&size_power, sizeof(ushort), 1, data_file) != 1) {
        printf("Error: Failed to read node size\n");
        fclose(data_file);
        return CMD_ERROR;
    }

    // Calculate actual size and allocate buffer
    size_t node_size = 1 << size_power;
    uchar* node_data = malloc(node_size);
    if (!node_data) {
        printf("Error: Failed to allocate memory for node\n");
        fclose(data_file);
        return CMD_ERROR;
    }

    // Seek back to start of node data
    fseek(data_file, CoreMap[node_index].file_offset, SEEK_SET);

    // Read entire node data
    if (fread(node_data, 1, node_size, data_file) != node_size) {
        printf("Error: Failed to read node data\n");
        free(node_data);
        fclose(data_file);
        return CMD_ERROR;
    }

    fclose(data_file);

    // Find free slot in Core array
    unsigned int slot = 0;
    while (slot < MaxCoreSize && Core[slot] != NULL) {
        slot++;
    }

    // Store node data
    Core[slot] = node_data;
    CoreMap[node_index].core_position = slot;
    CoreMap[node_index].is_loaded = 1;
    CoreSize++;

    printf("Successfully loaded node %d into Core position %d\n", node_index, slot);
    return CMD_SUCCESS;
}

int handle_print_vertex(char* args) {
    if (args) {
        print_argument_error("print-vertex", "", false);
        return CMD_ERROR;
    }
    
    printf("\nCurrent Vertex: (node %u, channel %u)\n", 
           CurrentVertex.node, CurrentVertex.channel);
    
    // Print current vertex string data
    char* current_string = get_string_data(CurrentVertex.node, CurrentVertex.channel);
    if (current_string) {
        printf("String: \"%s\"\n", current_string);
        free(current_string);
    } else {
        printf("String: <empty>\n");
    }
    
    // Check if at root position
    if (CurrentVertex.node == RootVertex.node && 
        CurrentVertex.channel == RootVertex.channel) {
        printf("Currently at root position\n");
    }

    // Print parent relationship
    uint parent_node=0;
    ushort parent_ch=0;
    int parent_count = 0;
    printf("Parents:\n");
    if (has_axis(Core[CurrentVertex.node], CurrentVertex.channel, PARENT_AXIS)) {
        int link_count = get_link_count(CurrentVertex.node, CurrentVertex.channel, PARENT_AXIS);
        if (link_count > 0)
        {
            for (int i = 0; i < link_count; i++)
            {
                if (get_link(CurrentVertex.node, CurrentVertex.channel,
                             PARENT_AXIS, i, &parent_node, &parent_ch) == LINK_SUCCESS)
                {
                    char *parent_string = get_string_data(parent_node, parent_ch);
                    printf("  %d: (node %u, channel %u) \"%s\"\n",
                           i + 1, parent_node, parent_ch,
                           parent_string ? parent_string : "<empty>");
                    if (parent_string){
                        free(parent_string);
                    }
                    parent_count++;
                }
            }
        }
        else{
            printf("No parents found\n");
        }
    }
    else{
        printf("No parents found\n");
    }

    // Print children relationships
    printf("Children:\n");
    uint child_node;
    ushort child_ch;
    int child_count = 0;
    if (has_axis(Core[CurrentVertex.node], CurrentVertex.channel, CHILD_AXIS))
    {
        int link_count = get_link_count(CurrentVertex.node, CurrentVertex.channel, CHILD_AXIS);
        if (link_count > 0)
        {
            for (int i = 0; i < link_count; i++)
            {
                if (get_link(CurrentVertex.node, CurrentVertex.channel,
                             CHILD_AXIS, i, &child_node, &child_ch) == LINK_SUCCESS)
                {
                    char *child_string = get_string_data(child_node, child_ch);
                    printf("  %d: (node %u, channel %u) \"%s\"\n",
                           i + 1, child_node, child_ch,
                           child_string ? child_string : "<empty>");
                    if (child_string){
                        free(child_string);
                    }
                    child_count++;
                }
            }
        }
        else
        {
            printf("No children found\n");
        }
    }
    else{
        printf("No children found\n");
    }
    return CMD_SUCCESS;
}
