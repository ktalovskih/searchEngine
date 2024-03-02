#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <gtest/gtest.h>
#include "thread"
#include "mutex"
#include <set>
#include <cmath>
#include <algorithm>
#include "Converter.h"

std::mutex m;
struct Entry {
    size_t doc_id, count;

    bool operator ==(const Entry& other) const {
        return (doc_id == other.doc_id &&
            count == other.count);
    }
};
class InvertedIndex {
    std::vector<std::string> docs;
    size_t i = 0;
    std::map<std::string, std::vector<Entry>> freq_dictionary;
    size_t count = 0;
public:
    std::set<std::string> allWords;
    InvertedIndex() = default;
    
    void words() {
        for (auto& doc : docs) {
            std::istringstream iss(doc);
            std::string word;
            while (iss >> word) {
                allWords.insert(word);
            }
        }
    }
    void UpdateDocumentBase(std::vector<std::string> inputDocs) {
        docs = inputDocs;
        words();
    }
   
    std::vector<Entry> GetWordCount(const std::string& word) {
        m.lock();
        std::vector<Entry> result;
        i = 0;
        count = 0;
        for (const auto& d : docs) {
            count = 0;
            std::istringstream iss(d);
            std::string buffer;
            while (iss >> buffer) {
                
                  if (buffer == word) {
                    count++;
                  }
            }
            if (!( count == 0)) {
                freq_dictionary[word].push_back({ i, count });
                result.push_back({ i, count });
            }
            
            i++;
            
        }
        m.unlock();
        return result;
    }
    friend class SearchServer;
};



struct RelativeIndex {
    size_t docId;
    float rank;

    bool operator ==(const RelativeIndex& other) const {
        return (docId == other.docId && rank == other.rank);
    }
};
class SearchServer {
    float maxCount = 0.0;
    std::vector<std::vector<std::pair<size_t, float>>> answers;
    std::vector<std::vector<RelativeIndex>> result;
    std::vector<std::string> uniqueWords;
    std::vector<RelativeIndex> rankEntries;
    std::map<size_t, int> wordCountMap;
public:
    SearchServer(InvertedIndex& idx, ConverterJSON& converter) : _index(idx), _converter(converter){ };
    

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input, int _maxRes)
    {
        answers.clear();
        rankEntries.clear();
        wordCountMap.clear();
        uniqueWords.clear();
        result.clear();
        for (auto& query : queries_input) {
            std::istringstream iss(query);
            std::string word;
            while (iss >> word) {
                uniqueWords.push_back(word);
            }


        }
        for (auto& uniqueWord : uniqueWords) {
            int sum = 0;
            auto it = _index.freq_dictionary.find(uniqueWord);
            if (it != _index.freq_dictionary.end()) {
                std::vector<Entry>& temp = it->second;

                for (const Entry& entry : temp) {
                    
                    wordCountMap[entry.doc_id] += entry.count;
                    

                }
                
            }
        }
        for (auto& wordCount : wordCountMap) {
            if (wordCount.second > maxCount) {
                maxCount = wordCount.second;
            }
        
        }
       
       for (auto& wordCount : wordCountMap) {
           
           float r = wordCount.second / maxCount;
           RelativeIndex temp{ wordCount.first, r };
           rankEntries.push_back(temp);
           
       }

       for (int i = 0; i < rankEntries.size(); i++) {
           for (int j = 0; j < rankEntries.size() - i - 1; j++) {
               if (rankEntries[j].rank < rankEntries[j + 1].rank) {
                   std::swap(rankEntries[j].rank, rankEntries[j + 1].rank);
                   std::swap(rankEntries[j].docId, rankEntries[j + 1].docId);
               }
           }
       }
       if (_maxRes >= 0 && _maxRes < rankEntries.size()) {
           
           rankEntries.erase(rankEntries.begin() + _maxRes, rankEntries.end());
       }
      
       
       result.emplace_back(rankEntries);
       answers.emplace_back();
       
       for (auto& rankEntry: rankEntries) {
           std::cout <<"DocID: " << rankEntry.docId << " rank: " << rankEntry.rank << std::endl;
           answers.back().emplace_back(std::make_pair(rankEntry.docId, rankEntry.rank));

           
       }

       _converter.putAnswers(answers);

       return result;
        
    }
private:
    InvertedIndex _index;
    ConverterJSON _converter;
};


