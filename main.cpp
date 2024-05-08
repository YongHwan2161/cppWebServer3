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
// #include <nlohmann/json.hpp>

#define PORT 8080
using namespace std;
using uint = unsigned int;
using ushort = unsigned short;
using uchar = unsigned char;
// using json = nlohmann::json;

std::vector<uchar *> CoRe;
std::vector<vector<tuple<int, ushort, long long, long long>>> order;
uint user = 1;
uint cNode[10];
ushort cCh[10];
uint pageNum[10];
pair<uint, ushort> copyNode;
int line_spacing = 5;
int text_size = 15;
vector<wstring> LogStr{};
unsigned int gar = 39701;
uint ttt = 166972;
vector<uchar> zero8 = {0, 0, 0, 0, 0, 0, 0, 0};
uchar initValues[18] = {14, 0, 0, 0, 1, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // node 추가시 초기값 설정 charToushort(uchar *arr)
ushort charToushort(uchar *arr)
{
    return (static_cast<ushort>(arr[0])) |
           (static_cast<ushort>(arr[1]) << 8);
}
ushort numCh(uint node) // node의 채널 수를 반환(채널 수는 2바이트로 저장됨)
{
    return (static_cast<ushort>(CoRe[node][4])) |
           (static_cast<ushort>(CoRe[node][5]) << 8);
}
uint charTouint(uchar *arr)
{
    // std::cerr << "call charTouint" << std::endl;
    return (static_cast<unsigned int>(arr[0])) |
           (static_cast<unsigned int>(arr[1]) << 8) |
           (static_cast<unsigned int>(arr[2]) << 16) |
           (static_cast<unsigned int>(arr[3]) << 24);
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
uchar *ushortToBytes(ushort val1)
{
    unsigned char *bytes = new unsigned char[2]; // 2 바이트 배열 동적 할당
    bytes[0] = static_cast<unsigned char>(val1 & 0xFF);
    bytes[1] = static_cast<unsigned char>((val1 >> 8) & 0xFF);
    return bytes; // 배열의 포인터 반환
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
uchar *pairToBytes2(uint node1, ushort ch1, uint node2, ushort ch2)
{
    unsigned char *bytes = new unsigned char[12]; // 6 바이트 배열 동적 할당

    bytes[0] = static_cast<unsigned char>(node1 & 0xFF);
    bytes[1] = static_cast<unsigned char>((node1 >> 8) & 0xFF);
    bytes[2] = static_cast<unsigned char>((node1 >> 16) & 0xFF);
    bytes[3] = static_cast<unsigned char>((node1 >> 24) & 0xFF);
    bytes[4] = static_cast<unsigned char>(ch1 & 0xFF);
    bytes[5] = static_cast<unsigned char>((ch1 >> 8) & 0xFF);
    bytes[6] = static_cast<unsigned char>(node2 & 0xFF);
    bytes[7] = static_cast<unsigned char>((node2 >> 8) & 0xFF);
    bytes[8] = static_cast<unsigned char>((node2 >> 16) & 0xFF);
    bytes[9] = static_cast<unsigned char>((node2 >> 24) & 0xFF);
    bytes[10] = static_cast<unsigned char>(ch2 & 0xFF);
    bytes[11] = static_cast<unsigned char>((ch2 >> 8) & 0xFF);

    return bytes; // 배열의 포인터 반환
}
uchar *chVec(uint node, ushort ch)
{
    uint posi = charTouint(CoRe[node] + 6 + 4 * ch);
    uint posiEnd;

    if (ch < numCh(node) - 1)
    {
        posiEnd = charTouint(CoRe[node] + 6 + 4 * (ch + 1));
    }
    else
    {
        posiEnd = 4 + charTouint(CoRe[node]);
    }
    uchar *result = new uchar[posiEnd - posi];
    for (int i = 0; i < posiEnd - posi; i++)
    {
        result[i] = CoRe[node][i + posi];
    }
    return result;
}
uchar *axis1(uint node, ushort ch)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    return CoRe[node] + startCoo + 4; // 포인터를 그대로 전달함.
}
uint axis2(uint node, ushort ch)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    return startRev + 4;
}
uchar *axis3(uint node, ushort ch)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    return CoRe[node] + startRev + 4 + sizeRev + 4;
}
uchar *axis(uint node, ushort ch, ushort axis)
{
    uint startAxis = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeAxis = 0;
    for (int i = 2; i <= axis; i++)
    {
        sizeAxis = charTouint(CoRe[node] + startAxis);
        startAxis += 4 + sizeAxis;
    }
    return CoRe[node] + startAxis + 4;
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
void changePair(uchar *arr, uint index, uint node, ushort ch)
{
    // node 값을 벡터에 할당
    arr[index] = static_cast<uchar>(node & 0xFF);
    arr[index + 1] = static_cast<uchar>((node >> 8) & 0xFF);
    arr[index + 2] = static_cast<uchar>((node >> 16) & 0xFF);
    arr[index + 3] = static_cast<uchar>((node >> 24) & 0xFF);

    // ch 값을 벡터에 할당
    arr[index + 4] = static_cast<uchar>(ch & 0xFF);
    arr[index + 5] = static_cast<uchar>((ch >> 8) & 0xFF);
}
void eraseCoo(uint node, ushort ch, uint index)
{
    // 6바이트 제거
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    vector<uchar> replace = vector<uchar>(CoRe[node] + startCoo + 4 + 6 * (index + 1), CoRe[node] + startRev + 4 + sizeRev);
    for (int i = startCoo + 4 + 6 * index; i < startRev - 2 + sizeRev; i++)
    {
        CoRe[node][i] = replace[i - startCoo - 4 - 6 * index]; // Rev 위치를 앞으로 6칸 이동
    }
    changeInt(CoRe[node], startCoo, sizeCoo - 6);
}
void eraseRev(uint node, ushort ch, uint index)
{
    // 6바이트 제거
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = charTouint(CoRe[node] + startCoo + 4 + sizeCoo);
    // CoRe[node].erase(CoRe[node].begin() + startRev + 4 + 6 * index, CoRe[node].begin() + startCoo + 4 + 6 * (index + 1));
    changePair(CoRe[node], startRev + 4 + 6 * index, 0, 0); // 제거할 때 메모리를 삭제시키지 않고 그냥 0으로 채움.(나중에 재활용하기 위함 + 프로그램 성능을 위해)
    uint sizeRev = charTouint(CoRe[node] + startRev);
    changeInt(CoRe[node], startRev, sizeRev - 6);
    // uint sizeCh = charTouint(vec, startCoo - 4);
    // changeInt(vec, startCoo - 4, sizeRev - 6);
}
void clearCoo(uint node, uint ch)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    changeInt(CoRe[node], startCoo, 0);
    vector<uchar> replace = vector<uchar>(CoRe[node] + startRev, CoRe[node] + startRev + 4 + sizeRev);
    for (int i = 4; i < 8 + sizeRev; i++)
    {
        CoRe[node][i] = replace[i - 4];
    }
    // vec.erase(vec.begin() + 4, vec.begin() + 4 + szC);
    // for (int i = 0; i < 4; i++)
    // {
    //     vec[i] = 0;
    // }
}
void clearRev(uint node, uint ch)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    changeInt(CoRe[node], startRev, 0);
    // uint szC = charTouint(vec, 0);
    // vec = vector<uchar>(vec.begin(), vec.begin() + 8 + szC);
    // for (int i = 0; i < 4; i++)
    // {
    //     vec[4 + szC + i] = 0;
    // }
}
void insertArr(uint node, uint index, uchar *add, uint addSize)
{
    uint sizeArr = 4 + charTouint(CoRe[node]);
    uchar *result = new uchar[sizeArr + addSize];
    changeInt(result, 0, sizeArr + addSize - 4);
    for (uint i = 4; i < index; ++i)
    {
        result[i] = CoRe[node][i];
    }
    for (uint i = 0; i < addSize; ++i)
    {
        result[index + i] = add[i];
    }
    for (uint i = index; i < sizeArr; ++i)
    {
        result[i + addSize] = CoRe[node][i];
    }
    delete[] CoRe[node]; // 메모리 해제 먼저 해야 함!
    CoRe[node] = result;
}
// void insertArr(uint32_t node, uint32_t index, uint8_t* add, uint32_t addSize) {
//     // CoRe[node]의 현재 크기를 구합니다. 이 예에서는 별도의 메커니즘으로 크기를 추적하고 있다고 가정합니다.
//     uint32_t originalSize = charTouint(CoRe[node]);  // 여기서 charTouint 함수는 첫 4바이트에서 크기 정보를 읽는 함수라고 가정합니다.

