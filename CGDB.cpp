#include <vector>
#include <iostream>

using uint = unsigned int;
using ushort = unsigned short;
using uchar = unsigned char;

std::vector<uchar *> Core;

uchar initValues[12] = {8, 0, 0, 0, 1, 0, 10, 0, 0, 0, 0, 0};
void create_new_node(){
    uchar *newNode = new uchar[12];
    for (int i = 0; i < 12; ++i)
    {
        newNode[i] = initValues[i];
    }
}
void create_DB()
{
    std::cout << "call create_DB()" << std::endl;
    for (int i = 0; i < 256; ++i){
        create_new_node();
    }
}
int main(int argc, char const *argv[])
{
    /* code */
    create_DB();
    std::cout << "end of main()" << std::endl;
    return 0;
}
