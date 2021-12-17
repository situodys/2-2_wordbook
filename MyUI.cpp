#include "MyUI.hpp"
#include <cstdlib>
#include <ctime>
#include <dirent.h>
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
vector<string> Auth_item = {"", "SIGN IN", "SIGN UP", "EXIT"};
vector<string> login_item = {"", "START", "LOG OUT", "EXIT"};
vector<string> start_item = {"", "STUDY", "INSERT", "TEST"};
char id[MAX_ID_LEN + 1];
char password[MAX_PW_LEN + 1];

/*-----------------------파일 관련-----------------------*/

/* openAuthFile
회원정보를 담은 Auth.dat파일을 엽니다.파일지시자를 반환합니다.*/
int openAuthFile() {
    int fd = open("./Auth.dat", O_CREAT | O_RDWR, 0644);
    if (fd == -1) {
        perror("openFile() error");
        return 1;
    }
    return fd;
}

/* isIdExist
회원가입 시 이미 같은 id가 Auth.dat에 존재하는지 확인합니다.
존재할 경우 true를 없는 경우 false를 반환합니다.*/
bool isIdExist(int fd, userInfo curUser) {
    userInfo userInFile;
    bool isExist = false;
    ssize_t rSize = 0;
    do {
        userInFile.setId("");
        userInFile.setPassword("");
        rSize = read(fd, &userInFile, sizeof(userInfo));

        if (rSize == -1) {
            perror("read userInFile() error!");
            exit(-2);
        }
        if (userInFile.getId() == curUser.getId()) {
            return true;
        }
    } while (rSize > 0);

    return isExist;
}

/* registerId
 * 등록하고자 하는 id,password를 userInfo클래스 형태의 인자로 받아 Auth.dat에
 * 작성합니다. fd는 열려있는 Auth.dat 파일지시자입니다. 등록이 완료 되었을 경우
 * Users내 id와 같은 이름의 새 폴더를 생성합니다*/
void registerId(int fd, userInfo curUser) {
    ssize_t wSize = 0;
    string path = "./Users/";
    userInfo tmp(curUser.getId(), curUser.getPassword());
    lseek(fd, (off_t)0, SEEK_END);
    wSize = write(fd, &tmp, sizeof(userInfo));
    if (wSize == -1) {
        perror("write new userInfo error!");
        exit(-2);
    }
    path += tmp.getId();
    // cout << path.c_str();
    mkdir(path.c_str(), 0777);
}

/* checkAuth
 * 로그인 페이지에서 사용자가 입력한 정보로 로그인이 가능한지 확인합니다. 열려
 * 있는 Auth.dat에서 차례로 값을 읽어 사용자가 입력한 id와 password와 일치하는
 * 경우가 있을 경우 로그인이 된 것으로 로그인 되었음을 알리고 true를
 * 반환합니다.*/
bool checkAuth(userInfo curUser) {
    int fd = openAuthFile();
    ssize_t rSize;
    userInfo info;
    // bool isPwWrong = false;
    // bool isNotRegistered = true;
    // bool loginResult = false;

    do {
        rSize = read(fd, &info, sizeof(userInfo));
        if (rSize == -1) {
            perror("read userInFile() error!");
            exit(-2);
        }
        if (curUser.getId() == info.getId()) {
            if (curUser.getPassword() == info.getPassword()) {
                alertWin("Login Success!");
                return true;
            }
        }
    } while (rSize > 0);

    close(fd);
    return false;
}

/* readWordFile
 * 저장된 단어 파일을 읽어오는 함수입니다.*/
vector<WordSet> readWordFile(string curUserId, string selectedFile) {
    char buf = '\0';
    vector<WordSet> tmp;
    string path = "./Users/";
    path += curUserId;
    path += "/";
    path += selectedFile;
    string eng;
    string kor;
    size_t rsize = 0;
    int a = 0;
    bool isEng = true;
    int fd = open(path.c_str(), O_RDONLY);

    do {
        buf = '\0';
        rsize = read(fd, &buf, sizeof(char));
        if (buf == ' ' || buf == '\n') {
            if (isEng) {
                isEng = false;
                a++;
            } else {
                isEng = true;
                a++;
            }
            if (a == 2) {
                tmp.push_back(WordSet(eng, kor));
                eng = "";
                kor = "";
                a = 0;
            }
        } else {
            if (isEng) {
                eng += buf;
            } else {
                kor += buf;
            }
        }
    } while (rsize > 0);

    close(fd);
    return tmp;
}
//화면 관련

