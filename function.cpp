#include "header.h"

void get_raw_data(unordered_map<string, pair<int, vector<string>>> &rawDatas) {
    for (const auto &entry : filesystem::directory_iterator(DATAFOLDER)) {
        filesystem::path path = entry.path();
        string filename = path.filename().generic_string();
        ifstream fin(path);
        if (fin.is_open()) {
            pair<int, vector<string>> cur;
            string title;
            int title_size = 0;
            getline(fin, title);
            istringstream sin(title);
            string word;
            while (sin >> word) cur.second.push_back(word), ++title_size;
            cur.first = title_size;
            while (fin >> word) cur.second.push_back(word);
            rawDatas[filename] = cur;
        }
        fin.close();
    }
}

string normalize(const string &a) {
    string b;
    for (const auto &ch : a)
        if (isalpha(ch)) b += tolower(ch);
    return b;
}

void write_data_to_disk(
    const unordered_map<string, vector<Word_in_file>> &data) {
    int count = 0;
    for (const auto &i : data) {
        if (count++ % 100 == 0) cout << count << '/' << data.size() << endl;
        filesystem::create_directory(WORDSFOLDER);
        ofstream fout(WORDSFOLDER + "/" + i.first);
        for (const auto &j : i.second) {
            fout << j.file_name << endl;
            for (const auto &k : j.pos) fout << k << ' ';
            fout << endl;
        }
        fout.close();
    }
}

void get_query_from_file(const vector<string> &query_initial,
                         vector<vector<Word_in_file>> &query_data) {
    for (const auto &i : query_initial) {
        ifstream fin(WORDSFOLDER + "/" + i);
        vector<Word_in_file> cur_word;
        for (string filename; getline(fin, filename);) {
            cur_word.push_back(Word_in_file(filename));
            string pos_line;
            getline(fin, pos_line);
            istringstream sin(pos_line);
            for (int pos; sin >> pos;) cur_word.back().pos.push_back(pos);
        }
        query_data.push_back(cur_word);
        fin.close();
    }
}

vector<Word_in_file> IntersectVectorCache(const vector<Word_in_file> &left,
                                          const vector<Word_in_file> &right) {
    vector<Word_in_file> result;
    auto il = left.begin(), ir = right.begin();
    while (il != left.end() && ir != right.end()) {
        if (il->file_name < ir->file_name)
            ++il;
        else if (ir->file_name < il->file_name)
            ++ir;
        else {
            int l = 0, r = 0;
            Word_in_file cur(il->file_name);
            while (l < il->pos.size() && r < ir->pos.size()) {
                if (il->pos[l] < ir->pos[r])
                    cur.pos.push_back(il->pos[l++]);
                else
                    cur.pos.push_back(ir->pos[r++]);
            }
            while (l < il->pos.size()) cur.pos.push_back(il->pos[l++]);
            while (r < ir->pos.size()) cur.pos.push_back(ir->pos[r++]);
            result.push_back(cur);
            ++il;
            ++ir;
        }
    }
    return result;
}

void rank_result(vector<Word_in_file> &result, const int &size) {
    // size always > 1
    vector<pair<Word_in_file, int>> priority;
    for (int i = 0; i < result.size(); ++i) {
        priority.push_back({result[i], 1});
        vector<vector<int>> priority_count(
            size - 1);  //[0] - 2 characters/priority; [size-2] - size
                        // characters/priority
        for (int j = 1, dis_cur = 1; j < result[i].pos.size(); ++j) {
            if (result[i].pos[j] - result[i].pos[j - 1] == 1) {
                if (dis_cur < size) ++dis_cur;
                priority_count[dis_cur - 2].push_back(result[i].pos[j]);
            } else
                dis_cur = 1;
        }
        for (int j = size - 2; j >= 0; --j) {
            if (priority_count[j].size() > 0) {
                priority[i].first.pos = priority_count[j];
                priority[i].second = j + 2;
                break;
            }
        }
    }
    sort(priority.begin(), priority.end(),
         [&](const pair<Word_in_file, int> &a,
             const pair<Word_in_file, int> &b) { return a.second > b.second; });
    for (int i = 0; i < result.size(); ++i) result[i] = priority[i].first;
}

#define PREVIEW 30
vector<string> get_content(
    const vector<Word_in_file> &search_result,
    unordered_map<string, pair<int, vector<string>>> &rawDatas) {
    vector<string> res;
    for (const auto &a : search_result) {
        int cs = rawDatas[a.file_name].second.size(),
            ts = rawDatas[a.file_name].first;
        string content;
        for (int i = 0; i < ts; ++i)
            content += rawDatas[a.file_name].second[i] + ' ';
        content += '\n';
        if (a.pos[0] < ts + PREVIEW / 2) {
            for (int i = ts; i < ts + PREVIEW; ++i)
                content += rawDatas[a.file_name].second[i] + ' ';
            content += "...";
        } else if (a.pos[0] > cs - PREVIEW / 2) {
            content += "...";
            for (int i = cs - PREVIEW; i < cs; ++i)
                content += rawDatas[a.file_name].second[i] + ' ';
        } else {
            content += "...";
            for (int i = a.pos[0] - PREVIEW / 2; i < a.pos[0] + PREVIEW / 2;
                 ++i)
                content += rawDatas[a.file_name].second[i] + ' ';
            content += "...";
        }
        res.push_back(content);
    }
    return res;
}