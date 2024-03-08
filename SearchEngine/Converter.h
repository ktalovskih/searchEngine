#include <vector>
#include <string>
#include "nlohmann/json.hpp"
#include <filesystem> 
#include <fstream>
#include "gtest/gtest.h"
#include <filesystem>  

struct Config {
    int maxRes;
    std::string path;
    bool caseInsensitiveSearch;
};
struct Requests {
    std::string requests;
};
class ConverterJSON {
    int answersCounter = 0;
    int fileNumber = 0;
    nlohmann::json dict;
    std::vector<Config> configs;
public:
    ConverterJSON() {};

    std::vector<Config> GetTextDocuments() {
        //get file path
        std::string fullPath(__FILE__);
        std::string::size_type pos = fullPath.find_last_of("\\/");
        std::string pathFile = fullPath.substr(0, pos);
        pathFile += "/config.JSON";
        //
        std::ifstream configFile(pathFile);

        if (!configFile.is_open()) {
            std::cerr << "Unable to open conjig file." << std::endl;
            return configs;
        }

        nlohmann::json dict;
        configFile >> dict;

        Config c;

        auto insensitive = dict["config"]["case_insensitive_search"];
        auto maxRes = dict["config"]["max_responses"];
        auto paths = dict["files"];

        for (auto& path : paths) {
            for (auto& max : maxRes) {
                configs.emplace_back(max, path, insensitive);
            }
        }


        configFile.close();

        return configs;

    }

    std::vector<Requests> GetRequests() {
        std::vector<Requests> requests;
        //get file path
        std::string fullPath(__FILE__);
        std::string::size_type pos = fullPath.find_last_of("\\/");
        std::string pathFile = fullPath.substr(0, pos);
        pathFile += "/requests.JSON";
        //
        std::ifstream configFile(pathFile);

        if (!configFile.is_open()) {
            std::cerr << "Unable to open requests file" << std::endl;
            return requests;
        }

        nlohmann::json dict;
        configFile >> dict;

        Requests r;

        for (const auto& request : dict["requests"]) {
            r.requests = request;
            requests.push_back(r);
        }

        for (const auto& req : requests) {
            std::cout << "req: " << req.requests << std::endl;
        }
        configFile.close();
        return requests;
    }

    void putAnswers(const std::vector<std::vector<std::pair<size_t, float>>>& answers) {
        //get file path
        std::string fullPath(__FILE__);
        std::string::size_type pos = fullPath.find_last_of("\\/");
        std::string pathFile = fullPath.substr(0, pos);
        pathFile += "/answer.JSON";
        //
        std::ofstream configFile(pathFile);
        if (!configFile.is_open()) {
            std::cerr << "Unable to open file for writing" << std::endl;
            return;
        }
        for (int i = 0; i < answers.size(); i++) {
            nlohmann::json requestJson;
            requestJson["relevance"] = nlohmann::json::array();

            requestJson["result"] = true;

            if (answers[i].empty()) {
                requestJson["result"] = false;

            }

            for (const auto& pair : answers[i]) {
                requestJson["relevance"].push_back({
                    {"docid", pair.first},
                    {"rank", pair.second}
                    });
            }

            dict["answers"]["request" + std::to_string(answersCounter + 1)] += requestJson;
        }

        configFile << std::setw(4) << dict << std::endl;

        std::cout << "JSON data has been written to answer.JSON" << std::endl;
        configFile.close();
        answersCounter++;
    }

};
