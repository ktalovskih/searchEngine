#include "nlohmann/json.hpp"
#include "gtest/gtest.h"
#include "Index.h"
#include <chrono>
#include <cctype>
std::mutex mute;
using namespace std;
void countWord(const std::string& word, InvertedIndex& idx) {

    idx.GetWordCount(word);
}
int main(int argc, char** argv) {
    std::vector<std::string> data;
    ConverterJSON converter;
    std::vector<Config> configs = converter.GetTextDocuments();
    InvertedIndex idx;
    std::vector<std::thread> threads;

    for (const auto& config : configs) {
        std::ifstream file(config.path);
        std::string temp = " ";

        if (!file.is_open()) {
            std::cerr << "Unable to open file: " << config.path << std::endl;
            continue;
        }
        char ch;
        while (file.get(ch)) {

            if (isalpha(ch) || ch == ' ') {
                if (ch >= 'A' && ch <= 'Z' && config.caseInsensitiveSearch) {
                    ch += 32;
                }
                temp += ch;
            }

        }

        std::cout << "Processing file: " << config.path << std::endl;
        data.push_back(temp);
        idx.UpdateDocumentBase(data);
    }

    for (const auto& word : idx.allWords) {
        threads.emplace_back(countWord, word, std::ref(idx));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto req = converter.GetRequests();
    SearchServer src(idx, converter);

    for (int i = 0; i < req.size(); i++) {
        auto request = req[i].requests;
        std::vector<std::string> temp;
        if (configs[0].caseInsensitiveSearch) {
            for (auto& r : request) {
                if (r >= 'A' && r <= 'Z') {
                    r += 32;
                }
            }
        }

        temp.push_back(request);
        src.search(temp, configs[0].maxRes);
    }
    if (argc > 1 && std::string(argv[1]) == "--run_tests") {
        std::cout << "\033[1;32m----------TESTING-----------\033[0m" << std::endl;      
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }
    printf("Press Enter to exit\n");
    getchar();
    return 0;
}
