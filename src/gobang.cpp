#include "gobang.h"

gobang::gobang(/* args */)
{
    Match_list.fd_num = 0;
    Match_ing.fd_num = 0;

    for (int i = 0; i < _GAME_USER_MAX_; i++)
    {
        Match_list.fd[i] = 0;
        Match_ing.fd[i][0] = 0;
        Match_ing.fd[i][1] = 0;
    }
    
}

gobang::~gobang()
{
}

void gobang::create_room(int fd, char *str, int len)
{
    int i ;
    
    for (i = 0; i < _GAME_USER_MAX_; i++)
    {
        /* 判断哪里有位置 */
        if (Match_list.fd[i] == 0) {
            Match_list.fd[i] = fd;

            std::string newstr(str);

            Match_list.room_name[i].append(newstr);
            break;
        }
    }

    Match_list.fd_num++;
    log("room name:%s", Match_list.room_name[i].c_str());
}

int gobang::get_list_numToitem(int num)
{
    
    return 0;
}