/* init_scr
 * ncurses에 대한 초기 설정을 합니다. 유니코드를 사용하기 위해 locale을 설정하고
 * 사용할 색깔 조합을 미리 저장해둡니다*/
void init_scr() {
    setlocale(LC_CTYPE, "ko_KR.utf-8");
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);
    init_pair(3, COLOR_YELLOW, COLOR_BLUE);
    init_pair(4, COLOR_RED, COLOR_WHITE);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    noecho();
    keypad(stdscr, TRUE);
}

/* drawTitle
 * 타이틀 화면을 그립니다.*/
void drawTitle(WINDOW *title) {
    wprintw(title, "\n\n\n\n\n");
    wprintw(title, "\t\t  __   __   __                       | |\n");
    wprintw(title, "\t\t |  | |  | |  |                      | |\n");
    wprintw(title, "\t\t |  | |  | |  |   __ __    _ __   __ | |\n");
    wprintw(title, "\t\t |  | |  | |  |  /     \\  |  __| /  _  |\n");
    wprintw(title, "\t\t |  \\_|  |_/  | |  ( )  | | |   |  (_| |\n");
    wprintw(title, "\t\t \\__  ____  __/  \\__ __/  |_|    \\__ ,_|\n\n");
    wprintw(title, "\t    ______                       _     _  \n");
    wprintw(title, "\t    | ___ \\                     | |   (_) \n");
    wprintw(title,
            "\t    | |_/ / _ __   __ _    ___  | |_   _    ___    ___\n");
    wprintw(title,
            "\t    |  __/ |  __| / _  |  / __| | __| | |  / __|  / _ \\\n");
    wprintw(title,
            "\t    | |    | |   | (_| | | (__  | |_  | | | (__  |  __|\n");
    wprintw(title,
            "\t    |_|    |_|    \\__,_|  \\___|  \\__| |_|  \\___|  \\___|\n");
    mvwprintw(title, 23, 60, "2018203014");
    mvwprintw(title, 24, 60, "Lee Jeong Hun");
}

/* fileList
 * 로그인한 유저 내에 저장된 단어 파일들을 윈도우에 그리는 함수입니다.  type에
 * 따라 사용가능한 입력을 다르게 출력합니다.*/
vector<WINDOW *> fileList(vector<string> fileSet, int type) {
    vector<WINDOW *> contents;
    contents.push_back(newwin(30, 75, 0, 0));
    wbkgd(contents[0], COLOR_PAIR(1));
    if (type == 1)
        mvwprintw(contents[0], 2, 40, "↑ ↓ exit: 'q' select: ENTER");
    else if (type == 2)
        mvwprintw(contents[0], 2, 30, "↑ ↓ exit: 'q' select: ENTER add: 'a'");

    contents.push_back(subwin(contents[0], 25, 70, 3, 3));
    for (int i = 0; i < fileSet.size(); i++) {
        contents.push_back(subwin(contents[1], 1, fileSet[i].size(), 4 + i, 4));
        wprintw(contents[i + 2], "%s", fileSet[i].c_str());
    }
    wborder(contents[1], 0, 0, 0, 0, 0, 0, 0, 0);
    wbkgd(contents[2], COLOR_PAIR(5));
    wrefresh(contents[0]);
    return contents;
}

/* drawWordSet
 * 인자로 뜻과 영단어를 담은 배열과 보여줄 범위 받아 화면에 단어를 출력하는
 * 함수입니다.*/
vector<WINDOW *> drawWordSet(vector<WordSet> wset, int range) {
    int wlen = wset.size();
    int col = 4;
    int row = 4;
    int start = (range - 1) * 10;

    vector<WINDOW *> contents;
    contents.push_back(newwin(30, 75, 0, 0));
    wbkgd(contents[0], COLOR_PAIR(1));
    mvwprintw(contents[0], 2, 55, "←  → exit: 'b'");

    contents.push_back(subwin(contents[0], 25, 35, 3, 3));
    wborder(contents[1], 0, 0, 0, 0, 0, 0, 0, 0);
    //'^', '|', '-', '-',
    contents.push_back(subwin(contents[0], 25, 35, 3, 38));
    wborder(contents[2], 0, 0, 0, 0, 0, 0, 0, 0);

    touchwin(contents[0]);
    for (int i = 0; i < 5; i++) {
        if (start + i >= wlen)
            break;
        mvwprintw(contents[1], (i + 1) * 4, 4,
                  wset[start + i].getEng().c_str());
        mvwprintw(contents[1], (i + 1) * 4, 19, "%s",
                  wset[start + i].getKor().c_str());
        /*mvwprintw(contents[2], (i + 1) * 4, 4, "%s",
                  wset[start + i].getKor().c_str());*/
    }
    mvwprintw(contents[1], 22, 17, "%d", 2 * range - 1);
    for (int i = 0; i < 5; i++) {
        if (start + i + 5 >= wlen)
            break;
        mvwprintw(contents[2], (i + 1) * 4, 4,
                  wset[start + i + 5].getEng().c_str());
        mvwprintw(contents[2], (i + 1) * 4, 19, "%s",
                  wset[start + i + 5].getKor().c_str());
    }
    mvwprintw(contents[2], 22, 17, "%d", 2 * range);

    wrefresh(contents[0]);
    return contents;
    // delete_menu(contents);
}

