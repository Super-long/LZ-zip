#include"huffman.h" //典型的编译依赖性 两个头文件反过来就有问题
#include"LZ77.h"    //TODO再思考下
#include<bits/stdc++.h>

using namespace std;
bool GetBit(std::stringstream& ss){
    static int i = 7;
    static unsigned char Bchar = 0;
    int x = 0;//128, 64, 32, 16, 8, 4, 2, 1
    static unsigned char bit[8] = {128, 64, 32, 16, 8, 4, 2, 1};
    ++i;
    if(i == 8){
        ss >> Bchar;
        i = 0;
    }
    return (Bchar&bit[i]);
}
int main(){
    LZ_zip::LZ77 temp("aaa.txt");
/*    temp.Encode();
    temp.ExchangeFilename("bbb.txt"); */
    temp.Encode();
    cout << "解码完成:\n";
    temp.Decode();
    LZ_zip::Huffman tree;
    tree.encode(temp);
    tree.show();
    tree.Decode();
/*     string str = "aaa";
    stringstream ss(str);
    string result;
    while(ss){
        GetBit(ss) ? result.push_back('1') : result.push_back('0');
    }
    cout << "all " << result.size() << endl;
    cout << result << endl; */
}