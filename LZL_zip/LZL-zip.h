#ifndef _LZLZIP_H_
#define _LZLZIP_H_

#include"LZ77.h"
#include"huffman.h"
#include<memory>
#include<iostream>

namespace LZ_zip{
    class LZL_zip{
        public:
            LZL_zip() = default;
            explicit LZL_zip(const std::string& str) :
                LZ_ptr(std::make_unique<LZ77>(str)),
                Huf_ptr(std::make_unique<Huffman>()){}
                
            void encode(){
                LZ_ptr->Encode();
                Huf_ptr->encode(LZ_ptr);
                Huf_ptr->show();
            }

            void decode(){
                Huf_ptr->Decode();
                auto code = Huf_ptr->ReturnDecodeResult();
                LZ_ptr->GetDecodeResult(code);
                LZ_ptr->Decode();
            }
        private:
            std::unique_ptr<Huffman> Huf_ptr;
            std::unique_ptr<LZ77> LZ_ptr;
    };
}

#endif 