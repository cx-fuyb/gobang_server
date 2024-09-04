/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INCLUDE_H
#define INCLUDE_H

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#define PORT                (1234)
#define _GAME_USER_MAX_     500

#define log(fmt,...)    printf("\e[42m[%s] %s:%d\e[0m--"#fmt"\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define err(fmt,...)    printf("\e[41m[%s] %s:%d\e[0m--"#fmt"\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

/* c */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/* linux */
#include "sys/socket.h"
#include <thread>
#include "netinet/in.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include<ctime>

/* driver */

#endif  /* INCLUDE_H */

/******************* (H) COPYRIGHT 2024 FUYEBING*****END OF FILE************/
