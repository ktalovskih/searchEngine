#include <vector>
#include <map>
#include <sstream>
#include "thread"
#include "mutex"
#include <set>
#include <algorithm>
#include <iostream>
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

