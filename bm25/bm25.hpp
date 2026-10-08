#ifndef BM25_H
#define BM25_H

#include <string>
#include <vector>

using namespace std;

class BM25 : public basename {
public:
  double k;
  double b;

  void set_tf(double k, double b) override;
  void set_model(const vector<vector<string>> &corpus, double k = 1.5,
                 double b = 0.75) override;
  void save_model(const string &filepath) override;
  void load_model(const string &filepath) override;

  ~BM25() {}
};

#endif BM25_H
