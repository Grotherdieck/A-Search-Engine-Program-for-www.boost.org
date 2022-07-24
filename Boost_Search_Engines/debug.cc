#include "searcher.hpp"

const std::string input = "/home/Router/Boost_Search_Engines/data/output/out.txt";

int main()
{
    // for test
    ns_searcher::Searcher* searcher = new ns_searcher::Searcher();
    searcher->InitSearch(input);
    std::string query;
    std::string jsonstring;
    std::ofstream ofs("debug.txt");
    while (1)
    {
        std::cout << "Please Enter Your Search Query# ";
        std::getline(std::cin, query);
        searcher->Search(query, jsonstring);
        ofs << jsonstring << std::endl;
    }
    
    return 0;
}