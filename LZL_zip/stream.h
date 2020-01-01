#ifndef STREAM_H_
#define STREAM_H_

#include<string>
#include<fstream>
#include<iostream>
#include<sstream>

namespace LZ_zip{

    class InputStream{
        public:
            explicit InputStream(std::string filename) : encode_file(filename) {
                if(!encode_file.is_open()){
                    std::cout << "File not open\n";
                }
            }

            template<typename T>
            bool ExchangeOpenFile(T&& temp){
                CloseFile();
                encode_file.open(temp);
            }

            std::string ReadFile(){
                std::ostringstream buf;
                buf << encode_file.rdbuf();
                return buf.str();
                //TODO : Value passing, Want to make features independent.
            }

            void Close(){
                CloseFile();
            }

            ~InputStream(){
                CloseFile();
            }
        private:
            std::fstream encode_file;
            void CloseFile(){
                if(encode_file.is_open())
                    encode_file.close();
            }
    };

    class OutputStream{
        public:
            explicit OutputStream(std::string filename) : decode_file(filename, std::ofstream::app) {
                if(!decode_file.is_open()){
                    std::cout << "File not open.\n";
                }
            }
            template<typename T>
            void WriteFile(T&& temp){
                if(decode_file.is_open()){
                    decode_file << temp;
                }
            }

            template<typename T>
            bool ExchangeOpenFile(T&& temp){
                decode_file.close();
                decode_file.open(temp);
                return decode_file.is_open();
            }

            void Close(){
                CloseFile();
            }

            ~OutputStream(){
                CloseFile();
            }

        private:
            std::ofstream decode_file;
            void CloseFile(){
                if(decode_file.is_open())
                    decode_file.close();
            }
    };
}


#endif