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
  for (size_t i = 0; i < num_docs_; i++)
  {
    const std::vector<std::string> &doc = corpus[i];
    doc_lengths_[i] = doc.size();
    total_ln += doc.size();

    std::unordered_map<std::string, int> term_counts;

    // count term frequencyies for this document
    for (size_t j = 0; j < doc.size(); j++)
    {
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
         it != unique_words.end(); ++it)
    {
      doc_frequencies[*it]++;
    }
  }

  //// calculate avg doc legtht
  avgdl_ = static_cast<double>(total_ln) / num_docs_;

  // calculate idf for every unique word
  idf_.clear();

  for (std::unordered_map<std::string, int>::iterator it =
           doc_frequencies.begin();
       it != doc_frequencies.end(); ++it)
  {
    std::string word = it->first;
    int df = it->second;

    double top = static_cast<double>(num_docs_) - df + 0.5;
    double bottom = static_cast<double>(df) + 0.5;
    double idf_val = std::log((top / bottom) + 1.0);
    idf_[word] = (idf_val > 0.0) ? idf_val : 0.0;
  }
}



/// now the search, retrieve part 
/// ill write about it in md file
std::vector<std::pair<int, double>> BM25::search(const std::vector<std::string> &query_tokens, int top_k){
  std::vector<double> scores(num_docs_, 0.0);

  for (int q = 0; q < query_tokens.size(); q++)
  {
    std::string word =query_tokens[q];


    if (idf_.find(word) == idf_.end())
      continue;

    double idf_val = idf_[word];

    for (size_t i = 0; i < num_docs_; i++)
    {
      if (doc_term_freqs_[i].find(word) == doc_term_freqs_[i].end())
        continue;

      int freq = doc_term_freqs_[i][word];
      double doc_len = static_cast<double>(doc_lengths_[i]);

      double numerator = freq * (k_ + 1.0);
      double length_norm = 1.0 - b_ + b_ * (doc_len);
      double denominator = freq + k_ * length_norm;

      scores[i] += idf_val * (numerator / denominator);
    }
  }

  std::vector<std::pair<int, double>> results;
  for (size_t i = 0; i < num_docs_; i++)
  {
    if (scores[i] > 0.0)
      results.push_back(std::make_pair(static_cast<int>(i), scores[i]));
  }

  for (size_t i =0; i < results.size(); i++)
  {
    for (size_t j = i + 1; j < results.size(); j++)
    {
      if (results[j].second > results[i].second)
      {
        std::pair<int, double> temp = results[i];
        results[i] = results[j];
        results[j] = temp;
      }
    }
  }

  if (results.size() > static_cast<size_t>(top_k))
    results.resize(top_k);

  return results;
}

void  BM25::load_model(const std::string &filepath)
{
  std::fstream file(filepath);
  if (file.is_open())
  {
    file >> num_docs_ >> avgdl_ >> k_ >> b_;
    file.close();
  }
}
