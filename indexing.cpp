#include "header.h"

void indexing() {
    auto now = chrono::system_clock::now();
    unordered_map<string, vector<Word_in_file>> data;
    for (const auto &entry : filesystem::directory_iterator(DATAFOLDER)) {
        filesystem::path path = entry.path();
        string filename = path.filename().generic_string();
        ifstream fin(path);
        if (fin.is_open()) {
            string word;
            for (int i = 0; fin >> word; ++i) {
                word = normalize(word);
                if (!data[word].size() ||
                    data[word].back().file_name != filename)
                    data[word].push_back(Word_in_file(filename));
                data[word].back().pos.push_back(i);
            }
        }
        fin.close();
    }
    cout << chrono::duration<double>(chrono::system_clock::now() - now).count()
         << " seconds\n";
    write_data_to_disk(data);
}

int main() {
    auto now = chrono::system_clock::now();
    indexing();
    cout << chrono::duration<double>(chrono::system_clock::now() - now).count()
         << " seconds";
    cin.get();
}