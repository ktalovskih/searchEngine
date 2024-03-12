# Search Engine
Search engine is a console application that can search special word (requsts) in local files and return results of searching. Result includes: a number of document, relative rank, which was made by a formula.

The requsts are specified in the file requst.JSON and paths in the conjig.JSON file.

Also, in the config, besides paths, there are max responses, case-insensitive search.

The first string sets the maximum count of responses, and the second sets will the application ignores the register of letters or not.

# How to Build on Linux
mkdir build_examples 

cmake -S . -B build_examples 

cmake --build build_examples

cd build_examples/SearchEngine

./SearchEngine

# How to Build on Windows

mkdir build_examples 

cmake -G "MinGW Makefiles" -S . -B build_examples

cmake --build build_examples 

cd build_examples/SearchEngine

./SearchEngine.exe
##
# How to Run the Tests
To run test you should add after run programs files --run_tests like an agrgument. After that in the command line will be results of tests
For example: ./SearchEngine.exe --run_tests. it is a running test in the Windows.
# An Example of Output
In the program window
##
Processing file: ../../../TestData/file004.txt

Processing file: ../../../TestData/file003.txt

Processing file: ../../../TestData/file002.txt

Processing file: ../../../TestData/file001.txt

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


