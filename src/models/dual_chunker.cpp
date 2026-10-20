#include <string>
#include <vector>

using namespace std;

class Chunk {
  string  file_path;
  size_t  first_character_index;
  size_t  last_character_index;
  string  content;
};

class Chunker {
  private:
    size_t  max_chunk_size_;
    const vector<string> python_separators_ = {"\nclass ", "\ndef "};

    const vector<string> markdown_separators_ = {"\n# ", "\n## ", "\n### ", "\n\n"};
  
  public:
    Chunker(size_t max_chunk_size = 2000) {
      max_chunk_size_ = max_chunk_size;
    }

    vector<Chunk> chunk_python(const &file_path const &content) {
      
      vector<Chunk> chunks;
      return chunks;
    }

    vector<Chunk> chunk_markdown(const string &file_path, const string &content) {
      vector<Chunk> chunks;
      return chunks;
    }
    
}
