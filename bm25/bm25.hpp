#ifndef BM25_H
#define BM25_H

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class BM25 {
private:
  double k_;
  double b_;
  double avgdl_;
  size_t num_docs_;

  std::vector<size_t> doc_lengths_;
  std::vector<std::unordered_map<std::string, int>> doc_term_freqs_;
  std::unordered_map<std::string, double> idf_;

public:
  /*
    constractor with defult bm25 params
    k is widely set to 1.2 in most modern frameworks
    and 1.2 1.5 both fall within the optimal range (1.2 - 2.0).
    anyways i need more study about it
  */
  BM25(double k = 1.2, double b = 0.75);

  // the core methods
  void fit(const std::vector<std::vector<std::string>> &corpus);
  std::vector<std::pair<int, double>>
  search(const std::vector<std::string> &query_tokens, int top_k);

  void save_model(const std::string &filepath);
  void laod_model(const std::string &filepath);

  ~BM25() = default
};

#endif
