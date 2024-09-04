#include "tcp_pthread.h"

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


/**
 * @brief  根据上位机命令做出对应响应
 * @param  void
 * @retval void
 */
void tcp_pthread::DeerSwitch(void)
{
	if (this->buff[3]<60) {		
		log("%d开始说小话", this->buff[3]);
		return;
	}

	switch(this->buff[3])
	{
		case A_POST_CREATE_HOUSE:		/* 应用端 */
			log("app post create house");
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
