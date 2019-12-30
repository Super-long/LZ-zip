#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include"LZ77.h"
#include<unordered_map>
#include<unordered_set>
#include<functional>
#include<bitset>
#include<queue>

#include<iostream>

//回去看蓝书编译依赖性
namespace LZ_zip{
    //TODO 类太大了 不好 为了更好的压缩比 牺牲时间
    class LZ77;
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

            std::unordered_map<int, std::string> distance_code;
            std::unordered_map<std::string, short> distance_number;
            std::unordered_map<int, std::string> length_code;
            std::unordered_map<std::string, short> length_number;
            std::unordered_map<int, std::string> literal_code;
            std::unordered_map<std::string, short> literal_number;

            template
            <typename key, typename value, typename target_>
            void encoding(target_ mp, int index);
            void Dencoding(const std::string_view&);
            void initial(const LZ77& LZ);
            void WritingFile();

            void dfs_distance(int Root, std::string&& str);
            void dfs_length(int Root, std::string&& str);
            void dfs_literal(int Root, std::string&& str);
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

                //移位压到文件中
            }


            //先写入文件 需要写成字节
            void Decode(){
                //从文件中得到一个string串
                //译码 一个数字对应所对应项数的字节数 但实际读取八的倍数 因为有补零
                std::string filecontent = ""//01串
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
/*                 cout << "literal : \n";
                for(const auto&[number, code] : literal_code){
                    std::cout << number << " :" << code << std::endl;
                }  */
                int three = 0;
                for(const auto&[number, code] : literal_code){
                    three += code.length() * literal[number];
                }
                cout << "压缩后的distance : " << ans << endl; //位数            
                cout << "压缩后的length : " << two << endl; //位数
                cout << "压缩后的literal : " << three << endl; //位数
            }
    };

    /**
     * @ 写入策略为每一项前面加一个数字 32位, 记录此项的总字节数, 最后补齐取余8
     * @ 每一项写到一起的原因为局部性
    */
    void Huffman::WritingFile(){
        OutputStream os("Temp.LZ-zip");
        //TODO 早上任务 :写入字节 然后测Dencoding函数 可以用bitsit测试

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
        cout << "root : " << root<< endl;
    }

    //TODO:设置为友元类不能直接使用vector 查下原因
    void Huffman::initial(const LZ77& LZ){
        auto code = LZ.code();
        for(const auto&[distance_, length_, literal_] : code){
            distance[distance_]++;
            length[length_]++;
            literal[literal_]++;
        }
        num_entry = code.size();
        DecodeResult.reserve(code.size() + 1);
    }

    //TODO 代码写的复用差 功能实现以后改一手
    void Huffman::Dencoding(const std::string_view& filecontent){
        size_t pos = 32;
        size_t n = 8;
        size_t Inedx = 32;
        std::string str;
        size_t distance_length  = ConStrNumber(filecontent.substr(pos, 32));
        size_t num = 0;
        pos += distance_length;
        //distance 应该写入成功
        while(Index < distance_length){
            str.push_back(filecontent[Index++]);
            if(distance_number.find(str) != distance_number.end()){
                DecodeResult[num++].distance = distance_number[str];
                str.clear();
            }
        }
        
        //length 应该写入成功
        num = 0;
        size_t length_length = ConStrNumber(filecontent.substr(pos, 32));
        pos += length_length;
        while(Index < length_length){
            str.push_back(filecontent[Index++]);
            if(length_number.find(str) != length_number.end()){
                DecodeResult[num++].length = length_number[str];
                str.clear();
            }
        }

        num = 0;
        size_t literal_length = ConStrNumber(filecontent.substr(pos, 32));
        while(Index < literal_length){
            str.push_back(filecontent[Index++]);
            if(literal_number.find(str) != literal_number.end()){
                DecodeResult[num++].literal = literal_number[str];
                str.clear();
            }
        }
        cout << "解析出来的项数 : " << num << endl;
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

    size_t ConStrNumber(const std::string_view& str){
        bitset<32> number(str, 0, 32);
        return number.to_ulong();
    }

}

#endif