//     // 새로운 배열의 총 크기를 계산합니다.
//     uint32_t newSize = originalSize + addSize;

//     // 새로운 배열 할당
//     uint8_t* result = new uint8_t[newSize];

//     // 새로운 크기 정보를 결과 배열에 저장
//     changeInt(result, 0, newSize - 4);

//     // 기존 데이터의 처음부터 삽입 위치까지를 복사
//     std::copy(CoRe[node] + 4, CoRe[node] + index + 4, result + 4);

//     // 추가할 데이터를 삽입 위치에 복사
//     std::copy(add, add + addSize, result + index + 4);

//     // 원래 데이터에서 삽입 위치 이후의 데이터를 새 위치에 복사
//     std::copy(CoRe[node] + index + 4, CoRe[node] + originalSize, result + index + addSize + 4);

//     // 기존 데이터 메모리 해제
//     delete[] CoRe[node];

//     // CoRe[node]를 새 배열로 업데이트
//     CoRe[node] = result;
// }
void pushCoo(uint node, ushort ch, uchar *add)
{
    ushort numCh = charToushort(CoRe[node] + 4);
    uint startCoo = charTouint(CoRe[node] + (6 + 4 * ch));
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    uint startCoo2 = 0;
    if (ch < numCh - 1)
    {
        startCoo2 = charTouint(CoRe[node] + 6 + 4 * (ch + 1));
    }
    else
    {
        // startCoo2 = CoRe[node].size();
        startCoo2 = 4 + charTouint(CoRe[node]);
    }
    if (sizeCoo + sizeRev + 14 > startCoo2 - startCoo)
    {
        for (ushort i = ch + 1; i < numCh; i++)
        {
            uint temp = charTouint(CoRe[node] + 6 + 4 * i);
            changeInt(CoRe[node], 6 + 4 * i, temp + 6);
        }
        insertArr(node, startRev, add, 6);
        // CoRe[node].insert(CoRe[node].begin() + startCoo + 4 + sizeCoo, add.begin(), add.end());
        changeInt(CoRe[node], startCoo, sizeCoo + 6);
    }
    else
    {
        changeInt(CoRe[node], startCoo, sizeCoo + 6);
        vector<uchar> RevVec = vector<uchar>(CoRe[node] + startRev, CoRe[node] + startRev + 4 + sizeRev);
        for (int i = startRev + 6; i < startRev + 10 + sizeRev; i++)
        {
            CoRe[node][i] = RevVec[i - startRev - 6];
        }
        for (int i = startRev; i < startRev + 6; i++)
        {
            CoRe[node][i] = add[i - startRev];
        }
    }
    // delete[] add;
}
void pushRev(uint node, ushort ch, uchar *add)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    uint endPosi = startRev + 4 + sizeRev;
    ushort numCh = charToushort(CoRe[node] + 4);
    uint startCoo2 = 0;
    if (ch < numCh - 1)
    {
        startCoo2 = charTouint(CoRe[node] + 6 + 4 * (ch + 1));
    }
    else
    {
        // startCoo2 = CoRe[node].size();
        startCoo2 = 4 + charTouint(CoRe[node]);
    }
    if (sizeCoo + sizeRev + 14 > startCoo2 - startCoo)
    {
        // CoRe[node].insert(CoRe[node].begin() + endPosi, add.begin(), add.end());
        insertArr(node, endPosi, add, 6);
        for (ushort i = ch + 1; i < numCh; i++)
        {
            uint temp = charTouint(CoRe[node] + 6 + 4 * i);
            changeInt(CoRe[node], 6 + 4 * i, temp + 6);
        }
    }
    else
    {
        for (int i = endPosi; i < endPosi + 6; i++)
        {
            CoRe[node][i] = add[i - endPosi];
        }
    }
    delete[] add;
    changeInt(CoRe[node], startRev, sizeRev + 6);
}
void pushRev2(uint node, ushort ch, uchar *add) // Brain 함수에서만 사용하는 특수용도(일반적으로 사용하면 안됨)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    uint endPosi = startRev + 4 + sizeRev;
    ushort numCh = charToushort(CoRe[node] + 4);
    // CoRe[node].insert(CoRe[node].begin() + endPosi, add.begin(), add.end());
    insertArr(node, endPosi, add, 12);
    for (ushort i = ch + 1; i < numCh; i++)
    {
        changeInt(CoRe[node], 6 + 4 * i, charTouint(CoRe[node] + 6 + 4 * i) + 12);
    }
    changeInt(CoRe[node], startRev, sizeRev + 12);
}
void pushAxis3(uint node, ushort ch, uchar *bitmap)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    uint startAxis3 = startRev + 4 + sizeRev;
    uint sizeAxis3 = charTouint(CoRe[node] + startAxis3);
    uint endPosi = startAxis3 + 4 + sizeAxis3;
    ushort numCh = charToushort(CoRe[node] + 4);
    // uint startCoo2 = 0;
    // if (ch < numCh - 1)
    // {
    //     startCoo2 = charTouint(CoRe[node] + 6 + 4 * (ch + 1));
    // }
    // else
    // {
    //     startCoo2 = 4 + charTouint(CoRe[node]);
    // }
    // uchar *add = uintToBytes(8);
    // insertArr(node, endPosi, add, 4);
    uint width = charTouint(CoRe[node] + startAxis3 + 4);
    uint height = charTouint(CoRe[node] + startAxis3 + 8);
    changeInt(CoRe[node], startAxis3, 16 + width * height);
    // insertArr(node, endPosi + 8, bitmap, 4);
    if (width != 0 && height != 0)
    {
        insertArr(node, endPosi + 4, bitmap, width * height);
    }
    // insertArr(node, endPosi + 12, bitmap, width * height);
    for (ushort i = ch + 1; i < numCh; i++)
    {
        uint temp = charTouint(CoRe[node] + 6 + 4 * i);
        changeInt(CoRe[node], 6 + 4 * i, temp + width * height);
    }
}
void insertCoo(uint node, ushort ch, int indexCoo, uchar *add)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    insertArr(node, startCoo + 4 + indexCoo, add, 6);
    // CoRe[node].insert(CoRe[node].begin() + startCoo + 4 + indexCoo, add.begin(), add.end());
    changeInt(CoRe[node], startCoo, sizeCoo + 6);
    ushort sizeCh = numCh(node);
    for (ushort i = ch + 1; i < sizeCh; i++)
    {
        uint temp = charTouint(CoRe[node] + 6 + 4 * i);
        changeInt(CoRe[node], 6 + 4 * i, temp + 6);
    }
}
void insertRev(uint node, ushort ch, int indexRev, uchar *add)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    insertArr(node, startRev + +indexRev, add, 6);
    // CoRe[node].insert(CoRe[node].begin() + startRev + 4 + indexRev, add.begin(), add.end());
    changeInt(CoRe[node], startRev, sizeRev + 6);
    ushort sizeCh = numCh(node);
    for (ushort i = ch + 1; i < sizeCh; i++)
    {
        uint temp = charTouint(CoRe[node] + 6 + 4 * i);
        changeInt(CoRe[node], 6 + 4 * i, temp + 6);
    }
}
uint startCh(uint node, ushort ch)
{
    return charTouint(CoRe[node] + 6 + 4 * ch);
}
uint sizeCoo(uint node, ushort ch)
{
    uint start = startCh(node, ch);
    return charTouint(CoRe[node] + start);
}
uint sizeRev(uint node, ushort ch)
{
    // std::cerr << "call sizeRev()" << std::endl;
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    return charTouint(CoRe[node] + startCoo + 4 + sizeCoo);
}
vector<uint> sizeCoRe(uint node, ushort ch)
{
    vector<uint> re(2);
    uint startCoo = startCh(node, ch);
    re[0] = charTouint(CoRe[node] + startCoo);
    re[1] = charTouint(CoRe[node] + startCoo + 4 + re[0]);
    return re;
}
void CoMove(uint ori, ushort oriCh, uint From, uint To, ushort ToCh)
{
    // uchar *ToCoo = new uchar[6];
    uchar ToCoo[6] = {0};
    uint start = startCh(ori, oriCh);
    for (int i = 0; i < charTouint(CoRe[ori] + start) / 6; i++)
    {
        uint ii = 4 + 6 * i;
        if (charTouint(CoRe[ori] + start + ii) == From)
        {
            std::copy(CoRe[ori] + start + ii, CoRe[ori] + start + ii + 6, ToCoo);
            eraseCoo(ori, oriCh, i);
        }
    }
    ushort revCh = charToushort(ToCoo + 4);
    pushCoo(To, ToCh, ToCoo); // pushCoo에서 ToCoo를 삭제함.
    vector<uint> szCR = sizeCoRe(From, revCh);
    start = startCh(From, revCh);
    for (int i = 0; i < szCR[1] / 6; i++)
    {
        if (charTouint(CoRe[From] + start + 8 + szCR[0] + 6 * i) == ori)
        {
            changePair(CoRe[From], start + 8 + szCR[0] + 6 * i, To, ToCh);
            break;
        }
    }
}
void link(uint prevNode, ushort prevCh, uint nextNode, ushort nextCh)
{
    pushCoo(prevNode, prevCh, pairToBytes(nextNode, nextCh));
    pushRev(nextNode, nextCh, pairToBytes(prevNode, prevCh));
}
std::wstring ushortToWstring(ushort num) // ushort 번호에 해당하는 wstring 글자를 반환하는 함수임. ushort 자체를 글자로 변환하는 거 아님(이거는 intToWString() 사용할 것)
{
    wchar_t wc = static_cast<wchar_t>(num);
    wstring ws(1, wc);
    return ws;
}
std::wstring intToWString(int num) // 숫자를 wstring으로 바꿔주는 함수
{
    std::wstringstream wss;
    wss << num;
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
uchar *word(uint node)
{
    // uchar *re1 = new uchar[1024];
    uchar re1[4000]; // 스택에 고정 크기 할당

    ushort nowPosi = 4;
    if (node >= ttt && node < ttt + 256)
    {
        re1[nowPosi] = (uchar)(node - ttt);
        nowPosi++;
    }
    else if (sizeRev(node, 0) > 0)
    {
        stack<uint> st;
        vector<uint> index;
        st.push(node);
        index.push_back(0);

        int ii = 0;
        while (!st.empty() && ii < 4000)
        {
            ii++;
            uint topNode = st.top();
            vector<uint> szCR = sizeCoRe(topNode, 0);
            if (szCR[1] == 6 * index.back())
            {
                if (st.size() == 1)
                    break;
                st.pop();
                index.pop_back();
                index.back()++;
                continue;
            }
            uint startCoo = startCh(topNode, 0);
            uint rev = charTouint(CoRe[topNode] + startCoo + 8 + szCR[0] + 6 * index.back());
            if (rev != 41155)
            {
                st.push(rev);
                index.push_back(0);
            }
            else
            {
                uchar chch = (uchar)(topNode - ttt);
                re1[nowPosi] = chch;
                nowPosi++;
                st.pop();
                index.pop_back();
                if (!index.empty())
                    index.back()++;
            }
        }
        if (ii > 4000) // 오류 처리 logic은 while 문 밖으로 꺼내는 게 좋음
        {
            // Log(L"word Error! node: " + intToWString(node));
        }
    }
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
uint ushortToNode(ushort charCode)
{
    uchar *byte = ushortToBytes(charCode);
    uint node1 = ttt + byte[0];
    uint startCoo = charTouint(CoRe[node1] + 6);
    uint sizeCoo = charTouint(CoRe[node1] + startCoo);
    uint node2 = 0;
    for (int i = 0; i < sizeCoo / 6; i++)
    {
        node2 = charTouint(CoRe[node1] + startCoo + 4 + 6 * i);
        uint startCoo2 = charTouint(CoRe[node2] + 6);
        uint sizeCoo2 = charTouint(CoRe[node2] + startCoo2);
        uint startRev = startCoo2 + 4 + sizeCoo2;
        uint node3 = charTouint(CoRe[node2] + startRev + 10);
        if (node3 == ttt + byte[1])
        {
            break;
        }
    }
    delete[] byte;
    return node2;
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
        uint node = ushortToNode(dd);
        uchar *widthData = axis(node, 0, 3);
        uint adv = charTouint(widthData + 8);
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
    const int maxWidth = 400;
    std::vector<std::wstring> lines;

    size_t start = 0;
    float width = 0;

    for (size_t end = 0; end <= input.size(); ++end)
    {
        ushort dd = static_cast<ushort>(input[end]);
        uint node = ushortToNode(dd);
        uchar *widthData = axis(node, 0, 3);
        uint adv = charTouint(widthData + 8);

        if (end == input.size() || input[end] == L'\n')
        {
            lines.push_back(input.substr(start, end - start));
            start = end + 1;
            width = 0;
        }
        else
        {
            float advance = adv >> 6;
            width += advance;
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
    LogToClient = L"";
    //wcout << text << endl;
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
    for (int i = line; i < LogStr.size(); i++)
    {
        LogToClient += LogStr[i] + L"\n";
        std::vector<std::wstring> lines = splitWstring(LogStr[i], L"\n");
        for (int j = 0; j < lines.size(); j++)
        {
            std::vector<wstring> lines2 = splitLinesW(lines[j]);
            for (const auto &line2 : lines2)
            {
                ii++;
                string str = wstringToUtf8(line2);
                // RenderText(str, 1000, 1000 - (500 + 15 * ii));
            }
        }
    }
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
    for (int i = 0; i < size / 2; i++)
    {
        ss.push_back(charToushort(val + 4 + 2 * i));
    }
    // delete[] val;
    for (int i = 0; i < ss.size(); i++)
    {
        wstring temp = ushortToWstring(ss[i]);
        re.append(temp);
    }
    return re;
}
uchar *Sheet(uint node)
{
    //cerr << "call Sheet()" << endl;
    constexpr uint reSize = 4000;
    uchar *re = new uchar[reSize]();
    ushort nowPosi = 4;

    if (node == 41155)
    {
        delete[] re;
        return wstringToUChar(L"Entrance");
    }
    int szR = sizeRev(node, 0);
    if (szR > 1)
    {
        delete[] re;
        return word(node);
    }
    else if (szR == 0)
    {
        cerr << "szR == 0" << endl;
        std::pair<uint, ushort> dd(node, 0);
        uint start1 = startCh(dd.first, dd.second);
        dd = charToPair(CoRe[dd.first] + start1 + 4);

        while (dd.first != node || dd.second != 0)
        {
            uchar *wordDD = word(dd.first); // Assuming word() allocates memory
            uint sizeWord = charTouint(wordDD);
            // wcout << L"sizeWord = " << intToWString(sizeWord) << endl;
            if (nowPosi + sizeWord <= reSize)
            {
                memcpy(re + nowPosi, wordDD + 4, sizeWord);
                nowPosi += sizeWord;
            }

            uint startCoo = startCh(dd.first, dd.second);
            if (charTouint(CoRe[dd.first] + startCoo) == 0)
            {
                delete[] wordDD;
                break;
            }
            dd = charToPair(CoRe[dd.first] + startCoo + 4);
            delete[] wordDD;
        }
    }
    cerr << "before sizeRe" << endl;
    uchar *sizeRe = uintToBytes(nowPosi - 4);
    cerr << "before memcpy()" << endl;
    memcpy(re, sizeRe, 4); // Copy 4 bytes of size
    cerr << "after sizeRe" << endl;
    delete[] sizeRe;
    if (!re)
    {
        std::cerr << "re is a null pointer." << endl;
    }
    else
    {
        std::cerr << "re is not a null pointer." << endl;
    }
    wstring ww = charToWstring(re);
    //wcout << L"ww = " << ww << endl;
    string ss = wstringToUtf8(ww);
    //std::cerr << "re = " << ss << std::endl;
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
        for (int i = 0; i < charTouint(CoRe[coord] + startCoo) / 6; i++)
        {
            uint cd = charTouint(CoRe[coord] + startCoo + 4 + 6 * i);
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
        uint startCoo = charTouint(CoRe[coord] + 6);
        uint sizeCoo = charTouint(CoRe[coord] + startCoo);
        for (int i = 0; i < sizeCoo / 6; i++)
        {
            uint cd = charTouint(CoRe[coord] + startCoo + 4 + 6 * i);
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
void clearCh(uint node, ushort ch)
{
    uint startCoo = startCh(node, ch);
    std::fill(CoRe[node] + startCoo, CoRe[node] + startCoo + 8, 0);
}
void addCh(uint node)
{
    ushort numch = numCh(node);
    changeShort(CoRe[node], 4, numch + 1);
    uint sizeNode = 4 + charTouint(CoRe[node]);
    uchar *temp2 = uintToBytes(sizeNode + 4);
    insertArr(node, 6 + 4 * numch, temp2, 4);
    delete[] temp2;
    uchar *z8 = new uchar[8]();
    insertArr(node, sizeNode, z8, 8);
    delete[] z8;
    for (int i = 0; i < numch; i++)
    {
        changeInt(CoRe[node], 6 + 4 * i, charTouint(CoRe[node] + 6 + 4 * i) + 4);
    }
    changeInt(CoRe[node], 0, sizeNode + 12);
}
void addCh2(uint node)
{
    ushort numch = numCh(node);
    changeShort(CoRe[node], 4, numch + 2);
    uint sizeNode = 4 + charTouint(CoRe[node]);
    uchar *temp2 = uintToBytes2(sizeNode + 8, sizeNode + 16);
    insertArr(node, 6 + 4 * numch, temp2, 8);
    delete[] temp2;
    uchar *z16 = new uchar[16]();
    insertArr(node, sizeNode, z16, 16);
    delete[] z16;
    for (int i = 0; i < numch; i++)
    {
        changeInt(CoRe[node], 6 + 4 * i, charTouint(CoRe[node] + 6 + 4 * i) + 8);
    }
    changeInt(CoRe[node], 0, sizeNode + 24);
}
bool isZ8(uchar *arr)
{
    // arr이 가리키는 메모리를 uint64_t로 해석하여 비교
    return *reinterpret_cast<uint64_t *>(arr) == 0;
}
void move(int numTo, uint user)
{
    uint temp{};
    uint tN = cNode[user];
    uint startCoo = startCh(cNode[user], cCh[user]);
    if (numTo > 0)
    {
        temp = numTo - 1;
        cNode[user] = charTouint(CoRe[cNode[user]] + startCoo + 4 + 6 * temp);
        cCh[user] = charToushort(CoRe[tN] + startCoo + 8 + 6 * temp); // cNode[user]를 변경했으므로 tN을 써야 함!!
    }
    else
    {
        temp = (-1 * numTo) - 1;
        uint szC = charTouint(CoRe[cNode[user]] + startCoo);
        cNode[user] = charTouint(CoRe[cNode[user]] + startCoo + 8 + szC + 6 * temp);
        cCh[user] = charToushort(CoRe[tN] + startCoo + 12 + szC + 6 * temp);
    }
}
void Brain(uint node, uchar *data) // 사이즈 정보 포함해서 받아야 함
{
    cerr << "call Brain()" << endl;
    auto start = std::chrono::high_resolution_clock::now();
    uint coord = 41155;
    ushort ch = 0;
    uint beforeCd = node;
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
            pair<uint, ushort> dh = charToPair(CoRe[coord] + startCoo + 4);
            if (sizeRev(dh.first, 0) == 0)
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
                    pushRev2(coord3, 0, pairToBytes2(coord, 0, dh.first, 0));
                    pushCoo(coord, 0, pairToBytes(coord3, 0));
                    int szC = sizeCoo(coord, 0) / 6 - 1;

                    for (int j = szC - 1; j >= 0; j--)
                    {
                        uint startCoo2 = startCh(coord, 0);
                        uint i46j = charTouint(CoRe[coord] + startCoo2 + 4 + 6 * j);
                        uchar *wordi46j = word(i46j);
                        uint sizeWordi46j = charTouint(wordi46j);
                        if (equal(strsum + 4, strsum + 4 + sizeStrsum, wordi46j + 4) && sizeStrsum < sizeWordi46j)
                        {
                            eraseCoo(coord, 0, j);
                            pushCoo(coord3, 0, pairToBytes(i46j, 0));
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
                        dh = charToPair(CoRe[dh.first] + startDh + 4);
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
                        pair<uint, ushort> temp3 = charToPair(CoRe[temp2.first] + startTemp2 + 4);
                        clearCh(temp2.first, temp2.second); // temp2를 위의 두 줄 코드보다 먼저 clear하면 안 됨!
                        pushCoo(temp.first, temp.second, pairToBytes(coord3, 1));
                        pushCoo(coord3, 1, pairToBytes(temp3.first, temp3.second));
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
        pushCoo(beforeCd, beforeCh, pairToBytes(coord, ch));
        beforeCd = coord;
        beforeCh = ch; // 현재 채널을 before로 이동
        save.push_back(make_pair(beforeCd, beforeCh));
    }
    pushCoo(beforeCd, beforeCh, pairToBytes(node, 0));
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
wstring findAndUpdateOrder(uint node, ushort ch, ushort num)
{
    wstring re = L"";
    long long tmp = 0;
    for (int i = 0; i < order[num].size(); i++)
    {
        if (node == get<0>(order[num][i]) && ch == get<1>(order[num][i]))
        {
            re = timeW(get<2>(order[num][i]));
            tmp = get<3>(order[num][i]);
            re = re + L"\t" + timeW(tmp);
            order[num].erase(order[num].begin() + i);
        }
    }
    time_t timer;
    time(&timer);
    order[num].push_back(make_tuple(node, ch, timer, tmp));
    return re;
}
void eraseOrder(uint node, ushort ch, ushort num)
{
    for (int i = 0; i < order[num].size(); i++)
    {
        if (node == get<0>(order[num][i]) && ch == get<1>(order[num][i]))
        {
            order[num].erase(order[num].begin() + i);
        }
    }
}
void read_order(const std::string &file_path)
{
    ifstream file(file_path, ios::binary);
    while (file)
    {
        unsigned int size;
        file.read(reinterpret_cast<char *>(&size), sizeof(size));
        if (!file)
            break;
        vector<tuple<int, ushort, long long, long long>> inner_v(size);
        for (auto &t : inner_v)
        {
            int i;
            ushort us;
            long long tt;
            long long tt2;
            file.read(reinterpret_cast<char *>(&i), sizeof(i));
            file.read(reinterpret_cast<char *>(&us), sizeof(us));
            file.read(reinterpret_cast<char *>(&tt), sizeof(tt));
            file.read(reinterpret_cast<char *>(&tt2), sizeof(tt2));
            t = make_tuple(i, us, tt, tt2);
        }
        order.push_back(inner_v);
    }
    file.close();
}
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
wstring contentList(uint node, ushort ch)
{
    wstring ws = L"";
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    if (sizeRev > 0)
    {
        uint prevNode = axis2(node, ch);
        wstring prev = L"";
        wstring pprev = L"";
        for (int i = 0; i < sizeRev / 6; i++)
        {
            wstring hpL = L"<a href='javascript:__doPostBack(\"LinkButtonRev" + intToWString(i + 1) + L"\", \"\")' onclick='sendTextToServer(" + intToWString(-(i + 1)) + L"); return false;'>" + intToWString(-(i + 1)) + L"</a>";
            uchar *sheetPrev = Sheet(charTouint(CoRe[node] + prevNode + 6 * i));
            prev += hpL + L". " + charToWstring(sheetPrev) + L"<br/>";
            delete[] sheetPrev;
            // prev += hpL + L". " + charToWstring(Sheet(*reinterpret_cast<uint *>(&prevNode[6 * i]))) + L"<br/>";
            uint node2 = charTouint(CoRe[node] + prevNode + 6 * i);
            ushort ch2 = charToushort(CoRe[node] + prevNode + 4 + 6 * i);
            uint startCoo2 = charTouint(CoRe[node2] + 6 + 4 * ch2);
            uint sizeCoo2 = charTouint(CoRe[node2] + startCoo2);
            uint startRev2 = startCoo2 + 4 + sizeCoo2;
            uint sizeRev2 = charTouint(CoRe[node2] + startRev2);
            // uchar *splPrev2 = CoRe[node2] + startRev2 + 4 + 6 * i;
            uint splPrev2 = startRev2 + 4 + 6 * i;
            // vector<uchar> splPrev2 = axon2(charTouint(prevNode, 6 * i), charToushort(prevNode, 4 + 6 * i));
            for (int j = 0; j < sizeRev2 / 6; j++)
            {
                // pprev += intToWString(-(i + 1)) + L"." + intToWString(j + 1) + L" " + charToWstring(Sheet(*reinterpret_cast<uint *>(&splPrev2[6 * j]))) + L"<br/>";
                uint sheetNode2 = charTouint(CoRe[node2] + splPrev2 + 6 * j);
                if (sheetNode2 > CoRe.size()){
                    Log(L"error: prev i = " + intToWString(i) + L", pprev j = " + intToWString(j) + L".");
                    continue;
                }
                uchar *sheetPP = Sheet(sheetNode2);
                pprev += intToWString(-(i + 1)) + L"." + intToWString(j + 1) + L" " + charToWstring(sheetPP) + L"<br/>";
                delete[] sheetPP;
            }
        }
        ws += pprev + L"<p class='prev'>" + prev;
    }
    uchar *sheetNode = Sheet(node);
    ws += L"<p class='this'>" + charToWstring(sheetNode) + L"<br/>" + L"<p class='next'>"; // 현재 단계
    delete[] sheetNode;

    uchar *nextNode = CoRe[node] + startCoo + 4;
    uint page = 1;
    for (int i = (page - 1) * 50; i < page * 50 - 1 && sizeCoo != 0; i++)
    {
        if (i == sizeCoo / 6)
        {
            break;
        }
        wstring hpL = L"<a href='javascript:__doPostBack(\"LinkButtonNext" + intToWString(i + 1) + L"\", \"\")' onclick='sendTextToServer(" + intToWString(i + 1) + L"); return false;'>" + intToWString(i + 1) + L"</a>";
        // ws += hpL + L". " + charToWstring(Sheet(*reinterpret_cast<uint *>(&nextNode[6 * i]))) + L"<br/>";
        uchar *sheetNext = Sheet(charTouint(nextNode + 6 * i));
        ws += hpL + L". " + charToWstring(sheetNext) + L"<br/>";
        delete[] sheetNext;
    }
    if (sizeCoo / 6 > 50)
    {
        ws += L"<br />Page ";
        for (int i = 1; i <= (sizeCoo / 6 / 50) + 1; i++)
        {
            ws += L" <a href='javascript:__doPostBack(\"PageButton" + intToWString(i) + L"\", \"\")' onclick='sendTextToServer(" + L"\"Page\" + " + intToWString(i) + L"); return false;'>" + intToWString(i) + L"</a>";
        }
    }
    ws += L"<style> .prev{margin-left: 10px;} .this{margin-left: 20px;} .next{margin-left:30px;}</style>";
    return ws;
}

void study(uint user)
{
    uint t1 = get<0>(order[user][0]);
    ushort t2 = get<1>(order[user][0]);
    while (numCh(t1) > t2 + 1)
    {
        order[user].erase(order[user].begin());
        t1 = get<0>(order[user][0]);
        t2 = get<1>(order[user][0]);
    }
    cNode[user] = t1;
    cCh[user] = t2;
    wcout << "98Node = " << intToWString(t1) << endl;
    wcout << "98Ch = " << to_wstring(t2) << endl;
    tuple<int, ushort, time_t, time_t> temp = order[user][0];
    eraseOrder(t1, t2, user);
    order[user].push_back(temp);
}
void study2(uint user)
{
    uint t1 = get<0>(order[user][0]);
    ushort t2 = get<1>(order[user][0]);
    eraseOrder(t1, t2, user);
    // study(user);
}
uint sizeGarbage()
{
    int i = 0;
    uint temp = charTouint(CoRe[gar] + 14);
    while (temp != gar)
    {
        temp = charTouint(CoRe[temp] + 14);
        i++;
    }
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
    infoStr = L"\norder.size: " + intToWString(order[1].size() - 1) + L"\nGarbage.size: " + intToWString(sizeGarbage()) + L"\nCsave: " + charToWstring(sh1) + L"," + charToWstring(sh2) + L"," + charToWstring(sh3) + L"\nvmSize: " + intToWString(vmSize) + L" kB\nvmRSS: " + intToWString(vmRSS) + L" kB\nFPS: " + intToWString(fps);
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
}
void pushGarbage(uint node)
{
    uint next = charTouint(CoRe[gar] + 14); // 14가 다음 노드 가리키는 위치임
    // uchar newCh[18] = {14, 0, 0, 0, 1, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //startCoo = 6이 아니라 10이 되어야 함.
    //  delete[] CoRe[gar]; //pushCoo에서 CoRe[gar]를 사용하므로 메모리 해제하면 안됨
    // CoRe[gar] = newCh;
    uchar *ptb = pairToBytes(node, 0);
    uchar *ptb2 = pairToBytes(next, 0);
    uchar newCh2[24] = {20, 0, 0, 0, 1, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 24; i++)
    {
        CoRe[node][i] = newCh2[i];
    }
    // delete[] CoRe[node];
    // CoRe[node] = newCh2;
    for (int i = 0; i < 6; i++)
    {
        CoRe[gar][i + 14] = ptb[i];
        CoRe[node][i + 14] = ptb2[i];
    }
    // pushCoo(gar, 0, ptb);
    // pushCoo(node, 0, ptb2);
    delete[] ptb;
    delete[] ptb2;
    // delete[] newCh2;
}
void clearToken(uint node)
{
    uint startCoo = startCh(node, 0);
    pair<uint, ushort> cod = charToPair(CoRe[node] + startCoo + 4);
    while (cod != make_pair((uint)node, (ushort)0))
    {
        pair<uint, ushort> temp = cod;
        startCoo = startCh(cod.first, cod.second);
        cod = charToPair(CoRe[cod.first] + startCoo + 4);
        // CoRe[temp.first][temp.second] = zero8;
        clearCh(temp.first, temp.second);
    }
}
void deleteNode(uint node)
{
    if (sizeRev(node, 0) == 6)
    {
        clearToken(node);
    }
    uint numch = numCh(node);
    for (int i = 0; i < numch; i++)
    {
        vector<uint> sizeCRn = sizeCoRe(node, i);
        uint startI = startCh(node, i);
        for (int j = 0; j < sizeCRn[0] / 6; j++)
        {
            // uint startI = startCh(node, i);
            pair<uint, ushort> temp = charToPair(CoRe[node] + startI + 4 + 6 * j);
            // int size = CoRe[temp.first].size();
            int size = numCh(temp.first);
            if (size > 0 && size <= temp.second + 1)
            {
                vector<uint> szCR = sizeCoRe(temp.first, temp.second);
                if (szCR[1] > 0)
                {
                    for (int k = 0; k < szCR[1] / 6; k++)
                    {
                        uint startTemp = startCh(temp.first, temp.second);
                        pair<uint, ushort> temp2 = charToPair(CoRe[temp.first] + startTemp + 8 + szCR[0] + 6 * k);
                        if (temp2 == make_pair((uint)node, (ushort)i))
                        {
                            // eraseRev(CoRe[temp.first], startTemp, 8 + szCR[0] + 6 * k);
                            eraseRev(temp.first, temp.second, k);
                            szCR[1] = szCR[1] - 6;
                        }
                    }
                }
            }
        }
        for (int j = 0; j < sizeCRn[1] / 6; j++)
        {
            pair<uint, ushort> temp = charToPair(CoRe[node] + startI + 8 + sizeCRn[0] + 6 * j);
            int size = numCh(temp.first);
            if (size > 0 && size <= temp.second + 1)
            {
                uint startTemp = startCh(temp.first, temp.second);
                uint szC = charTouint(CoRe[temp.first] + startTemp);
                if (szC > 0)
                {
                    for (int k = 0; k < szC / 6; k++)
                    {
                        pair<uint, ushort> temp2 = charToPair(CoRe[temp.first] + startTemp + 4 + 6 * k);
                        if (temp2 == make_pair((uint)node, (ushort)i))
                        {
                            // eraseCoo(CoRe[temp.first], startTemp, 4 + 6 * k);
                            eraseCoo(temp.first, temp.second, k);
                            szC = szC - 6; // szC를 6 빼주어야 다음 loop에서 오류 안 남!!
                        }
                    }
                }
            }
        }
        eraseOrder(node, i, 1);
    }
    // CoRe[node].clear();
    pushGarbage(node);
}
void cut(uint node, ushort ch, uint index)
{
    vector<uint> sizeCRn = sizeCoRe(node, ch);
    uint startCoo = startCh(node, ch);
    pair<uint, ushort> temp = charToPair(CoRe[node] + startCoo + 4 + 6 * index);
    // uint startTemp = startCh(temp.first, temp.second);
    int size = numCh(temp.first);
    if (size > 0 && size <= temp.second + 1)
    {
        vector<uint> szCR = sizeCoRe(temp.first, temp.second);
        if (szCR[1] > 0)
        {
            for (int k = 0; k < szCR[1] / 6; k++)
            {
                if (temp == make_pair(cNode[1], cCh[1]))
                {
                    // eraseRev(CoRe[temp.first], startTemp, 8 + szCR[0] + 6 * k);
                    eraseRev(temp.first, temp.second, k);
                }
            }
        }
    }
    // eraseCoo(CoRe[node], startCoo, 4 + 6 * index);
    eraseCoo(node, ch, index);
}
uint popGarbage()
{
    int i = 0;
    uint re = charTouint(CoRe[gar] + 14);
    uint next = charTouint(CoRe[re] + 14);
    clearCh(re, 0);
    clearCh(gar, 0);
    uchar *ptb = pairToBytes(next, 0);
    pushCoo(gar, 0, ptb);
    delete[] ptb;
    return re;
}
void AddStringToNode(const string &str, uint node, ushort ch, uint user)
{
    cerr << "call AddStringToNode()" << endl;
    wstring wstr = utf8ToWstring(str);
    uchar *wstr2 = wstringToUChar(wstr);
    time_t timer;
    time(&timer);
    uint newcd = CoRe.size();
    if (sizeGarbage() > 10)
    { // 쓰레기통에서 재활용
        wcout << L"sizeGabage() > 10" << endl;
        newcd = popGarbage();
        wcout << L"popGarbage(l = " << intToWString(newcd) << endl;
        addCh(newcd); // make count of ch as 2
        link(node, ch, newcd, 1);
        order[user].push_back(make_tuple(newcd, 1, timer, timer)); // 생성시간 = timer
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
        link(node, ch, newcd, 1);
        order[user].push_back(make_tuple(newcd, 1, timer, timer));
        Brain(newcd, wstr2);
    }
    delete[] wstr2;
}
void save(string directory)
{
    auto start = std::chrono::high_resolution_clock::now();

    ofstream out(directory + "Brain3-test.bin", ios::binary);
    int ii = 0;
    uint size = CoRe.size();
    out.write(reinterpret_cast<const char *>(&size), 4);
    for (int i = 0; i < CoRe.size(); i++)
    {
        uint size = charTouint(CoRe[i]);
        if (size > 100000)
        {
            break;
        }
        out.write(reinterpret_cast<const char *>(CoRe[i]), size + 4);
    }
    out.close();
    ofstream file2(directory + "order3-test.bin", ios::binary);
    for (const auto &inner_v : order)
    {
        unsigned int size = inner_v.size();
        file2.write(reinterpret_cast<const char *>(&size), sizeof(size));
        for (const auto &t : inner_v)
        {
            file2.write(reinterpret_cast<const char *>(&get<0>(t)), sizeof(get<0>(t)));
            file2.write(reinterpret_cast<const char *>(&get<1>(t)), sizeof(get<1>(t)));
            file2.write(reinterpret_cast<const char *>(&get<2>(t)), sizeof(get<2>(t)));
            file2.write(reinterpret_cast<const char *>(&get<3>(t)), sizeof(get<3>(t)));
        }
    }
    file2.close();
    // saveMapToFile(directory + "bitmapGlyphMap.bin", bitmapGlyphMap);
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
    AddStringToNode(data["username"], 34196, 1, 1);
    uint startCoo = charTouint(CoRe[34196] + 6 + 4 * 1);
    uint sizeCoo = charTouint(CoRe[34196] + startCoo);
    AddStringToNode(data["password"], charTouint(CoRe[34196] + startCoo + sizeCoo - 6), 1, 1);
    AddStringToNode(data["email"], charTouint(CoRe[34196] + startCoo + sizeCoo - 6), 1, 1);
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
    uint cch = numCh(cNode[user]);
    wstring content = intToWString(user) + L"\t" + intToWString(cNode[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + contentList(cNode[user], cCh[user]) + L"\t" + intToWString(CoRe.size()) + L"\t" + inputText + L"\t" + findAndUpdateOrder(cNode[user], cCh[user], user) + L"\t" + LogToClient + L"\t" + infoStr + L"\t" + intToWString(cch) + L"\t" + intToWString(copyNode.first) + L"\t" + intToWString((int)copyNode.second);
    return content;
}
void handleLogin(const std::string &requestBody, int client_socket)
{
    auto data = parsePostData(requestBody);
    // bool isAuthenticated = checkLogin(data["username"], data["password"]);
    uchar *IDList = axis1(34196, 1);
    bool check = false;
    uint startCoo = charTouint(CoRe[34196] + 6 + 4 * 1);
    uint sizeIDList = charTouint(CoRe[34196] + startCoo);
    wstring content = L"";
    for (int i = 0; i < sizeIDList / 6 - 1; i++)
    {
        uchar *sheetID = Sheet(*reinterpret_cast<uint *>(&IDList[6 * i]));
        if (utf8ToWstring(data["username"]) == charToWstring(sheetID)) // ID가 존재하는 경우
        {
            uint startCoo = startCh(34196, 1);
            pair<uint, ushort> userID = charToPair(CoRe[34196] + startCoo + 4 + 6 * i);
            uint startUserID = startCh(userID.first, userID.second);
            pair<uint, ushort> Pass = charToPair(CoRe[userID.first] + startUserID + 4);
            if (utf8ToWstring(data["password"]) == charToWstring(Sheet(Pass.first))) // 비밀번호가 동일한 경우
            {
                uint startPass = startCh(Pass.first, Pass.second);
                pair<uint, ushort> start = charToPair(CoRe[Pass.first] + startPass + 4);
                user = i + 1;
                cNode[user] = start.first;
                cCh[user] = start.second;
                // content = intToWString(user) + L"\t" + intToWString(start.first) + L"\t" + intToWString(start.first) + L"\t" + contentList(start.first, start.second) + L"\t" + intToWString(CoRe.size()) + L"\t"; // 시작 화면을 보냄
                sendMsg(client_socket, makeContent(user, L""));
            }
            else
            {
                content = intToWString(0) + L"\t" + intToWString(34199) + L"\t" + L"0" + L"\t" + L"비밀번호가 틀립니다. 다시 입력해 주세요." + L"\t" + intToWString(CoRe.size()) + L"\t"; // password 입력 화면을 보냄 user, node, ch, sheet 순서
                sendMsg(client_socket, content);
            }
            check = true;
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
void change_data(uint node, uchar *data)
{
    clearToken(node);
    clearCh(node, 0);
    Brain(node, data);
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
        uchar *IDList = axis1(34196, 1);
        uint startCoo = charTouint(CoRe[34196] + 6 + 4 * 1);
        uint sizeIDList = charTouint(CoRe[34196] + startCoo);
        bool check = true;
        for (int i = 0; i < sizeIDList / 6; i++)
        {
            if (utf8ToWstring(username) == charToWstring(Sheet(*reinterpret_cast<uint *>(&IDList[6 * i])))) // ID가 존재하는 경우
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
                            content = intToWString(user) + L"\t" + intToWString(34198) + L"\t" + L"0" + L"\t" + charToWstring(sheet34198) + L"\t" + intToWString(CoRe.size()) + L"\t"; // 아이디 입력 화면을 보냄 user, node, ch, sheet 순서
                            delete[] sheet34198;
                            std::cerr << "content = " << wstringToUtf8(content) << std::endl;
                            sendMsg(client_socket, content);
                        }
                        else if (clientMvec[1] == L"34,198" || clientMvec[1] == L"34198")
                        { // 아이디를 입력한 상태(비밀번호 입력 화면을 보내야 함)
                            // vector<uchar> IDList = CoRe[34196][1].first;
                            cerr << "clientMvec[1] = " << wstringToUtf8(clientMvec[1]) << endl;
                            uchar *IDList = axis1(34196, 1);
                            bool check = false;
                            uint startCoo = charTouint(CoRe[34196] + 6 + 4 * 1);
                            uint sizeIDList = charTouint(CoRe[34196] + startCoo);
                            wcout << L"sizeIDList = " << intToWString(sizeIDList) << endl;
                            for (int i = 0; i < sizeIDList / 6; i++)
                            {
                                if (clientMvec[3] == charToWstring(Sheet(*reinterpret_cast<uint *>(&IDList[6 * i])))) // ID가 존재하는 경우
                                {
                                    check = true;
                                    content = intToWString(i + 1) + L"\t" + intToWString(34199) + L"\t" + L"0" + L"\t" + charToWstring(Sheet(34199)) + L"\t" + intToWString(CoRe.size()) + L"\t"; // password 입력 화면을 보냄 user, node, sheet 순서
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
                        if (clientMvec[1] == L"34,199" || clientMvec[1] == L"34199")
                        { // 비밀번호 입력한 상태
                            uint startCoo = startCh(34196, 1);
                            pair<uint, ushort> userID = charToPair(CoRe[34196] + startCoo + 4 + 6 * (user - 1));
                            uint startUserID = startCh(userID.first, userID.second);
                            pair<uint, ushort> Pass = charToPair(CoRe[userID.first] + startUserID + 4);
                            if (clientMvec[3] == charToWstring(Sheet(Pass.first))) // 비밀번호가 동일한 경우
                            {
                                uint startPass = startCh(Pass.first, Pass.second);
                                pair<uint, ushort> start = charToPair(CoRe[Pass.first] + startPass + 4);
                                cNode[user] = start.first;
                                cCh[user] = start.second;
                                sendMsg(client_socket, makeContent(user, L""));
                            }
                            else
                            {
                                content = intToWString(user) + L"\t" + intToWString(34199) + L"\t" + L"0" + L"\t" + L"비밀번호가 틀립니다. 다시 입력해 주세요." + L"\t" + intToWString(CoRe.size()) + L"\t"; // password 입력 화면을 보냄 user, node, ch, sheet 순서
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
                                    cerr << "989898" << endl;
                                }
                                else if (num == 982) // if not working 98 function
                                {
                                    study2(user);
                                    content = intToWString(user) + L"\t" + intToWString(cNode[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + contentList(cNode[user], cCh[user]) + L"\t" + intToWString(CoRe.size()) + L"\t98";
                                    sendMsg(client_socket, content);
                                }
                                else if (num == 99)
                                {
                                    copyNode = make_pair(cNode[user], cCh[user]);
                                    sendMsg(client_socket, makeContent(user, L""));
                                }
                                else if (num == 100)
                                {
                                    if (copyNode.first != 0 && copyNode.second != 0)
                                    {
                                        link(cNode[user], cCh[user], copyNode.first, copyNode.second);
                                    }
                                    sendMsg(client_socket, makeContent(user, L""));
                                }
                                else if ((num > 0 && num <= sizeCoo(cNode[user], cCh[user]) / 6) || (num < 0 && -num <= sizeRev(cNode[user], cCh[user]) / 6))
                                {
                                    move(num, user);
                                    sendMsg(client_socket, makeContent(user, L""));
                                }
                            }
                            else
                            {
                                // std::cout << "Invalid argument: the wstring cannot be converted to an integer." << std::endl;
                                uint startCoo = charTouint(CoRe[6478] + 10);
                                uint sizeCoo = charTouint(CoRe[6478] + startCoo) / 6;
                                for (int i = 0; i < sizeCoo; i++)
                                { // 바로가기 기능 구현
                                    uint nextNode = charTouint(CoRe[6478] + startCoo + 4 + 6 * i);
                                    uchar *sheetNode = Sheet(nextNode);
                                    wstring ws = charToWstring(sheetNode);
                                    if (clientMvec[3] == ws)
                                    {
                                        ushort nextCh = charToushort(CoRe[6478] + startCoo + 8 + 6 * i);
                                        cNode[user] = nextNode;
                                        cCh[user] = nextCh;
                                        delete[] sheetNode;
                                        sendMsg(client_socket, makeContent(user, L"", L""));
                                        break;
                                    }
                                    else
                                    {
                                        delete[] sheetNode;
                                    }
                                }
                                if (clientMvec[3][0] == L'/')
                                {
                                    string str = wstringToUtf8(clientMvec[3].substr(1));
                                    AddStringToNode(str, cNode[user], cCh[user], user);
                                    sendMsg(client_socket, makeContent(user, L"", L""));
                                }
                                else if (inputText == "시작" || inputText == "start")
                                {
                                    cNode[user] = 0;
                                    cCh[user] = 1;
                                    sendMsg(client_socket, makeContent(user, L"", L""));
                                }
                                else if (inputText == "수정")
                                {
                                    uchar *sheetNode = Sheet(cNode[user]);
                                    sendMsg(client_socket, makeContent(user, L"@" + charToWstring(sheetNode), L""));
                                    delete[] sheetNode;
                                }
                                else if (clientMvec[3][0] == L'@')
                                {
                                    string str = inputText.substr(1);
                                    wstring wstr = utf8ToWstring(str);
                                    uchar *wstr2 = wstringToUChar(wstr);
                                    change_data(cNode[1], wstr2);
                                    delete[] wstr2;
                                    sendMsg(client_socket, makeContent(user, L""));
                                }
                                else if (clientMvec[3][0] == L'#') // Search
                                {
                                    string str = inputText.substr(1);
                                    wstring wstr = utf8ToWstring(str);
                                    uchar *wstr2 = wstringToUChar(wstr);
                                    uint dataSz = charTouint(wstr2);
                                    uint Node = firstToken(wstr2, dataSz);
                                    cNode[user] = Node;
                                    cCh[user] = 1;
                                    sendMsg(client_socket, makeContent(user, L""));
                                }
                                else if (inputText == "save" || inputText == "저장")
                                {
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
                                    deleteNode(cNode[user]);
                                    study(1);
                                    sendMsg(client_socket, makeContent(user, L"", L"del98 complete!"));
                                }
                                else if (inputText == "ch+")
                                { // channel plus
                                    ushort nc = numCh(cNode[user]);
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
                                    ushort nc = numCh(cNode[user]);
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
                                    content = intToWString(user) + L"\t" + intToWString(cNode[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + utf8ToWstring(content_html) + L"\t" + intToWString(CoRe.size()) + L"\thtml";
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
                                    content = intToWString(user) + L"\t" + intToWString(cNode[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + utf8ToWstring(content_html) + L"\t" + intToWString(CoRe.size()) + L"\tcpp";
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
                                        cut(cNode[1], cCh[1], tt);
                                        content = intToWString(user) + L"\t" + intToWString(cNode[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + contentList(cNode[user], cCh[user]) + L"\t" + intToWString(CoRe.size()) + L"\t";
                                        sendMsg(client_socket, content);
                                    }
                                    else
                                    {
                                        Log(L"올바른 입력 형식이 아닙니다. ");
                                    }
                                    // display(cNode[1], cCh[1]);
                                    inputText.clear();
                                }
                                else if (inputText.size() >= 4 && inputText.substr(0, 4) == "104,")
                                { // 자식 node로 이동
                                    cerr << "call 104, function" << endl;
                                    vector<string> spl = splitStringASCII(inputText, ',');
                                    if (spl.size() == 3)
                                    {
                                        uint startCoo = startCh(cNode[1], cCh[1]);
                                        uint res2 = charTouint(CoRe[cNode[1]] + startCoo + 4 + 6 * (stringToUint(spl[1]) - 1));
                                        int tt = 6 * (stringToUint(spl[2]) - 1);
                                        CoMove(cNode[1], cCh[1], res2, charTouint(CoRe[cNode[1]] + startCoo + 4 + tt), charToushort(CoRe[cNode[1]] + startCoo + 8 + tt));
                                        content = intToWString(user) + L"\t" + intToWString(cNode[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + contentList(cNode[user], cCh[user]) + L"\t" + intToWString(CoRe.size()) + L"\t";
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
                                        uint startCoo = startCh(cNode[1], cCh[1]);
                                        int tt = stringToUint(spl[1]) - 1;
                                        uint deln = charTouint(CoRe[cNode[1]] + startCoo + 4 + 6 * tt);
                                        cut(cNode[1], cCh[1], tt);
                                        deleteNode(deln);
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
                                        uint node = stringToUint(spl[1]);
                                        ushort ch = stringToUint(spl[2]);
                                        cNode[1] = node;
                                        cCh[1] = ch;
                                        content = intToWString(user) + L"\t" + intToWString(cNode[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + contentList(cNode[user], cCh[user]) + L"\t" + intToWString(CoRe.size()) + L"\t";
                                        sendMsg(client_socket, content);
                                    }
                                    else
                                    {
                                        Log(L"올바른 입력 형식이 아닙니다. ");
                                    }
                                    // clearInputText();
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

int main(int argc, char const *argv[])
{
    auto start = std::chrono::high_resolution_clock::now();
    std::locale::global(std::locale("en_US.UTF-8"));
    //std::wcout.imbue(std::locale());
    // RAM에 Brain UpRoad
    std::ifstream in("Brain3-test.bin", std::ios::binary);
    // int ii = 0;
    uchar *size2 = new uchar[4];
    in.read(reinterpret_cast<char *>(size2), sizeof(uint));
    uint size3 = charTouint(size2);
    wstring ww = intToWString(size3);
    //std::cerr << "Node: " << wstringToUtf8(ww) << std::endl;
    //Log(L"Node" + ww);
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

    string file_path2 = "order3-test.bin";
    read_order(file_path2);
    cNode[1] = 0;
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