//testing
bool areVectorsEqual(const std::vector<RelativeIndex>& a, const std::vector<RelativeIndex>& b, double tolerance) {
    if (a.size() != b.size()) {
        
        return false;
    }
    
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::abs(a[i].rank - b[i].rank) > tolerance || a[i].docId != b[i].docId) {

            
            return false;
        }
    }
    
    return true;
}



using namespace std;
void TestInvertedIndexFunctionality(
    const vector<string>& docs,
    const vector<string>& requests,
    const std::vector<vector<Entry>>& expected
) {
    std::vector<std::vector<Entry>> result;
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    for (auto& request : requests) {
        std::vector<Entry> word_count = idx.GetWordCount(request);
        result.push_back(word_count);
    }
    ASSERT_EQ(result, expected);
}
TEST(TestCaseInvertedIndex, TestBasic) {
    const vector<string> docs = {
    "london is the capital of great britain",
    "big ben is the nickname for the Great bell of the striking clock"
    };
    const vector<string> requests = { "london", "the" };
    const vector<vector<Entry>> expected = {
    {
    {0, 1}
    }, {
    {0, 1}, {1, 3}
    }
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}
TEST(TestCaseInvertedIndex, TestBasic2) {
    const vector<string> docs = {
    "milk milk milk milk water water water",
    "milk water water",
    "milk milk milk milk milk water water water water water",
    "americano cappuchino"
    };
    const vector<string> requests = { "milk", "water", "cappuchino" };
    const vector<vector<Entry>> expected = {
    {
    {0, 4}, {1, 1}, {2, 5}
    }, {
    {0, 3}, {1, 2}, {2, 5}
    }, {
    {3, 1}
    }
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}
TEST(TestCaseInvertedIndex, TestInvertedIndexMissingWord) {
    const vector<string> docs = {
    "a b c d e f g h i j k l",
    "statement"
    };
    const vector<string> requests = { "m", "statement" };
    const vector<vector<Entry>> expected = {
    {
    }, {
    {1, 1}
    }
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}
//searchTest
//

TEST(TestCaseSearchServer, TestSimple) {
    const vector<string> docs = {
    "milk milk milk milk water water water",
    "milk water water",
    "milk milk milk milk milk water water water water water",
    "americano cappuccino"
    };

    const vector<string> request = { "milk water", "sugar" };
    const std::vector<vector<RelativeIndex>> expected = {
    { {2, 1}, {0, 0.7}, {1, 0.3} } };
    InvertedIndex idx;
    
    idx.UpdateDocumentBase(docs);
    for (auto& word : idx.allWords)
    {
        idx.GetWordCount(word);
    }
    ConverterJSON converter;
    SearchServer srv(idx, converter);
    std::vector<vector<RelativeIndex>> result = srv.search(request,5);
    EXPECT_TRUE(areVectorsEqual(result[0], expected[0], 1e-6));
}
TEST(TestCaseSearchServer, TestTop5) {
    const vector<string> docs = {
    "london is the capital of great britain",
    "paris is the capital of france",
    "berlin is the capital of germany",
    "rome is the capital of italy",
    "madrid is the capital of spain",
    "lisboa is the capital of portugal",
    "bern is the capital of switzerland",
    "moscow is the capital of russia",
    "kiev is the capital of ukraine",
    "minsk is the capital of belarus",
    "astana is the capital of kazakhstan",
    "beijing is the capital of china",
    "tokyo is the capital of japan",
    "bangkok is the capital of thailand",
    "welcome to moscow the capital of russia the third rome",
    "amsterdam is the capital of netherlands",
    "helsinki is the capital of finland",
    "oslo is the capital of norway",
    "stockholm is the capital of sweden",
    "riga is the capital of latvia",
    "tallinn is the capital of estonia",
    "warsaw is the capital of poland",
    };
    const vector<string> request = { "moscow is the capital of russia" };
    const std::vector<vector<RelativeIndex>> expected = {
    {
    {7, 1},
    {14, 1},
    {0,0.6666666865348816},
    {1, 0.6666666865348816},
    {2, 0.6666666865348816}
    }
    };
    InvertedIndex idx;
    ConverterJSON converter;
    idx.UpdateDocumentBase(docs);
    for (auto& word : idx.allWords)
    {
        idx.GetWordCount(word);
    }
    SearchServer srv(idx, converter);
    std::vector<vector<RelativeIndex>> result = srv.search(request,5);
    EXPECT_TRUE(areVectorsEqual(result[0], expected[0], 1e-6));
}