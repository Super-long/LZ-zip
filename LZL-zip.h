#ifndef _LZLZIP_H_
#define _LZLZIP_H_

#include"LZ77.h"
#include"huffman.h"
#include<memory>

namespace LZ_zip{
    class LZL_zip{
        public:
            LZL_zip() = default;
            explicit LZL_zip(const std::string& str) :
                LZ_ptr(std::make_unique<LZ77>(str)),
                Huf_ptr(std::make_unique<Huffman>()){} 
            void encode(){
                Huf_ptr->encode(LZ_ptr);
            }

            void decode(){
                Huf_ptr->Decode();
                LZ_ptr->GetDecodeResult(Huf_ptr->ReturnDecodeResult());
                LZ_ptr->Decode(); //解析完写入另一个文件
            }
        private:
            std::unique_ptr<Huffman> Huf_ptr;
            std::unique_ptr<LZ77> LZ_ptr;
    };
}

#endif 