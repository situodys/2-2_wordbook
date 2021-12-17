#ifndef __WORDSET_H__
#define __WORDSET_H__

#include <string>
using namespace std;

#define MAX_WORD_LEN 30
class WordSet {
  private:
    char eng[MAX_WORD_LEN + 1];
    char kor[MAX_WORD_LEN + 1];

  public:
    WordSet();
    WordSet(string eng, string kor);
    string getEng() { return (string)this->eng; }
    string getKor() { return (string)this->kor; }
    //~WordSet();
};

#endif
