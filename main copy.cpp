#include <vector>
#include <stack>
#include <iostream>
#include <fstream>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sstream>
#include <locale>
#include <codecvt>
#include <sys/socket.h>
#include <thread>
#include <map>
#include <regex>
#include <unordered_set>
// #include <nlohmann/json.hpp>

#define PORT 8080
using namespace std;
using uint = unsigned int;
using ushort = unsigned short;
using uchar = unsigned char;
// using json = nlohmann::json;

std::vector<uchar *> CoRe;
std::vector<std::unique_ptr<uchar[]>> Core;
// std::vector<vector<tuple<int, ushort, long long, long long>>> order;
uint user = 1;
uint cvertex[10];
ushort cCh[10];
uint pageNum[10];
pair<uint, ushort> copyvertex;
int line_spacing = 5;
int text_size = 15;
vector<wstring> LogStr{};
unsigned int gar = 39701;
uint orderStart = 36673;
uint ttt = 166972;
vector<uchar> zero8 = {0, 0, 0, 0, 0, 0, 0, 0};
uchar initValues[18] = {14, 0, 0, 0, 1, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // vertex 추가시 초기값 설정 charToushort(uchar *arr)
// ushort charToushort(const uchar *arr)
// {
//     return (static_cast<ushort>(arr[0])) |
//            (static_cast<ushort>(arr[1]) << 8);
// }
ushort charToushort(const uchar *arr)
{
    ushort result;
    asm(
        "movzbl (%1), %%eax \n\t"  // Load arr[0] into eax and zero-extend to 32 bits
        "movzbl 1(%1), %%ecx \n\t" // Load arr[1] into ecx and zero-extend to 32 bits
        "shl $8, %%ecx \n\t"       // Shift ecx left by 8 bits (arr[1] << 8)
        "or %%ecx, %%eax \n\t"     // Combine the two bytes into a 16-bit value in eax
        "movw %%ax, %0 \n\t"       // Move the result from ax to the output variable
        : "=r"(result)             // output operand
        : "r"(arr)                 // input operand
        : "eax", "ecx"             // clobbered registers
    );
    return result;
}
ushort numCh(uint vertex) // vertex의 채널 수를 반환(채널 수는 2바이트로 저장됨)
{
    return (static_cast<ushort>(CoRe[vertex][4])) |
           (static_cast<ushort>(CoRe[vertex][5]) << 8);
}
uint charTouint(const uchar *arr)
{
    return (static_cast<unsigned int>(arr[0])) |
           (static_cast<unsigned int>(arr[1]) << 8) |
           (static_cast<unsigned int>(arr[2]) << 16) |
           (static_cast<unsigned int>(arr[3]) << 24);
}
uint endOfCh(uint vertex, ushort ch)
{
    uint re = charTouint(CoRe[vertex] + 6 + 4 * (ch + 1));
    return re;
}
pair<uint, ushort> charToPair(uchar *arr)
{
    unsigned int uint_val =
        (static_cast<unsigned int>(arr[0])) |
        (static_cast<unsigned int>(arr[1]) << 8) |
        (static_cast<unsigned int>(arr[2]) << 16) |
        (static_cast<unsigned int>(arr[3]) << 24);

    ushort ushort_val =
        (static_cast<ushort>(arr[4])) |
        (static_cast<ushort>(arr[5]) << 8);

    return {uint_val, ushort_val};
}
long charToLong(uchar *arr)
{
    return (static_cast<long>(arr[0])) |
           (static_cast<long>(arr[1]) << 8) |
           (static_cast<long>(arr[2]) << 16) |
           (static_cast<long>(arr[3]) << 24) |
           (static_cast<long>(arr[4]) << 32) |
           (static_cast<long>(arr[5]) << 40) |
           (static_cast<long>(arr[6]) << 48) |
           (static_cast<long>(arr[7]) << 56);
}
uchar *ushortToBytes(ushort val1)
{
    unsigned char *bytes = new unsigned char[2]; // 2 바이트 배열 동적 할당
    bytes[0] = static_cast<unsigned char>(val1 & 0xFF);
    bytes[1] = static_cast<unsigned char>((val1 >> 8) & 0xFF);
    return bytes; // 배열의 포인터 반환
}
std::unique_ptr<uchar[]> ushortToUniqBytes(ushort val1)
{
    auto bytes = std::make_unique<uchar[]>(2); // 2 바이트 배열 동적 할당
    bytes[0] = static_cast<uchar>(val1 & 0xFF);
    bytes[1] = static_cast<uchar>((val1 >> 8) & 0xFF);
    return bytes; // unique_ptr 반환
}
uchar *uintToBytes(uint val1)
{
    unsigned char *bytes = new unsigned char[4]; // 4 바이트 배열 동적 할당

    bytes[0] = static_cast<unsigned char>(val1 & 0xFF);
    bytes[1] = static_cast<unsigned char>((val1 >> 8) & 0xFF);
    bytes[2] = static_cast<unsigned char>((val1 >> 16) & 0xFF);
    bytes[3] = static_cast<unsigned char>((val1 >> 24) & 0xFF);

    return bytes; // 배열의 포인터 반환
}
uchar *uintToBytes2(uint val1, uint val2)
{
    unsigned char *bytes = new unsigned char[8]; // 8 바이트 배열 동적 할당

    bytes[0] = static_cast<unsigned char>(val1 & 0xFF);
    bytes[1] = static_cast<unsigned char>((val1 >> 8) & 0xFF);
    bytes[2] = static_cast<unsigned char>((val1 >> 16) & 0xFF);
    bytes[3] = static_cast<unsigned char>((val1 >> 24) & 0xFF);
    bytes[4] = static_cast<unsigned char>(val2 & 0xFF);
    bytes[5] = static_cast<unsigned char>((val2 >> 8) & 0xFF);
    bytes[6] = static_cast<unsigned char>((val2 >> 16) & 0xFF);
    bytes[7] = static_cast<unsigned char>((val2 >> 24) & 0xFF);

    return bytes; // 배열의 포인터 반환
}
uchar *pairToBytes(uint32_t val1, uint16_t val2)
{
    unsigned char *bytes = new unsigned char[6]; // 6 바이트 배열 동적 할당

    bytes[0] = static_cast<unsigned char>(val1 & 0xFF);
    bytes[1] = static_cast<unsigned char>((val1 >> 8) & 0xFF);
    bytes[2] = static_cast<unsigned char>((val1 >> 16) & 0xFF);
    bytes[3] = static_cast<unsigned char>((val1 >> 24) & 0xFF);
    bytes[4] = static_cast<unsigned char>(val2 & 0xFF);
    bytes[5] = static_cast<unsigned char>((val2 >> 8) & 0xFF);

    return bytes; // 배열의 포인터 반환
}
uchar *pairToBytes2(uint vertex1, ushort ch1, uint vertex2, ushort ch2)
{
    unsigned char *bytes = new unsigned char[12]; // 6 바이트 배열 동적 할당

    bytes[0] = static_cast<unsigned char>(vertex1 & 0xFF);
    bytes[1] = static_cast<unsigned char>((vertex1 >> 8) & 0xFF);
    bytes[2] = static_cast<unsigned char>((vertex1 >> 16) & 0xFF);
    bytes[3] = static_cast<unsigned char>((vertex1 >> 24) & 0xFF);
    bytes[4] = static_cast<unsigned char>(ch1 & 0xFF);
    bytes[5] = static_cast<unsigned char>((ch1 >> 8) & 0xFF);
    bytes[6] = static_cast<unsigned char>(vertex2 & 0xFF);
    bytes[7] = static_cast<unsigned char>((vertex2 >> 8) & 0xFF);
    bytes[8] = static_cast<unsigned char>((vertex2 >> 16) & 0xFF);
    bytes[9] = static_cast<unsigned char>((vertex2 >> 24) & 0xFF);
    bytes[10] = static_cast<unsigned char>(ch2 & 0xFF);
    bytes[11] = static_cast<unsigned char>((ch2 >> 8) & 0xFF);

    return bytes; // 배열의 포인터 반환
}
uchar *longlongToBytes(long long val)
{
    unsigned char *bytes = new unsigned char[8]; // 6 바이트 배열 동적 할당

    bytes[0] = static_cast<unsigned char>(val & 0xFF);
    bytes[1] = static_cast<unsigned char>((val >> 8) & 0xFF);
    bytes[2] = static_cast<unsigned char>((val >> 16) & 0xFF);
    bytes[3] = static_cast<unsigned char>((val >> 24) & 0xFF);
    bytes[4] = static_cast<unsigned char>((val >> 32) & 0xFF);
    bytes[5] = static_cast<unsigned char>((val >> 40) & 0xFF);
    bytes[6] = static_cast<unsigned char>((val >> 48) & 0xFF);
    bytes[7] = static_cast<unsigned char>((val >> 56) & 0xFF);
    return bytes;
}
uchar *chVec(uint vertex, ushort ch)
{
    uint posi = charTouint(Core[vertex].get() + 6 + 4 * ch);
    uint posiEnd;

    if (ch < numCh(vertex) - 1)
    {
        posiEnd = charTouint(Core[vertex].get() + 6 + 4 * (ch + 1));
    }
    else
    {
        posiEnd = 4 + charTouint(Core[vertex].get());
    }
    uchar *result = new uchar[posiEnd - posi];
    for (int i = 0; i < posiEnd - posi; i++)
    {
        result[i] = Core[vertex][i + posi];
    }
    return result;
}
uint startAxis(uint vertex, ushort ch, ushort axis)
{
    uint startCh = charTouint(Core[vertex].get() + 6 + 4 * ch);
    return charTouint(Core[vertex].get() + startCh + 2 + 4 * axis);
}
void changeShort(uchar *arr, uint index, ushort update)
{
    arr[index] = static_cast<uchar>(update & 0xFF);
    arr[index + 1] = static_cast<uchar>((update >> 8) & 0xFF);
}
void changeInt(uchar *arr, uint index, uint update)
{
    arr[index] = static_cast<uchar>(update & 0xFF);
    arr[index + 1] = static_cast<uchar>((update >> 8) & 0xFF);
    arr[index + 2] = static_cast<uchar>((update >> 16) & 0xFF);
    arr[index + 3] = static_cast<uchar>((update >> 24) & 0xFF);
}
void changePair(uchar *arr, uint index, uint vertex, ushort ch)
{
    // vertex 값을 벡터에 할당
    arr[index] = static_cast<uchar>(vertex & 0xFF);
    arr[index + 1] = static_cast<uchar>((vertex >> 8) & 0xFF);
    arr[index + 2] = static_cast<uchar>((vertex >> 16) & 0xFF);
    arr[index + 3] = static_cast<uchar>((vertex >> 24) & 0xFF);

    // ch 값을 벡터에 할당
    arr[index + 4] = static_cast<uchar>(ch & 0xFF);
    arr[index + 5] = static_cast<uchar>((ch >> 8) & 0xFF);
}
void changeLong(uchar *arr, uint index, long long update)
{
    arr[index] = static_cast<uchar>(update & 0xFF);
    arr[index + 1] = static_cast<uchar>((update >> 8) & 0xFF);
    arr[index + 2] = static_cast<uchar>((update >> 16) & 0xFF);
    arr[index + 3] = static_cast<uchar>((update >> 24) & 0xFF);
    arr[index + 4] = static_cast<uchar>((update >> 32) & 0xFF);
    arr[index + 5] = static_cast<uchar>((update >> 40) & 0xFF);
    arr[index + 6] = static_cast<uchar>((update >> 48) & 0xFF);
    arr[index + 7] = static_cast<uchar>((update >> 52) & 0xFF);
}
void erasePair(uint vertex, ushort ch, ushort axis, uint index)
{
    uint startCh = charTouint(Core[vertex].get() + 6 + 4 * ch);
    uint startAxis = charTouint(Core[vertex].get() + startCh + 2 + 4 * axis);
    uint startPair = startAxis + 6 * index;
    for (int i = startPair; i < startPair + 6; i++)
    {
        Core[vertex][i] = 0;
    }
}
void fillArr(uchar *arr, uint size, uchar fill)
{
    for (int i = 0; i < size; i++)
    {
        arr[i] = fill;
    }
}
void insertArr(uchar *&Arr, uint index, const uchar *add, uint addSize)
{
    uint sizeArr = 4 + charTouint(Arr);
    uchar *result = new uchar[sizeArr + addSize];
    changeInt(result, 0, sizeArr + addSize - 4);

    for (uint i = 4; i < index; ++i)
    {
        result[i] = Arr[i];
    }
    for (uint i = 0; i < addSize; ++i)
    {
        result[index + i] = add[i];
    }
    for (uint i = index; i < sizeArr; ++i)
    {
        result[i + addSize] = Arr[i];
    }

    // 기존 배열 메모리 해제
    delete[] Arr;

    // 새로운 배열을 원본 포인터에 할당
    Arr = result;
}
void insertArr2(std::unique_ptr<uchar[]> &Arr, uint index, const uchar *add, uint addSize)
{
    uint sizeArr = 4 + charTouint(Arr.get());
    std::unique_ptr<uchar[]> result = std::make_unique<uchar[]>(sizeArr + addSize);
    changeInt(result.get(), 0, sizeArr + addSize - 4);

    for (uint i = 4; i < index; ++i)
    {
        result[i] = Arr[i];
    }
    for (uint i = 0; i < addSize; ++i)
    {
        result[index + i] = add[i];
    }
    for (uint i = index; i < sizeArr; ++i)
    {
        result[i + addSize] = Arr[i];
    }

    // Move the new array to Arr
    Arr = std::move(result);
}
// ushort numAxis(uint vertex, ushort ch)
// {
//     uint startCh = charTouint(CoRe[vertex] + 6 + 4 * ch);
//     return charToushort(CoRe[vertex] + charTouint(CoRe[vertex] + 6 + 4 * ch));
// }
void pushPair(uint vertex, ushort ch, ushort axis, uint pushvertex, ushort pushCh)
{
    ushort numCh = charToushort(Core[vertex].get() + 4);
    uint startCh = charTouint(Core[vertex].get() + 6 + 4 * ch);
    uint startAxis = charTouint(Core[vertex].get() + startCh + 2 + 4 * axis);
    uint endAxis = charTouint(Core[vertex].get() + 2 + 4 * (axis + 1));
    uint countPair = (endAxis - startAxis) / 6;
    bool check = false;
    for (int i = 0; i < countPair; i++)
    {
        uint nvertex = charTouint(Core[vertex].get() + startAxis + 6 * countPair);
        ushort nch = charToushort(Core[vertex].get() + startAxis + 6 * countPair + 4);
        if (nvertex == 0 && nch == 0) // recycle
        {
            check = true;
            changePair(Core[vertex].get(), startAxis + 6 * countPair, pushvertex, pushCh);
            break;
        }
    }
    if (check == false)
    {
        insertArr2(Core[vertex], endAxis, pairToBytes(pushvertex, pushCh), 6);
        ushort numaxis = charToushort(Core[vertex].get() + startCh);
        for (int i = axis + 1; i <= numaxis; i++)
        {
            startAxis = charTouint(Core[vertex].get() + startCh + 2 + 4 * i);
            changeInt(Core[vertex].get(), startCh + 2 + 4 * i, startAxis + 6);
        }
        for (int i = ch + 1; i <= numCh; i++)
        {
            startCh = charTouint(Core[vertex].get() + 6 + 4 * i);
            changeInt(Core[vertex].get(), 6 + 4 * i, startCh + 6);
        }
    }
}
// void pushCoo(uint vertex, ushort ch, uchar *add)
// {
//     ushort numCh = charToushort(CoRe[vertex] + 4);
//     uint startCoo = charTouint(CoRe[vertex] + (6 + 4 * ch));
//     uint sizeCoo = charTouint(CoRe[vertex] + startCoo);
//     uint startRev = startCoo + 4 + sizeCoo;
//     uint sizeRev = charTouint(CoRe[vertex] + startRev);
//     uint startCoo2 = endOfCh(vertex, ch);
//     ushort nAxis = numAxis(vertex, ch);
//     if (nAxis == 2)
//     {
//         if (sizeCoo + sizeRev + 14 > startCoo2 - startCoo)
//         {
//             for (ushort i = ch + 1; i < numCh; i++)
//             {
//                 uint temp = charTouint(CoRe[vertex] + 6 + 4 * i);
//                 changeInt(CoRe[vertex], 6 + 4 * i, temp + 6);
//             }
//             insertArr(CoRe[vertex], startRev, add, 6);
//             // CoRe[vertex].insert(CoRe[vertex].begin() + startCoo + 4 + sizeCoo, add.begin(), add.end());
//             changeInt(CoRe[vertex], startCoo, sizeCoo + 6);
//         }
//         else
//         {
//             changeInt(CoRe[vertex], startCoo, sizeCoo + 6);
//             vector<uchar> RevVec = vector<uchar>(CoRe[vertex] + startRev, CoRe[vertex] + startRev + 4 + sizeRev);
//             for (int i = startRev + 6; i < startRev + 10 + sizeRev; i++)
//             {
//                 CoRe[vertex][i] = RevVec[i - startRev - 6];
//             }
//             for (int i = startRev; i < startRev + 6; i++)
//             {
//                 CoRe[vertex][i] = add[i - startRev];
//             }
//         }
//     }
//     else if (nAxis == 3)
//     {
//         insertArr(CoRe[vertex], startRev, add, 6); // inserArr에서 전체 byte 수 변경함
//         changeInt(CoRe[vertex], startCoo, sizeCoo + 6);
//         for (ushort i = ch + 1; i < numCh; i++)
//         {
//             uint temp = charTouint(CoRe[vertex] + 6 + 4 * i);
//             changeInt(CoRe[vertex], 6 + 4 * i, temp + 6);
//         }
//     }
//     delete[] add;
// }
// void pushRev(uint vertex, ushort ch, uchar *add)
// {
//     uint startCoo = charTouint(CoRe[vertex] + 6 + 4 * ch);
//     uint sizeCoo = charTouint(CoRe[vertex] + startCoo);
//     uint startRev = startCoo + 4 + sizeCoo;
//     uint sizeRev = charTouint(CoRe[vertex] + startRev);

//     uint endPosi = startRev + 4 + sizeRev;
//     ushort numCh = charToushort(CoRe[vertex] + 4);
//     uint startCoo2 = endOfCh(vertex, ch);
//     ushort nAxis = numAxis(vertex, ch);
//     if (nAxis == 2)
//     {
//         if (sizeCoo + sizeRev + 14 > startCoo2 - startCoo)
//         {
//             insertArr(CoRe[vertex], endPosi, add, 6);
//             for (ushort i = ch + 1; i < numCh; i++)
//             {
//                 uint temp = charTouint(CoRe[vertex] + 6 + 4 * i);
//                 changeInt(CoRe[vertex], 6 + 4 * i, temp + 6);
//             }
//             changeInt(CoRe[vertex], startRev, sizeRev + 6);
//         }
//         else
//         {
//             for (int i = endPosi; i < endPosi + 6; i++)
//             {
//                 CoRe[vertex][i] = add[i - endPosi];
//             }
//         }
//     }
//     else if (nAxis == 3)
//     {
//         insertArr(CoRe[vertex], endPosi, add, 6);
//         for (ushort i = ch + 1; i < numCh; i++)
//         {
//             uint temp = charTouint(CoRe[vertex] + 6 + 4 * i);
//             changeInt(CoRe[vertex], 6 + 4 * i, temp + 6);
//         }
//         changeInt(CoRe[vertex], startRev, sizeRev + 6);
//     }

//     delete[] add;
//     changeInt(CoRe[vertex], startRev, sizeRev + 6);
// }
// void pushRev2(uint vertex, ushort ch, uchar *add) // Brain 함수에서만 사용하는 특수용도(일반적으로 사용하면 안됨)
// {
//     uint startCoo = charTouint(CoRe[vertex] + 6 + 4 * ch);
//     uint sizeCoo = charTouint(CoRe[vertex] + startCoo);
//     uint startRev = startCoo + 4 + sizeCoo;
//     uint sizeRev = charTouint(CoRe[vertex] + startRev);
//     uint endPosi = startRev + 4 + sizeRev;
//     ushort numCh = charToushort(CoRe[vertex] + 4);
//     // CoRe[vertex].insert(CoRe[vertex].begin() + endPosi, add.begin(), add.end());
//     insertArr(CoRe[vertex], endPosi, add, 12);
//     for (ushort i = ch + 1; i < numCh; i++)
//     {
//         changeInt(CoRe[vertex], 6 + 4 * i, charTouint(CoRe[vertex] + 6 + 4 * i) + 12);
//     }
//     changeInt(CoRe[vertex], startRev, sizeRev + 12);
// }
void pushAxis3(uint vertex, ushort ch, uchar *bitmap)
{
    uint startCoo = charTouint(Core[vertex].get() + 6 + 4 * ch);
    uint sizeCoo = charTouint(Core[vertex].get() + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(Core[vertex].get() + startRev);
    uint startAxis3 = startRev + 4 + sizeRev;
    uint sizeAxis3 = charTouint(Core[vertex].get() + startAxis3);
    uint endPosi = startAxis3 + 4 + sizeAxis3;
    ushort numCh = charToushort(Core[vertex].get() + 4);
    // uint startCoo2 = 0;
    // if (ch < numCh - 1)
    // {
    //     startCoo2 = charTouint(CoRe[vertex] + 6 + 4 * (ch + 1));
    // }
    // else
    // {
    //     startCoo2 = 4 + charTouint(CoRe[vertex]);
    // }
    // uchar *add = uintToBytes(8);
    // insertArr(vertex, endPosi, add, 4);
    uint width = charTouint(Core[vertex].get() + startAxis3 + 4);
    uint height = charTouint(Core[vertex].get() + startAxis3 + 8);
    changeInt(Core[vertex].get(), startAxis3, 16 + width * height);
    // insertArr(vertex, endPosi + 8, bitmap, 4);
    if (width != 0 && height != 0)
    {
        insertArr2(Core[vertex], endPosi + 4, bitmap, width * height);
    }
    // insertArr(vertex, endPosi + 12, bitmap, width * height);
    for (ushort i = ch + 1; i < numCh; i++)
    {
        uint temp = charTouint(Core[vertex].get() + 6 + 4 * i);
        changeInt(Core[vertex].get(), 6 + 4 * i, temp + width * height);
    }
}
void insertCoo(uint vertex, ushort ch, int indexCoo, uchar *add)
{
    uint startCoo = charTouint(Core[vertex].get() + 6 + 4 * ch);
    uint sizeCoo = charTouint(Core[vertex].get() + startCoo);
    insertArr2(Core[vertex], startCoo + 4 + indexCoo, add, 6);
    // CoRe[vertex].insert(CoRe[vertex].begin() + startCoo + 4 + indexCoo, add.begin(), add.end());
    changeInt(Core[vertex].get(), startCoo, sizeCoo + 6);
    ushort sizeCh = numCh(vertex);
    for (ushort i = ch + 1; i < sizeCh; i++)
    {
        uint temp = charTouint(Core[vertex].get() + 6 + 4 * i);
        changeInt(Core[vertex].get(), 6 + 4 * i, temp + 6);
    }
}
void insertRev(uint vertex, ushort ch, int indexRev, uchar *add)
{
    uint startCoo = charTouint(Core[vertex].get() + 6 + 4 * ch);
    uint sizeCoo = charTouint(Core[vertex].get() + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(Core[vertex].get() + startRev);
    insertArr2(Core[vertex], startRev + +indexRev, add, 6);
    // CoRe[vertex].insert(CoRe[vertex].begin() + startRev + 4 + indexRev, add.begin(), add.end());
    changeInt(Core[vertex].get(), startRev, sizeRev + 6);
    ushort sizeCh = numCh(vertex);
    for (ushort i = ch + 1; i < sizeCh; i++)
    {
        uint temp = charTouint(Core[vertex].get() + 6 + 4 * i);
        changeInt(Core[vertex].get(), 6 + 4 * i, temp + 6);
    }
}
uint startCh(uint vertex, ushort ch)
{
    return charTouint(Core[vertex].get() + 6 + 4 * ch);
}
// uint sizeCoo(uint vertex, ushort ch)
// {
//     uint start = startCh(vertex, ch);
//     return charTouint(CoRe[vertex] + start);
// }
// uint sizeRev(uint vertex, ushort ch)
// {
//     // std::cerr << "call sizeRev()" << std::endl;
//     uint startCoo = charTouint(CoRe[vertex] + 6 + 4 * ch);
//     uint sizeCoo = charTouint(CoRe[vertex] + startCoo);
//     return charTouint(CoRe[vertex] + startCoo + 4 + sizeCoo);
// }
// vector<uint> sizeCoRe(uint vertex, ushort ch)
// {
//     vector<uint> re(2);
//     uint startCoo = startCh(vertex, ch);
//     re[0] = charTouint(CoRe[vertex] + startCoo);
//     re[1] = charTouint(CoRe[vertex] + startCoo + 4 + re[0]);
//     return re;
// }
uint sizeAxis(uint vertex, ushort ch, ushort axis)
{
    uint startCh = charTouint(Core[vertex].get() + 6 + 4 * ch);
    ushort nAxis = charToushort(Core[vertex].get() + startCh);
    if (axis > nAxis - 1)
    {
        return 0;
    }
    uint startAxis = charTouint(Core[vertex].get() + startCh + 2 + 4 * axis);
    uint endAxis = charTouint(Core[vertex].get() + startCh + 2 + 4 * (axis + 1));
    // std::cerr << "startCh ==" << startCh << ", startAxis = " << startAxis << ", endAxis = " << endAxis << std::endl;

    return endAxis - startAxis;
}
// uint startAxis3(uint vertex, ushort ch)
// {
//     if (numAxis(vertex, ch) >= 3)
//     {
//         uint startCoo2 = charTouint(CoRe[vertex] + 6 + 4 * ch);
//         uint sizeCoo2 = charTouint(CoRe[vertex] + startCoo2);
//         uint startRev2 = startCoo2 + 4 + sizeCoo2;
//         uint sizeRev2 = charTouint(CoRe[vertex] + startRev2);
//         return startRev2 + 4 + sizeRev2;
//     }
//     else
//     {
//         return 0;
//     }
// }
void CoMove(uint ori, ushort oriCh, uint From, uint To, ushort ToCh)
{
    // uchar *ToCoo = new uchar[6];
    // uchar ToCoo[6] = {0};
    uint vertex2 = 0;
    ushort ch2 = 0;
    uint startCh = charTouint(Core[ori].get() + 6 + 4 * oriCh);
    uint sizeAxis0 = sizeAxis(ori, oriCh, 0);
    uint startAxis0 = charTouint(Core[ori].get() + startCh + 2);
    for (int i = 0; i < sizeAxis0 / 6; i++)
    {
        // uint ii = 4 + 6 * i;
        if (charTouint(Core[ori].get() + startAxis0 + 6 * i) == From)
        {
            // std::copy(CoRe[ori] + start + ii, CoRe[ori] + start + ii + 6, ToCoo);
            vertex2 = charTouint(Core[ori].get() + startAxis0 + 6 * i);
            ch2 = charToushort(Core[ori].get() + startAxis0 + 6 * i + 4);
            erasePair(ori, oriCh, 0, i);
            // eraseCoo(ori, oriCh, i);
        }
    }
    // ushort revCh = charToushort(ToCoo + 4);
    // pushCoo(To, ToCh, ToCoo); // pushCoo에서 ToCoo를 삭제함.
    pushPair(To, ToCh, 0, vertex2, ch2);
    // sizeAxis0 = sizeAxis(From, ch2, 0);
    uint sizeAxis1 = sizeAxis(From, ch2, 1);
    // vector<uint> szCR = sizeCoRe(From, revCh);
    startCh = charTouint(Core[From].get() + 6 + 4 * ch2);
    // start = startCh(From, ch2);
    uint startAxis1 = charTouint(Core[From].get() + startCh + 6);
    for (int i = 0; i < sizeAxis1 / 6; i++)
    {
        if (charTouint(Core[From].get() + startAxis1 + 6 * i) == ori)
        {
            changePair(Core[From].get(), startAxis1 + 6 * i, To, ToCh);
            break;
        }
    }
}
void link(uint prevvertex, ushort prevCh, uint nextvertex, ushort nextCh)
{
    pushPair(prevvertex, prevCh, 0, nextvertex, nextCh);
    pushPair(nextvertex, nextCh, 1, prevvertex, prevCh);
    // pushCoo(prevvertex, prevCh, pairToBytes(nextvertex, nextCh));
    // pushRev(nextvertex, nextCh, pairToBytes(prevvertex, prevCh));
}
std::wstring ushortToWstring(ushort num) // ushort 번호에 해당하는 wstring 글자를 반환하는 함수임. ushort 자체를 글자로 변환하는 거 아님(이거는 intToWString() 사용할 것)
{
    wchar_t wc = static_cast<wchar_t>(num);
    wstring ws(1, wc);
    return ws;
}
std::wstring intToWString(int num) // 숫자를 wstring으로 바꿔주는 함수
{
    // std::cerr << "call intToWstring" << std::endl;
    std::wstringstream wss;
    wss << num;
    // std::cerr << "end intToWstring" << std::endl;
    return wss.str();
}
std::wstring utf8ToWstring(const std::string &utf8Str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(utf8Str);
}
std::u32string utf8ToUtf32(const std::string &utf8Str)
{
    std::u32string utf32Text;

    for (size_t i = 0; i < utf8Str.length();)
    {
        unsigned char lead = utf8Str[i];
        int charLength = 1;
        char32_t charCode = 0;

        if (lead < 0x80)
        {
            charCode = lead;
        }
        else if ((lead >> 5) == 0x06)
        { // 2-byte character
            charLength = 2;
            charCode = lead & 0x1F;
        }
        else if ((lead >> 4) == 0x0E)
        { // 3-byte character
            charLength = 3;
            charCode = lead & 0x0F;
        }
        else if ((lead >> 3) == 0x1E)
        { // 4-byte character
            charLength = 4;
            charCode = lead & 0x07;
        }
        else
        {
            throw std::runtime_error("Invalid UTF-8 lead byte");
        }

        if (i + charLength > utf8Str.length())
        {
            throw std::runtime_error("Incomplete UTF-8 character sequence");
        }

        for (int j = 1; j < charLength; ++j)
        {
            unsigned char follow = utf8Str[i + j];
            if ((follow >> 6) != 0x02)
            {
                throw std::runtime_error("Invalid UTF-8 follow byte");
            }
            charCode = (charCode << 6) | (follow & 0x3F);
        }

        utf32Text.push_back(charCode);
        i += charLength;
    }
    return utf32Text;
}
std::string wstringToUtf8(const std::wstring &wstr)
{
    if (wstr.empty())
        return std::string();

    size_t sizeNeeded = wcstombs(nullptr, wstr.c_str(), 0);
    std::vector<char> buffer(sizeNeeded + 1);
    wcstombs(buffer.data(), wstr.c_str(), buffer.size());
    return std::string(buffer.data());
}
ushort numAxis(uint vertex, ushort ch)
{
    uint startCh = charTouint(Core[vertex].get() + 6 + 4 * ch);
    return charToushort(Core[vertex].get() + startCh);
}
void chInfo(uint vertex, ushort ch)
{
    uint size1 = charTouint(CoRe[vertex]);
    uint size2 = charTouint(Core[vertex].get());
    std::cerr << "size --> " << size1 << ", " << size2 << std::endl;
    ushort nCh1 = charToushort(CoRe[vertex] + 4);
    ushort nCh2 = charToushort(Core[vertex].get() + 4);
    std::cerr << "nCh --> " << nCh1 << ", " << nCh2 << std::endl;
    uint startCh1 = charTouint(CoRe[vertex] + 6 + 4 * ch);
    uint startCh2 = charTouint(Core[vertex].get() + 6 + 4 * ch);
    std::cerr << "startCh --> " << startCh1 << ", " << startCh2 << std::endl;
    uint sizeAxis0_1 = charTouint(CoRe[vertex] + startCh1);
    uint sizeAxis0_2 = sizeAxis(vertex, ch, 0);
    ushort nAxis = numAxis(vertex, ch);
    std::cerr << "numAxis = " << nAxis << std::endl;
    std::cerr << "sizeAxis0 --> " << sizeAxis0_1 << ", " << sizeAxis0_2 << std::endl;
    uint sizeAxis1_1 = charTouint(CoRe[vertex] + startCh1 + 4 + sizeAxis0_1);
    uint sizeAxis1_2 = sizeAxis(vertex, ch, 1);
    std::cerr << "sizeAxis1 --> " << sizeAxis1_1 << ", " << sizeAxis1_2 << std::endl;
}
void chInfo3(uint vertex, ushort ch)
{
    std::cerr << "vertex = " << vertex << ", ch = " << ch << std::endl;
    uint size1 = charTouint(CoRe[vertex]);
    uint size2 = charTouint(Core[vertex].get());
    std::cerr << "size --> " << size1 << ", " << size2 << std::endl;
    ushort nCh1 = charToushort(CoRe[vertex] + 4);
    ushort nCh2 = charToushort(Core[vertex].get() + 4);
    std::cerr << "nCh --> " << nCh1 << ", " << nCh2 << std::endl;
    uint startCh1 = charTouint(CoRe[vertex] + 6 + 4 * ch);
    uint startCh2 = charTouint(Core[vertex].get() + 6 + 4 * ch);
    std::cerr << "startAxis0 --> " << startCh1 << ", " << startCh2 << std::endl;
    uint sizeAxis0_1 = charTouint(CoRe[vertex] + startCh1);
    uint sizeAxis0_2 = sizeAxis(vertex, ch, 0);
    ushort nAxis = numAxis(vertex, ch);
    std::cerr << "numAxis = " << nAxis << std::endl;
    std::cerr << "sizeAxis0 --> " << sizeAxis0_1 << ", " << sizeAxis0_2 << std::endl;
    uint startAxis1 = startCh1 + 4 + sizeAxis0_1;
    std::cerr << "startAxis1 --> " << startAxis1 << ", " << charTouint(Core[vertex].get() + startCh2 + 6) << std::endl;
    uint sizeAxis1_1 = charTouint(CoRe[vertex] + startAxis1);
    uint sizeAxis1_2 = sizeAxis(vertex, ch, 1);
    std::cerr << "sizeAxis1 --> " << sizeAxis1_1 << ", " << sizeAxis1_2 << std::endl;
    uint startAxis2 = startAxis1 + 4 + sizeAxis1_1;
    std::cerr << "startAxis2 --> " << startAxis2 << ", " << charTouint(Core[vertex].get() + startCh2 + 10) << std::endl;
    uint sizeAxis2_1 = charTouint(CoRe[vertex] + startAxis2);
    uint sizeAxis2_2 = sizeAxis(vertex, ch, 2);
    std::cerr << "sizeAxis2 --> " << sizeAxis2_1 << ", " << sizeAxis2_2 << std::endl;
}
uchar *word(uint vertex)
{
    // uchar *re1 = new uchar[1024];

    uchar re1[4000]; // 스택에 고정 크기 할당
    ushort nowPosi = 4;
    uint nAxis = numAxis(vertex, 0);
    // std::cerr << "call word(), vertex = " << vertex << ", nAxis = " << nAxis << std::endl;
    if (vertex >= ttt && vertex < ttt + 256)
    {
        re1[nowPosi] = (uchar)(vertex - ttt);
        nowPosi++;
    }
    // else if (sizeRev(vertex, 0) > 0)
    else if (nAxis > 1)
    {
        stack<uint> st;
        vector<uint> index;
        st.push(vertex);
        index.push_back(0);
        int ii = 0;
        while (!st.empty() && ii < 4000)
        {
            ii++;
            uint topvertex = st.top();
            uint szCoo = sizeAxis(topvertex, 0, 0);
            uint startCh0 = charTouint(Core[topvertex].get() + 6);
            nAxis = charToushort(Core[topvertex].get() + startCh0);
            // std::cerr << "nAxis = " << nAxis << std::endl;
            uint szRev = 0;
            if (nAxis == 1)
            {
                szRev = 0;
            }
            else
            {
                szRev = sizeAxis(topvertex, 0, 1);
            }
            // vector<uint> szCR = sizeCoRe(topvertex, 0);
            if (szRev == 6 * index.back())
            {
                if (st.size() == 1)
                    break;
                st.pop();
                index.pop_back();
                index.back()++;
                continue;
            }

            // uint startCoo = startCh(topvertex, 0);
            uint startAxis1 = startAxis(topvertex, 0, 1);
            uint rev = charTouint(Core[topvertex].get() + startAxis1 + 6 * index.back());
            // std::cerr << "rev = " << rev << std::endl;
            if (rev != 41155)
            {
                st.push(rev);
                index.push_back(0);
            }
            else
            {
                uchar chch = (uchar)(topvertex - ttt);
                re1[nowPosi] = chch;
                nowPosi++;
                st.pop();
                index.pop_back();
                if (!index.empty())
                    index.back()++;
            }
        }
    }
    // std::cerr << "nowPosi = " << nowPosi << std::endl;
    uchar *sizeRe = uintToBytes(nowPosi - 4);
    re1[0] = sizeRe[0];
    re1[1] = sizeRe[1];
    re1[2] = sizeRe[2];
    re1[3] = sizeRe[3];
    delete[] sizeRe;
    // 동적 배열에 필요한 크기만큼만 할당
    uchar *result = new uchar[nowPosi];
    std::copy(re1, re1 + nowPosi, result); // std::copy를 사용하여 re1의 내용을 result로 복사

    return result;
}
uint ushortTovertex(ushort charCode)
{
    uchar *byte = ushortToBytes(charCode);
    uint vertex1 = ttt + byte[0];
    uint startCh1 = charTouint(Core[vertex1].get() + 6);
    uint startCh1b = charTouint(CoRe[vertex1] + 6);
    // std::cerr << "startCh1 = " << startCh1 << ", startCh1b = " << startCh1b << std::endl;
    // std::cerr << "vertex1 = " << vertex1 << ", nAxis = " << charToushort(Core[vertex1].get() + startCh1) << std::endl;
    uint sizeCoo = charTouint(CoRe[vertex1] + startCh1b);
    uint sizeAxis0 = sizeAxis(vertex1, 0, 0);
    // std::cerr << "sizeCoo = " << sizeCoo << ", sizeAxis0 = " << sizeAxis0 << std::endl;
    uint vertex2 = 0;
    for (int i = 0; i < sizeAxis0 / 6; i++)
    {
        // uint startAxis0 = charTouint(Core[vertex1].get() + startCh1 + 2);
        uint startAxis0 = startAxis(vertex1, 0, 0);
        vertex2 = charTouint(Core[vertex1].get() + startAxis0 + 6 * i);
        // std::cerr << "vertex2 = " << vertex2 << std::endl;
        uint startCh12 = charTouint(Core[vertex2].get() + 6);
        // std::cerr << "startCh12 = " << startCh12 << std::endl;
        //  uint sizeAxis12 = charTouint(Core[vertex2].get() + startCh12);
        uint sizeAxis12 = sizeAxis(vertex2, 0, 0);
        // std::cerr << "sizeAxis12 = " << sizeAxis12 << std::endl;
        uint startAxis1 = startAxis(vertex2, 0, 1);
        uint vertex3 = charTouint(Core[vertex2].get() + startAxis1 + 6);
        // std::cerr << "vertex3 = " << vertex3 << std::endl;

        if (vertex3 == ttt + byte[1])
        {
            break;
        }
    }
    // std::cerr << "end for loop" << std::endl;
    delete[] byte;
    return vertex2;
}

vector<wstring> splitWstring(const wstring &input, const wstring &del)
{
    std::wistringstream wss(input);
    std::wstring token;
    std::vector<std::wstring> tokens;

    while (std::getline(wss, token, del[0]))
    {
        tokens.push_back(token);
    }

    return tokens;
}
std::vector<std::string> splitStringASCII(const std::string &input, char delimiter)
{
    std::istringstream iss(input);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(iss, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}
float stringAdvance(const std::string &text)
{
    float advance = 0;
    std::u32string utf32Text = utf8ToUtf32(text);

    for (auto c : utf32Text)
    {
        ushort dd = static_cast<ushort>(c);
        uint vertex = ushortTovertex(dd);
        uint widthData = startAxis(vertex, 0, 3);
        uint adv = charTouint(CoRe[vertex] + widthData + 8);
        // auto glyphIter = bitmapGlyphMap.find(c);
        // if (glyphIter == bitmapGlyphMap.end())
        // {
        //     continue;
        // }
        // BitmapGlyphData glyph = glyphIter->second;
        advance += (adv >> 6);
    }
    return advance;
}
std::vector<std::string> splitLines(string &input)
{
    const int maxWidth = 400;
    std::vector<std::string> lines;

    size_t start = 0, end = 0;
    float width = 0;
    auto utf8CharLength = [](unsigned char byte) -> size_t
    {
        if (byte >= 0xF0)
            return 4;
        else if (byte >= 0xE0)
            return 3;
        else if (byte >= 0xC0)
            return 2;
        return 1;
    };

    while (end < input.size())
    {
        unsigned char byte = static_cast<unsigned char>(input[end]);
        size_t charLength = utf8CharLength(byte);
        string currentChar = input.substr(end, charLength);
        if (input[end] == '\n')
        {
            lines.push_back(input.substr(start, end - start + 1));
            start = ++end;
            width = 0;
            continue;
        }
        float advance = stringAdvance(currentChar);
        width = width + advance;

        if (width > maxWidth)
        {
            lines.push_back(input.substr(start, end - start));
            start = end;
            width = 0;
        }
        else
        {
            end += charLength;
            if (end >= input.size())
            {
                lines.push_back(input.substr(start));
            }
        }
    }
    return lines;
}
std::vector<std::wstring> splitLinesW(const std::wstring &input)
{
    // std::cerr << "call splitLinesW()" << std::endl;
    const int maxWidth = 400;
    std::vector<std::wstring> lines;

    size_t start = 0;
    float width = 0;

    for (size_t end = 0; end <= input.size(); ++end)
    {
        // std::cerr << "enter for loop" << std::endl;
        ushort dd = static_cast<ushort>(input[end]);
        // std::cerr << "dd = " << dd << std::endl;
        uint vertex = ushortTovertex(dd);
        // uint widthData = startAxis(vertex, 0, 3);
        // uint adv = charTouint(Core[vertex].get() + widthData + 8);
        if (end == input.size() || input[end] == L'\n')
        {
            lines.push_back(input.substr(start, end - start));
            start = end + 1;
            width = 0;
        }
        else
        {
            // float advance = adv >> 6;
            // width += advance;
            if (width > maxWidth)
            {
                lines.push_back(input.substr(start, end - start));
                start = end;
                width = 0;
            }
        }
    }
    return lines;
}
wstring LogToClient = L"";
void Log(wstring text)
{
    std::cerr << "call Log()" << std::endl;
    LogToClient = L"";
    // wcout << text << endl;
    if (text != L"")
    {
        LogStr.push_back(text);
    }
    int line = 0;
    int size = LogStr.size();
    if (size > 15)
    { // 15줄만 출력하기
        line = size - 15;
    }
    uchar ii = 0;
    // for (int i = line; i < LogStr.size(); i++)
    // {
    //     LogToClient += LogStr[i] + L"\n";
    //     std::vector<std::wstring> lines = splitWstring(LogStr[i], L"\n");
    //     for (int j = 0; j < lines.size(); j++)
    //     {
    //         std::vector<wstring> lines2 = splitLinesW(lines[j]);
    //         for (const auto &line2 : lines2)
    //         {
    //             ii++;
    //             string str = wstringToUtf8(line2);
    //             // RenderText(str, 1000, 1000 - (500 + 15 * ii));
    //         }
    //     }
    // }
}
uchar *wstringToUChar(const std::wstring &str)
{
    // 동적 배열 할당
    unsigned char *result = new unsigned char[str.size() * 2 + 4];
    unsigned char *size = uintToBytes(2 * str.size());
    result[0] = size[0];
    result[1] = size[1];
    result[2] = size[2];
    result[3] = size[3];
    delete[] size;
    int index = 4;
    for (wchar_t c : str)
    {
        unsigned short ushort_c = static_cast<unsigned short>(c);
        result[index++] = static_cast<unsigned char>(ushort_c & 0xFF);
        result[index++] = static_cast<unsigned char>((ushort_c >> 8) & 0xFF);
    }

    return result;
}
std::wstring charToWstring(uchar *val) // size 정보 담아서 전달되어야 함.
{
    wstring re = L"";
    vector<unsigned short> ss;
    uint size = charTouint(val);
    // std::cerr << "size = " << size << std::endl;
    for (int i = 0; i < size / 2; i++)
    {
        ss.push_back(charToushort(val + 4 + 2 * i));
    }
    // delete[] val;
    for (int i = 0; i < ss.size(); i++)
    {
        wstring temp = ushortToWstring(ss[i]);
        // std::wcout << L"temp = " << temp << std::endl;
        re.append(temp);
    }
    return re;
}
uchar *Sheet(uint vertex)
{
    // cerr << "call Sheet()" << endl;
    constexpr uint reSize = 4000;
    uchar *re = new uchar[reSize]();
    ushort nowPosi = 4;
    uint startCh0 = charTouint(Core[vertex].get() + 6);
    ushort nAxis = charToushort(Core[vertex].get() + startCh0);
    // std::cerr << "nAxis = " << nAxis << std::endl;
    if (vertex == 41155)
    {
        delete[] re;
        return wstringToUChar(L"Entrance");
    }
    // int szR = sizeAxis(vertex, 0, 1);
    if (nAxis > 1)
    {
        delete[] re;
        return word(vertex);
    }
    else if (nAxis == 1)
    {
        std::pair<uint, ushort> dd(vertex, 0);
        // uint start1 = startCh(dd.first, dd.second);
        uint startAxis0 = startAxis(dd.first, dd.second, 0);
        dd = charToPair(Core[dd.first].get() + startAxis0);
        // std::cerr << "dd.first = " << dd.first << std::endl;
        // chInfo(dd.first, dd.second);
        while (dd.first != vertex || dd.second != 0)
        {
            uchar *wordDD = word(dd.first); // Assuming word() allocates memory
            uint sizeWord = charTouint(wordDD);
            // std::cerr << "sizeWord = " << sizeWord << std::endl;
            //  wcout << L"sizeWord = " << intToWString(sizeWord) << endl;
            if (nowPosi + sizeWord <= reSize)
            {
                memcpy(re + nowPosi, wordDD + 4, sizeWord);
                nowPosi += sizeWord;
            }

            startAxis0 = startAxis(dd.first, dd.second, 0);
            // uint startAxis = charTouint(Core[dd.first].get() + startCoo + 2)
            if (charTouint(Core[dd.first].get() + startAxis0) == 0)
            {
                delete[] wordDD;
                break;
            }
            dd = charToPair(Core[dd.first].get() + startAxis0);
            // std::cerr << "dd.first = " << dd.first << std::endl;
            delete[] wordDD;
        }
    }
    uchar *sizeRe = uintToBytes(nowPosi - 4);
    memcpy(re, sizeRe, 4); // Copy 4 bytes of size
    delete[] sizeRe;
    // if (!re)
    // {
    //     std::cerr << "re is a null pointer." << endl;
    // }
    // else
    // {
    //     std::cerr << "re is not a null pointer." << endl;
    // }
    wstring ww = charToWstring(re);
    // wcout << L"ww = " << ww << endl;
    string ss = wstringToUtf8(ww);
    // std::cerr << "re = " << ss << std::endl;
    return re;
}
bool startsWith(const std::wstring &str, const std::wstring &prefix)
{
    if (str.size() < prefix.size())
    {
        return false;
    }
    return std::equal(prefix.begin(), prefix.end(), str.begin());
}
vector<uint> tokenize(vector<uchar> data)
{
    uint coord = 41155;
    // ushort ch = 0;
    vector<uint> re;
    while (data.size() > 0) // str이 빌 때까지 반복
    {
        bool check2 = false;
        uint startCoo = startCh(coord, 0);
        for (int i = 0; i < charTouint(Core[coord].get() + startCoo) / 6; i++)
        {
            uint cd = charTouint(Core[coord].get() + startCoo + 4 + 6 * i);
            uchar *wordcd = word(cd);
            uint sizeWordCd = charTouint(wordcd);
            if (std::equal(wordcd + 4, wordcd + 4 + sizeWordCd, data.begin())) // 기존 글자 존재할 경우 추가 글자 확인
            {
                coord = cd;
                check2 = true;
                break;
            }
        }
        if (!check2)
        {
            if (coord != 41155)
            {
                re.push_back(coord);
                uchar *ws = word(coord);
                vector<uchar> temp(data.begin() + charTouint(ws), data.end());
                data = temp;
                coord = 41155;
            }
        }
    }
    return re;
}
std::vector<uchar> combineVector(const std::vector<uchar> &vec1, const std::vector<uchar> &vec2)
{
    std::vector<uchar> result;
    result.reserve(vec1.size() + vec2.size()); // 미리 메모리를 할당하여 효율성을 높입니다.
    result.insert(result.end(), vec1.begin(), vec1.end());
    result.insert(result.end(), vec2.begin(), vec2.end());
    return result;
}
uchar *combineArr(uchar *arr1, uchar *arr2)
{
    uint size1 = charTouint(arr1);
    uint size2 = charTouint(arr2);
    uchar *result = new uchar[4 + size1 + size2]; // arr1 and arr2 must contain size infomation.
    uchar *sizeRe = uintToBytes(size1 + size2);
    result[0] = sizeRe[0];
    result[1] = sizeRe[1];
    result[2] = sizeRe[2];
    result[3] = sizeRe[3];
    delete[] sizeRe;
    for (int i = 4; i < 4 + size1; i++)
    {
        result[i] = arr1[i];
    }
    for (int i = 4 + size1; i < 4 + size1 + size2; i++)
    {
        result[i] = arr2[i - size1];
    }
    return result;
}
bool areEqual(uchar *arr1, uchar *arr2, uint size)
{
    for (int i = 0; i < size; i++)
    {
        if (arr1[i] != arr2[i])
        {
            return false;
        }
    }
    return true;
}
uint firstToken(uchar *data, uint size) // size 정보 포함해서 받아야 함
{
    bool check2 = true;
    uint coord = 41155;
    while (check2)
    {
        check2 = false;
        uint startCoo = charTouint(Core[coord].get() + 6);
        uint sizeCoo = charTouint(Core[coord].get() + startCoo);
        for (int i = 0; i < sizeCoo / 6; i++)
        {
            uint cd = charTouint(Core[coord].get() + startCoo + 4 + 6 * i);
            uchar *wordcd = word(cd); // 사이즈 포함된 값임
            uint sizeWord = charTouint(wordcd);
            if (areEqual(wordcd + 4, data, sizeWord)) // 기존 글자 존재할 경우 추가 글자 확인
            {
                coord = cd;
                if (sizeWord < size)
                {
                    check2 = true;
                }
                delete[] wordcd;
                break;
            }
            delete[] wordcd;
        }
    }
    return coord;
}
void clearCh(uint vertex, ushort ch)
{
    uint startCoo = startCh(vertex, ch);
    // uint endCh = endOfCh(vertex, ch);
    changeInt(Core[vertex].get(), 0, 10);
    std::fill(Core[vertex].get() + startCoo, Core[vertex].get() + startCoo + 8, 0);
}
void addCh(uint vertex)
{
    std::cerr << "add addCh()" << std::endl;
    ushort numch = numCh(vertex);
    changeShort(Core[vertex].get(), 4, numch + 1);
    uint sizevertex = 4 + charTouint(Core[vertex].get());
    uchar *temp2 = uintToBytes(sizevertex + 4);
    insertArr2(Core[vertex], 6 + 4 * numch, temp2, 4);
    delete[] temp2;
    uchar *z8 = new uchar[8]();
    insertArr2(Core[vertex], sizevertex, z8, 8);
    delete[] z8;
    for (int i = 0; i < numch; i++)
    {
        changeInt(Core[vertex].get(), 6 + 4 * i, charTouint(Core[vertex].get() + 6 + 4 * i) + 4);
    }
    changeInt(Core[vertex].get(), 0, sizevertex + 12);
}
void addCh2(uint vertex)
{
    ushort numch = numCh(vertex);
    changeShort(Core[vertex].get(), 4, numch + 2);
    uint sizevertex = 4 + charTouint(Core[vertex].get());
    uchar *temp2 = uintToBytes2(sizevertex + 8, sizevertex + 16);
    insertArr2(Core[vertex], 6 + 4 * numch, temp2, 8);
    delete[] temp2;
    uchar *z16 = new uchar[16]();
    insertArr2(Core[vertex], sizevertex, z16, 16);
    delete[] z16;
    for (int i = 0; i < numch; i++)
    {
        changeInt(Core[vertex].get(), 6 + 4 * i, charTouint(Core[vertex].get() + 6 + 4 * i) + 8);
    }
    changeInt(Core[vertex].get(), 0, sizevertex + 24);
}
bool isZ8(uchar *arr)
{
    // arr이 가리키는 메모리를 uint64_t로 해석하여 비교
    return *reinterpret_cast<uint64_t *>(arr) == 0;
}
void move(int numTo, uint user)
{
    uint temp{};
    uint tN = cvertex[user];
    uint tartCh = startCh(cvertex[user], cCh[user]);
    if (numTo > 0)
    {
        temp = numTo - 1;
        uint startAxis0 = startAxis(cvertex[user], cCh[user], 0);
        cvertex[user] = charTouint(Core[cvertex[user]].get() + startAxis0 + 6 * temp);
        cCh[user] = charToushort(Core[tN].get() + startAxis0 + 4 + 6 * temp); // cvertex[user]를 변경했으므로 tN을 써야 함!!
    }
    else
    {
        temp = (-1 * numTo) - 1;
        uint startAxis1 = startAxis(cvertex[user], cCh[user], 1);
        // uint szC = charTouint(Core[cvertex[user]].get() + startCoo);
        cvertex[user] = charTouint(Core[cvertex[user]].get() + startAxis1 + 6 * temp);
        cCh[user] = charToushort(Core[tN].get() + startAxis1 + 4 + 6 * temp);
    }
}
void Brain(uint vertex, uchar *data) // 사이즈 정보 포함해서 받아야 함
{
    cerr << "call Brain()" << endl;
    auto start = std::chrono::high_resolution_clock::now();
    uint coord = 41155;
    ushort ch = 0;
    uint beforeCd = vertex;
    ushort beforeCh = 0;
    vector<pair<uint, ushort>> save;
    save.push_back(make_pair(beforeCd, beforeCh));
    int recyle = 0;
    uchar addCh3 = 0;
    uchar addToken = 0;
    // 첫 번째 char 확인, 없으면 추가
    uint dataSz = charTouint(data);
    uint posi = 4;
    while (posi < 4 + dataSz) // data 빌 때까지 반복
    {
        bool check2 = false;
        bool checkRecycle = false;
        coord = firstToken(data + posi, dataSz);
        // 기존 글자 비존재 하는 경우 채널 돌면서 추가 글자 확인
        uchar *wordCoord = word(coord);
        for (int i = 1; i < numCh(coord); i++)
        {
            if (beforeCd == coord && beforeCh == i)
                continue;
            uchar *chVecA = chVec(coord, i);
            bool isZ8A = isZ8(chVecA);
            delete[] chVecA;
            if (isZ8A)
            {
                ch = i;
                check2 = true;
                checkRecycle = true;
                continue;
            }
            uint startCoo = startCh(coord, i);
            pair<uint, ushort> dh = charToPair(Core[coord].get() + startCoo + 4);
            if (sizeAxis(dh.first, 0, 1) == 0)
            {
                continue;
            }
            else
            {
                uchar *word_dh = word(dh.first);
                uchar *strsum = combineArr(wordCoord, word_dh);
                delete[] word_dh;
                uint sizeStrsum = charTouint(strsum);
                // if (equal(strsum + 4, strsum + 4 + sizeStrsum, data + posi) && sizeStrsum <= (dataSz - posi))
                if (areEqual(strsum + 4, data + posi, sizeStrsum))
                {
                    uint coord3 = CoRe.size(); // coord3 = strsum 들어갈 좌표
                    uchar *newCh = new uchar[18];
                    for (int i = 0; i < 18; ++i)
                    {
                        newCh[i] = initValues[i];
                    }
                    CoRe.push_back(newCh);
                    addCh2(coord3);
                    // pushRev2(coord3, 0, pairToBytes2(coord, 0, dh.first, 0));
                    // pushRev(coord3, 0, pairToBytes(coord, 0));
                    // pushRev(coord3, 0, pairToBytes(dh.first, 0));
                    // pushCoo(coord, 0, pairToBytes(coord3, 0));
                    pushPair(coord3, 0, 1, coord, 0);
                    pushPair(coord3, 0, 1, dh.first, 0);
                    pushPair(coord, 0, 0, coord3, 0);
                    int szC = sizeAxis(coord, 0, 0) / 6 - 1;

                    for (int j = szC - 1; j >= 0; j--)
                    {
                        uint startCoo2 = startCh(coord, 0);
                        uint i46j = charTouint(Core[coord].get() + startCoo2 + 4 + 6 * j);
                        uchar *wordi46j = word(i46j);
                        uint sizeWordi46j = charTouint(wordi46j);
                        if (equal(strsum + 4, strsum + 4 + sizeStrsum, wordi46j + 4) && sizeStrsum < sizeWordi46j)
                        {
                            // eraseCoo(coord, 0, j);
                            erasePair(coord, 0, 0, j);
                            // pushCoo(coord3, 0, pairToBytes(i46j, 0));
                            pushPair(coord3, 0, 0, i46j, 0);
                            Log(L"\"" + charToWstring(wordi46j) + L"\"을(를) \"" + charToWstring(strsum) + L"\" 하위로 이동!");
                        }
                        delete[] wordi46j;
                    }
                    pair<uint, ushort> temp;
                    pair<uint, ushort> temp2 = dh;
                    bool check3 = false;
                    int ii = 0;
                    while (dh != make_pair((uint)coord, (unsigned short)i))
                    {
                        if (++ii > 1000 || isZ8(chVec(dh.first, dh.second)))
                        {
                            check3 = true;
                            break;
                        }
                        temp = dh;
                        uint startDh = startCh(dh.first, dh.second);
                        dh = charToPair(Core[dh.first].get() + startDh + 4);
                        if (dh.first == beforeCd && dh.second == beforeCh)
                        {
                            for (int j = 1; j < save.size(); j++)
                            {
                                if (save[j] == make_pair((uint)coord, (ushort)i))
                                {
                                    temp = save[j - 1];
                                    temp2 = save[j + 1];
                                    save[j] = make_pair((uint)coord3, (ushort)1);
                                    save.erase(save.begin() + j + 1);
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    if (check3 == false)
                    {
                        clearCh(coord, i);
                        clearCh(temp.first, temp.second);
                        uint startTemp2 = startCh(temp2.first, temp2.second);
                        pair<uint, ushort> temp3 = charToPair(Core[temp2.first].get() + startTemp2 + 4);
                        clearCh(temp2.first, temp2.second); // temp2를 위의 두 줄 코드보다 먼저 clear하면 안 됨!
                        // pushCoo(temp.first, temp.second, pairToBytes(coord3, 1));
                        // pushCoo(coord3, 1, pairToBytes(temp3.first, temp3.second));
                        pushPair(temp.first, temp.second, 0, coord3, 1);
                        pushPair(coord3, 1, 0, temp3.first, temp3.second);
                        coord = coord3;
                        ch = 2;
                        check2 = true;
                        checkRecycle = false;
                        addToken++;
                        break;
                    }
                    else
                    {
                        clearCh(coord, i);
                        coord = coord3;
                        ch = 2;
                        check2 = true;
                        checkRecycle = false;
                        addToken++;
                        break;
                    }
                }
                delete[] strsum;
            }
        }
        if (checkRecycle == true)
            recyle++;
        else if (check2 == false)
        {
            ch = numCh(coord);
            addCh(coord);
            addCh3++;
        }
        wordCoord = word(coord);
        // vector<uchar> temp(data.begin() + charTouint(wordCoord2, 0), data.end());
        // data = temp;
        posi += charTouint(wordCoord);
        delete[] wordCoord;
        // pushCoo(beforeCd, beforeCh, pairToBytes(coord, ch));
        pushPair(beforeCd, beforeCh, 0, coord, ch);
        beforeCd = coord;
        beforeCh = ch; // 현재 채널을 before로 이동
        save.push_back(make_pair(beforeCd, beforeCh));
    }
    // pushCoo(beforeCd, beforeCh, pairToBytes(vertex, 0));
    pushPair(beforeCh, beforeCh, 0, vertex, 0);
    Log(intToWString(recyle) + L"개 재활용!");
    Log(intToWString(addCh3) + L"개 채널 추가!");
    Log(intToWString(addToken) + L"개 Token 추가!");
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    Log(L"걸린 시간: " + intToWString(duration.count()) + L"ms");
}
std::wstring getCurrentTime()
{
    std::time_t rawtime;
    std::tm *timeinfo;
    wchar_t buffer[80];

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::wcsftime(buffer, 80, L"%Y-%m-%d %H:%M:%S", timeinfo);
    return buffer;
}
wstring timeW(time_t timer)
{
    std::tm *timeinfo;
    wchar_t buffer[80];
    timeinfo = std::localtime(&timer);
    std::wcsftime(buffer, 80, L"%Y-%m-%d %H:%M:%S", timeinfo);
    return buffer;
}
// wstring findAndUpdateOrder(uint vertex, ushort ch, ushort num)
// {
//     wstring re = L"";
//     long long tmp = 0;
//     for (int i = 0; i < order[num].size(); i++)
//     {
//         if (vertex == get<0>(order[num][i]) && ch == get<1>(order[num][i]))
//         {
//             re = timeW(get<2>(order[num][i]));
//             tmp = get<3>(order[num][i]);
//             re = re + L"\t" + timeW(tmp);
//             order[num].erase(order[num].begin() + i);
//         }
//     }
//     time_t timer;
//     time(&timer);
//     order[num].push_back(make_tuple(vertex, ch, timer, tmp));
//     return re;
// }
uint numOrder(uint user)
{
    // uint startCoo = charTouint(CoRe[orderStart] + 10); // ch = 1
    // std::cerr << "call numOrder" << std::endl;
    uint startCh1 = charTouint(Core[orderStart].get() + 10);
    ushort numAxis = charToushort(Core[orderStart].get() + startCh1);
    uint startAxis0 = charTouint(Core[orderStart].get() + startCh1 + 2);
    uint vertex = charTouint(Core[orderStart].get() + startAxis0 + 6 * (user - 1));
    ushort ch = charToushort(Core[orderStart].get() + startAxis0 + 6 * (user - 1) + 4);
    uint vertexT = vertex;
    ushort chT = ch;
    uint startAxis2 = charTouint(Core[vertex].get() + startCh1 + 10);
    uint vertex2 = charTouint(Core[vertex].get() + startAxis2);
    ushort ch2 = charToushort(Core[vertex].get() + startAxis2 + 4);
    uint ii = 1;
    // std::cerr << "vertex2 = " << vertex2 << ", ch2 = " << ch2 << std::endl;
    while (vertex2 != vertexT || ch2 != chT)
    {
        ii++;
        vertex = vertex2;
        ch = ch2;
        if (ii == 30018 || ii == 30019)
        {
            uint size = charTouint(Core[vertex].get());
            chInfo3(vertex, ch);
            std::cerr << "ii = " << ii << ", numAxis = " << numAxis << ", startAxis2 = " << startAxis2 << ", vertex2 = " << vertex2 << ", ch2 = " << ch2 << ", size = " << size << std::endl;
        }
        startCh1 = charTouint(Core[vertex].get() + 6 + 4 * ch);
        // std::cerr << "startCh1 = " << startCh1 << std::endl;
        numAxis = charToushort(Core[vertex].get() + startCh1);
        // std::cerr << "numAxis = " << numAxis << std::endl;
        startAxis2 = startAxis(vertex, ch, 2);
        // std::cerr << "startAxis2 = " << startAxis2 << std::endl;
        vertex2 = charTouint(Core[vertex].get() + startAxis2);
        ch2 = charToushort(Core[vertex].get() + startAxis2 + 4);
        // std::cerr << "vertex2 = " << vertex2 << ", ch2 = " << ch2 << std::endl;
    }
    std::cerr << "ii = " << ii << std::endl;
    return ii;
}
wstring findAndUpdateOrder(uint vertex0, ushort ch0, ushort user)
{
    wstring re = L"";
    long tmp = 0;
    // uint orderSize = numOrder(user);
    time_t timer;
    time(&timer);
    uint startCh = charTouint(Core[orderStart].get() + 10); // ch = 1
    uint startAxis0 = startAxis(orderStart, 1, 0);
    uint vertex = charTouint(Core[orderStart].get() + startAxis0 + 6 * (user - 1));
    ushort ch = charToushort(Core[orderStart].get() + startAxis0 + 6 * (user - 1) + 4);
    uint vertexT = vertex;
    ushort chT = ch;
    uint startX3 = startAxis(vertex, ch, 2);
    uint vertex2 = charTouint(Core[vertex].get() + startX3);
    ushort ch2 = charToushort(Core[vertex].get() + startX3 + 4);
    bool check = false;
    std::cerr << "vertex0 = " << vertex0 << ", vertex2 = " << vertex2 << ", vertexT = " << vertexT << "ch0 = " << ch0 << ", ch2 = " << ch2 << ", chT = " << chT << std::endl;
    if (vertexT == vertex0 && chT == ch0)
    {
        startCh = charTouint(Core[orderStart].get() + 10); // ch = 1
        changePair(Core[orderStart].get(), startAxis0 + 6 * (user - 1), vertex2, ch2);
        re = timeW(charToLong(Core[vertexT].get() + startX3 + 6));
        tmp = charToLong(Core[vertexT].get() + startX3 + 14);
        re = re + L"\t" + timeW(tmp);
        std::wcout << L"re = " << re << std::endl;
    }
    std::cerr << "vertex0 = " << vertex0 << ", vertex2 = " << vertex2 << ", vertexT = " << vertexT << "ch0 = " << ch0 << ", ch2 = " << ch2 << ", chT = " << chT << std::endl;
    while (vertex != vertexT || ch != chT)
    {
        vertex = vertex2;
        ch = ch2;
        startX3 = startAxis(vertex, ch, 2);
        vertex2 = charTouint(Core[vertex].get() + startX3);
        ch2 = charToushort(Core[vertex].get() + startX3 + 4);
        // std::cerr << "vertex2 = " << vertex2 <<  ", ch2 = " << ch2 << std::endl;
        if (check == true && vertex2 == vertexT && ch2 == chT)
        {
            uint startAxis32 = startAxis(vertex2, ch2, 2);
            changePair(Core[vertex2].get(), startAxis32, vertex0, ch0);
        }
        if (vertex2 == vertex0 && ch2 == ch0)
        {
            uint startAxis32 = startAxis(vertex2, ch2, 2);
            uint vertex3 = charTouint(Core[vertex2].get() + startAxis32);
            ushort ch3 = charToushort(Core[vertex2].get() + startAxis32 + 4);
            re = timeW(charToLong(Core[vertex].get() + startX3 + 6));
            tmp = charToLong(Core[vertex].get() + startX3 + 14);
            re = re + L"\t" + timeW(tmp);
            std::wcout << L"re = " << re << std::endl;
            changePair(Core[vertex].get(), startX3, vertex3, ch3);
            changePair(Core[vertex2].get(), startAxis32, vertexT, chT);
            changeLong(Core[vertex2].get(), startAxis32 + 6, timer);
            check = true;
        }
    }
    return re;
}
void eraseOrder(uint vertex0, ushort ch0, ushort user)
{
    // uint orderSize = numOrder(user);
    uint startCoo = charTouint(Core[orderStart].get() + 10); // ch = 1
    uint vertex = charTouint(Core[orderStart].get() + startCoo + 4 + 6 * (user - 1));
    ushort ch = charToushort(Core[orderStart].get() + startCoo + 4 + 6 * (user - 1) + 4);
    uint vertexT = vertex;
    ushort chT = ch;
    uint startX3 = startAxis(vertex, ch, 2);
    uint vertex2 = charTouint(Core[vertex].get() + startX3 + 4);
    ushort ch2 = charToushort(Core[vertex].get() + startX3 + 8);
    while (vertex != vertexT || ch != chT)
    {
        vertex = vertex2;
        ch = ch2;
        startX3 = startAxis(vertex, ch, 2);
        vertex2 = charTouint(Core[vertex].get() + startX3 + 4);
        ch2 = charToushort(Core[vertex].get() + startX3 + 8);
        if (vertex2 == vertex0 && ch2 == ch0)
        {
            // uint startAxis32 = startAxis(vertex2, ch2, 2);
            uint vertex3 = charTouint(Core[vertex2].get() + startX3 + 4);
            ushort ch3 = charToushort(Core[vertex2].get() + startX3 + 8);
            changePair(Core[vertex].get(), startX3 + 4, vertex3, ch3);
        }
    }
}
void pushOrder(uint vertex0, ushort ch0, uint user, long timer1, long timer2)
{
    // uint orderSize = numOrder(user);
    uint startCoo = charTouint(Core[orderStart].get() + 10); // ch = 1
    uint vertex = charTouint(Core[orderStart].get() + startCoo + 4 + 6 * (user - 1));
    ushort ch = charToushort(Core[orderStart].get() + startCoo + 4 + 6 * (user - 1) + 4);
    uint vertexT = vertex;
    ushort chT = ch;
    uint startX3 = startAxis(vertex, ch, 2);
    uint vertex2 = charTouint(Core[vertex].get() + startX3 + 4);
    ushort ch2 = charToushort(Core[vertex].get() + startX3 + 8);
    uint ii = 0;
    while (vertex2 != vertexT || ch2 != chT)
    {
        ii++;
        vertex = vertex2;
        ch = ch2;
        startX3 = startAxis(vertex, ch, 2);
        vertex2 = charTouint(Core[vertex].get() + startX3 + 4);
        ch2 = charToushort(Core[vertex].get() + startX3 + 8);
        if (vertex2 == vertex0 && ch2 == ch0)
        {
            // uint startAxis32 = startAxis(vertex2, ch2, 2);
            uint vertex3 = charTouint(Core[vertex2].get() + startX3 + 4);
            ushort ch3 = charToushort(Core[vertex2].get() + startX3 + 8);
            changePair(Core[vertex].get(), startX3 + 4, vertex3, ch3);
        }
    }
    changePair(Core[vertex].get(), startX3 + 4, vertex0, ch0);
    ushort numAxis = charToushort(Core[vertex].get() + charTouint(Core[vertex].get() + 6 + 4 * ch));
    if (numAxis == 2)
    {
        uchar *arr = new uchar[26];
        uchar *aa = uintToBytes(22);
        ushort numch2 = numCh(vertex0);
        uint startCoo = charTouint(Core[vertex0].get() + (6 + 4 * ch0));
        uint sizeCoo = charTouint(Core[vertex0].get() + startCoo);
        uint startRev = startCoo + 4 + sizeCoo;
        uint sizeRev = charTouint(Core[vertex0].get() + startRev);
        uchar *pair = pairToBytes(vertexT, chT);
        uchar *ll = longlongToBytes(timer1);
        uchar *ll2 = longlongToBytes(timer2);
        for (int k = 0; k < 4; k++)
        {
            arr[k] = aa[k];
        }
        for (int k = 0; k < 6; k++)
        {
            arr[4 + k] = pair[k];
        }
        for (int k = 0; k < 8; k++)
        {
            arr[10 + k] = ll[k];
        }
        for (int k = 0; k < 8; k++)
        {
            arr[18 + k] = ll2[k];
        }
        delete[] aa;
        delete[] pair;
        delete[] ll;
        delete[] ll2;
        for (int k = ch; k < numch2 - 1; k++)
        {
            uint uu = charTouint(Core[vertex0].get() + 6 + 4 * (k + 1));
            changeInt(Core[vertex0].get(), 6 + 4 * (k + 1), uu + 26);
        }
        insertArr2(Core[vertex0], startRev + 4 + sizeRev, arr, 26);
    }
    else if (numAxis == 3)
    {
        startX3 = startAxis(vertex0, ch0, 2);
        changePair(Core[vertex0].get(), startX3 + 4, vertexT, chT);
    }
}
// void eraseOrdervertex(uint vertex, ushort user)
// {
//     for (int i = 0; i < order[user].size(); i++)
//     {
//         if (vertex == get<0>(order[user][i]))
//         {
//             order[user].erase(order[user].begin() + i);
//         }
//     }
// }
// void read_order(const std::string &file_path)
// {
//     ifstream file(file_path, ios::binary);
//     while (file)
//     {
//         unsigned int size;
//         file.read(reinterpret_cast<char *>(&size), sizeof(size));
//         if (!file)
//             break;
//         vector<tuple<int, ushort, long long, long long>> inner_v(size);
//         for (auto &t : inner_v)
//         {
//             int i;
//             ushort us;
//             long long tt;
//             long long tt2;
//             file.read(reinterpret_cast<char *>(&i), sizeof(i));
//             file.read(reinterpret_cast<char *>(&us), sizeof(us));
//             file.read(reinterpret_cast<char *>(&tt), sizeof(tt));
//             file.read(reinterpret_cast<char *>(&tt2), sizeof(tt2));
//             t = make_tuple(i, us, tt, tt2);
//         }
//         order.push_back(inner_v);
//     }
//     file.close();
// }
void sendMsg(int ClientSocket, std::wstring content)
{
    // wcout << "content = " << content << endl;
    //  Create a wide string converter
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    // Convert the wide string to a multibyte string
    std::string strContent = converter.to_bytes(content);

    // Create the response
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(strContent.size()) + "\r\n\r\n" + strContent;

    // Send the response
    const char *buf = response.c_str();
    int total_sent = 0;
    while (total_sent < response.size())
    {
        int sent = send(ClientSocket, buf + total_sent, response.size() - total_sent, 0);
        if (sent == -1)
        {
            // 오류 처리
            std::cerr << "send failed with error: " << std::endl;
            break;
        }
        total_sent += sent;
    }
}
wstring contentList(uint vertex, ushort ch, uint page = 1)
{
    std::cerr << "call contentList" << std::endl;
    wstring ws = L"";
    // uint startCoo = charTouint(Core[vertex].get() + 6 + 4 * ch);
    //  uint sizeCoo = charTouint(Core[vertex].get() + startCoo);
    uint sizeAxis0 = sizeAxis(vertex, ch, 0);
    // uint startRev = startCoo + 4 + sizeCoo;
    // uint sizeRev = charTouint(Core[vertex].get() + startRev);
    uint sizeAxis1 = sizeAxis(vertex, ch, 1);
    // std::cerr << "sizeAxis0 = " << sizeAxis0 << ", sizeAcis1 = " << sizeAxis1 << std::endl;
    if (sizeAxis1 > 0)
    {
        uint prevvertex = startAxis(vertex, ch, 1);
        wstring prev = L"";
        wstring pprev = L"";
        for (int i = 0; i < sizeAxis1 / 6; i++)
        {
            wstring hpL = L"<a href='javascript:__doPostBack(\"LinkButtonRev" + intToWString(i + 1) + L"\", \"\")' onclick='sendTextToServer(" + intToWString(-(i + 1)) + L"); return false;'>" + intToWString(-(i + 1)) + L"</a>";
            uchar *sheetPrev = Sheet(charTouint(Core[vertex].get() + prevvertex + 6 * i));
            prev += hpL + L". " + charToWstring(sheetPrev) + L"<br/>";
            std::wcout << "prev = " << prev << std::endl;
            delete[] sheetPrev;
            // prev += hpL + L". " + charToWstring(Sheet(*reinterpret_cast<uint *>(&prevvertex[6 * i]))) + L"<br/>";
            uint vertex2 = charTouint(Core[vertex].get() + prevvertex + 6 * i);
            ushort ch2 = charToushort(Core[vertex].get() + prevvertex + 4 + 6 * i);
            uint startCh2 = charTouint(Core[vertex2].get() + 6 + 4 * ch2);
            uint sizeCoo2 = sizeAxis(vertex2, ch2, 0);
            uint startRev2 = charTouint(Core[vertex2].get() + startCh2 + 6);
            uint sizeRev2 = sizeAxis(vertex2, ch2, 1);
            // uchar *splPrev2 = CoRe[vertex2] + startRev2 + 4 + 6 * i;
            uint splPrev2 = startRev2 + 6 * i;
            // vector<uchar> splPrev2 = axon2(charTouint(prevvertex, 6 * i), charToushort(prevvertex, 4 + 6 * i));
            for (int j = 0; j < sizeRev2 / 6; j++)
            {
                // pprev += intToWString(-(i + 1)) + L"." + intToWString(j + 1) + L" " + charToWstring(Sheet(*reinterpret_cast<uint *>(&splPrev2[6 * j]))) + L"<br/>";
                uint sheetvertex2 = charTouint(Core[vertex2].get() + splPrev2 + 6 * j);
                if (sheetvertex2 > Core.size())
                {
                    Log(L"error: prev i = " + intToWString(i) + L", pprev j = " + intToWString(j) + L".");
                    continue;
                }
                uchar *sheetPP = Sheet(sheetvertex2);
                pprev += intToWString(-(i + 1)) + L"." + intToWString(j + 1) + L" " + charToWstring(sheetPP) + L"<br/>";
                delete[] sheetPP;
            }
        }
        ws += pprev + L"<p class='prev'>" + prev;
    }
    uchar *sheetvertex = Sheet(vertex);
    ws += L"<p class='this'>" + charToWstring(sheetvertex) + L"<br/>" + L"<p class='next'>"; // 현재 단계
    std::wcout << L"this content = " << ws << std::endl;
    delete[] sheetvertex;
    uint startCh = charTouint(Core[vertex].get() + 6 + 4 * ch);
    uint startAxis0 = charTouint(Core[vertex].get() + startCh + 2);
    // uchar *nextvertex = Core[vertex].get() + startAxis0;
    for (int i = (page - 1) * 50; i < page * 50 - 1 && sizeAxis0 != 0; i++)
    {
        if (i == sizeAxis0 / 6)
        {
            break;
        }
        wstring hpL = L"<a href='javascript:__doPostBack(\"LinkButtonNext" + intToWString(i + 1) + L"\", \"\")' onclick='sendTextToServer(" + intToWString(i + 1) + L"); return false;'>" + intToWString(i + 1) + L"</a>";
        // ws += hpL + L". " + charToWstring(Sheet(*reinterpret_cast<uint *>(&nextvertex[6 * i]))) + L"<br/>";
        uint nextvertex = charTouint(Core[vertex].get() + startAxis0 + 6 * i);
        ushort nextCh = charToushort(Core[vertex].get() + startAxis0 + 6 * i + 4);
        if (i == 10)
        {
            chInfo3(nextvertex, nextCh);
        }
        uchar *sheetNext = Sheet(nextvertex);
        ws += hpL + L". " + charToWstring(sheetNext) + L"<br/>";
        std::wcout << "next content = " << ws << std::endl;
        delete[] sheetNext;
    }
    if (sizeAxis0 / 6 > 50)
    {
        ws += L"<br />Page ";
        for (int i = 1; i <= (sizeAxis0 / 6 / 50) + 1; i++)
        {
            ws += L" <a href='javascript:__doPostBack(\"PageButton" + intToWString(i) + L"\", \"\")' onclick='sendTextToServer(" + L"\"Page\" + " + intToWString(i) + L"); return false;'>" + intToWString(i) + L"</a>";
        }
    }
    ws += L"<style> .prev{margin-left: 10px;} .this{margin-left: 20px;} .next{margin-left:30px;}</style>";
    return ws;
}

void study(uint user)
{
    uint startCh = charTouint(Core[orderStart].get() + 10); // ch = 1
    uint startAxis0 = startAxis(orderStart, 1, 0);
    uint vertex = charTouint(Core[orderStart].get() + startAxis0 + 6 * (user - 1));
    ushort ch = charToushort(Core[orderStart].get() + startAxis0 + 6 * (user - 1) + 4);
    cvertex[user] = vertex;
    cCh[user] = ch;
    // uint startCoo2 = charTouint(CoRe[vertex] + 6 + 4 * ch);
    // uint sizeCoo2 = charTouint(CoRe[vertex] + startCoo2);
    // uint startRev2 = startCoo2 + 4 + sizeCoo2;
    // uint sizeRev2 = charTouint(CoRe[vertex] + startRev2);
    // uint startAxis3 = startRev2 + 4 + sizeRev2;
    // uint vertex2 = charTouint(CoRe[vertex] + startAxis3 + 4);
    // ushort ch2 = charToushort(CoRe[vertex] + startAxis3 + 8);
    // findAndUpdateOrder(vertex, ch, user);
}
// void study2(uint user)
// {
//     uint t1 = get<0>(order[user][0]);
//     ushort t2 = get<1>(order[user][0]);
//     eraseOrder(t1, t2, user);
//     // study(user);
// }
uint sizeGarbage()
{
    std::cerr << "call sizeGarbage" << std::endl;
    int i = 0;
    uint temp = charTouint(Core[gar].get() + startAxis(gar, 0, 0));
    while (temp != gar)
    {
        temp = charTouint(Core[temp].get() + startAxis(temp, 0, 0));
        i++;
    }
    std::cerr << "end sizeGarbage" << std::endl;
    return i;
}
size_t vmSize = 0, vmRSS = 0;
int fps = 0;
bool getMemoryUsage(size_t &vmSize, size_t &vmRSS)
{
    std::ifstream statusFile("/proc/self/status");
    std::string line;

    if (!statusFile.is_open())
    {
        return false;
    }

    while (std::getline(statusFile, line))
    {
        if (line.substr(0, 6) == "VmSize")
        {
            sscanf(line.c_str(), "VmSize: %lu kB", &vmSize);
        }
        else if (line.substr(0, 6) == "VmRSS:")
        {
            sscanf(line.c_str(), "VmRSS: %lu kB", &vmRSS);
        }
    }

    statusFile.close();
    return true;
}
wstring infoStr = L"";
void info()
{
    uchar *sh1 = Sheet(34875);
    uchar *sh2 = Sheet(34878);
    uchar *sh3 = Sheet(34880);
    infoStr = L"\norder.size: " + intToWString(numOrder(1)) + L"\nGarbage.size: " + intToWString(sizeGarbage()) + L"\nCsave: " + charToWstring(sh1) + L"," + charToWstring(sh2) + L"," + charToWstring(sh3) + L"\nvmSize: " + intToWString(vmSize) + L" kB\nvmRSS: " + intToWString(vmRSS) + L" kB\nFPS: " + intToWString(fps);
    wcout << infoStr << endl;
    delete[] sh1;
    delete[] sh2;
    delete[] sh3;
    std::vector<std::wstring> lines = splitWstring(infoStr, L"\n");
    short ll = lines.size();
    for (int i = 0; i < ll; ++i)
    {
        string line = wstringToUtf8(lines[i]);
        // RenderText(line, 1000, 1000 - (20 + 15 * i));
    }
    std::cerr << "end info" << std::endl;
}
void pushGarbage(uint vertex) // 오류 있음 수정해야 함.
{
    uint next = charTouint(Core[gar].get() + 14); // 14가 다음 노드 가리키는 위치임
    // uchar newCh[18] = {14, 0, 0, 0, 1, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //startCoo = 6이 아니라 10이 되어야 함.
    //  delete[] CoRe[gar]; //pushCoo에서 CoRe[gar]를 사용하므로 메모리 해제하면 안됨
    // CoRe[gar] = newCh;
    uchar *ptb = pairToBytes(vertex, 0);
    uchar *ptb2 = pairToBytes(next, 0);
    uchar newCh2[24] = {20, 0, 0, 0, 1, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 24; i++)
    {
        Core[vertex][i] = newCh2[i];
    }
    // delete[] CoRe[vertex];
    // CoRe[vertex] = newCh2;
    for (int i = 0; i < 6; i++)
    {
        Core[gar][i + 14] = ptb[i];
        Core[vertex][i + 14] = ptb2[i];
    }
    // pushCoo(gar, 0, ptb);
    // pushCoo(vertex, 0, ptb2);
    delete[] ptb;
    delete[] ptb2;
    // delete[] newCh2;
}
void clearToken(uint vertex)
{
    uint startCoo = startCh(vertex, 0);
    pair<uint, ushort> cod = charToPair(Core[vertex].get() + startCoo + 4);
    while (cod != make_pair((uint)vertex, (ushort)0))
    {
        pair<uint, ushort> temp = cod;
        startCoo = startCh(cod.first, cod.second);
        cod = charToPair(Core[cod.first].get() + startCoo + 4);
        // CoRe[temp.first][temp.second] = zero8;
        clearCh(temp.first, temp.second);
    }
}
void deletevertex(uint vertex)
{
    if (sizeAxis(vertex, 0, 1) == 6)
    {
        clearToken(vertex);
    }
    uint numch = numCh(vertex);
    for (int i = 0; i < numch; i++)
    {
        // vector<uint> sizeCRn = sizeCoRe(vertex, i);
        uint sizeAxis0 = sizeAxis(vertex, i, 0);
        uint sizeAxis1 = sizeAxis(vertex, i, 1);
        uint startI = startCh(vertex, i);
        for (int j = 0; j < sizeAxis0 / 6; j++)
        {
            // uint startI = startCh(vertex, i);
            uint startAxis0 = charTouint(Core[vertex].get() + startI + 2);
            pair<uint, ushort> temp = charToPair(Core[vertex].get() + startAxis0 + 6 * j);
            // int size = CoRe[temp.first].size();
            int size = numCh(temp.first);
            if (size > 0 && size <= temp.second + 1)
            {
                // uint szAxis0 = sizeAxis(temp.first, temp.second, 0);
                uint szAxis1 = sizeAxis(temp.first, temp.second, 1);
                // vector<uint> szCR = sizeCoRe(temp.first, temp.second);
                if (szAxis1 > 0)
                {
                    for (int k = 0; k < szAxis1 / 6; k++)
                    {
                        uint startTemp = startCh(temp.first, temp.second);
                        uint startAxis1 = charTouint(Core[temp.first].get() + startTemp + 6);
                        pair<uint, ushort> temp2 = charToPair(Core[temp.first].get() + startAxis1 + 6 * k);
                        if (temp2 == make_pair((uint)vertex, (ushort)i))
                        {
                            // eraseRev(CoRe[temp.first], startTemp, 8 + szCR[0] + 6 * k);
                            // eraseRev(temp.first, temp.second, k);
                            erasePair(temp.first, temp.second, 1, k);
                            szAxis1 = szAxis1 - 6;
                        }
                    }
                }
            }
        }
        for (int j = 0; j < sizeAxis1 / 6; j++)
        {
            uint startAxis1 = startAxis(vertex, i, 1);
            pair<uint, ushort> temp = charToPair(Core[vertex].get() + startAxis1 + 6 * j);
            // int size = numCh(temp.first);
            ushort numCh = charToushort(Core[temp.first].get() + 4);
            if (numCh > 0 && numCh <= temp.second + 1)
            {
                uint startTemp = startCh(temp.first, temp.second);
                // uint szC = charTouint(CoRe[temp.first] + startTemp);
                uint szAxis0 = sizeAxis(temp.first, temp.second, 0);
                if (szAxis0 > 0)
                {
                    for (int k = 0; k < szAxis0 / 6; k++)
                    {
                        uint startAxis0 = charTouint(Core[temp.first].get() + startTemp + 2);
                        pair<uint, ushort> temp2 = charToPair(Core[temp.first].get() + startAxis0 + 6 * k);
                        if (temp2 == make_pair((uint)vertex, (ushort)i))
                        {
                            // eraseCoo(CoRe[temp.first], startTemp, 4 + 6 * k);
                            // eraseCoo(temp.first, temp.second, k);
                            erasePair(temp.first, temp.second, 0, k);
                            szAxis0 = szAxis0 - 6; // szC를 6 빼주어야 다음 loop에서 오류 안 남!!
                        }
                    }
                }
            }
        }
        eraseOrder(vertex, i, 1);
    }
    // CoRe[vertex].clear();
    pushGarbage(vertex);
}
void cut(uint vertex, ushort ch, uint index)
{
    // vector<uint> sizeCRn = sizeCoRe(vertex, ch);
    // uint sizeAxis0 = sizeAxis(vertex, ch, 0);
    // uint sizeAxis1 = sizeAxis(vertex, ch, 1);
    uint startAxis0 = startAxis(vertex, ch, 0);
    pair<uint, ushort> temp = charToPair(Core[vertex].get() + startAxis0 + 6 * index);
    // uint startTemp = startCh(temp.first, temp.second);
    int size = numCh(temp.first);
    if (size > 0 && size <= temp.second + 1)
    {
        // vector<uint> szCR = sizeCoRe(temp.first, temp.second);
        // uint szAxis0 = sizeAxis(temp.first, temp.second, 0);
        uint szAxis1 = sizeAxis(temp.first, temp.second, 1);
        if (szAxis1 > 0)
        {
            for (int k = 0; k < szAxis1 / 6; k++)
            {
                if (temp == make_pair(cvertex[1], cCh[1]))
                {
                    // eraseRev(CoRe[temp.first], startTemp, 8 + szCR[0] + 6 * k);
                    // eraseRev(temp.first, temp.second, k);
                    erasePair(temp.first, temp.second, 1, k);
                }
            }
        }
    }
    // eraseCoo(CoRe[vertex], startCoo, 4 + 6 * index);
    // eraseCoo(vertex, ch, index);
    erasePair(vertex, ch, 0, index);
}
uint popGarbage()
{
    int i = 0;
    uint re = charTouint(Core[gar].get() + 14);
    uint next = charTouint(Core[re].get() + 14);
    clearCh(re, 0);
    clearCh(gar, 0);
    // uchar *ptb = pairToBytes(next, 0);
    //  pushCoo(gar, 0, ptb); // pushCoo에서 delete[]
    pushPair(gar, 0, 0, next, 0);
    // delete[] ptb;
    return re;
}
void AddStringTovertex(const string &str, uint vertex, ushort ch, uint user)
{
    cerr << "call AddStringTovertex()" << endl;
    wstring wstr = utf8ToWstring(str);
    uchar *wstr2 = wstringToUChar(wstr);
    time_t timer;
    time(&timer);
    uint newcd = CoRe.size();
    if (sizeGarbage() > 10)
    { // 쓰레기통에서 재활용
        newcd = popGarbage();
        addCh(newcd); // make count of ch as 2
        link(vertex, ch, newcd, 1);
        // order[user].push_back(make_tuple(newcd, 1, timer, timer)); // 생성시간 = timer
        pushOrder(newcd, 1, user, timer, timer);
        Brain(newcd, wstr2);
    }
    else
    {
        uchar *newCh2 = new uchar[18];
        for (int i = 0; i < 18; ++i)
        {
            newCh2[i] = initValues[i];
        }
        CoRe.push_back(newCh2);
        addCh(newcd);
        link(vertex, ch, newcd, 1);
        // order[user].push_back(make_tuple(newcd, 1, timer, timer));
        pushOrder(newcd, 1, user, timer, timer);
        Brain(newcd, wstr2);
    }
    delete[] wstr2;
}
void AddStringTovertex2(const string &str1, const string &str2, uint vertex, ushort ch, uint user)
{
    wstring wstr1 = utf8ToWstring(str1);
    wstring wstr2 = utf8ToWstring(str2);
    uchar *char1 = wstringToUChar(wstr1);
    uchar *char2 = wstringToUChar(wstr2);
    time_t timer;
    time(&timer);
    uint newcd = CoRe.size();
    if (sizeGarbage() > 11)
    { // 쓰레기통에서 재활용
        wcout << L"sizeGabage() > 10" << endl;
        newcd = popGarbage();
        wcout << L"popGarbage = " << intToWString(newcd) << endl;
        addCh(newcd); // make count of ch as 2
        link(vertex, ch, newcd, 1);
        // order[user].push_back(make_tuple(newcd, 1, timer, timer)); // 생성시간 = timer
        pushOrder(newcd, 1, user, timer, timer);
        Brain(newcd, char1);
        uint newcd2 = popGarbage();
        addCh(newcd2);
        link(newcd, 1, newcd2, 1);
        // order[user].push_back(make_tuple(newcd2, 1, timer, timer)); // 생성시간 = timer
        pushOrder(newcd2, 1, user, timer, timer);
        Brain(newcd2, char2);
    }
    else
    {
        uchar *newCh2 = new uchar[18];
        for (int i = 0; i < 18; ++i)
        {
            newCh2[i] = initValues[i];
        }
        CoRe.push_back(newCh2);
        addCh(newcd);
        link(vertex, ch, newcd, 1);
        // order[user].push_back(make_tuple(newcd, 1, timer, timer));
        pushOrder(newcd, 1, user, timer, timer);
        Brain(newcd, char1);
        uchar *newCh3 = new uchar[18];
        for (int i = 0; i < 18; ++i)
        {
            newCh2[i] = initValues[i];
        }
        CoRe.push_back(newCh3);
        addCh(newcd + 1);
        link(newcd, 1, newcd + 1, 1);
        // order[user].push_back(make_tuple(newcd + 1, 1, timer, timer));
        pushOrder(newcd + 1, 1, user, timer, timer);
        Brain(newcd + 1, char2);
    }
    delete[] char1;
    delete[] char2;
}
void save(string directory)
{
    std::cerr << "call save()" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    ofstream out(directory + "Brain6.bin", ios::binary);
    int ii = 0;
    uint sizeCore = Core.size();
    out.write(reinterpret_cast<const char *>(&sizeCore), 4);
    for (int i = 0; i < sizeCore; i++)
    {
        uint size = charTouint(Core[i].get());
        if (size > 1000000)
        {
            std::cerr << "out of size!" << std::endl;
            break;
        }
        out.write(reinterpret_cast<const char *>(Core[i].get()), size + 4);
    }
    out.close();
    auto end = std::chrono::high_resolution_clock::now();

    // 경과 시간 계산 (밀리초 단위)
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // 경과 시간 출력 (밀리초 단위)
    Log(L"저장시간: " + intToWString(duration.count()) + L"ms");
}
unsigned int stringToUint(const std::string &input)
{
    std::istringstream iss(input);
    unsigned int number = 0;
    iss >> number;
    return number;
}
bool tryConvertToInt(const std::string &str, int &outValue)
{
    std::istringstream iss(str);
    iss >> outValue;

    // 파싱 후 스트림의 끝에 도달했는지 확인 (다른 문자가 없는지 확인)
    return !iss.fail() && iss.eof();
}
// URL 디코딩 함수
std::string urlDecode(std::string str)
{
    std::string result;
    result.reserve(str.length());
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] == '+')
        {
            result += ' ';
        }
        else if (str[i] == '%' && str.length() > i + 2)
        {
            std::string hex = str.substr(i + 1, 2);
            char x = static_cast<char>(std::stoi(hex, nullptr, 16));
            result += x;
            i += 2;
        }
        else
        {
            result += str[i];
        }
    }
    return result;
}
// 클라이언트로부터 데이터를 읽고 처리하는 함수
std::map<std::string, std::string> parsePostData(const std::string &postData)
{
    std::map<std::string, std::string> dataMap;

    std::istringstream postDataStream(postData);
    std::string line;

    // Skip the first boundary line
    std::getline(postDataStream, line);

    // Read the content lines
    while (std::getline(postDataStream, line))
    {
        // Check if the line contains form data
        if (line.find("Content-Disposition: form-data") != std::string::npos)
        {
            // Find the name and value of the form data
            std::string key;
            std::string value;

            // Skip "Content-Disposition: form-data; name="
            size_t startPos = line.find("name=\"") + 6;
            size_t endPos = line.find("\"", startPos);
            key = line.substr(startPos, endPos - startPos);

            // Skip empty line
            std::getline(postDataStream, line);

            // Get the value
            std::getline(postDataStream, value);
            // Remove '\r' character from the value
            // string value2 = value.substr(0, value.size() - 1);

            // Store the key-value pair in the map
            dataMap[key] = value.substr(0, value.size() - 1);
        }
    }

    return dataMap;
}
std::map<std::string, std::string> parseQuery(const std::string &query)
{
    std::map<std::string, std::string> data;
    std::istringstream queryStream(query);
    std::string pair;

    while (std::getline(queryStream, pair, '&'))
    {
        auto delimiterPos = pair.find('=');
        auto key = pair.substr(0, delimiterPos);
        auto value = pair.substr(delimiterPos + 1);

        // URL 디코딩 필요 시 여기에 추가
        value = urlDecode(value);
        data[key] = value;
    }

    return data;
}
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    for (int i = 0; i < argc; i++)
    {
        std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    return 0;
}
// bool insertUser(const std::string &username, const std::string &email, const std::string &password)
// {
//     sqlite3 *db;
//     char *zErrMsg = 0;
//     int rc;
//     // 데이터베이스 파일 열기 (없으면 생성)
//     rc = sqlite3_open("example.db", &db);
//     if (rc)
//     {
//         std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
//         return false;
//     }
//     // 테이블 존재 여부 확인
//     // bool tableExists = false;
//     std::string sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='USERS';";
//     // rc = sqlite3_exec(db, sql.c_str(), [](void *data, int argc, char **argv, char **azColName) -> int {
//     //     //tableExists = true;
//     //     return 0;
//     // }, nullptr, &zErrMsg);
//     // if (rc != SQLITE_OK)
//     // {
//     //     std::cerr << "SQL error: " << zErrMsg << std::endl;
//     //     sqlite3_free(zErrMsg);
//     // }
//     // 테이블이 없으면 생성
//     //    if (!tableExists)
//     //    {
//     // sql = "CREATE TABLE USERS (USERNAME TEXT NOT NULL UNIQUE, EMAIL TEXT NOT NULL, PASSWORD TEXT NOT NULL);";
//     // rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);
//     // if (rc != SQLITE_OK)
//     //{
//     //   std::cerr << "SQL error: " << zErrMsg << std::endl;
//     //  sqlite3_free(zErrMsg);
//     //  return false;
//     //}
//     //    }
//     // 사용자 데이터를 저장하기 위한 SQL 쿼리 생성
//     sql = "INSERT INTO USERS (USERNAME, EMAIL, PASSWORD) VALUES ('" + username + "', '" + email + "', '" + password + "');";
//     // SQL 쿼리 실행
//     rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
//     if (rc != SQLITE_OK)
//     {
//         std::cerr << "SQL error: " << zErrMsg << std::endl;
//         sqlite3_free(zErrMsg);
//     }
//     else
//     {
//         std::cout << "Record inserted successfully" << std::endl;
//     }
//     // 데이터베이스 연결 닫기
//     sqlite3_close(db);
//     return rc == SQLITE_OK;
// }
void handleSignUp(const std::string &requestBody, int clientSocket)
{
    auto data = parseQuery(requestBody);

    // 데이터베이스에 사용자 정보를 저장합니다. 실제 사용 시, 비밀번호 해시 등의 처리가 필요합니다.
    // if (insertUser(data["username"], data["email"], data["password"]))
    // {
    //     std::string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nSignup Successful!";
    //     write(clientSocket, response.c_str(), response.length());
    // }
    // else
    // {
    //     std::string response = "HTTP/1.1 500 Internal Server Error\nContent-Type: text/plain\n\nSignup Failed!";
    //     write(clientSocket, response.c_str(), response.length());
    // }
    std::string response = "Signup Successful!";
    AddStringTovertex(data["username"], 34196, 1, 1);
    uint startCoo = charTouint(Core[34196].get() + 6 + 4 * 1);
    uint sizeCoo = charTouint(Core[34196].get() + startCoo);
    AddStringTovertex(data["password"], charTouint(Core[34196].get() + startCoo + sizeCoo - 6), 1, 1);
    AddStringTovertex(data["email"], charTouint(Core[34196].get() + startCoo + sizeCoo - 6), 1, 1);
    send(clientSocket, response.c_str(), response.size(), 0);
}
// bool checkLogin(const std::string &username, const std::string &password)
// {
//     sqlite3 *db;
//     char *zErrMsg = 0;
//     int rc;
//     bool loginSuccess = false;

//     // 데이터베이스 열기
//     rc = sqlite3_open("example.db", &db);
//     if (rc)
//     {
//         std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
//         return false;
//     }

//     std::string sql = "SELECT * FROM USERS WHERE USERNAME='" + username + "' AND PASSWORD='" + password + "';";

//     // 쿼리 실행을 위한 콜백 함수
//     auto callback = [](void *data, int argc, char **argv, char **azColName) -> int
//     {
//         *(bool *)data = true; // 결과가 있는 경우 loginSuccess를 true로 설정
//         return 0;
//     };
//     // SQL 쿼리 실행
//     rc = sqlite3_exec(db, sql.c_str(), callback, &loginSuccess, &zErrMsg);
//     if (rc != SQLITE_OK)
//     {
//         std::cerr << "SQL error: " << zErrMsg << std::endl;
//         sqlite3_free(zErrMsg);
//     }

//     // 데이터베이스 닫기
//     sqlite3_close(db);

//     return loginSuccess;
// }
wstring makeContent(uint user, wstring inputText, wstring Log2 = L"")
{
    Log(Log2);
    info();
    ushort cch = charToushort(Core[cvertex[user]].get() + 4) - 1;
    wstring contentBody = contentList(cvertex[user], cCh[user]);
    wstring timeContent = findAndUpdateOrder(cvertex[user], cCh[user], user);
    wstring content = intToWString(user) + L"\t" + intToWString(cvertex[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + contentBody + L"\t" + intToWString(Core.size()) + L"\t" + inputText + L"\t" + timeContent + L"\t" + LogToClient + L"\t" + infoStr + L"\t" + intToWString(cch) + L"\t" + intToWString(copyvertex.first) + L"\t" + intToWString((int)copyvertex.second);
    // std::wcout << L"content = " << content << std::endl;
    return content;
}
void handleLogin(const std::string &requestBody, int client_socket)
{
    auto data = parsePostData(requestBody);
    // bool isAuthenticated = checkLogin(data["username"], data["password"]);
    uint IDList = startAxis(34196, 1, 0);
    // std::cerr << "IDList = " << IDList << std::endl;
    bool check = false;
    uint startCh1 = charTouint(Core[34196].get() + 6 + 4 * 1);
    // uint sizeIDList = charTouint(Core[34196].get() + startCh1);
    uint sizeIDList = sizeAxis(34196, 1, 0);
    // std::cerr << "sizeIDList = " << sizeIDList << std::endl;

    wstring content = L"";
    for (int i = 0; i < sizeIDList / 6 - 1; i++)
    {
        uint IDvertex = charTouint(Core[34196].get() + IDList + 6 * (i));
        // std::cerr << "i = " << i << ", IDvertex = " << IDvertex << std::endl;
        uchar *sheetID = Sheet(IDvertex);
        wstring ww = charToWstring(sheetID);
        // std::wcout << L"sheetID = " << ww << std::endl;
        if (utf8ToWstring(data["username"]) == ww) // ID가 존재하는 경우
        {
            // startCh1 = startCh(34196, 1);
            uint startAxis0 = charTouint(Core[34196].get() + startCh1 + 2);
            pair<uint, ushort> userID = charToPair(Core[34196].get() + startAxis0 + 6 * i);
            uint startUserID = startCh(userID.first, userID.second);
            uint startAxis0_2 = charTouint(Core[userID.first].get() + startUserID + 2);
            pair<uint, ushort> Pass = charToPair(Core[userID.first].get() + startAxis0_2);
            wstring www = charToWstring(Sheet(Pass.first));
            // std::wcout << L"www = " << www << std::endl;
            if (utf8ToWstring(data["password"]) == www) // 비밀번호가 동일한 경우
            {
                std::cerr << "password 동일" << std::endl;
                uint startPass = startCh(Pass.first, Pass.second);
                uint startAxis0_Pass = charTouint(Core[Pass.first].get() + startPass + 2);
                pair<uint, ushort> start = charToPair(Core[Pass.first].get() + startAxis0_Pass);
                user = i + 1;
                cvertex[user] = start.first;
                cCh[user] = start.second;
                // content = intToWString(user) + L"\t" + intToWString(start.first) + L"\t" + intToWString(start.first) + L"\t" + contentList(start.first, start.second) + L"\t" + intToWString(CoRe.size()) + L"\t"; // 시작 화면을 보냄
                sendMsg(client_socket, makeContent(user, L""));
            }
            else
            {
                content = intToWString(0) + L"\t" + intToWString(34199) + L"\t" + L"0" + L"\t" + L"비밀번호가 틀립니다. 다시 입력해 주세요." + L"\t" + intToWString(CoRe.size()) + L"\t"; // password 입력 화면을 보냄 user, vertex, ch, sheet 순서
                sendMsg(client_socket, content);
            }
            check = true;
            delete[] sheetID;
            break;
        }
        delete[] sheetID;
    }
    if (!check)
    {
        content = intToWString(0) + L"\t" + intToWString(34198) + L"\t" + L"0" + L"\t" + L"없는 아이디입니다. 다시 아이디를 입력해 주세요."; // 다시 아이디 입력 화면을 보냄
        sendMsg(client_socket, content);
    }
    std::string response;
    // if (isAuthenticated)
    // {
    //     uint startCoo = startCh(34196, 1);
    //     pair<uint, ushort> userID = charToPair(CoRe[34196] + startCoo + 4 + 6 * (user - 1));
    //     uint startUserID = startCh(userID.first, userID.second);
    //     pair<uint, ushort> Pass = charToPair(CoRe[userID.first] + startUserID + 4);
    //     uint startPass = startCh(Pass.first, Pass.second);
    //     pair<uint, ushort> start = charToPair(CoRe[Pass.first] + startPass + 4);
    //     // content = intToWString(user) + L"\t" + intToWString(start.first) + L"\t" + intToWString(start.first) + L"\t" + contentList(start.first, start.second) + L"\t" + intToWString(CoRe.size()) + L"\t"; // 시작 화면을 보냄
    //     sendMsg(client_socket, makeContent(user, start.first, start.second, contentList(start.first, start.second), L""));
    //     // response = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nLogin Successful!";
    // }
    // else
    // {
    //     response = "HTTP/1.1 401 Unauthorized\nContent-Type: text/plain\n\nLogin Failed!";
    //     write(client_socket, response.c_str(), response.length());
    // }
}
void change_data(uint vertex, uchar *data)
{
    clearToken(vertex);
    clearCh(vertex, 0);
    Brain(vertex, data);
}
std::string handle_check_duplicate(const std::string &request)
{
    // Parse username from the request
    std::regex rgx("GET /checkDuplicate\\?username=([^ ]+) HTTP/1.1");
    std::smatch match;
    if (std::regex_search(request, match, rgx))
    {
        std::string username = match[1].str();
        // Here you would perform the actual duplicate check
        uint IDList = startAxis(34196, 1, 1);
        uint startCoo = charTouint(Core[34196].get() + 6 + 4 * 1);
        uint sizeIDList = charTouint(Core[34196].get() + startCoo);
        bool check = true;
        for (int i = 0; i < sizeIDList / 6; i++)
        {
            if (utf8ToWstring(username) == charToWstring(Sheet(*reinterpret_cast<uint *>(Core[34196].get() + IDList + 6 * i)))) // ID가 존재하는 경우
            {
                return "HTTP/1.1 200 OK\nContent-Length: 18\n\nUsername not available";
            }
        }
    }
    return "HTTP/1.1 200 OK\nContent-Length: 18\n\nUsername available";
}
int client_socket = 0;
int Network()
{
    struct sockaddr_in server_addr;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Server socket creation failed.\n";
        return -1;
    }
    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080); // 포트 번호는 8080으로 설정
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        std::cerr << "setsockopt error.\n";
        return -1;
    }
    // 서버 소켓에 주소 바인드
    if (::bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cerr << "Bind error.\n";
        return -1;
    }
    // 클라이언트의 연결 요청을 대기
    if (listen(serverSocket, 5) == -1)
    {
        std::cerr << "Listen error.\n";
        return -1;
    }
    while (true)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        client_socket = accept(serverSocket, (struct sockaddr *)&client_addr, &client_addr_size);

        if (client_socket == -1)
        {
            std::cerr << "Accept error.\n";
            continue;
        }

        // 클라이언트와의 통신을 위한 코드를 여기에 작성하세요.
        char buffer[20000];
        int bytesReceived = recv(client_socket, buffer, sizeof(buffer) - 1, 0); // Leave space for null terminator
        if (bytesReceived > 0)
        {
            std::string request(buffer);
            buffer[bytesReceived] = '\0'; // Null-terminate the received data
                                          // Create a wide string converter
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

            // Convert the multibyte string to a wide string
            std::wstring ws = converter.from_bytes(buffer);
            // std::cerr << "Received message: " << wstringToUtf8(ws) << std::endl;
            //  wchar_t type3[1024];
            //  int charsConverted = MultiByteToWideChar(CP_UTF8, 0, buffer, bytesReceived, type3, sizeof(type3) / sizeof(wchar_t));
            wstring firstLine = ws.substr(0, ws.find(L"\r\n"));
            wstring method = firstLine.substr(0, firstLine.find(L" HTTP"));
            wstring content = L"";
            if (method == L"GET /")
            {
                // std::cerr << "get GET request" << std::endl;
                std::ifstream file("index2.html");
                if (!file.is_open())
                {
                    std::cerr << "Failed to open file" << std::endl;
                    continue; // Skip this iteration
                }
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + to_string(content.size()) + "\r\n\r\n" + content;
                send(client_socket, response.c_str(), response.size(), 0);
            }
            if (startsWith(method, L"GET /checkDuplicate") == true)
            {
                string response = "";
                response = handle_check_duplicate(request);
                send(client_socket, response.c_str(), response.size(), 0);
            }
            else if (method == L"POST /login")
            {
                auto contentPos = request.find("\r\n\r\n");
                if (contentPos != std::string::npos)
                {
                    std::string postData = request.substr(contentPos + 4);
                    handleLogin(postData, client_socket);
                }
            }
            else if (method == L"POST /signup")
            {
                auto headerEndPos = request.find("\r\n\r\n");
                if (headerEndPos != std::string::npos)
                {
                    std::string requestBody = request.substr(headerEndPos + 4);
                    handleSignUp(requestBody, client_socket);
                }
            }
            else if (method == L"POST /" || method == L"POST /8080")
            {
                std::cerr << "get POST request!!!" << std::endl;
                std::wstring prefix = L"\r\n\r\n";
                size_t startPos = ws.find(prefix);
                if (startPos != std::wstring::npos)
                {
                    wstring clientMessage = ws.substr(startPos + 4);
                    // std::cerr << "clientMessage: " << wstringToUtf8(clientMessage) << std::endl;
                    std::vector<std::wstring> clientMvec = splitWstring(clientMessage, L"\t");
                    std::cerr << "clientMvec[0]: " << wstringToUtf8(clientMvec[0]) << endl;
                    wstringstream wss(clientMvec[0]);
                    std::wstring userString = wss.str(); // 스트림 내용을 wstring으로 변환
                    wss >> user;
                    std::cerr << "user = " << wstringToUtf8(userString) << std::endl;
                    if (user == 0)
                    { // 처음 접속 시 보내는 내용(로그인 화면)
                        std::cerr << "user == 0" << std::endl;
                        if (clientMvec[1] == L"0")
                        {
                            cerr << "첫 화면" << endl;
                            uchar *sheet34198 = Sheet(34198);
                            wcout << L"Sheet(34198) =" << charToWstring(sheet34198) << endl;
                            content = intToWString(user) + L"\t" + intToWString(34198) + L"\t" + L"0" + L"\t" + charToWstring(sheet34198) + L"\t" + intToWString(CoRe.size()) + L"\t"; // 아이디 입력 화면을 보냄 user, vertex, ch, sheet 순서
                            delete[] sheet34198;
                            std::cerr << "content = " << wstringToUtf8(content) << std::endl;
                            sendMsg(client_socket, content);
                        }
                        else if (clientMvec[1] == L"34,198" || clientMvec[1] == L"34198")
                        { // 아이디를 입력한 상태(비밀번호 입력 화면을 보내야 함)
                            // vector<uchar> IDList = CoRe[34196][1].first;
                            cerr << "clientMvec[1] = " << wstringToUtf8(clientMvec[1]) << endl;
                            uint IDList = startAxis(34196, 1, 1);
                            bool check = false;
                            uint startCoo = charTouint(Core[34196].get() + 6 + 4 * 1);
                            uint sizeIDList = charTouint(Core[34196].get() + startCoo);
                            wcout << L"sizeIDList = " << intToWString(sizeIDList) << endl;
                            for (int i = 0; i < sizeIDList / 6; i++)
                            {
                                if (clientMvec[3] == charToWstring(Sheet(*reinterpret_cast<uint *>(Core[34196].get() + IDList + 6 * i)))) // ID가 존재하는 경우
                                {
                                    check = true;
                                    content = intToWString(i + 1) + L"\t" + intToWString(34199) + L"\t" + L"0" + L"\t" + charToWstring(Sheet(34199)) + L"\t" + intToWString(CoRe.size()) + L"\t"; // password 입력 화면을 보냄 user, vertex, sheet 순서
                                    sendMsg(client_socket, content);
                                    break;
                                }
                            }
                            if (!check)
                            {
                                content = intToWString(user) + L"\t" + intToWString(34198) + L"\t" + L"0" + L"\t" + L"없는 아이디입니다. 다시 아이디를 입력해 주세요."; // 다시 아이디 입력 화면을 보냄
                                sendMsg(client_socket, content);
                            }
                        }
                    }
                    else
                    { // 아이디까지 입력한 이후 상태
                        string inputText = wstringToUtf8(clientMvec[3]);
                        std::cerr << "inputText = " << inputText << std::endl;
                        if (clientMvec[1] == L"34,199" || clientMvec[1] == L"34199")
                        { // 비밀번호 입력한 상태
                            uint startCoo = startCh(34196, 1);
                            pair<uint, ushort> userID = charToPair(Core[34196].get() + startCoo + 4 + 6 * (user - 1));
                            uint startUserID = startCh(userID.first, userID.second);
                            pair<uint, ushort> Pass = charToPair(Core[userID.first].get() + startUserID + 4);
                            if (clientMvec[3] == charToWstring(Sheet(Pass.first))) // 비밀번호가 동일한 경우
                            {
                                uint startPass = startCh(Pass.first, Pass.second);
                                pair<uint, ushort> start = charToPair(Core[Pass.first].get() + startPass + 4);
                                cvertex[user] = start.first;
                                cCh[user] = start.second;
                                sendMsg(client_socket, makeContent(user, L""));
                            }
                            else
                            {
                                content = intToWString(user) + L"\t" + intToWString(34199) + L"\t" + L"0" + L"\t" + L"비밀번호가 틀립니다. 다시 입력해 주세요." + L"\t" + intToWString(CoRe.size()) + L"\t"; // password 입력 화면을 보냄 user, vertex, ch, sheet 순서
                                sendMsg(client_socket, content);
                            }
                        }
                        else
                        { // LogIn 이후 상태
                            int num = 0;
                            if (tryConvertToInt(inputText, num))
                            {
                                cerr << "num = " << wstringToUtf8(intToWString(num)) << endl;
                                if (num == 98)
                                {
                                    study(user);
                                    sendMsg(client_socket, makeContent(user, L"98"));
                                }
                                else if (num == 982) // if not working 98 function
                                {
                                    // study2(user);
                                    content = intToWString(user) + L"\t" + intToWString(cvertex[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + contentList(cvertex[user], cCh[user]) + L"\t" + intToWString(CoRe.size()) + L"\t98";
                                    sendMsg(client_socket, content);
                                }
                                else if (num == 99)
                                {
                                    copyvertex = make_pair(cvertex[user], cCh[user]);
                                    sendMsg(client_socket, makeContent(user, L""));
                                }
                                else if (num == 100)
                                {
                                    if (copyvertex.first != 0 && copyvertex.second != 0)
                                    {
                                        link(cvertex[user], cCh[user], copyvertex.first, copyvertex.second);
                                    }
                                    sendMsg(client_socket, makeContent(user, L""));
                                }
                                else if ((num > 0 && num <= sizeAxis(cvertex[user], cCh[user], 0) / 6) || (num < 0 && -num <= sizeAxis(cvertex[user], cCh[user], 1) / 6))
                                {
                                    move(num, user);
                                    sendMsg(client_socket, makeContent(user, L""));
                                }
                            }
                            else
                            {
                                // std::cout << "Invalid argument: the wstring cannot be converted to an integer." << std::endl;
                                uint startCh_6478 = charTouint(Core[6478].get() + 10); // ch = 1
                                uint startAxis0 = charTouint(Core[6478].get() + startCh_6478 + 2);
                                uint sizeAxis0 = sizeAxis(6478, 1, 0) / 6;
                                for (int i = 0; i < sizeAxis0; i++)
                                { // 바로가기 기능 구현
                                    uint nextvertex = charTouint(Core[6478].get() + startAxis0 + 6 * i);
                                    uchar *sheetvertex = Sheet(nextvertex);
                                    wstring ws = charToWstring(sheetvertex);
                                    if (clientMvec[3] == ws)
                                    {
                                        ushort nextCh = charToushort(Core[6478].get() + startAxis0 + 4 + 6 * i);
                                        cvertex[user] = nextvertex;
                                        cCh[user] = nextCh;
                                        delete[] sheetvertex;
                                        sendMsg(client_socket, makeContent(user, L"", L""));
                                        break;
                                    }
                                    else
                                    {
                                        delete[] sheetvertex;
                                    }
                                }
                                if (clientMvec[3][0] == L'/')
                                {
                                    string str = wstringToUtf8(clientMvec[3].substr(1));
                                    AddStringTovertex(str, cvertex[user], cCh[user], user);
                                    sendMsg(client_socket, makeContent(user, L"", L""));
                                }
                                else if (inputText == "시작" || inputText == "start")
                                {
                                    cvertex[user] = 0;
                                    cCh[user] = 1;
                                    sendMsg(client_socket, makeContent(user, L""));
                                }
                                else if (inputText == "수정")
                                {
                                    uchar *sheetvertex = Sheet(cvertex[user]);
                                    sendMsg(client_socket, makeContent(user, L"@" + charToWstring(sheetvertex), L""));
                                    delete[] sheetvertex;
                                }
                                else if (clientMvec[3][0] == L'@')
                                {
                                    string str = inputText.substr(1);
                                    wstring wstr = utf8ToWstring(str);
                                    uchar *wstr2 = wstringToUChar(wstr);
                                    change_data(cvertex[1], wstr2);
                                    delete[] wstr2;
                                    sendMsg(client_socket, makeContent(user, L""));
                                }
                                else if (clientMvec[3][0] == L'#') // Search
                                {
                                    string str = inputText.substr(1);
                                    wstring wstr = utf8ToWstring(str);
                                    uchar *wstr2 = wstringToUChar(wstr);
                                    uint dataSz = charTouint(wstr2);
                                    uint vertex = firstToken(wstr2, dataSz);
                                    cvertex[user] = vertex;
                                    cCh[user] = 1;
                                    sendMsg(client_socket, makeContent(user, L""));
                                }
                                else if (inputText == "save" || inputText == "저장")
                                {
                                    std::cerr << "start save()" << std::endl;
                                    save("");
                                    sendMsg(client_socket, makeContent(user, L"", L"save complete!"));
                                }
                                else if (inputText == "backUp")
                                {
                                    save("backup/");
                                    sendMsg(client_socket, makeContent(user, L"", L"backup complete!"));
                                }
                                else if (inputText == "del98")
                                { // 삭제하고 + 98
                                    deletevertex(cvertex[user]);
                                    study(1);
                                    sendMsg(client_socket, makeContent(user, L"", L"del98 complete!"));
                                }
                                else if (inputText == "ch+")
                                { // channel plus
                                    ushort nc = numCh(cvertex[user]);
                                    if (cCh[user] + 1 < nc)
                                    {
                                        cCh[user] += 1;
                                    }
                                    else
                                    {
                                        cCh[user] = 0;
                                    }
                                    sendMsg(client_socket, makeContent(user, L"", L""));
                                }
                                else if (inputText == "ch-")
                                { // 삭제하고 + 98
                                    ushort nc = numCh(cvertex[user]);
                                    if (cCh[user] > 0)
                                    {
                                        cCh[user] -= 1;
                                    }
                                    else
                                    {
                                        cCh[user] = nc - 1;
                                    }
                                    sendMsg(client_socket, makeContent(user, L"", L""));
                                }
                                else if (inputText == "html")
                                { // edit index2.html file
                                    std::ifstream file("index2.html");
                                    if (!file.is_open())
                                    {
                                        std::cerr << "Failed to open file" << std::endl;
                                        continue; // Skip this iteration
                                    }
                                    std::string content_html((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                                    content = intToWString(user) + L"\t" + intToWString(cvertex[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + utf8ToWstring(content_html) + L"\t" + intToWString(CoRe.size()) + L"\thtml";
                                    sendMsg(client_socket, content);
                                }
                                else if (inputText == "editHtml")
                                {
                                    std::ofstream file("index2.html");
                                    if (file.is_open())
                                    {
                                        // 요청 본문에서 "content" 필드의 값을 파일에 쓴다고 가정
                                        file << wstringToUtf8(clientMvec[4]);
                                        file.close();
                                        std::cout << "File updated successfully.\n";
                                    }
                                    else
                                    {
                                        std::cout << "Error opening file.\n";
                                    }
                                }
                                else if (inputText == "cpp")
                                { // edit index2.html file
                                    std::ifstream file("new20.cpp");
                                    if (!file.is_open())
                                    {
                                        std::cerr << "Failed to open file" << std::endl;
                                        continue; // Skip this iteration
                                    }
                                    std::string content_html((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                                    content = intToWString(user) + L"\t" + intToWString(cvertex[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + utf8ToWstring(content_html) + L"\t" + intToWString(CoRe.size()) + L"\tcpp";
                                    sendMsg(client_socket, content);
                                }
                                else if (inputText == "editcpp")
                                {
                                    std::ofstream file("new20.cpp");
                                    if (file.is_open())
                                    {
                                        // 요청 본문에서 "content" 필드의 값을 파일에 쓴다고 가정
                                        file << wstringToUtf8(clientMvec[4]);
                                        file.close();
                                        std::cout << "File updated successfully.\n";
                                    }
                                    else
                                    {
                                        std::cout << "Error opening file.\n";
                                    }
                                }
                                else if (inputText.size() >= 4 && inputText.substr(0, 4) == "103,")
                                { // 연결 해제
                                    vector<string> spl = splitStringASCII(inputText, ',');
                                    if (spl.size() == 2)
                                    {
                                        int tt = stringToUint(spl[1]) - 1;
                                        cut(cvertex[1], cCh[1], tt);
                                        content = intToWString(user) + L"\t" + intToWString(cvertex[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + contentList(cvertex[user], cCh[user]) + L"\t" + intToWString(CoRe.size()) + L"\t";
                                        sendMsg(client_socket, content);
                                    }
                                    else
                                    {
                                        Log(L"올바른 입력 형식이 아닙니다. ");
                                    }
                                    // display(cvertex[1], cCh[1]);
                                    inputText.clear();
                                }
                                else if (inputText.size() >= 4 && inputText.substr(0, 4) == "104,")
                                { // 자식 vertex로 이동
                                    cerr << "call 104, function" << endl;
                                    vector<string> spl = splitStringASCII(inputText, ',');
                                    if (spl.size() == 3)
                                    {
                                        uint startCoo = startCh(cvertex[1], cCh[1]);
                                        uint res2 = charTouint(Core[cvertex[1]].get() + startCoo + 4 + 6 * (stringToUint(spl[1]) - 1));
                                        int tt = 6 * (stringToUint(spl[2]) - 1);
                                        CoMove(cvertex[1], cCh[1], res2, charTouint(Core[cvertex[1]].get() + startCoo + 4 + tt), charToushort(Core[cvertex[1]].get() + startCoo + 8 + tt));
                                        content = intToWString(user) + L"\t" + intToWString(cvertex[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + contentList(cvertex[user], cCh[user]) + L"\t" + intToWString(CoRe.size()) + L"\t";
                                        sendMsg(client_socket, content);
                                    }
                                    else
                                    {
                                        Log(L"올바른 입력 형식이 아닙니다. ");
                                    }
                                    // clearInputText();
                                }
                                else if (inputText.size() >= 4 && inputText.substr(0, 4) == "del,")
                                {
                                    vector<string> spl = splitStringASCII(inputText, ',');
                                    if (spl.size() == 2)
                                    {
                                        uint startCoo = startCh(cvertex[1], cCh[1]);
                                        int tt = stringToUint(spl[1]) - 1;
                                        uint deln = charTouint(Core[cvertex[1]].get() + startCoo + 4 + 6 * tt);
                                        cut(cvertex[1], cCh[1], tt);
                                        deletevertex(deln);
                                        Log(intToWString(deln) + L" 삭제!");
                                    }
                                    else
                                    {
                                        Log(L"올바른 입력 형식이 아닙니다. ");
                                    }
                                    // info();
                                    sendMsg(client_socket, makeContent(user, L"", L""));
                                    // clearInputText();
                                }
                                else if (inputText.size() >= 5 && inputText.substr(0, 5) == "move,")
                                {
                                    cerr << "call move function" << endl;
                                    vector<string> spl = splitStringASCII(inputText, ',');
                                    if (spl.size() == 3)
                                    {
                                        uint vertex = stringToUint(spl[1]);
                                        ushort ch = stringToUint(spl[2]);
                                        cvertex[1] = vertex;
                                        cCh[1] = ch;
                                        content = intToWString(user) + L"\t" + intToWString(cvertex[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + contentList(cvertex[user], cCh[user]) + L"\t" + intToWString(CoRe.size()) + L"\t";
                                        sendMsg(client_socket, content);
                                    }
                                    else
                                    {
                                        Log(L"올바른 입력 형식이 아닙니다. ");
                                    }
                                    // clearInputText();
                                }
                                else if (inputText.size() >= 4 && inputText.substr(0, 4) == "Page")
                                {
                                    vector<string> spl = splitStringASCII(inputText, 'e');
                                    if (spl.size() == 2)
                                    {
                                        uint page = stringToUint(spl[1]);
                                        content = intToWString(user) + L"\t" + intToWString(cvertex[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + contentList(cvertex[user], cCh[user], page) + L"\t" + intToWString(CoRe.size()) + L"\t";
                                        sendMsg(client_socket, content);
                                    }
                                    else
                                    {
                                        sendMsg(client_socket, makeContent(user, L"", L"올바른 입력 형식이 아닙니다."));
                                    }
                                }
                                else if (inputText.size() >= 4 && inputText.substr(0, 4) == "map/")
                                {
                                    vector<string> spl = splitStringASCII(inputText, '/');
                                    if (spl.size() == 3)
                                    {
                                        AddStringTovertex2(spl[1], spl[2], cvertex[user], cCh[user], user);
                                        sendMsg(client_socket, makeContent(user, L"", L""));
                                        sendMsg(client_socket, makeContent(user, L"", L"올바른 입력 형식이 아닙니다."));
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    std::cerr << "POST request without content" << std::endl;
                }
            }
        }

        close(client_socket);
    }

    close(serverSocket);
}
int Network2()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    const char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        std::cerr << "socket failed" << endl;
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 443
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8081);

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept an incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Read data sent by the client
    read(new_socket, buffer, 1024);
    std::cout << "Message from client: " << buffer << std::endl;

    // Send a response back to client
    send(new_socket, hello, strlen(hello), 0);
    std::cout << "Hello message sent\n";

    // Close the connection
    close(new_socket);
    close(server_fd);

    return 0;
}
// 해시 함수 정의
struct TupleHash
{
    size_t operator()(const tuple<int, ushort> &key) const
    {
        return hash<int>()(get<0>(key)) ^ hash<ushort>()(get<1>(key));
    }
};

// 비교 연산자 정의
struct TupleEqual
{
    bool operator()(const tuple<int, ushort> &lhs, const tuple<int, ushort> &rhs) const
    {
        return get<0>(lhs) == get<0>(rhs) && get<1>(lhs) == get<1>(rhs);
    }
};
void removeDuplicates(vector<tuple<int, ushort, long long, long long>> &vec)
{
    unordered_set<tuple<int, ushort>, TupleHash, TupleEqual> seen;
    auto it = vec.begin();

    while (it != vec.end())
    {
        auto key = make_tuple(get<0>(*it), get<1>(*it));
        if (seen.find(key) != seen.end())
        {
            it = vec.erase(it);
        }
        else
        {
            seen.insert(key);
            ++it;
        }
    }
}
void copyArr(uchar *fromArr, uchar *toArr, uint fromIndex, uint toIndex, uint size)
{
    for (int i = 0; i < size; i++)
    {
        toArr[toIndex + i] = fromArr[fromIndex + i];
    }
}

int main(int argc, char const *argv[])
{
    auto start = std::chrono::high_resolution_clock::now();
    std::locale::global(std::locale("en_US.UTF-8"));
    // std::wcout.imbue(std::locale());
    //  RAM에 Brain UpRoad
    std::ifstream in("Brain5.bin", std::ios::binary);
    // int ii = 0;
    uchar *size2 = new uchar[4];
    in.read(reinterpret_cast<char *>(size2), sizeof(uint));
    uint size3 = charTouint(size2);
    delete[] size2;
    wstring ww = intToWString(size3);
    // std::cerr << "vertex: " << wstringToUtf8(ww) << std::endl;
    // Log(L"vertex" + ww);
    for (int i = 0; i < size3; i++)
    {
        uchar *size1 = new uchar[4];
        in.read(reinterpret_cast<char *>(size1), sizeof(uint));
        uint size = charTouint(size1);
        uchar *outer = new uchar[size + 4];
        outer[0] = size1[0];
        outer[1] = size1[1];
        outer[2] = size1[2];
        outer[3] = size1[3];
        delete[] size1;
        in.read(reinterpret_cast<char *>(&outer[4]), size);
        CoRe.push_back(outer);
    }
    in.close();
    for (int i = 0; i < CoRe.size(); i++)
    {
        ushort numch = charToushort(CoRe[i] + 4);
        auto outer2 = std::make_unique<uchar[]>(6 + 4 * (numch + 1));
        uint totalBytes = 0;
        changeInt(outer2.get(), 0, 2 + 4 * (numch + 1));//6 + 4 * (numch + 1)에서 4를 빼야 함(size에 처음 4바이트는 미포함되므로 ) 
        changeShort(outer2.get(), 4, numch);
        uint size = charTouint(CoRe[i]);
        if (i == 41200)
        {
            std::cerr << "i = 41200, size = " << size << std::endl;
        }
        for (int j = 0; j < numch; j++)
        {
            // if (i == 41200)
            // {
            //     std::cerr << "i = 41200, j = " << j << ", totalbytes = " << totalBytes << std::endl;
            // }
            if (6 + (4 * j) >= size + 4)
            {
                std::cerr << "Index out of range for outer array." << std::endl;
                return 1;
            }
            ushort nAxis = 2;
            uint startAxis1 = charTouint(CoRe[i] + 6 + (4 * j));
            if (startAxis1 >= size + 4)
            {
                std::cerr << "Invalid startAxis1 index." << std::endl;
                return 1;
            }
            uint sizeAxis1 = charTouint(CoRe[i] + startAxis1);
            uint sizeAxis2 = 0, startAxis2 = 0, sizeAxis3 = 0, endAxis2 = 0;

            if (sizeAxis1 > size)
            {
                std::cerr << "i == " << i << ", startAxis1 = " << startAxis1 << ", sizeAxis1 = " << sizeAxis1 << ", size = " << size << std::endl;
                nAxis = 0;
            }
            else
            {

                startAxis2 = startAxis1 + 4 + sizeAxis1;

                if (startAxis2 > size + 4)
                {
                    // std::cerr << "Invalid startAxis2 index. i =" << i << std::endl;
                    nAxis = 1;
                }
                else
                {
                    sizeAxis2 = charTouint(CoRe[i] + startAxis2);
                    if (sizeAxis2 < 4)
                        sizeAxis2 = 0;
                    if (sizeAxis1 == 0 && sizeAxis2 > 0)
                    {
                        nAxis = 2;
                    }

                    if (j < numch - 1 && charTouint(CoRe[i] + 6 + 4 * (j + 1)) <= sizeAxis2)
                    {
                        nAxis = 1;
                    }
                    else if (j == numch - 1 && sizeAxis2 > size)
                    {
                        nAxis = 1;
                    }
                    else if (sizeAxis2 == 0)
                    {
                        nAxis = 1;
                        // if (sizeAxis1 == 0 || i == 15422 || i == 15547 || i == 30089)
                        if (sizeAxis1 == 0 || j == numch - 1)
                        {
                            nAxis = 3;
                            endAxis2 = startAxis2 + 4 + sizeAxis2;
                            // sizeAxis3 = charTouint(CoRe[i] + endAxis2);
                            sizeAxis3 = size - endAxis2;
                        }
                        else if (charTouint(CoRe[i] + 6 + 4 * (j + 1)) > startAxis2 + 4)
                        {
                            nAxis = 3;
                            endAxis2 = startAxis2 + 4 + sizeAxis2;
                            // sizeAxis3 = charTouint(CoRe[i] + endAxis2);
                            sizeAxis3 = charTouint(CoRe[i] + 6 + 4 * (j + 1)) - endAxis2 - 4;
                        }
                    }
                    else
                    {
                        endAxis2 = startAxis2 + 4 + sizeAxis2;
                        if (endAxis2 > size + 4)
                        {
                            // std::cerr << "Invalid endAxis2 index. i = " << i << ", j = " << j << ", startAxis1 = " << startAxis1 << ", sizeAxis1 = " << sizeAxis1 << ", startAxis2 = " << startAxis2 << ", sizeAxis2 = " << sizeAxis2 << ", endAxis2 = " << endAxis2 << ", size = " << size << std::endl;
                            nAxis = 1;
                        }
                        else if (j < numch - 1 && nAxis == 2)
                        {
                            uint startNextCh = charTouint(CoRe[i] + 6 + (4 * (j + 1)));
                            if (startNextCh > endAxis2)
                            {
                                nAxis = 3;
                                if (endAxis2 > size + 4)
                                {
                                    std::cerr << "Invalid endAxis2 index for sizeAxis3." << std::endl;
                                    return 1;
                                }
                                // sizeAxis3 = charTouint(CoRe[i] + endAxis2);
                                sizeAxis3 = startNextCh - endAxis2 - 4;
                                if (i == 41200)
                                {
                                    std::cerr << "find!! " << ", startNextCh = " << startNextCh << ", endAxis2 = " << endAxis2 << ", sizeAxis3 = " << charTouint(CoRe[i] + endAxis2) << std::endl;
                                }
                            }
                        }
                        else if (4 + size > endAxis2)
                        {
                            uint startNextCh = charTouint(CoRe[i] + 6 + (4 * (j + 1)));
                            if (j == numch - 1)
                            {
                                sizeAxis3 = size - endAxis2;
                            }
                            else if (startNextCh < endAxis2)
                            {
                                sizeAxis3 = startNextCh - endAxis2 - 4;
                            }
                            nAxis = 3;
                            if (endAxis2 > size + 4)
                            {
                                std::cerr << "Invalid endAxis2 index for sizeAxis3. i = " << i << ", j = " << j << ", startAxis1 = " << startAxis1 << ", sizeAxis1 = " << sizeAxis1 << ", startAxis2 = " << startAxis2 << ", sizeAxis2 = " << sizeAxis2 << ", endAxis2 = " << endAxis2 << ", size = " << size << std::endl;
                                nAxis = 2;
                            }
                            // sizeAxis3 = charTouint(CoRe[i] + endAxis2);
                        }
                    }
                }
            }
            if (endAxis2 == size + 4 && nAxis == 3)
            {
                nAxis = 2;
            }

            if (sizeAxis3 > size && nAxis == 3)
            {
                nAxis = 2;
                size = endAxis2 - 4;
                std::cerr << "i = " << i << ", j = " << j << ", numch = " << numch << ", numAxis = " << nAxis << ", startAxis1 = " << startAxis1 << ", sizeAxis1 = " << sizeAxis1 << ", startAxis2 = " << startAxis2 << ", sizeAxis2 = " << sizeAxis2 << ", endAxis2 = " << endAxis2 << ", sizeAxis3 = " << sizeAxis3 << ", size = " << size << std::endl;
                // sizeAxis3 = 22;
            }
            std::unique_ptr<uchar[]> byteCh;
            uint bytes = 0;
            uint aa = 6 + 4 * (numch + 1) + totalBytes;
            if (nAxis == 0)
            {
                bytes = 6;
                byteCh = std::make_unique<uchar[]>(bytes);
                changeShort(byteCh.get(), 0, nAxis);
                changeInt(byteCh.get(), 2, aa + bytes);
            }
            else if (nAxis == 1)
            {
                bytes = 10 + sizeAxis1;
                byteCh = std::make_unique<uchar[]>(bytes);
                changeShort(byteCh.get(), 0, nAxis);
                changeInt(byteCh.get(), 2, aa + 10);
                changeInt(byteCh.get(), 6, aa + bytes);
                copyArr(CoRe[i], byteCh.get(), startAxis1 + 4, 10, sizeAxis1);
            }
            else if (nAxis == 2)
            {
                if (startAxis1 == startAxis2)
                {
                    std::cerr << "i == " << i << ", startAxis1 = " << startAxis1 << ", startAxis2 = " << startAxis2 << std::endl;
                }
                bytes = 14 + sizeAxis1 + sizeAxis2;
                byteCh = std::make_unique<uchar[]>(bytes);
                changeShort(byteCh.get(), 0, nAxis);
                changeInt(byteCh.get(), 2, aa + 14);
                changeInt(byteCh.get(), 6, aa + 14 + sizeAxis1);
                changeInt(byteCh.get(), 10, aa + bytes);
                copyArr(CoRe[i], byteCh.get(), startAxis1 + 4, 14, sizeAxis1);
                copyArr(CoRe[i], byteCh.get(), startAxis2 + 4, 14 + sizeAxis1, sizeAxis2);
            }
            else
            {
                // if (i == 26563)
                // {
                //     std::cerr << "startAxis1 = " << startAxis1 << ", i == 26563, sizeAxis1 = " << sizeAxis1 << ", size = " << size << ", startAxis2 = " << startAxis2 << ", sizeAxis2 = " << sizeAxis2 << ", endAxis2 = " << endAxis2 << ", sizeAxis3 = " << sizeAxis3 << std::endl;
                // }
                bytes = 18 + sizeAxis1 + sizeAxis2 + sizeAxis3;
                byteCh = std::make_unique<uchar[]>(bytes);
                changeShort(byteCh.get(), 0, nAxis);
                changeInt(byteCh.get(), 2, aa + 18);
                changeInt(byteCh.get(), 6, aa + 18 + sizeAxis1);
                changeInt(byteCh.get(), 10, aa + 18 + sizeAxis1 + sizeAxis2);
                changeInt(byteCh.get(), 14, aa + bytes);
                copyArr(CoRe[i], byteCh.get(), startAxis1 + 4, 18, sizeAxis1);
                copyArr(CoRe[i], byteCh.get(), startAxis2 + 4, 18 + sizeAxis1, sizeAxis2);
                copyArr(CoRe[i], byteCh.get(), endAxis2 + 4, 18 + sizeAxis1 + sizeAxis2, sizeAxis3);
            }
            changeInt(outer2.get(), 6 + 4 * j, 6 + 4 * (numch + 1) + totalBytes);
            insertArr2(outer2, 6 + 4 * (numch + 1) + totalBytes, byteCh.get(), bytes);
            totalBytes += bytes;
        }
        changeInt(outer2.get(), 6 + 4 * (numch), 6 + 4 * (numch + 1) + totalBytes);
        Core.push_back(std::move(outer2));
        // uint size = charTouint(CoRe[i]);
        // uint size2 = charTouint(Core[i].get());
        // if (size != size2){
        //     std::cerr << "!= size : i = " << i << ", sizeCoRe = " << size << ", sizeCore = " << size2 << std::endl;
        // }
    }
    uint startCh = charTouint(Core[0].get() + 6 + 4 * 1);
    uint startAxis0 = charTouint(Core[0].get() + startCh + 2);
    changeShort(Core[0].get(), startAxis0 + 6 * 10 + 4, 0);
    chInfo3(41200, 653);

    // for (int i = 0; i < CoRe.size(); i++)
    // {
    //     ushort numch = charToushort(CoRe[i] + 4);
    //     uchar *outer2 = new uchar[6 + 4 * (numch + 1)];
    //     uint totalBytes = 0;
    //     changeInt(outer2, 0, 6 + 4 * (numch + 1));
    //     for (int j = 0; j < numch; j++)
    //     {
    //         uint size = charTouint(CoRe[i]);
    //         // Index 범위를 체크하는 코드 추가
    //         if (6 + (4 * j) >= size + 4)
    //         {
    //             std::cerr << "Index out of range for outer array." << std::endl;
    //             delete[] outer2;
    //             return 1;
    //         }
    //         ushort nAxis = 2;
    //         uint startAxis1 = charTouint(CoRe[i] + 6 + (4 * j));
    //         if (startAxis1 >= size + 4)
    //         {
    //             std::cerr << "Invalid startAxis1 index." << std::endl;
    //             // delete[] outer;
    //             delete[] outer2;
    //             return 1;
    //         }
    //         uint sizeAxis1 = charTouint(CoRe[i] + startAxis1);
    //         uint sizeAxis2, startAxis2, sizeAxis3, endAxis2;
    //         if (sizeAxis1 == 0)
    //         {
    //             nAxis = 0;
    //         }
    //         else
    //         {
    //             startAxis2 = startAxis1 + 4 + sizeAxis1;
    //             sizeAxis2 = charTouint(CoRe[i] + startAxis2);
    //             if (sizeAxis2 == 0)
    //             {
    //                 nAxis = 1;
    //             }
    //             endAxis2 = startAxis2 + 4 + sizeAxis2;
    //             sizeAxis3 = 0;
    //             if (j < numch - 1 && nAxis == 2)
    //             {
    //                 if (charTouint(CoRe[i] + 6 + (4 * (j + 1))) > endAxis2)
    //                 {
    //                     nAxis = 3;
    //                     sizeAxis3 = charTouint(CoRe[i] + endAxis2);
    //                 }
    //             }
    //             else if (4 + size > endAxis2)
    //             {
    //                 nAxis = 3;
    //                 sizeAxis3 = charTouint(CoRe[i] + endAxis2);
    //             }
    //         }

    //         uchar *byteCh;
    //         uint bytes = 0;
    //         if (nAxis == 0)
    //         {
    //             byteCh = ushortToBytes(nAxis);
    //             bytes = 2;
    //         }
    //         else if (nAxis == 1)
    //         {
    //             bytes = 10 + sizeAxis1;
    //             byteCh = new uchar[bytes];
    //             changeShort(byteCh, 0, nAxis);
    //             changeInt(byteCh, 2, startAxis1 + 10);
    //             changeInt(byteCh, 6, startAxis1 + bytes);
    //             copyArr(CoRe[i], byteCh, startAxis1 + 4, 10, sizeAxis1);
    //         }
    //         else if (nAxis == 2)
    //         {
    //             bytes = 14 + sizeAxis1 + sizeAxis2;
    //             byteCh = new uchar[bytes];
    //             changeShort(byteCh, 0, nAxis);
    //             changeInt(byteCh, 2, startAxis1 + 14);
    //             changeInt(byteCh, 6, startAxis1 + 14 + sizeAxis1);
    //             changeInt(byteCh, 10, startAxis1 + bytes);
    //             copyArr(CoRe[i], byteCh, startAxis1 + 4, 14, sizeAxis1);
    //             copyArr(CoRe[i], byteCh, startAxis2 + 4, 14 + sizeAxis1, sizeAxis2);
    //         }
    //         else
    //         {
    //             bytes = 18 + sizeAxis1 + sizeAxis2 + sizeAxis3;
    //             byteCh = new uchar[bytes];
    //             changeShort(byteCh, 0, nAxis);
    //             changeInt(byteCh, 2, startAxis1 + 18);
    //             changeInt(byteCh, 6, startAxis1 + 18 + sizeAxis1);
    //             changeInt(byteCh, 10, startAxis1 + 18 + sizeAxis1 + sizeAxis2);
    //             changeInt(byteCh, 14, startAxis1 + bytes);
    //             copyArr(CoRe[i], byteCh, startAxis1 + 4, 18, sizeAxis1);
    //             copyArr(CoRe[i], byteCh, startAxis2 + 4, 18 + sizeAxis1, sizeAxis2);
    //             copyArr(CoRe[i], byteCh, endAxis2 + 4, 18 + sizeAxis1 + sizeAxis2, sizeAxis3);
    //         }
    //         changeInt(outer2, 6 + 4 * j, 6 + 4 * (numch + 1) + totalBytes);
    //         insertArr(outer2, 6 + 4 * numch, byteCh, bytes);
    //         delete[] byteCh;
    //         totalBytes += bytes;
    //     }
    //     changeInt(outer2, 6 + 4 * (numch + 1), 6 + 4 * (numch + 1) + totalBytes);
    //     Core.push_back(outer2);
    // }

    std::cout << "numOrder: " << numOrder(1) << "" << std::endl;
    cvertex[1] = 0;
    cCh[1] = 1;

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    Log(L"loading time: " + intToWString(duration.count()) + L"ms");
    getMemoryUsage(vmSize, vmRSS);

    thread t1(Network);
    thread t2(Network2);
    t1.join();
    t2.join();
    return 0;
}