#pragma once

#ifndef BRANCH_H
#define BRANCH_H

#include <iostream>
#include <map>
#include <fstream>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

namespace branch_info
{
    struct Data
    {
        unsigned long length;
        // Key: name_package, value: version package
        unordered_map<string, string> packages;

        Data();

        // Add package in map
        void push(string name, string version);
    };

    class Branch
    {
    private:
        string branch_arch;
        string branch_name;
        string branch_url;
        shared_ptr<Data> data;

        // Function for determining the size of uploaded data by a get request
        static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);

        // Getting REST API data
        json get();

        bool fromJson(json branch_data);
        json toJson(unique_ptr<Data> answer);

    public:
        Branch(string, string);
        ~Branch();

        string getBranchName();

        // Launching a get request
        bool getDataFromUrl();

        // Comparing the names of two branches
        void comparePackagesName(json&, const string, const string, const unordered_map<string, string> &);

        // Comparison of versions of common packages
        void compareSharedPackagesVersion(json&, const string, const string, const unordered_map<string, string> &);

        // Getting a list of packages and their versions
        unordered_map<string, string> getDataPackages();
    };

    void writeData(const string file_path, json& data);
}

#endif