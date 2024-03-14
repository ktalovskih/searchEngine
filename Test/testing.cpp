#include "../SearchEngine/Index.h"
#include <gtest/gtest.h>

using namespace std;

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
    {0, 0.6666666865348816},
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

int main() {

  testing::InitGoogleTest(); RUN_ALL_TESTS();
  printf("Press Enter to exit\n");
  getchar();
  return 0;
}
