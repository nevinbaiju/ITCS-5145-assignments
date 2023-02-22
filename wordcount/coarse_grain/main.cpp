#include <chrono>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <mutex>

#include "Dictionary.cpp"
#include "MyHashtable.cpp"

std::mutex hash_table_mutex;

//Tokenize a string into individual word, removing punctuation at the
//end of words
std::vector<std::vector<std::string>> tokenizeLyrics(const std::vector<std::string> files) {
  std::vector<std::vector<std::string>> ret;

  for(auto filename : files) {
    //std::cout<<"reading "<<filename<<"\n";
    std::vector<std::string> my_vect;
    std::ifstream in (filename);

    std::string line;

    //For each line
    while (getline(in, line, '\n')) {
      //Skip all like starting with [
      if (line[0] == '[')
        continue;

      std::stringstream ssline (line);
      //For all words
      while (ssline) {
        std::string word;
        ssline >> word;
        if (ssline) {
          //remove punctuation at the end of word
          while (word.length() > 0
                 && std::ispunct(word[word.length() - 1])) {
            word.pop_back();
          }
          my_vect.push_back(word);
        }
      }
    }
    //std::cout<<"read "<<my_vect.size()<<" words\n";
    ret.push_back(my_vect);
  }
  return ret;
}

void hash_words(std::vector<std::string> & filecontent, Dictionary<std::string, int> & dict){

  for (auto & w : filecontent) {
    hash_table_mutex.lock();
    int count = dict.get(w);
    ++count;
    dict.set(w, count);
    hash_table_mutex.unlock();
  }
}

int main(int argc, char **argv)
{
  if (argc < 4) {
    std::cerr<<"usage: ./main <sources> <testword> <threshold>"<<std::endl;
    return -1;
  }

  // Parse Command Line
  std::string source = argv[1];
  std::string testWord = argv[2];
  int32_t thresholdCount = std::stoi(argv[3]);

  // Obtain List of Files
  std::vector<std::string> files;
  std::ifstream in (source);
  std::string line;
  while (getline(in, line, '\n')) {
    files.push_back(line);
  }

  // Tokenize Lyrics
  auto wordmap = tokenizeLyrics(files);

  MyHashtable<std::string, int> ht;
  Dictionary<std::string, int> & dict = ht;

  // write code here
  // Start Timer
  auto start =std::chrono::steady_clock::now();
  int total_threads = wordmap.size();
  std::thread fileThreads[total_threads];

  // Populate Hash Table
  int i = 0;
  // Populate Hash Table
  // for (int i=0; i<total_threads; i++){
  //   fileThreads[i] = std::thread(hash_words, wordmap[i], dict);
  // }
  for (auto & filecontent: wordmap) {
    fileThreads[i] = std::thread(hash_words, std::ref(filecontent), std::ref(dict));
    i++;
    // hash_words(filecontent, dict);
  }
  for(int i=0; i<total_threads; i++){
    fileThreads[i].join();
  }

  // Stop Timer
  auto stop = std::chrono::steady_clock::now();
  std::chrono::duration<double> time_elapsed = stop-start;

  // Check Hash Table Values
  
  // for (auto it : dict) {
  //   if (it.second > thresholdCount)
  //     std::cout << it.first << " " << it.second << std::endl;
  // }

  // Need for test cases.
  std::cout << ht.get(testWord) << std::endl;

  std::cerr << time_elapsed.count()<<"\n";

  return 0;
}