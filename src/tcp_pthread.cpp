#include "tcp_pthread.h"
#include "gobang.h"
extern gobang *gb;

void *sever_dispose_thread(void *args)
{
	tcp_pthread *tp  = (tcp_pthread *)args;

    // able to be cancel
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

	for (;;) {
		char revData;
		int ret=recv(tp->fd, &revData,1,0);
        
		if (ret > 0) {
            tp->out_time = 0;
            uint8_t tcp_date = revData & 0xff;
            log("%d", revData);
            tp->analysis(tcp_date);
		}
	}
}

tcp_pthread::tcp_pthread(int fd)
{
    this->fd = fd;
    this->out_time = 0;

    this->cnt = 0;
	this->step = 0;

    log("正在创建线程 %d", this->fd);
    pthread_create(&this->thread, NULL, sever_dispose_thread,(void *)this);
    log("线程创建成功");
}
void tcp_pthread::tx_list(uint16_t tx_fb, std::string str)
{
	char data[40];
	int i;

	data[0] = tx_fb>>8;
	data[1] = tx_fb&0x00ff;

	for (i = 2; i<str.length()+1+2; i++) {
		data[i] = str.c_str()[i-2];
		//log("%x", str.c_str()[i-2]);
	}
	
	log("txfd:%d, len:%d  str:%s", tx_fb, (int)str.length()+1+2, str.c_str());
	this->tx(A_POST_GET_HOUSE, (uint8_t *)data, str.length()+1+2);
}

/**
 * @brief  根据上位机命令做出对应响应
 * @param  void
 * @retval void
 */
void tcp_pthread::DeerSwitch(void)
{
	switch(this->buff[3])
	{
		case A_POST_CREATE_HOUSE:		/* 创建房间 */
			log("app post create house");
			gb->create_room(this->fd, (char *)&this->buff[4], 0);
			this->tx_uint8(A_POST_CREATE_HOUSE, 0);
			break;
		case A_JOIN_SERVER_ID:		/* 用户获取id */
			this->tx_uint16(A_JOIN_SERVER_SUCCEED_ID, this->fd);
			break;
		case A_POST_GET_HOUSE:		/* 用户获取id */
			log("收到请求");
			for (int i = 0; i < gb->Match_list.fd_num; i++) {
				if (gb->Match_list.fd[i] != 0) {
					tx_list(gb->Match_list.fd[i], gb->Match_list.room_name[i]);	
					//log("发送的时候:%s", gb->Match_list.room_name[i].c_str());
					//log("等待");
					//sleep(1);
					usleep(50);
					//log("等待结束");
				}
			}
			
			tx_list(0, "");	
			break;
		case A_JOIN_ROOM_ID:{		/* 加入房间 */
			
			int join_num = (this->buff[4]<<8) | (this->buff[5]);
			log("join fd%d", join_num);

			for (int i = 0; i < _GAME_USER_MAX_; i++)
			{
				if (gb->Match_list.fd[i] != 0) {
					if (gb->Match_list.fd[i] == join_num) {
						log("join succeed!");
						this->tx_uint16(A_JOIN_ROOM_SUCCEED_ID, join_num);
						this->fb_tx(join_num, A_JOIN_ROOM_SUCCEED_ID, (uint8_t *)&this->fd, 2);

						log("网络月老牵线成功!");
					}
				}
			}
			
			break;
		}
		case A_TELL_DATA:		/* 用户获取id */
			this->tx_uint16(A_JOIN_SERVER_SUCCEED_ID, this->fd);
			break;
	}
}

/**
* @brief  上位机指令解析 
 * @param  void
 * @retval void
 */
void tcp_pthread::DeerParsing(void)
{
	uint8_t ack = 0;
	
	for (int i=0; i < this->rxlen; i++) {
		ack += this->buff[i];
	}
	
	if (ack == this->buff[this->rxlen]) {
		this->DeerSwitch();
		out_time = 0;
		//update_flag = 1;
	}	
}

void tcp_pthread::analysis(uint8_t date)
{
	switch(this->step) {
		case 0:
			this->cnt = 0;
			this->buff[this->cnt++] = date;
			if (date == 0xFD) {
				this->step = 1;
			}
			break;
		case 1:
			this->buff[this->cnt++] = date;
			if (date == 0xDF) {
				this->step = 2;
			} else {
				this->step = 0;
			}
			break;
		case 2:
			this->buff[this->cnt++] = date;
		
			if (this->cnt - 1 == 2) {
				this->rxlen = date;
			}
			
			if (this->cnt > this->rxlen) {
				this->step = 3;
				this->cnt = 0;
			}
			
			break;
			
		default :
		{};
	}
	
	if (this->step == 3) {
		this->step = 0;
		this->DeerParsing();
	}
}

void tcp_pthread::fb_tx(int fb, uint8_t fun, uint8_t *date, uint8_t datelen)
{
	uint8_t SendBuff[70];
    uint8_t len = 0;

    /* 帧头 */
    SendBuff[len++] = 0xFD;
    SendBuff[len++] = 0xDF;

    /* 总长度 先占位 */
    SendBuff[len++] = 0;

    /* 功能帧 */
    SendBuff[len++] = fun;

    /* 数据 */
    for (int max = (datelen + len), now = len; len < max; len++) {
        SendBuff[len] = date[len - now];
    }

    /* 获取实际长度 */
    SendBuff[2] = len;

    /* 校验位 */
    SendBuff[len] = 0;
    for (int i = 0; i < len; i++) {
        SendBuff[len] += SendBuff[i];
    }

	if (send(fb, SendBuff, len+1, MSG_NOSIGNAL) == -1) {
		//err("发送失败%d", fb);
	} else {
		//err("发送成功%d", fb);
	}
}

/**
 * @brief  通信发送底层函数
 * @param  void
 * @retval void
 */
void tcp_pthread::tx(uint8_t fun, uint8_t *date, uint8_t datelen)
{
    fb_tx(this->fd, fun, date, datelen);
}
/**
 * @brief  发送16位数据
 * @param  void
 * @retval void
 */
void tcp_pthread::tx_uint8(uint8_t fun, uint8_t date)
{
   	this->tx(fun, &date, 1);
}

/**
 * @brief  发送16位数据
 * @param  void
 * @retval void
 */
void tcp_pthread::tx_uint16(uint8_t fun, uint16_t date)
{
    uint8_t txdate[2];
    txdate[0] = (uint8_t)(date >> 8);
    txdate[1] = (uint8_t)date;

   	this->tx(fun, txdate, 2);
}

tcp_pthread::~tcp_pthread()
{

}
