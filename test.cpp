#include"LZ77.h"
#include<bits/stdc++.h>

using namespace std;

int main(){
    LZ_zip::LZ77 temp("aaa.txt");
    temp.Encode();
    temp.ExchangeFilename("bbb.txt");
    temp.Encode();
    temp.Decode();
    temp.show();
}