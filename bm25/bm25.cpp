#include "bm25.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>

/// constuctorr
BM25::BM25(double k, double b) {
  k_ = k;
  b_ = b;
  avgdl_ = 0.0;
  num_docs_ = 0;
}

// fit (indexing the corpus)
void BM25::fit(const std::vector<std::vector<std::string>> &corpus) {

  num_docs_ = corpus.size();
  if (num_docs_ == 0)
    return;

  doc_lengths_.resize(num_docs_);
  doc_term_freqs_.resize(num_docs_);

  std::unordered_map<std::string, int> doc_frequencies;
  long long total_ln = 0;

  // loop through each document
  for (int i = 0; i < num_docs_; i++) {
    const std::vector<std::string> &doc = corpus[i];
    doc_lengths_[i] = doc.size();
    total_ln += doc.size();

    std::unordered_map<std::string, int> term_counts;

    // count term frequencyies for this document
    for (int j = 0; j < doc.size(); j++) {
      std::string word = doc[j];
      term_counts[word]++;
    }
    doc_term_freqs_[i] = term_counts;

    /*
      count how many docs contains each word (for IDF)
      using set to make sure i count each word only once per doc
    */
    std::set<std::string> unique_words(doc.begin(), doc.end());
    for (std::set<std::string>::iterator it = unique_words.begin();
         it != unique_words.end(); ++it) {
      doc_frequencies[*it]++;
    }
  }

  //// calculate avg doc legtht
  avgdl_ = static_cast<double>(total_ln) / num_docs_;

  // calculate idf for every unique word
  idf_.clear();

  for (std::unordered_map<std::string, int>::iterator it =
           doc_frequencies.begin();
       it != doc_frequencies.end(); ++it) {
    std::string word = it->first;
    int df = it->second;

    double top = static_cast<double>(num_docs_) - df + 0.5;
    double bottom = static_cast<double>(df) + 0.5;
    double idf_val = std::log((top / bottom) + 1.0);
    idf_[word] = (idf_val > 0.0) ? idf_val : 0.0;
  }
}
