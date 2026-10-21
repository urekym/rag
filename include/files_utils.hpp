#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP


#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>


using namespace std;
namespace fs= filesystem;

vector<string> get_all_files(const string &dir_path)
{

  vector<string> files;

  for (auto const &dir_entry : fs::recursive_directory_iterator(dir_path))
  {
    string  ext = dir_entry.path().extension().string();

    if (ext == ".py" || ext == ".md")
      files.push_back(dir_entry.path().string());
  }
  return files;
}

string  read_file_content(const string &filepath)
{
  ifstream  file(filepath);
  if (!file.is_open()) return "";

  stringstream  buffer;

  buffer << file.rdbuf();
  return buffer.str();
}


inline vector<string> tokenize(const string &text) {


    vector<string> tokens;
    stringstream ss(text);
    string word;
    
    while (ss >> word)
    {
        string clean_word = "";
        
        for (char c : word) {
            if (isalnum(c))
              clean_word += tolower(c);
        }
        
        if (!clean_word.empty())
            tokens.push_back(clean_word);
    }
    return tokens;

}

#endif
