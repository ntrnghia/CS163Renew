#pragma once
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

#define DATAFOLDER string("Data")
#define WORDSFOLDER string("Words")

struct Word_in_file {
    string file_name;
    vector<int> pos;
    Word_in_file(){};
    Word_in_file(string x) : file_name(x){};
    Word_in_file(string x, int y) : file_name(x), pos({y}){};
};

void get_raw_data(unordered_map<string, pair<int, vector<string>>> &rawDatas);
string normalize(const string &a);
void write_data_to_disk(
    const unordered_map<string, vector<Word_in_file>> &data);
void get_query_from_file(const vector<string> &query_initial,
                         vector<vector<Word_in_file>> &query_data);
vector<Word_in_file> IntersectVectorCache(const vector<Word_in_file> &left,
                                          const vector<Word_in_file> &right);
void rank_result(vector<Word_in_file> &result, const int &size);
vector<string> get_content(
    const vector<Word_in_file> &search_result,
    unordered_map<string, pair<int, vector<string>>> &rawDatas);
