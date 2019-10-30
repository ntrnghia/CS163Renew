#include "header.h"

void search_intersection_whole(
    const vector<string> &query_initial, vector<string> &output,
    unordered_map<string, pair<int, vector<string>>> &rawDatas) {
    vector<vector<Word_in_file>> query_data;
    get_query_from_file(query_initial, query_data);
    sort(query_data.begin(), query_data.end(),
         [&](const vector<Word_in_file> &a, const vector<Word_in_file> &b) {
             return a.size() < b.size();
         });
    vector<Word_in_file> intersect_result;
    if (query_data.size() > 0) intersect_result = query_data.front();
    for (int i = 1; i < query_data.size(); ++i)
        intersect_result =
            IntersectVectorCache(intersect_result, query_data[i]);
    // make_cache(query, cache);
    rank_result(intersect_result, query_initial.size());
    // make_smart_cache(query, cache);
    output = get_content(intersect_result, rawDatas);
    // save_search_history(query_initial);
}

int main() {
    cout << "Getting raw data...";
    auto t1 = chrono::system_clock::now();
    unordered_map<string, pair<int, vector<string>>> rawDatas;
    get_raw_data(rawDatas);
    auto t2 = chrono::system_clock::now();
    while (true) {
        system("CLS");
        // if (Tree.search_history.size() != 0) cout << "Search history:\n";
        // for (string word : Tree.search_history) cout << word << endl;
        // cout <<
        // "--------------------------------------------------------"
        //      << endl;

        cout << "Insert words which want to search: ";
        string input;
        getline(cin, input);
        if (input == "exit") break;

        vector<string> query_initial;
        istringstream sin(input);
        for (string word; sin >> word;)
            query_initial.push_back(normalize(word));
        vector<string> output;
        auto t3 = chrono::system_clock::now();
        search_intersection_whole(query_initial, output, rawDatas);
        auto t4 = chrono::system_clock::now();

        auto t5 = chrono::system_clock::now();
        for (const string &print : output) cout << print << endl << endl;
        auto t6 = chrono::system_clock::now();

        cout << "Get raw data time: "
             << chrono::duration<double>(t2 - t1).count() << " seconds" << endl;
        cout << output.size() << " results ("
             << chrono::duration<double>(t4 - t3).count() << " seconds)"
             << endl;
        cout << "Printing time: " << chrono::duration<double>(t6 - t5).count()
             << " seconds" << endl;

        cout << "Press Enter to search the next keyword!";
        cin.ignore(INT_MAX, '\n');
    };
}