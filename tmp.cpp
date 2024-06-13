#include <iostream>

using uchar = unsigned char;
using ushort = unsigned short;

ushort charToushort(const uchar* arr) {
    ushort result;
    asm (
        "movzbl (%1), %%eax \n\t"  // Load arr[0] into eax and zero-extend to 32 bits
        "movzbl 1(%1), %%ecx \n\t" // Load arr[1] into ecx and zero-extend to 32 bits
        "shl $8, %%ecx \n\t"       // Shift ecx left by 8 bits (arr[1] << 8)
        "or %%ecx, %%eax \n\t"     // Combine the two bytes into a 16-bit value in eax
        "movw %%ax, %0 \n\t"       // Move the result from ax to the output variable
        : "=r" (result)            // output operand
        : "r" (arr)                // input operand
        : "eax", "ecx"             // clobbered registers
    );
    return result;
}

int main() {
    uchar arr[2] = {0x34, 0x12};  // Example input
    ushort result = charToushort(arr);
    std::cout << std::hex << result << std::endl;  // Expected output: 0x1234
    return 0;
}
