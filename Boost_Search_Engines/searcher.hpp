#include "index.hpp"
#include <algorithm>
#include "jsoncpp/json/json.h"

namespace ns_searcher
{
    // 倒排查找后处理完的节点
    // 保证一个结点只映射一个网页
    struct InvertedNode
    {
        uint64_t doc_id;
        int32_t weight;
        std::vector<std::string> keywords;
        InvertedNode() : doc_id(0), weight(0) {}
    };
    class Searcher
    {
    public:
        Searcher() {}
        ~Searcher() {}
    private:
        ns_index::Index* index;// 供我们进行查找的索引
    public:
        void InitSearch(const std::string& input)
        {
            //1.获取或创建index对象
            index = ns_index::Index::GetInstance();
            // std::cout << "获取index单例成功..." << std::endl;
            LOG(NORMAL, "获取index单例成功...");
            //2.根据index对象建立索引
            if (index->BuildIndex(input))
                // std::cout << "建立正排索引和倒排索引成功..." << std::endl;
                LOG(NORMAL, "建立正排索引和倒排索引成功");
            else 
            {
                LOG(FATAL, "建立索引失败,进程结束...");
                exit(5);
            }
        }
        // 提供搜索功能
        // 第一个参数表示搜索关键字 json_string是返回给用户的搜索结果。
        void Search(const std::string& query, std::string& json_string)
        {
            std::vector<std::string> keywords;
            //1.分词:用户的搜索内容首先进行分词
            ust::JiebaTools::CutString(query, keywords);
            //2.触发:根据分词的各个词进行index查找
            // 设计一个结构 把这次倒排搜索的处理结果都存起来
            std::vector<InvertedNode> InvertedNodeVec;
            std::unordered_map<uint64_t, InvertedNode> processmp;
            for (auto wd : keywords)
            {
                // 建立索引时是忽略大小写的，搜索时也要
                boost::to_lower(wd);
                // 怎么查呢？ 先查倒排获得一个倒排拉链
                auto* inverted_list = index->GetInvertedIndex(wd);
                if (inverted_list == nullptr)
                {
                    // 如果关键字在倒排索引中没有 那说明我们加载的文档里头都没有 那只能看下一个词了
                    continue;
                }
                // 遍历搜索出来的倒排索引
                for (auto&& elem : *inverted_list)
                {
                    auto& item = processmp[elem.doc_id];
                    item.doc_id = elem.doc_id;
                    item.keywords.push_back(elem.keyword);
                    item.weight += elem.weight;
                }
            }
            // 在遍历这个processmp 把结点都整到InvertedNodeVec里头
            for (auto& pi : processmp)
            {
                InvertedNodeVec.push_back(std::move(pi.second));
            }
            //3.合并排序:汇总查找结果，按照相关性(weight)进行降序排序
            std::sort(InvertedNodeVec.begin(), InvertedNodeVec.end(), [](const InvertedNode& e1, const InvertedNode& e2)
            {
                return e1.weight > e2.weight;
            });
            //4.构建，构建json串(jsoncpp)
            Json::Value root;
            for (auto&& item : InvertedNodeVec)
            {
                ns_index::DocInfo* docinfo = index->GetForwardIndex(item.doc_id);
                if (docinfo == nullptr) continue;
                // jsoncpp是用来完成序列化和反序列化的
                Json::Value cur;
                cur["title"] = docinfo->title;
                // id和权值just for debug
                cur["id"] = (int)docinfo->doc_id;
                cur["url"] = docinfo->url;
                cur["weight"] = item.weight;
                // GetDesc
                //找到word在html_content中的首次出现，
                // 然后往前找50字节(如果没有，从begin开始)，往后找100字节(如果没有，到end就可以的)
                //截取出这部分内容
                cur["summary"] = GetSummary(*docinfo, item.keywords[0]);
                root.append(cur);
            }
            // Json::FastWriter writer; release
            Json::StyledWriter writer;

            json_string = writer.write(root);
        }
    private:
        std::string GetSummary(const ns_index::DocInfo& doc, const std::string& word)
        {
            //1. 找到首次出现的位置
            auto iter = std::search(doc.content.begin(), doc.content.end(), word.begin(), word.end(),
            [](char x, char y){
                return tolower(x) == tolower(y);
            });
            if (iter == doc.content.end())
            {
                return doc.title;
            }
            size_t pos = iter - doc.content.begin();
            //2. 获取start位置和end位置
            const size_t prev_steps = 50;
            const size_t next_steps = 100;
            size_t start = 0;
            size_t end = doc.content.size() - 1;
            if (pos + 1 > prev_steps + start) start = pos + 1 - prev_steps;
            if (end + 1 > next_steps + pos) end = pos + next_steps - 1;
            //3. 截取子串
            if (start > end) 
            {
                LOG(WARNING, "没有截取到摘要串:" + std::to_string(doc.doc_id));
                return "None";
            }
            return doc.content.substr(start, end - start + 1);            
        }
    };
}