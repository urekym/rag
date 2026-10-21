#include "../include/rag.hpp"

int main() {
    string target_dir = "../data/vllm-0.10.1/";
    cout << "Scanning files in " << target_dir << ".......\n";

    vector<string> file_paths = get_all_files(target_dir);
    cout << "Found " << file_paths.size() << " source files.\n";

    if (file_paths.empty()) {
        cout << "No files found! Check your path.\n";
        return 0;
    }
    bool found_minecraft = false;
    for (size_t i = 0; i < file_paths.size(); i++) {
      if (file_paths[i].find("Minecraft") != string::npos) {
          cout << "Found Minecraft file at index: " << i << " -> " << file_paths[i] << "\n";
          found_minecraft = true;
      }
    }
    if (!found_minecraft) {
      cout << "WARNING: Minecraft/README.md was NOT found by get_all_files!\n";
}
    vector<vector<string>> corpus;

    // Read and tokenize every file into the corpus
    for (const auto &path : file_paths) {
        string content = read_file_content(path);
        if (content.empty()) continue;

        // FIXED: Tokenize the file content into words!
        vector<string> tokens = tokenize(content);
        corpus.push_back(tokens);
    }

    cout << "Fitting BM25 index...\n";
    BM25 bm25(1.2, 0.75);
    bm25.fit(corpus);
    cout << "BM25 Index built successfully!\n";

    string query;
    
    while (true) {
        cout << "\nEnter a search query (or 'exit'): ";
        
        // FIXED: Added missing comma between cin and query
        if (!getline(cin, query) || query == "exit") break;

        // FIXED: Tokenize the query string so BM25 has words to search for!
        vector<string> query_tokens = tokenize(query);

        auto results = bm25.search(query_tokens, 1);
        cout << "Top results:\n";

        if (results.empty()) {
            cout << "No matching documents found.\n";
        } else {
            for (const auto &res : results) {
                cout << "  Doc ID: " << res.first << " | Score: " << res.second 
                     << " | File: " << file_paths[res.first] << "\n";
            }
        }
    }

    return 0;
}
