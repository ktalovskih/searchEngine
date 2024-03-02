# searchEngine
# How to Build on Linux
mkdir build_examples 

cmake -S . -B build_examples 

sudo cmake --build build_examples --target install 

cd build_examples/SearchEngine 

./SearchEngine

# How to Build on Windows

mkdir build_examples 

cmake -S . -B build_examples

cmake --build build_examples --target ALL_BUILD 

##
# An Example of Output

In the program window
##
Processing file: C:/Users/whisper/Desktop/2/CMakeProject1/file001.txt

Processing file: C:/Users/whisper/Desktop/2/CMakeProject1/file002.txt

Processing file: C:/Users/whisper/Desktop/2/CMakeProject1/file003.txt

Processing file: C:/Users/whisper/Desktop/2/CMakeProject1/file004.txt

req: milk water

req: sugar

DocID: 2 rank: 1

DocID: 0 rank: 0.7

DocID: 1 rank: 0.3

JSON data has been written to answer.JSON

JSON data has been written to answer.JSON

Press Enter to exit

##
In the anwers.JSON



    "answers": {
        "request1": [
            {
                "relevance": [
                    {
                        "docid": 2,
                        "rank": 1.0
                    },
                    {
                        "docid": 0,
                        "rank": 0.699999988079071
                    },
                    {
                        "docid": 1,
                        "rank": 0.30000001192092896
                    }
                ],
                "result": true
            }
        ],
        "request2": [
            {
                "relevance": [],
                "result": false
            }
        ]
    }

