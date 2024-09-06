/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef GOBANG_H
#define GOBANG_H

#include "tcp_pthread.h"
#include "incl.h"
#include "string.h"

typedef struct {
    uint16_t fd[_GAME_USER_MAX_];  //哪个用户
    std::string room_name[_GAME_USER_MAX_]; 
    uint16_t fd_num;        //有几个用户
}Match_list_t;
    
class gobang
{
private:

public:
    /* 大厅正在寻找对手的信息 */
    Match_list_t Match_list = {
        .fd_num = 0,
    };

    /* 正在打比赛的信息 */
    struct {
        uint16_t fd[_GAME_USER_MAX_][2];   //哪两个用户
        uint16_t fd_num;            //有几对用户
    }Match_ing = {
        .fd_num = 0,
    };

    //线程数
    tcp_pthread *tp_list[_GAME_USER_MAX_];
    uint16_t tp_num = 0;
public:
    gobang(/* args */);
    ~gobang();

    int get_list_numToitem(int num);
    void create_room(int fd, char *str, int len);
};

#endif  /* GOBANG_H */

/******************* (H) COPYRIGHT 2024 FUYEBING*****END OF FILE************/