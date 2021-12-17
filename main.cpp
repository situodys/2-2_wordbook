#include "MyUI.hpp"
#include "WordSet.hpp"
#include <iostream>
#include <locale.h>
#include <ncursesw/ncurses.h>

using namespace std;

int main(int argc, char const *argv[]) {
    cout << "\e[8;30;100t";
    WINDOW *titleScr;
    vector<WINDOW *> login_menu;
    int cursor = 0;
    init_scr();
    bkgd(COLOR_PAIR(1));

    //사용자 정보를 담을 폴더 생성
    mkdir("./Users", 0777);

    titleScr = subwin(stdscr, 30, 75, 0, 0);

    drawTitle(titleScr);
    refresh();
    login_menu = auth_menu_list();
    bool isfirst = true;
    bool is_once_loggedin = false;

    while (1) {
        if (isfirst) {
            isfirst = false;
            cursor = scroll_auth(login_menu, 1, 0);
        } else {
            cursor = scroll_auth(login_menu, cursor, 0);
        }

        //로그인
        if (cursor == 1) {
            userInfo input = login_window();
            //로그인 된 경우
            if (checkAuth(input)) {
                do {
                    werase(titleScr);
                    drawTitle(titleScr);
                    wrefresh(titleScr);
                    cursor = 0;
                    delete_menu(login_menu);
                    login_menu = user_login_list();
                    cursor = scroll_auth(login_menu, 1, 0);

                    // study, insert, test화면으로전환
                    if (cursor == 1) {
                        delete_menu(login_menu);
                        login_menu = user_start_list();
                        cursor = scroll_auth(login_menu, 1, 1);
                        do {
                            werase(titleScr);
                            wrefresh(titleScr);
                            if (cursor == 1) {
                                //단어장 보는 페이지
                                study_page(input);
                            } else if (cursor == 2) {
                                //단어 추가 하기
                                insert_page(input);
                            } else if (cursor == 3) {
                                //단어 시험 보기
                                test_page(input);
                            } else if (cursor == QUIT) {
                                delete_menu(login_menu);
                                break;
                            }
                            cursor = scroll_auth(login_menu, cursor, 1);
                        } while (1);

                    }
                    //로그아웃하기
                    else if (cursor == 2) {
                        delete_menu(login_menu);
                        login_menu = auth_menu_list();
                        cursor = 0;
                        isfirst = true;
                        break;
                    }
                    //종료하기
                    else if (cursor == 3) {
                        endwin();
                        return 0;
                    }
                    // login_menu = user_login_list();
                    // touchwin(login_menu[0]);
                    // wrefresh(login_menu[0]);
                } while (1);
            } else {
                alertWin("Wrong Id or Password");
            }
        }
        //회원가입
        else if (cursor == 2) {
            signup_window(titleScr);
        }
        //종료
        else if (cursor == 3) {
            endwin();
            return 0;
        } /*else if (cursor == QUIT) {
            isfirst = false;
            continue;
        }*/
        touchwin(stdscr);
        touchwin(login_menu[0]);
        wrefresh(stdscr);
        wrefresh(login_menu[0]);
    }

    getch();
    endwin();
    return 0;
}
