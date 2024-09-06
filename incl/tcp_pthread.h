/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TCP_PTHREAD_H
#define TCP_PTHREAD_H

#include "incl.h"


#define 		A_POST_CREATE_HOUSE				0xE1
#define 		A_JOIN_SERVER_ID                0xE2
#define 		A_JOIN_SERVER_SUCCEED_ID        0xE3
#define 		A_POST_GET_HOUSE                0xE4
#define 		A_JOIN_ROOM_ID                  0xE5
#define 		A_JOIN_ROOM_SUCCEED_ID          0xE6
#define 		A_TELL_DATA                     0xE7
#define 		A_TELL_STRING                   0xE8

class tcp_pthread
{
public:
    int out_time;

    /* data */
    uint16_t fd;

private:
    pthread_t thread;
    
    uint8_t step;
	uint8_t cnt;
	uint8_t buff[100];
	uint8_t rxlen;
    void DeerSwitch(void);
    void DeerParsing(void);
    void fb_tx(int fb, uint8_t fun, uint8_t *date, uint8_t datelen);
    void tx_list(uint16_t fb, std::string str);
public:
    void analysis(uint8_t date);
    void tx(uint8_t fun, uint8_t *date, uint8_t datelen);
    void tx_uint8(uint8_t fun, uint8_t date);
    void tx_uint16(uint8_t fun, uint16_t date);

    tcp_pthread(int fd);
    ~tcp_pthread();
};

#endif  /* TCP_PTHREAD_H */

/******************* (H) COPYRIGHT 2024 FUYEBING*****END OF FILE************/
