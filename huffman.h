#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include"LZ77.h"
#include<unordered_map>
#include<unordered_set>
#include<functional>
#include<bitset>
#include<queue>
#include<list>

#include<iostream>

//回去看蓝书编译依赖性
namespace LZ_zip{
    //TODO 在这个设置一个全局变量 测试writingfile函数写入的总字节数 记得删除
    size_t AllBytesInFile;


    //TODO 类太大了 不好 为了更好的压缩比 牺牲时间
    class LZ77;
    int ConStrNumber(const std::string& str);
    char ConStrchar(const std::string& str, size_t pos, size_t n);
    bool GetBit(std::stringstream& ss);
    std::string GetBytes(char ch);

    class Huffman{
        private:
            int root;
            int num_entry;
            std::vector<int> HuffmanTree[65537];

            std::vector<typename LZ77::CodeNode> DecodeResult;
            /**
             * The reason for not keeping short is parameter of map is that the  
             * process of building huffman tree will have nodes exceeding the 
             * short range.
            */
            std::unordered_map<int, int> distance; 
            std::unordered_map<int, int> length;
            std::unordered_map<unsigned short, int> literal;

            std::list<int> OrderDistance;
            std::list<int> OrderLength;
            std::list<unsigned short> OrderLiteral;

            std::unordered_map<int, std::string> distance_code;
            std::unordered_map<std::string, short> distance_number;
            std::unordered_map<int, std::string> length_code;
            std::unordered_map<std::string, short> length_number;
            std::unordered_map<int, std::string> literal_code;
            std::unordered_map<std::string, unsigned short> literal_number;

            template
            <typename key, typename value, typename target_>
            void encoding(target_ mp, int index);

            template
            <typename mp, typename Code>
            void writingfile(std::string& major, OutputStream& os, const mp& key_value, Code& code);
  
            void Dencoding(const std::string&);
            void initial(const LZ77& LZ);
            

            void dfs_distance(int Root, std::string&& str);
            void dfs_length(int Root, std::string&& str);
            void dfs_literal(int Root, std::string&& str);
            bool ZeroFill(std::string& str) const;
            void init();

            bool operator=(const Huffman&) = delete;
        public:
            Huffman() = default;
            /**
             * @ initial : Get a triple from LZ77 to initialize data.
             * @ encoding : get a huffman tree.
             * @ dfs_X : get Encoded code.
             * @ init : Initialize this vector.
            */
            void encode(const LZ77& LZ){
                initial(LZ);

                encoding<int, int, decltype(distance)>(distance, 32768);
                dfs_distance(root, "");
                init();

                encoding<int, int, decltype(length)>(length, 256);
                dfs_length(root, "");
                init();

                encoding<int, unsigned short, decltype(literal)>(literal, 256);
                dfs_literal(root, "");
                init();

                WritingFile();
                cout << "写文件的总字节数为 : " << AllBytesInFile << endl;
            }

            void WritingFile();  //TODO 测试完改成私有

/*             void Decode(){
                InputStream input("Temp.LZ-zip");
                std::string str = input.ReadFile();
                std::stringstream ss(str);
                std::string filecontent;//保证这个是一个01串
                //底下丢字节了
                int count = 0;
                while(ss){
                    GetBit(ss) ? filecontent.push_back('1') 
                               : filecontent.push_back('0');
                    count++;
                }

                cout << "循环 : " << count << endl;
                for(int i=0;i<8;++i) 
                    filecontent.push_back(GetBit(ss));
                cout << "读取到的位数 : " << filecontent.size() << endl;
                Dencoding(filecontent);
            } */

            void Decode(){
                InputStream input("Temp.LZ-zip");
                std::string str = input.ReadFile(); 
                std::string filecontent;  
                for(int i=0;i<str.size();++i){
                    filecontent.append(std::move(GetBytes(str[i])));
                }
                cout << "解析出来的位数 : " << filecontent.size() << endl;
                Dencoding(filecontent);
            }

            void show() {
/*                 for(const auto&[number, code] : distance_code){
                    std::cout << number << " :" << code << std::endl;
                }  */
                int ans = 0;
                for(const auto&[number, code] : distance_code){
                    ans += code.length() * distance[number];
                }
                int two = 0;
/*                 cout << "length : \n";
                for(const auto&[number, code] : length_code){
                    std::cout << number << " :" << code << std::endl;
                }  */
                for(const auto&[number, code] : length_code){
                    two += code.length() * length[number];
                }
                cout << "literal : \n";
                for(const auto&[number, code] : literal_code){
                    std::cout << number << " :" << code << std::endl;
                } 
                int three = 0;
                for(const auto&[number, code] : literal_code){
                    three += code.length() * literal[number];
                }
                cout << "压缩后的distance : " << ans << endl; //位数            
                cout << "压缩后的length : " << two << endl; //位数
                cout << "压缩后的literal : " << three << endl; //位数
                cout << "总位数 : " << ans+two+three << endl;
                cout << "总字节数 : " << (ans+two+three)/8 << " 余 :"<<(ans+two+three)%8 << endl;
            }
    };

    /**
     * @ 写入策略为每一项前面加一个数字 32位, 记录此项的总字节数, 最后补齐取余8
     * @ 每一项写到一起的原因为局部性
    */
    void Huffman::WritingFile(){
        OutputStream os("Temp.LZ-zip");
        std::string major;
        writingfile(major, os, OrderDistance, distance_code);
        writingfile(major, os, OrderLength, length_code);
        writingfile(major, os, OrderLiteral, literal_code);
        if (ZeroFill(major)) os.WriteFile(ConStrchar(major, 0, 8));
    }

