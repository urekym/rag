#ifndef MODELS_HPP
#define MODELS_HPP

#include <string>
#include <vector>
#include <variant>
using namespace std;


class MinimalSource {
  public:
    string file_path;
    int first_character_index;
    int last_character_index;
};


class UnansweredQuestion {
  public:
    string question_id;
    string question;
};


class AnsweredQuestion : public UnansweredQuestion {
  public:
  vector<MinimalSource> sources;
  string answer;
};


class RagDataset {
  public:
    vector<variant<AnsweredQuestion, UnansweredQuestion>> rag_questions;
};


class MinimalSearchResults {
  public:
    string  question_id;
    string  question;
    vector<MinimalSource> retrieved_sources;
}; 


class MinimalAnswer : public MinimalSearchResults{
  public:
    string answer;
};


class StudentSearchResults {
  public:
    vector<MinimalSearchResults> search_results;
    int k;
};


class StudentSearchResultsAndAnswer {
  public:
    vector<MinimalAnswer> search_results;
    int k;
};

#endif
