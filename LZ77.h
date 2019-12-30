#ifndef _LZ77_H_
#define _LZ77_H_

#include<fstream>
#include<vector>
#include<string>
#include<string_view>
#include"stream.h"
//它的基本假设是，如果一个字符串要重复，那么也是在附近重复，远的地方就不用找了
//适合于有大量的局部重复的子串

//策略 
/**
 * @ 
*/
namespace LZ_zip{
    using std::cout;
    using std::endl;
    class LZ77{
        private:
            struct CodeNode
            {
                int off;
                int len;
                char nextchar;
            };
        private:
            //forbid initialization no-const static member.
            short maxWindow = 32767;
            std::vector<CodeNode> NodeQueue;
            Stream FileStream;
            long long Filelength;
            std::string DecodefileContent;
        private:
            void LZ_encode();
            void encoding(const std::string_view& file);
            void LZ_decode();
        public:
            LZ77() = default;
            explicit LZ77(std::string str) : FileStream(str),Filelength(){}
            
            void ExchangeWindow(short win) noexcept{
                maxWindow = win;
            }

            void show() const{
                using std::cout; using std::endl;
                std::cout << "all " << NodeQueue.size() << std::endl;
                for(auto T : NodeQueue){
                    cout << T.off << " " <<T.len << " " << T.nextchar << endl;
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
            if(NodeQueue[i].len == 0) {
                DecodefileContent += NodeQueue[i].nextchar;
            } else {
                int len = DecodefileContent.length();
                len -= NodeQueue[i].off;
                //这里用右值更好 可惜库里没有 可以自己后面加上
                std::string temp = DecodefileContent.substr(len, NodeQueue[i].len);
                DecodefileContent += temp + NodeQueue[i].nextchar;
            }//涉及大量字符串的拼接
        }
    }

    void LZ77::LZ_encode(){
        //这里消耗资源太大
        std::string FileContent = FileStream.ReadFile();
        DecodefileContent.resize(FileContent.size() + 1);
        std::cout << "this is file content.\n";
        std::cout << FileContent << std::endl;
        encoding(FileContent);
    }

    void LZ77::encoding(const std::string_view& file){
        int window1 = 0, window2 = 0;
        for(int i = 0; i < file.length(); i++) {
            if(i + 1 <= maxWindow) window1 = i; //确定查询窗口的尺寸 
            else window1 = maxWindow;
            
            if(i + window1 < file.length()) window2 = window1;  
            else window2 = file.length() - i;
            
            const std::string_view& str1 = file.substr(i - window1, window1);
            const std::string_view& str2 = file.substr(i, window2);
            
            int off = -1;
            while(true) {
                if(window2 == 0) break;
                const std::string_view& str3 = str2.substr(0, window2);
                off = str1.find(str3);
                if(off != -1) break; 
                window2--;
                if(window2 <= 0) break;
            }

            if(off != -1) {
                CodeNode cd;
                cd.len = window2;
                cd.off = window1 - off;
                cd.nextchar = file[i + window2];
                NodeQueue.push_back(std::move(cd));
                i += window2;
            } else {
                CodeNode cd;
                cd.len = 0;
                cd.off = 0;
                cd.nextchar = file[i + window2];
                NodeQueue.push_back(std::move(cd));
            }
        }
    }
}


#endif