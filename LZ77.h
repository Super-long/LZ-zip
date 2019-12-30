#ifndef _LZ77_H_
#define _LZ77_H_

#include<fstream>
#include<vector>
#include<string>
#include<string_view>
#include"stream.h"
#include"huffman.h"
//它的基本假设是，如果一个字符串要重复，那么也是在附近重复，远的地方就不用找了
//适合于有大量的局部重复的子串

//策略 
/**
 * @ 
*/

namespace LZ_zip{
    //TODO 记得删除
    using std::cout;
    using std::endl;
    //可把文件解为三元组
    class LZ77{
        friend class Huffman;//使用其中的CodeNode
        
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
            InputStream FileStream;
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
            explicit LZ77(std::string str) : FileStream(str){}
            
            void ExchangeWindow(short win) noexcept{
                maxWindow = win;
            }

            void show() const{
                using std::cout; using std::endl;
                std::cout << "结点项数 : " << NodeQueue.size() << std::endl;
                for(auto T : NodeQueue){
                    cout << T.distance << " " <<T.length << " " << T.literal << endl;
                }
                cout << DecodefileContent << endl;
            }

            void Encode() {LZ_encode();}
            void Decode() {LZ_decode();}
            void ExchangeFilename(std::string str){
                NodeQueue.clear();//想想到底数据需要清空吗
                FileStream.ExchangeOpenFile(str);
            }

    };
    void LZ77::LZ_decode(){
        for(int i = 0; i < NodeQueue.size(); i++) {
            if(NodeQueue[i].length == 0) {
                DecodefileContent += NodeQueue[i].literal;
            } else {
                int length = DecodefileContent.length();
                length -= NodeQueue[i].distance;
                //TODO 涉及大量字符串的拼接 这里用右值更好 可惜库里没有 可以自己后面加上
                std::string temp = DecodefileContent.substr(length, NodeQueue[i].length);
                DecodefileContent += temp + NodeQueue[i].literal;
            }
        }
    }

    void LZ77::LZ_encode(){
        //TODO 这里消耗资源太大
        std::string FileContent = FileStream.ReadFile();
        cout << "文件大小 : " << FileContent.size() << endl;
        DecodefileContent.resize(FileContent.size() + 1);
        encoding(FileContent);
    }

    //太慢, 必须优化
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