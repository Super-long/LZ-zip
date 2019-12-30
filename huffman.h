#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include"LZ77.h"
#include<unordered_map>
#include<unordered_set>
#include<queue>

#include<iostream>

//回去看蓝书编译依赖性
namespace LZ_zip{
    //TODO 类太大了 不好 为了更好的压缩比 牺牲时间
    class LZ77; //编译依赖 厉害了
    class Huffman{
        using pas = std::pair<int,int>;
        private:
            int index = 32768;
            //用到的index不多 可以考虑离散化
            std::vector<int> HuffmanTree[65537];
            int root;
            std::unordered_map<int, int> distance; //不放short的原因是建树的过程会有节点超过short范围
            std::unordered_map<int, int> length;
            std::unordered_map<char, int> literal;
            std::priority_queue<pas, std::vector<pas>, std::greater<pas> > QueOfDistance;

            std::unordered_map<int, std::string> distance_code;
            std::unordered_set<std::string> distance_set;

            void initial(const LZ77& LZ);
            bool operator=(const Huffman&) = delete;
            void encoding();
            void dfs(int Root, const std::string str);
        public:
            Huffman() = default;
            void encode(const LZ77& LZ){
                initial(LZ);
                encoding(); //得到树
                dfs(root, "");//得到编码
            }

            void Decode(){

            }

            void show() {
                for(const auto&[number, code] : distance_code){
                    std::cout << number << " :" << code << std::endl;
                } 
                int ans = 0;
                for(const auto&[number, code] : distance_code){
                    ans += code.length() * distance[number];
                }
                cout << "压缩后的distance : " << ans << endl;
            }
    };

    void Huffman::initial(const LZ77& LZ){//TODO:不能直接使用vector 查下原因
        auto code = LZ.code(); //出来就是引用
        for(auto[distance_, length_, literal_] : code){
            distance[distance_]++;
            length[length_]++;
            literal[literal_]++;
        }
        for(auto x : distance){
            std::cout << "code : " << x.first << " number : " << x.second << std::endl;
        }
    }

    //更慢了
    void Huffman::dfs(int Root, const std::string str){
        if(HuffmanTree[Root].empty()){
            distance_code[Root] = std::move(str);
            
            return;
        }
        dfs(HuffmanTree[Root][0], str + '0');
        dfs(HuffmanTree[Root][1], str + '1');
    }

    void Huffman::encoding(){
        for(const auto&[distance_, number_] : distance){
            QueOfDistance.push(std::make_pair(number_, distance_));
        }
        while(QueOfDistance.size() != 1){
            pas para1 = QueOfDistance.top();
            QueOfDistance.pop();
            pas para2 = QueOfDistance.top();
            QueOfDistance.pop();
            pas cur = std::make_pair(para1.first + para2.first, ++index);
            QueOfDistance.push(cur);
            HuffmanTree[index].emplace_back(std::move(para1.second));
            HuffmanTree[index].emplace_back(std::move(para2.second));
        }
        root = QueOfDistance.top().second;
    }
}


#endif