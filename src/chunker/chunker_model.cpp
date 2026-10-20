#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "../json/include/nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

int main(void) {
  ofstream  python("main.py");
  
  if (!python.is_open()) return -1;
  python << "if __name__ == '__main__':\n\tprint('hello from cpp')";
  python.close();

  ifstream  clm("constrained_llm.py");
  
  string  file_content((istreambuf_iterator<char>(clm)), istreambuf_iterator<char>());
  

  ofstream maybe("clm.py");
  maybe << file_content;

  vector<vector<char>> chunks;
  chunks.push_back(vector<char>());
  
  int index = 0;
  int j = 0;
  /*for (int i = 0; i < file_content.size(); i++)
    cout << file_content[i];
  return 1;*/
  string  end_points = {""};
  for (int i = 0; i < file_content.length(); i++) {
    // cout << "skata" << "\n";
    if (j >= 2000 && file_content[i]) {
      chunks.push_back(vector<char>());
      index++;
      j = 0;
    }
    chunks[index].push_back(file_content[i]);
    j++;
  }


  json data = json::array();
  

  for (int i = 0; i < chunks.size(); i++) {
    string  chunk_str(chunks[i].begin(), chunks[i].end());
    json chunk_dict;
    chunk_dict["chunk_id"] = i;
    chunk_dict["content"] = chunk_str;
    data.push_back(chunk_dict);
  }


  ofstream file("output.json");
  
  if (file.is_open()) {
    file << data.dump(4);
    file.close();
    cout << "skata done \n";
  }

  else {
    cout << "skata error \n";
  }
  for (int i = 0; i < 1; i++) {
    for (int j = 0; j < chunks[i].size(); j++)
      cout << chunks[i][j];
  }
  
  for (int i = 0; i < chunks.size(); i++)
    cout << chunks[i].size() << "\n";
  


  maybe.close();


  //while (getline(Hello, line))
  //  cout << "token:"<<line<<"\n";

  clm.close();

}

