#include "Dictionary.cpp"
#include "MyHashtable.cpp"


int main(int argc, char **argv){
    MyHashtable<std::string, int> ht;
    Dictionary<std::string, int>& dict = ht;
    
    for(int i=0; i<100; i++){
        dict.set("so", i);
    }

}