    bool Huffman::ZeroFill(std::string& str) const {
        size_t len = 8 - str.size();
        if(len){
            while(len--) str.push_back('0');        
            return true;
        }
        else return false;
    }

    template
    <typename mp, typename Code>
    void Huffman::writingfile
    (std::string& major, OutputStream& os, const mp& key_value, Code& code){
        for(const auto&number : key_value){
            major.append(code[number]);
            while(major.size() >= 8){
                os.WriteFile(ConStrchar(major, 0, 8));
                ++AllBytesInFile;
                major.erase(0,8); //字符串这样操作效率较低
            }
        }
    }

    template
    <typename key, typename value, typename target_>
    void Huffman::encoding(target_ mp, int index){
        
        using pa = std::pair<key, value>;
        std::priority_queue<pa, std::vector<pa>, std::greater<pa>> QueOfDistance;
        
        for(const auto&[parameter_, number_] : mp){
            QueOfDistance.emplace(std::make_pair(number_, parameter_));
        }
        
        while(QueOfDistance.size() != 1){
            pa para1 = QueOfDistance.top();
            QueOfDistance.pop();
            pa para2 = QueOfDistance.top();
            QueOfDistance.pop();
            pa cur = std::make_pair(para1.first + para2.first, ++index);
            QueOfDistance.emplace(std::move(cur));
            HuffmanTree[index].emplace_back(std::move(para1.second));
            HuffmanTree[index].emplace_back(std::move(para2.second));
        }
        root = QueOfDistance.top().second;
    }

    //TODO:设置为友元类不能直接使用vector 查下原因
    void Huffman::initial(const LZ77& LZ){
        auto code = LZ.code();
        for(const auto&[distance_, length_, literal_] : code){
            distance[distance_]++;
            OrderDistance.push_back(distance_);
            length[length_]++;
            OrderLength.push_back(length_);
            literal[literal_]++;
            OrderLiteral.push_back(literal_);
            //cout << "(" << distance_ << "," << length_ << "," << literal_<<")\n";
        }
        num_entry = code.size();
        DecodeResult.reserve(code.size() + 1);
    }

    //TODO 代码写的复用差 功能实现以后改一手
    void Huffman::Dencoding(const std::string& filecontent){
        size_t pos = 32;
        size_t n = 8;
        size_t Index = 0;
        std::string str;
        
        size_t entry = 0;
        while(entry < num_entry){
            str.push_back(filecontent[Index++]);
            if(distance_number.find(str) != distance_number.end()){
                DecodeResult[entry++].distance = distance_number[str];
                str.clear();
            }
        }

        entry = 0;
        while(entry < num_entry){
            str.push_back(filecontent[Index++]);
            if(length_number.find(str) != length_number.end()){
                DecodeResult[entry++].length = length_number[str];
                str.clear();
            }
        }
        entry = 0;
        while(entry < num_entry){
            str.push_back(filecontent[Index++]);
            if(literal_number.find(str) != literal_number.end()){
                DecodeResult[entry++].literal = literal_number[str];
                str.clear();
            }
        }
    }

    void Huffman::dfs_distance(int Root, std::string&& str){
        if(HuffmanTree[Root].empty()){
            distance_number[str] = static_cast<short>(Root);
            distance_code[Root] = std::move(str);
            return;
        }
        dfs_distance(HuffmanTree[Root][0], str + '0');
        dfs_distance(HuffmanTree[Root][1], str + '1');
    }

    void Huffman::dfs_length(int Root, std::string&& str){
        if(HuffmanTree[Root].empty()){
            length_number[str] = static_cast<short>(Root);
            length_code[Root] = std::move(str);
            return;
        }
        dfs_length(HuffmanTree[Root][0], str + '0');
        dfs_length(HuffmanTree[Root][1], str + '1');
    }

    void Huffman::dfs_literal(int Root, std::string&& str){
        if(HuffmanTree[Root].empty()){
            literal_number[str] = static_cast<short>(Root);
            literal_code[Root] = std::move(str);
            return;
        }
        dfs_literal(HuffmanTree[Root][0], str + '0');
        dfs_literal(HuffmanTree[Root][1], str + '1');
    }

    void Huffman::init(){
        for(int i = 0 ;i < 65537; ++i) HuffmanTree[i].clear();
    }

    int ConStrNumber(const std::string& str){
        std::bitset<32> number(str, static_cast<size_t>(0), static_cast<size_t>(32));
        return static_cast<int>(number.to_ulong());
    }

    char ConStrchar(const std::string& str, size_t pos, size_t n){
        std::bitset<8> number(str, pos , n);
        return static_cast<char>(number.to_ulong());
    }

    //TODO : 这样会丢失字符 写完再想想
    bool GetBit(std::stringstream& ss){
        static int i = 7;
        static char Bchar = 0;
        static unsigned char bit[8] = {128, 64, 32, 16, 8, 4, 2, 1};
        ++i;
        if(i == 8){
            ss >> Bchar;
            i = 0;
        }
        return (Bchar&bit[i]);
    }
    
    std::string GetBytes(char ch){
        static unsigned char bit[8] = {128, 64, 32, 16, 8, 4, 2, 1};
        std::string str;
        for(int i=0;i<8;++i){
            ch&bit[i] ? str.push_back('1') : str.push_back('0');
        }
        return str;
    }
}

#endif