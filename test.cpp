#include"huffman.h" //典型的编译依赖性 两个头文件反过来就有问题
#include"LZ77.h"    //TODO再思考下
#include<bits/stdc++.h>

using namespace std;

int main(){
    LZ_zip::LZ77 temp("aaa.txt");
/*     temp.Encode();
    temp.ExchangeFilename("bbb.txt"); */
    temp.Encode();
    temp.Decode();
    temp.show();
    LZ_zip::Huffman tree;
    tree.encode(temp);
    tree.show();
}