#include "bm25.hpp"
#include <iostream>
#include <vector>

int main()
{
    //// lets imagine a simple chunked corpus contains this data
    std::vector<std::vector<std::string>> corpus = {
        //doc 0; server documentation
        {"vllm", "server", "startup", "configuration", "port", "host", "api"},
        
        // doc 1: lora implementation code
        {"def", "load_lora_adapter", "model", "adapter_path", "weights", "scale"},
        
        // doc 2: general Python utility
        {"def", "process_tokens", "text", "tokenizer", "max_length", "return"},
        
        // coc 3: OpenAI compatible server endpoint
        {"vllm", "openai", "compatible", "server", "endpoints", "chat", "completions"},
        
        // doc 4: short config file
        {"server", "port", "8000", "workers", "4"},
        
        // Doc 5: unrelated helper
        {"def", "calculate_metrics", "recall", "precision", "ground_truth", "predictions"}
    };



    //initialize BM25 with k = 1.2 and b = 0.75
    // ill discuss later the conventions between 1.5 and 1.2 in k 
    BM25 bm25(1.5, 0.75);

    // fit the index across all 6 documents
    bm25.fit(corpus);
    std::cout << "success indexed " << corpus.size() << " documents!\n\n";

    // testing Query 1:looking for server configuration
    std::vector<std::string> query1 = {"vllm", "server"};
    std::vector<std::pair<int, double>> results1 = bm25.search(query1, 3);

    std::cout << "Search Results for query: ['vllm', 'server']\n";
    for (size_t i = 0; i < results1.size(); i++) {
        std::cout << "  Rank " << i + 1 
                  << " -> Document ID: " << results1[i].first 
                  << " | Score: " << results1[i].second << "\n";
    }

    // testing Query 2: looking for code/lora logic
    std::vector<std::string> query2 = {"load_lora_adapter", "weights"};
    std::vector<std::pair<int, double>> results2 = bm25.search(query2, 3);

    std::cout << "\nSearch Results for query: ['load_lora_adapter', 'weights']\n";
    for (size_t i = 0; i < results2.size(); i++) {
        std::cout << "  Rank " << i + 1 
                  << " -> Document ID: " << results2[i].first 
                  << " | Score: " << results2[i].second << "\n";
    }

    return 0;
}
