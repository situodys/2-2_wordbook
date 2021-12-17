#ifndef __MYUI_H__
#define __MYUI_H__

#include "WordSet.hpp"
#include <fcntl.h>
#include <iostream>
#include <list>
#include <locale.h>
#include <ncursesw/ncurses.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using namespace std;

#define ENTER 10
#define QUIT 99
#define ADD 111
#define MAX_ID_LEN 32
#define MAX_PW_LEN 40

// char id[MAX_ID_LEN + 1];
// char password[MAX_PW_LEN + 1];

class userInfo {
  private:
    char id[MAX_ID_LEN + 1];
    char password[MAX_PW_LEN + 1];

  public:
    userInfo() {
        memset(this->id, 0x00, MAX_ID_LEN + 1);
        memset(this->password, 0x00, MAX_PW_LEN + 1);
    }
    userInfo(string i, string p) {
        memcpy(this->id, i.c_str(), MAX_ID_LEN);
        memcpy(this->password, p.c_str(), MAX_PW_LEN);
    }
    string getId() { return (string)this->id; }
    string getPassword() { return (string)this->password; }
    void setId(string i) { memcpy(this->id, i.c_str(), MAX_ID_LEN); }
    void setPassword(string p) {
        memcpy(this->password, p.c_str(), MAX_PW_LEN);
    }
    userInfo operator=(userInfo source) {
        return userInfo(source.getId(), source.getPassword());
    }
};

void init_scr();
void drawTitle(WINDOW *title);

int openAuthFile();
bool checkAuth(userInfo curUser);
bool isIdExist(int fd, userInfo curUser);
void registerId(int fd, userInfo curUser);

vector<WordSet> readWordFile(string curUserId, string selectedFile);
vector<WINDOW *> fileList(vector<string> fileSet, int type);
vector<WINDOW *> drawWordSet(vector<WordSet> wset, int range);

vector<WINDOW *> auth_menu_list();
vector<WINDOW *> user_login_list();
vector<WINDOW *> user_start_list();
void alertWin(string msg);
vector<WordSet> readWordFile(string curUserId, string selectedFile);

bool checkAuth(userInfo user);
userInfo login_window();
void signup_window(WINDOW *titleScr);
void delete_menu(vector<WINDOW *> contents);
int scroll_auth(vector<WINDOW *> contents, int idx, int a);
int scroll_file(vector<WINDOW *> contents, int idx, int a);
void study_page(userInfo user);
vector<WINDOW *> inputPrompt(vector<WordSet> wordsInFile, string fileName);
void insert_page(userInfo user);
vector<WINDOW *> testView();
void test_page(userInfo user);

#endif