/* auth_menu_list
 * 로그인 되기 전 메뉴를 그리는 함수입니다.*/
vector<WINDOW *> auth_menu_list() {
    vector<WINDOW *> contents;
    contents.push_back(newwin(30, 25, 0, 75));
    wbkgd(contents[0], COLOR_PAIR(2));

    for (int i = 1; i <= 3; i++) {
        contents.push_back(
            subwin(contents[0], 1, Auth_item[i].size(), 5 + (i - 1) * 10, 85));
        wprintw(contents[i], "%s", Auth_item[i].c_str());
    }
    wbkgd(contents[1], COLOR_PAIR(3));
    wrefresh(contents[0]);
    return contents;
}

/* user_login_list
 * 로그인 이후 메뉴를 그리는 함수입니다.*/
vector<WINDOW *> user_login_list() {
    vector<WINDOW *> uContents;
    uContents.push_back(newwin(30, 25, 0, 75));
    wbkgd(uContents[0], COLOR_PAIR(2));

    for (int i = 1; i <= 3; i++) {
        uContents.push_back(subwin(uContents[0], 1, login_item[i].size(),
                                   5 + (i - 1) * 10, 85));
        wprintw(uContents[i], "%s", login_item[i].c_str());
    }
    wbkgd(uContents[1], COLOR_PAIR(3));
    wrefresh(uContents[0]);
    return uContents;
}

/* user_start_list
 * 로그인 이후 start를 누른 후의 메뉴를 그리는 함수입니다.*/
vector<WINDOW *> user_start_list() {
    vector<WINDOW *> uContents;
    uContents.push_back(newwin(30, 25, 0, 75));
    wbkgd(uContents[0], COLOR_PAIR(2));

    for (int i = 1; i <= 3; i++) {
        uContents.push_back(subwin(uContents[0], 1, start_item[i].size(),
                                   5 + (i - 1) * 10, 85));
        wprintw(uContents[i], "%s", start_item[i].c_str());
    }
    wbkgd(uContents[1], COLOR_PAIR(3));
    wrefresh(uContents[0]);
    return uContents;
}

/* alertWin
 * 사용자에게 특정 알림을 전할 때 팝업되는 작은 화면입니다. 인자로 전달받은
 * 내용을 띄우며 알려줍니다*/
void alertWin(string msg) {
    WINDOW *alrtWin = newwin(8, 20, 10, 36);
    wprintw(alrtWin, msg.c_str());
    mvwprintw(alrtWin, 7, 1, "press any key...");
    wbkgd(alrtWin, COLOR_PAIR(4));
    wrefresh(alrtWin);
    wgetch(alrtWin);
    delwin(alrtWin);
}

/* scroll_auth
 * 메뉴에서 방향키로 메뉴의 이동을 보여주는 함수입니다. 현재 커서가 가리키는
 * 메뉴는 노란색을 글씨로 표현됩니다 인자로 메뉴 윈도우와 처음 가리키는 메뉴의
 * 위치를 받습니다. key에 입력된 방향키, 엔터키, 'q' 처리합니다. 방향키의 경우
 * 입력받은 값에 따라 선택을 표시하고 엔터키 입력 시 현재 커서가 가리키는 값을
 * 반환합니다. 'q'의 경우 선언해둔 QUIT값을 반환합니다*/
