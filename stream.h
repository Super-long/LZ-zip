#ifndef STREAM_H_
#define STREAM_H_

#include<string>
#include<fstream>
#include<iostream>
#include<sstream>

namespace LZ_zip{

    //TODO:对于close考虑异常安全性
    class Stream{
        public:
            explicit Stream(std::string filename) : encode_file(filename) {
                if(!encode_file.is_open()){
                    std::cout << "未打开\n"; //需要考虑
                }
            }

            bool ExchangeOpenFile(std::string str){
                CloseFile();
                encode_file.open(str);
            }

            std::string ReadFile(){
                std::ostringstream buf;
                buf << encode_file.rdbuf();
                return buf.str(); //值传递 关键想让功能独立
            }

            void Close(){
                CloseFile();
            }

            ~Stream(){
                CloseFile();
            }
        private:
            std::fstream encode_file;
            void CloseFile(){
                if(encode_file.is_open())
                    encode_file.close();
            }
    };
}


#endif