#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "cppjieba/Jieba.hpp"

namespace ust
{
    class FileTools
    {
    public:
        static bool Read(const std::string& file_path, std::string& out)
        {
            std::ifstream ifs(file_path, std::ios::in);
            if (!ifs.is_open())
            {
                std::cerr << "open file " << file_path << "failed" << std::endl;
                return false;
            }
            std::string line;
            // 利用operator bool
            while (std::getline(ifs, line))
            {
                out += line;
            }
            ifs.close();
            return true;
        }
    };
    class StringTools
    {
    public:
        static void CutString(const std::string& str, std::vector<std::string>& results, const std::string& sep)
        {
            // boost库中的split函数
            boost::split(results, str, boost::is_any_of(sep), boost::token_compress_on);
        }
    };
    // 仿照demo.cpp 建立分词工具
    // 词库
    const char *const DICT_PATH = "./dict/jieba.dict.utf8";
    const char *const HMM_PATH = "./dict/hmm_model.utf8";
    const char *const USER_DICT_PATH = "./dict/user.dict.utf8";
    const char *const IDF_PATH = "./dict/idf.utf8";
    const char *const STOP_WORD_PATH = "./dict/stop_words.utf8";
    class JiebaTools
    {
    private:
        // 为了防止每次都要重复建立 搞成static
        static cppjieba::Jieba jieba;
    public:
        static void CutString(const std::string& src, std::vector<std::string>& out)
        {
            jieba.CutForSearch(src, out);
        }
    };
    cppjieba::Jieba JiebaTools::jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
}