int scroll_auth(vector<WINDOW *> contents, int idx, int a) {
    int key;
    int selected = idx;
    int menu_length = contents.size() - 1;
    while (1) {
        key = getch();
        if (key == KEY_DOWN || key == KEY_UP) {
            wbkgd(contents[selected], COLOR_PAIR(2));
            wnoutrefresh(contents[selected]);
            if (key == KEY_DOWN) {
                selected = (selected + 1) % menu_length;
                if (selected == 0) {
                    selected = menu_length;
                }

            } else {
                selected = (selected + menu_length - 1) % menu_length;
                if (selected == 0) {
                    selected = menu_length;
                }
            }
            wbkgd(contents[selected], COLOR_PAIR(3));
            wnoutrefresh(contents[selected]);
            doupdate();
        } else if (key == ENTER) {
            return selected;
        } else if (key == 'q') {
            if (a) {
                return QUIT;
            }
        }
    }
}

/* scroll_file
 * 파일에 대한 현재 커서의 이동을 보이는 함수입니다. scroll_auth와 유사하고
 * add에 입력에 대한 추가 저리가 있습니다 */
int scroll_file(vector<WINDOW *> contents, int idx, int a) {
    int key;
    int selected = idx;
    int menu_length = contents.size() - 2;

    while (1) {
        key = getch();
        if (key == KEY_DOWN || key == KEY_UP) {
            wbkgd(contents[selected + 1], COLOR_PAIR(1));
            wnoutrefresh(contents[selected + 1]);
            if (key == KEY_DOWN) {
                selected = (selected + 1) % menu_length;
                if (selected == 0) {
                    selected = menu_length;
                }

            } else {
                selected = (selected + menu_length - 1) % menu_length;
                if (selected == 0) {
                    selected = menu_length;
                }
            }
            wbkgd(contents[selected + 1], COLOR_PAIR(5));
            wnoutrefresh(contents[selected + 1]);
            doupdate();
        } else if (key == ENTER) {
            return selected + 1;
        } else if (key == 'q') {
            return QUIT;
        } else if (key == 'a')
            if (a == 2)
                return ADD;
    }
}

/* login_window
 * 메뉴에서 로그인 선택 시 해당 화면을 나타내는 함수입니다. 사용자에게
 * id,password를 입력받고 해당 값을 userInfo 객체에 담아 반환합니다.*/
userInfo login_window() {
    echo();
    WINDOW *login;
    memset(id, 0x00, MAX_ID_LEN + 1);
    memset(password, 0x00, MAX_PW_LEN + 1);

    login = newwin(30, 25, 0, 75);
    wbkgd(login, COLOR_PAIR(2));
    wmove(login, 5, 4);
    wprintw(login, "ID: ");
    wmove(login, 8, 4);
    wprintw(login, "PW: ");
    mvwgetstr(login, 5, 8, id);
    mvwgetstr(login, 8, 8, password);
    noecho();
    wrefresh(login);
    return userInfo((string)id, (string)password);
}

/* signup_window
 * 회원가입 선택 시 화면을 나타내는 함수입니다. 입력받은 값으로 회원가입이
 * 가능하다면 Auth.dat에 저장해 가입하고 만약 중복된 아이디인 경우 팝업으로
 * 알립니다*/
void signup_window(WINDOW *titleScr) {
    userInfo curUser;
    int authfd = openAuthFile();

    while (1) {
        memset(id, 0x00, MAX_ID_LEN + 1);
        memset(password, 0x00, MAX_PW_LEN + 1);
        echo();
        WINDOW *signup;

        signup = newwin(30, 25, 0, 75);
        wbkgd(signup, COLOR_PAIR(2));
        wmove(signup, 5, 4);
        wprintw(signup, "ID: ");
        wmove(signup, 8, 4);
        wprintw(signup, "PW: ");
        mvwgetstr(signup, 5, 8, id);
        mvwgetstr(signup, 8, 8, password);
        wrefresh(signup);
        if (!strlen(id) || !strlen(password)) {
            noecho();
            alertWin("nothing inserted\ntry again");
            delwin(signup);
            werase(titleScr);
            drawTitle(titleScr);
            wrefresh(titleScr);
            continue;
        }
        noecho();
        break;
    }
    curUser.setId(id);
    curUser.setPassword(password);

    //중복아이디가 아닐 경우 계정 생성, 계정 생성 알람창 구현
    if (!isIdExist(authfd, curUser)) {
        registerId(authfd, curUser);
        alertWin("register Success!");
    }
    //아이디 중복임을 알려주고 다시 입력받음
    else {
        alertWin("Id exist try another");
    }

    close(authfd);
}

