#include "incl.h"
#include "gobang.h"
#include <csignal>

gobang *gb;
int fd;
bool running = true;

void signalHandler(int signum) {
    log(" 触发Ctrl+C ");
    running = false;

    for (int i = 0; i < gb->tp_num; i++) {
        delete gb->tp_list[gb->tp_num];
    }

    delete gb;

    log("delete ok !");
    close(fd);  // 关闭监听套接字以中断 accept
}

int main() {
    // 注册信号处理函数
    std::signal(SIGINT, signalHandler);

    log("Hello Ubuntu");

    /* get linux socket device */
    fd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > fd) {
		err("linux-sever socket fall! %d", fd);	
		return -1;
	} else {
        log("Apply to socket succeed");
    }

    /* apply post */
    struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == SO_ERROR) {
		err("端口不可用 %d", PORT);
		return -1;
	} else {
        log("post: %d available", PORT);
    }
    
    /* start accept */
	listen(fd, _GAME_USER_MAX_);
	log("linux-sever 启动成功...");

    gb = new gobang();
    log("gobang-sever 创建成功...");

    /* 为加入网络的设备添加线程 */
	for (;;) {
		int clifd = accept(fd, NULL, NULL);
        if (!running) break;
		if (clifd < 0) {

			err("socket:%d join 失败", fd);
		} else {
            if (gb->tp_num < _GAME_USER_MAX_) {
                
                /* 创建一个通信线程 */
                gb->tp_list[gb->tp_num] = new tcp_pthread(clifd);
                gb->tp_num++;//用户数+1

                log("连接成功 clifd:%d", clifd);
            } else {
                err("已达最大用户数目，无法添加");
            }
			
		}
	}

    // close(fd);
    log("linux-sever 关闭成功...");
    return 0;
}
