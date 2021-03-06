#ifndef _LZ77_H_
#define _LZ77_H_

#include<fstream>
#include<vector>
#include<string>
#include<string_view>
#include"stream.h"
#include"huffman.h"

namespace LZ_zip{
    class LZ77{
        friend class Huffman;
        friend class LZL_zip;

        private:
            struct CodeNode
            {
                int distance;
                int length;
                char literal;
            };
        private:
            //forbid initialization no-const static member.
            short maxWindow = 32767;
            std::vector<CodeNode> NodeQueue;
            std::vector<CodeNode> DecodeResult;
            InputStream FileStream;
            std::string filename;
            std::string DecodefileContent;
        private:
            void LZ_encode();
            void encoding(const std::string_view& file);
            void LZ_decode();
        public:
            LZ77() = default;
            const auto& code()const{
                return NodeQueue;
            }
            explicit LZ77(std::string str) : FileStream(str), filename(std::move(str)) {}
            
            void ExchangeWindow(short win) noexcept{
                maxWindow = win;
            }

            const std::string& Filename() const{
                return filename;
            }

            void show() const{
                using std::cout; using std::endl;
                for(auto T : NodeQueue){
                    cout << T.distance << " " <<T.length << " " << T.literal << endl;
                }
                cout << DecodefileContent << endl;
            }

            void Encode() {LZ_encode();}
            void Decode() {LZ_decode();}
            
            void ExchangeFilename(std::string str){
                NodeQueue.clear();
                FileStream.ExchangeOpenFile(str);
                filename = std::move(str);
            }

            void GetDecodeResult(const decltype(DecodeResult)& result){
                DecodeResult.assign(result.begin(), result.end());
            }

    };
    void LZ77::LZ_decode(){
        for(int i = 0; i < DecodeResult.size(); i++) {
            if(DecodeResult[i].length == 0) {
                DecodefileContent += DecodeResult[i].literal;
            } else {
                int length = DecodefileContent.length();
                length -= DecodeResult[i].distance;
                //Concatenation involving a large number of strings, Rvalue is better. Unfortunately std didn't have it, I can later added.  
                std::string temp = DecodefileContent.substr(length, DecodeResult[i].length);
                DecodefileContent += temp + DecodeResult[i].literal;
            }
        }
        OutputStream os("Decode-" + filename + ".LZL-zip");
        os.WriteFile(DecodefileContent);
    }

    void LZ77::LZ_encode(){
        const std::string& FileContent = FileStream.ReadFile();
        std::cout << "File size : " << FileContent.size() << std::endl;
        DecodefileContent.resize(FileContent.size() + 1);
        encoding(FileContent);
    }

    //so slow.
    void LZ77::encoding(const std::string_view& file){
        int window1 = 0, window2 = 0;
        for(int i = 0; i < file.length(); i++) {
            if(i + 1 <= maxWindow) window1 = i;
            else window1 = maxWindow;
            
            if(i + window1 < file.length()) window2 = window1;  
            else window2 = file.length() - i;
            
            const std::string_view& str1 = file.substr(i - window1, window1);
            const std::string_view& str2 = file.substr(i, window2);
            
            int distance = -1;
            while(true) {
                if(window2 == 0) break;
                const std::string_view& str3 = str2.substr(0, window2);
                distance = str1.find(str3);
                if(distance != -1) break; 
                window2--;
                if(window2 <= 0) break;
            }

            if(distance != -1) {
                CodeNode temp;
                temp.length = window2;
                temp.distance = window1 - distance;
                temp.literal = file[i + window2];
                NodeQueue.push_back(std::move(temp));
                i += window2;
            } else {
                CodeNode temp;
                temp.length = 0;
                temp.distance = 0;
                temp.literal = file[i + window2];
                NodeQueue.push_back(std::move(temp));
            }
        }
    }
}


#endif