/* delete_menu
 * 페이지를 구성하는 윈도우를 vector에 저장하여 사용합니다. 방향키 입력에 대한
 * 변화 혹은 화면 전환이 있을 경우 앞서 기존 윈도우를 제거하고 변경된 값으로
 * 다시 그리는 방식으로 기존 윈도우를 제거하는 함수입니다 */
void delete_menu(vector<WINDOW *> contents) {
    for (int i = 0; i < contents.size(); i++) {
        delwin(contents[i]);
    }
}

/* study_page
 * 로그인한 유저가 study를 선택 시 적절한 윈도우를 생성하고 기능을 수행하는
 * 함수입니다.*/
void study_page(userInfo loginUser) {
    string path = "./Users/";
    path += loginUser.getId();
    DIR *dirp = opendir(path.c_str());
    // DIR *dirp = openLoginUserDir(loginUser.getId());
    struct dirent *dirInfo;
    vector<string> fileSet;

    while ((dirInfo = readdir(dirp)) != NULL) {
        fileSet.push_back(dirInfo->d_name);
    }
    closedir(dirp);
    vector<WINDOW *> contents = fileList(fileSet, 1);

    int cur = 1;

    bool isSelected = false;
    bool isExit = false;
    while (1) {
        cur = scroll_file(contents, 1, 1);
        werase(contents[0]);
        delete_menu(contents);
        if (cur == QUIT) {
            alertWin("exit study");
            break;
        } else if (fileSet[cur - 2].length() <= 3) {
            alertWin("select anothor ");
            touchwin(contents[0]);
            wrefresh(contents[0]);
            contents = fileList(fileSet, 1);
            continue;
        } else {
            vector<WordSet> wordsInFile =
                readWordFile(loginUser.getId(), fileSet[cur - 2]);
            if (wordsInFile.size() == 0) {
                noecho();
                alertWin("Empty File!\nselect another");
                contents = fileList(fileSet, 1);
                wrefresh(contents[0]);
                continue;
            }
            int curPage = 1;
            int k;
            int maxPage = wordsInFile.size() / 10;
            if (wordsInFile.size() % 10)
                maxPage += 1;
            while (1) {
                vector<WINDOW *> viewWord = drawWordSet(wordsInFile, curPage);
                k = getch();
                if (k == KEY_LEFT) {
                    if (curPage == 1)
                        curPage = maxPage;
                    else {
                        curPage -= 1;
                    }
                } else if (k == KEY_RIGHT) {
                    if (curPage == maxPage)
                        curPage = 1;
                    else {
                        curPage += 1;
                    }
                } else if (k == 'b') {
                    alertWin("study exit!");
                    isExit = true;
                    werase(viewWord[0]);
                    break;
                }
            }
            if (isExit) {
                break;
            }
        }
    }
}

/* inputPrompt
 * insert_page에서 add를 선택했을 경우에 해당하는 윈도우를 생성하는 함수입니다
 * .*/
vector<WINDOW *> inputPrompt(vector<WordSet> wordsInFile, string fileName) {
    vector<WINDOW *> addDisplay;
    addDisplay.push_back(newwin(30, 75, 0, 0));
    wbkgd(addDisplay[0], COLOR_PAIR(1));
    mvwprintw(addDisplay[0], 1, 45, "*Insert 'q' to exit ");
    addDisplay.push_back(subwin(addDisplay[0], 24, 65, 2, 3));
    wborder(addDisplay[1], 0, 0, 0, 0, 0, 0, 0, 0);
    addDisplay.push_back(subwin(addDisplay[0], 3, 28, 26, 3));
    addDisplay.push_back(subwin(addDisplay[0], 3, 28, 26, 40));
    wborder(addDisplay[2], ' ', ' ', ' ', 0, ' ', ' ', ' ', ' ');
    wborder(addDisplay[3], ' ', ' ', ' ', 0, ' ', ' ', ' ', ' ');
    mvwprintw(addDisplay[1], 1, 1, ">> %d words are in %s ", wordsInFile.size(),
              fileName.c_str());
    mvwprintw(addDisplay[2], 1, 1, "Eng: ");
    mvwprintw(addDisplay[3], 1, 1, "Kor: ");

    wrefresh(addDisplay[0]);
    return addDisplay;
}

/* insert_page
 * 로그인한 유저가 insert를 선택 시 적절한 윈도우를 생성하고 기능을 수행하는
 * 함수입니다.*/
