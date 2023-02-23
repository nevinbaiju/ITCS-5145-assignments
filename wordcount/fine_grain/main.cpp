#include <chrono>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <mutex>

#include "Dictionary.cpp"
#include "MyHashtable.cpp"


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

void increment(std::string key, Dictionary<std::string, int> & dict){
  // std::cout<< "getting\n";
  // std::cout << key <<"\n";
  // std::cout<< "before getting\n";
  int count = dict.get(key);
  ++count;
  // std::cout<< "setting\n";
  dict.set(key, count);
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
  // Populate Hash Table
  // for (int i=0; i<total_threads; i++){
  //   fileThreads[i] = std::thread(hash_words, wordmap[i], dict);
  // }
  int i = 0;
  int length = wordmap.size();
  for (auto & filecontent: wordmap) {
    std::cout << i << "/" << wordmap.size() << '\n';
    int length = filecontent.size();
    // std::cout << "Thread sizesdfsdf=" << length << "\n";
    // std::thread fileThreads[length];
    std::vector<std::thread> fileThreads;
    int wc = 0;
    for (auto & w : filecontent) {
      std::cout << wc << "/" << filecontent.size() << '\n';
      fileThreads.push_back(std::thread(increment, w, std::ref(dict)));
      // increment(w, dict);
      if(fileThreads.size() == 20){
        for(int i=fileThreads.size()-1; i>=0; i--){
          fileThreads[i].join();
          fileThreads.pop_back();
        }
      }
      wc++;
    }
    for(int i=fileThreads.size()-1; i>=0; i--){
          fileThreads[i].join();
          fileThreads.pop_back();
    }
    i++;
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
