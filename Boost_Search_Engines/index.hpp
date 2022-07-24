#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <mutex>
#include "usetools.hpp"
#include "log.hpp"

namespace ns_index
{
    
    // 存储文档信息的结构体
    struct DocInfo
    {
        std::string title;  // 文档标题
        std::string content;// 去标签后的文档内容
        std::string url;    // 文档官网
        uint64_t doc_id;    // 文档id 
    };
    // 存储倒排映射到的节点的结构体
    struct InvertElem
    {
        uint64_t doc_id;    // 文档id
        std::string keyword;// 关键字
        int32_t weight;     // 权重
    };
    // 倒排索引所引导的"拉链“
    typedef std::vector<InvertElem> InvertedList;
    // 索引类
    class Index
    {
    private:
        Index() {}
        Index(const Index&) = delete;
        Index& operator=(const Index&) = delete;
        static Index* instance;
        static std::mutex mtx;
    public:
        // 获取单例
        static Index* GetInstance()
        {
            if (instance == nullptr)
            {
                std::lock_guard<std::mutex> lg(mtx);
                if (instance == nullptr)
                {
                    instance = new Index();
                }
            }
            return instance;
        }
    private:
        // 正排索引是用id找文档内容，所以我们选择数组作为它的数据结构
        // 可以直接从数组下标映射到文档内容
        std::vector<DocInfo> forward_index;
        // 一个关键字可能和多个文档关联 所以用哈希从字符串映射到一个数组(倒排拉链)
        std::unordered_map<std::string, InvertedList> inverted_index;
    public:
        // 构造和析构
        ~Index() {};
    public:
        // 正排序索引提供根据文档id获得文档信息的结构体的地址(因为它太大了)
        DocInfo* GetForwardIndex(const uint64_t& doc_id)
        {
            if (doc_id >= forward_index.size())
            {
                // std::cerr << "doc_id out of range." << std::endl;
                LOG(WARNING, "doc_id : " + std::to_string(doc_id) + "out of range.");
                return nullptr;
            }
            return &forward_index[doc_id];
        }
        // 根据关键字string返回倒排拉链
        InvertedList* GetInvertedIndex(const std::string& keyword)
        {
            if (inverted_index.count(keyword) != 0)
                return &(inverted_index[keyword]);
            // std::cerr << keyword << " has no InvertedList" << std::endl;
            LOG(NORMAL, keyword + "关键词在倒排索引中不存在...");
            return nullptr;
        }
        // 根据去标签格式化后的文档构建正排和倒排索引
        // data/output/out.txt
    public:
        bool BuildIndex(const std::string& ParseredFilePath)
        {
            std::ifstream ifs(ParseredFilePath, std::ios::in | std::ios::binary);
            if (!ifs.is_open())
            {
                // std::cerr << "sorry, " << ParseredFilePath << " open error." << std::endl;
                LOG(FATAL, "Sorry, " + ParseredFilePath + " open error.");
                return false;
            }

            // 根据我们前面写入的规则 可以按行读
            std::string line;
            int count = 0;
            while (std::getline(ifs, line))
            {
                // 建立正排索引 直接读出来然后push到forward_index里头去就行了
                // 因为建立倒排索引需要文件的信息 所以返回值设为DocInfo*
                // std::cout << line << std::endl;
                DocInfo* pdoc = BuildForwardIndex(line);
                if (pdoc == nullptr)
                {
                    // std::cerr << "Build forward index error" << std::endl;
                    LOG(WARNING, std::to_string(forward_index.size()) + ": build forward index error");
                    continue;
                }
                // std::string check(' ', pdoc->title.size());
                // if (pdoc->title.empty() || pdoc->title == check);
                // {
                //     std::cout << "error title empty!, doc id : " << pdoc->doc_id << std::endl;
                    
                //     exit(1);
                // }
                // std::cout << pdoc->title << std::endl;
                if (!BuildInvertedIndex(*pdoc))
                {
                    // std::cerr << "Build " << pdoc->title << " inverted index error" << std::endl;
                    LOG(WARNING, std::to_string(pdoc->doc_id) + ": build inverted index error.");
                    continue;
                }
                ++count;
                if (count % 50 == 0) 
                    // std::cout << "当前已经建立的索引文档数目: " << count << std::endl;
                    LOG(NORMAL, "当前已经建立的索引文档数目: " + std::to_string(count)); 
            }
            if (inverted_index.empty() || forward_index.empty())
            {
                LOG(FATAL, "index empty, can't work");
                return false;
            }
            return true;
        }
    private:
        // 建立正排索引
        DocInfo* BuildForwardIndex(const std::string& file)
        {
            // version1:手写分割字符串
            // // file:title\3content\3url
            // // 1.分割字符串并填充doc
            // size_t pos1 = file.find('\3');
            // if (pos1 == std::string::npos)
            //     return nullptr;
            // DocInfo doc;
            // doc.title = file.substr(0, pos1);
            // size_t pos2 = file.find('\3', pos1);
            // if (pos2 == std::string::npos)
            //     return nullptr; 
            // doc.content = file.substr(pos1 + 1, pos2 - pos1 - 1);
            // doc.url = file.substr(pos2 + 1);
            // doc.doc_id = forward_index.size();
            // //2.将doc插入forward_index中
            // forward_index.push_back(std::move(doc));
            // return &(forward_index.back());
            // 利用boost库
            std::vector<std::string> results;
            const std::string sep = "\3";
            ust::StringTools::CutString(file, results, sep);
            if (results.size() != 3)
            {
                LOG(WARNING, "Cut file : " + std::to_string(forward_index.size()) + "error, it doesn't have three \\3.");
                return nullptr;
            }
            DocInfo doc;
            doc.url = std::move(results.back());
            results.pop_back();
            doc.content = std::move(results.back());
            results.pop_back();
            doc.title = std::move(results.back());
            results.pop_back();
            doc.doc_id = forward_index.size();
            forward_index.push_back(std::move(doc));
            return &(forward_index.back());
        }
        // 建立倒排索引
        bool BuildInvertedIndex(const DocInfo& doc)
        {
            // DocInfo {title content url doc_id}
            // 最终是要建立word->倒排拉链的映射
            // 1. 分词
            std::vector<std::string> title_words;
            ust::JiebaTools::CutString(doc.title, title_words);
            std::vector<std::string> content_words;
            ust::JiebaTools::CutString(doc.content, content_words);
            //2. 词频统计
            struct word_cnt
            {
                int32_t title_cnt;
                int32_t content_cnt;
                word_cnt() : title_cnt(0), content_cnt(0) {}
            };
            std::unordered_map<std::string, word_cnt> cntmp;
            for (auto wd : title_words)
            {
                boost::to_lower(wd);
                ++cntmp[std::move(wd)].title_cnt;
            }
            for (auto wd : content_words)
            {
                boost::to_lower(wd);
                ++cntmp[std::move(wd)].content_cnt;
            }
            // if (doc.doc_id == 701)
            // {
            //     for (auto&& wd : title_words) std::cout << wd << ' ';
            //     for (auto&& wd : content_words) std::cout << wd << ' ';
            //     std::cout << std::endl;
            //     for (auto&& p : cntmp)
            //     {
            //         std::cout << p.first << ' ' << p.second.title_cnt << ' ' << p.second.content_cnt << std::endl;
            //     }
            // }
            //3.定义相关性并构造倒排索引
            // InvertElem { doc_id keyword weight }
            uint64_t docid = doc.doc_id;
            const int vtitle = 10;
            const int vcontent = 1;
            for (auto&& wd_cnt : cntmp)
            {
                InvertElem elem;
                elem.doc_id = docid;
                elem.keyword = wd_cnt.first;
                elem.weight = vtitle * wd_cnt.second.title_cnt + vcontent * wd_cnt.second.content_cnt;
                inverted_index[wd_cnt.first].push_back(std::move(elem));
            }
            return true;
        }
    };
    Index* Index::instance = nullptr;
    std::mutex Index::mtx;
}