void insert_page(userInfo loginUser) {
    string path = "./Users/";
    path += loginUser.getId();
    DIR *dirp = opendir(path.c_str());
    struct dirent *dirInfo;
    vector<string> fileSet;

    while ((dirInfo = readdir(dirp)) != NULL) {
        fileSet.push_back(dirInfo->d_name);
    }
    closedir(dirp);
    vector<WINDOW *> contents = fileList(fileSet, 2);

    int cur = 1;
    bool isAdded = false;
    bool isSelected = false;
    bool isExit = false;
    while (1) {
        if (isAdded) {
            fileSet.clear();
            path = "./Users/";
            path += loginUser.getId();
            dirp = opendir(path.c_str());
            while ((dirInfo = readdir(dirp)) != NULL) {
                fileSet.push_back(dirInfo->d_name);
            }
            closedir(dirp);
            contents = fileList(fileSet, 2);
            isAdded = false;
        }
        cur = scroll_file(contents, 1, 2);
        werase(contents[0]);
        delete_menu(contents);
        if (cur == ADD) {
            echo();
            char newFname[MAX_ID_LEN + 1];
            bool isExist = false;
            while (1) {
                memset(newFname, 0x00, MAX_ID_LEN + 1);
                isExist = false;
                WINDOW *addFileWin = newwin(5, 25, 12, 26);
                wmove(addFileWin, 2, 1);
                wprintw(addFileWin, ">>new FILE name: ");
                wbkgd(addFileWin, COLOR_PAIR(4));
                mvwgetstr(addFileWin, 3, 1, newFname);
                wrefresh(addFileWin);

                if (strlen(newFname) == 0) {
                    alertWin("Empty Input!\ntry again");
                    contents = fileList(fileSet, 2);
                    wrefresh(contents[0]);
                    continue;
                }

                for (int i = 0; i < fileSet.size(); i++) {
                    if (fileSet[i].length() <= 3)
                        continue;
                    if (newFname ==
                        fileSet[i].substr(0, fileSet[i].length() - 4)) {
                        alertWin("same name exist\ntry another");
                        isExist = true;
                        contents = fileList(fileSet, 2);
                        wrefresh(contents[0]);
                        break;
                    }
                }
                if (!isExist) {
                    break;
                    werase(contents[0]);
                    delete_menu(contents);
                }
            }
            path += "/";
            path += newFname;
            path += ".txt";
            creat(path.c_str(), 0777);
            isAdded = true;
            noecho();
            continue;
        } else if (cur == QUIT) {
            alertWin("exit insert!");
            break;
        } else if (fileSet[cur - 2].length() <= 3) {
            alertWin("select anothor ");
            touchwin(contents[0]);
            wrefresh(contents[0]);
            contents = fileList(fileSet, 2);
            continue;
        } else {
            echo();
            wrefresh(contents[0]);
            vector<WordSet> wordsInFile =
                readWordFile(loginUser.getId(), fileSet[cur - 2]);
            int xpos = 0;
            int ypos = 0;
            char uEng[MAX_WORD_LEN + 1];
            char uKor[MAX_WORD_LEN + 1];
            char idt = ' ';
            path = "./Users/";
            path += loginUser.getId();
            path += "/";
            path += fileSet[cur - 2];
            int fd = open(path.c_str(), O_WRONLY | O_APPEND);
            if (fd == -1) {
                perror("open error!");
                exit(-1);
            }

            vector<WINDOW *> addDisplay =
                inputPrompt(wordsInFile, fileSet[cur - 2]);
            int count = 0;
            while (1) {
                memset(uEng, 0x00, MAX_WORD_LEN + 1);
                memset(uEng, 0x00, MAX_WORD_LEN + 1);
                mvwgetstr(addDisplay[2], 1, 6, uEng);
                if (!strcmp(uEng, "q")) {
                    close(fd);
                    werase(addDisplay[0]);
                    wrefresh(addDisplay[0]);
                    isAdded = true;
                    break;
                }
                mvwgetstr(addDisplay[3], 1, 6, uKor);
                wmove(addDisplay[2], 1, 6);
                wclrtoeol(addDisplay[2]);
                wmove(addDisplay[3], 1, 6);
                wclrtoeol(addDisplay[3]);
                write(fd, uEng, strlen(uEng));
                write(fd, &idt, strlen(&idt));
                write(fd, uKor, strlen(uKor));
                write(fd, &idt, strlen(&idt));
                getyx(addDisplay[1], ypos, xpos);
                if (ypos == 22) {
                    for (int i = 1; i <= ypos; i++) {
                        wmove(addDisplay[1], i, 1);
                        wclrtoeol(addDisplay[1]);
                    }
                    wborder(addDisplay[1], 0, 0, 0, 0, 0, 0, 0, 0);
                    mvwprintw(addDisplay[1], 1, 1, ">> %d words are in file",
                              wordsInFile.size() + count);
                    ypos = 1;
                    // wmove(addDisplay[1], 1, 1);
                    // wclrtoeol(addDisplay[1]);
                }
                mvwprintw(addDisplay[1], ++ypos, 1,
                          ">>%d. (%s,%s) add success! ", ++count, uEng, uKor);
                touchwin(addDisplay[0]);
                wrefresh(addDisplay[0]);
            }
            close(fd);
            noecho();

            // wgetch(addDisplay[0]);
        }
    }
}

