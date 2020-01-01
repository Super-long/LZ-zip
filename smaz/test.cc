#include <iostream>
#include "decondig.h"
#include "decondig.cc"
int main(){
    std::string initial = "Nothing is more difficult, and therefore more precious, than to be able to decide";
    std::string encode;
    deconding::smaz_compress(initial.data(),initial.size(),&encode);
    std::cout << encode << std::endl;
    std::string decode ;
    deconding::smaz_decompress(encode.data(),encode.size(),&decode);
    std::cout << decode << std::endl;
}