#include"LZL-zip.h"

int main(){
    //Please create this file by yourself;
    LZ_zip::LZL_zip zip("TestCompress.txt");
    zip.encode();
    zip.decode();
}