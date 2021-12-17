#include "WordSet.hpp"
#include <string.h>

WordSet::WordSet() {
    memset(this->eng, 0x00, MAX_WORD_LEN + 1);
    memset(this->kor, 0x00, MAX_WORD_LEN + 1);
}

WordSet::WordSet(string eng, string kor) {
    memcpy(this->eng, eng.c_str(), MAX_WORD_LEN);
    memcpy(this->kor, kor.c_str(), MAX_WORD_LEN);
}