/* testView
 * test_page에서 화면을 구성하는 윈도우를 생성하는 함수입니다.*/
vector<WINDOW *> testView() {
    vector<WINDOW *> contents;
    contents.push_back(newwin(30, 75, 0, 0));
    wbkgd(contents[0], COLOR_PAIR(1));
    contents.push_back(subwin(contents[0], 20, 48, 3, 3));
    wborder(contents[1], 0, 0, 0, 0, 0, 0, 0, 0);
    contents.push_back(subwin(contents[0], 20, 15, 3, 54));
    wborder(contents[2], 0, 0, 0, 0, 0, 0, 0, 0);
    contents.push_back(subwin(contents[0], 3, 25, 24, 3));
    wborder(contents[3], ' ', ' ', ' ', 0, ' ', ' ', ' ', ' ');
    wrefresh(contents[0]);
    return contents;
}

/* test_page
 * 로그인한 유저가 test를 선택 시 적절한 윈도우를 생성하고 기능을 수행하는
 * 함수입니다.*/
void test_page(userInfo loginUser) {
    string path = "./Users/";
    path += loginUser.getId();
    DIR *dirp = opendir(path.c_str());
    struct dirent *dirInfo;
    vector<string> fileSet;

    while ((dirInfo = readdir(dirp)) != NULL) {
        fileSet.push_back(dirInfo->d_name);
    }
    closedir(dirp);
    vector<WINDOW *> contents = fileList(fileSet, 1);
    int cur = 1;

    bool isSelected = false;
    bool isExit = false;
    while (1) {
        cur = scroll_file(contents, 1, 1);
        werase(contents[0]);
        delete_menu(contents);
        if (cur == QUIT) {
            alertWin("exit test");
            break;
        } else if (fileSet[cur - 2].length() <= 3) {
            alertWin("select anothor ");
            touchwin(contents[0]);
            wrefresh(contents[0]);
            contents = fileList(fileSet, 1);
            continue;

        } else {
            vector<WordSet> wordsInFile =
                readWordFile(loginUser.getId(), fileSet[cur - 2]);
            int numberOfWords = wordsInFile.size();
            bool isInRange = false;
            int testNum = 0;
            echo();
            if (wordsInFile.size() == 0) {
                noecho();
                alertWin("Empty File!\nselect another");
                contents = fileList(fileSet, 1);
                wrefresh(contents[0]);
                continue;
            }
            while (1) {
                WINDOW *setNum = newwin(5, 25, 12, 26);
                wmove(setNum, 1, 1);
                wprintw(setNum, ">>Set test numbers ");
                wmove(setNum, 2, 1);
                wprintw(setNum, "(MAX: %d", wordsInFile.size());
                wmove(setNum, 3, 1);
                wbkgd(setNum, COLOR_PAIR(4));
                wscanw(setNum, "%d", &testNum);
                wrefresh(setNum);
                werase(setNum);
                delwin(setNum);
                if (testNum > 0 && testNum <= numberOfWords)
                    isInRange = true;
                else {
                    alertWin("not in proper range\ntry again!");
                    contents = fileList(fileSet, 1);
                    wrefresh(contents[0]);
                }
                if (isInRange)
                    break;
            }
            noecho();
            werase(contents[0]);
            touchwin(contents[0]);
            wrefresh(contents[0]);

            vector<WINDOW *> testDisplay = testView();
            int idx = 0;
            int ran = 0;
            int type = 0;
            int yloc = 1;
            vector<bool> isAsked(numberOfWords);
            vector<int> typeSet;
            vector<WordSet> AskedWord;
            vector<WordSet> wrongAns;
            char ans[MAX_WORD_LEN + 1];
            echo();
            srand(time(NULL));
            for (int i = 0; i < testNum; i++) {
                ran = rand();
                idx = ran % numberOfWords;
                if (isAsked[idx]) {
                    i--;
                    continue;
                }
                isAsked[idx] = true;
                type = ran % 2;
                AskedWord.push_back(wordsInFile[idx]);
                typeSet.push_back(type);
            }

            mvwprintw(testDisplay[0], 1, 3, "Cheer up! ");

            for (int i = 0; i < testNum; i++) {
                memset(ans, 0x00, MAX_WORD_LEN + 1);
                if (typeSet[i] == 1)
                    mvwprintw(testDisplay[1], yloc, 1, "%d. %s 의 뜻을 쓰시오 ",
                              i + 1, AskedWord[i].getEng().c_str());
                else
                    mvwprintw(testDisplay[1], yloc, 1,
                              "%d. %s에 해당하는 단어를 쓰시오 ", i + 1,
                              AskedWord[i].getKor().c_str());
                mvwprintw(testDisplay[3], 1, 1, "Ans:");
                touchwin(testDisplay[0]);
                wrefresh(testDisplay[0]);
                mvwgetstr(testDisplay[3], 1, 5, ans);
                wmove(testDisplay[3], 1, 5);
                wclrtoeol(testDisplay[3]);
                if (typeSet[i]) {
                    if (ans == AskedWord[i].getKor()) {
                        mvwprintw(testDisplay[2], yloc, 1, "%d. Right!", i + 1);
                    } else {
                        mvwprintw(testDisplay[2], yloc, 1, "%d. Wrong!", i + 1);
                        wrongAns.push_back(AskedWord[i]);
                    }
                } else {
                    if (ans == AskedWord[i].getEng()) {
                        mvwprintw(testDisplay[2], yloc, 1, "%d. Right!", i + 1);
                    } else {
                        mvwprintw(testDisplay[2], yloc, 1, "%d. Wrong!", i + 1);
                        wrongAns.push_back(AskedWord[i]);
                    }
                }
                yloc++;
                if (yloc == 19) {
                    for (int i = 1; i <= yloc; i++) {
                        wmove(testDisplay[1], i, 1);
                        wclrtoeol(testDisplay[1]);
                        wmove(testDisplay[2], i, 1);
                        wclrtoeol(testDisplay[2]);
                    }
                    wborder(testDisplay[1], 0, 0, 0, 0, 0, 0, 0, 0);
                    wborder(testDisplay[2], 0, 0, 0, 0, 0, 0, 0, 0);
                    yloc = 1;
                }
            }
            noecho();
            WINDOW *testResult = newwin(10, 45, 10, 20);
            wborder(testResult, '|', '|', '-', '-', 0, 0, 0, 0);
            wbkgd(testResult, COLOR_PAIR(5));
            wmove(testResult, 1, 15);
            wprintw(testResult, "*Test Result*");
            wmove(testResult, 2, 1);
            wprintw(testResult, "Right: %d Wrong: %d",
                    testNum - wrongAns.size(), wrongAns.size());
            wmove(testResult, 7, 1);
            wprintw(testResult, "Do you want to see Wrong answers?");
            wmove(testResult, 8, 1);
            wprintw(testResult, ">>Press 'y' Yes,or 'n' No");

            int key = 0;
            while (1) {
                key = wgetch(testResult);
                if (key == 'y' || key == 'n') {
                    break;
                }
            }
            if (key == 'n') {
                alertWin("test exit!");
                break;
            }
            if (key == 'y') {
                if (wrongAns.size() == 0) {
                    alertWin("Nothing to show!");
                    break;
                }
                werase(testDisplay[0]);
                touchwin(testDisplay[0]);
                wrefresh(testDisplay[0]);
                delete_menu(testDisplay);

                int curPage = 1;
                int k;
                int maxPage = wrongAns.size() / 10;
                if (wrongAns.size() % 10)
                    maxPage += 1;
                while (1) {
                    vector<WINDOW *> viewWord = drawWordSet(wrongAns, curPage);

                    k = getch();
                    if (k == KEY_LEFT) {
                        if (curPage == 1)
                            curPage = maxPage;
                        else {
                            curPage -= 1;
                        }
                    } else if (k == KEY_RIGHT) {
                        if (curPage == maxPage)
                            curPage = 1;
                        else {
                            curPage += 1;
                        }
                    } else if (k == 'b') {
                        alertWin("show Wrong Word exit!");
                        werase(viewWord[0]);
                        break;
                    }
                }
                break;
            }
        }
    }
}
