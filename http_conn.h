#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h> // 文件描述符操作
#include <sys/epoll.h>
#include "locker.h"
#include "threadpool.h"
#include "http_conn.h"
#include <signal.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/uio.h>

class http_conn {
    public:
        // 状态机模型
       // HTTP请求方法
        enum METHOD {GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS,CONNECT};
         /*
         
          解析客户端请求时，主状态机的状态
          CHECK_STATE_REQUESTKINE : 请求行
          CHECK_STATE_HEADER ： 请求头部字段
          CHECK_STATE_CONTENT ： 请求体

         */
        
        enum CHECK_STATE {CHECK_STATE_REQUESTKINE = 0, CHECK_STATE_HEADER, CHECK_STATE_CONTENT};
        
        /*
          服务器HTTP 请求的可能结果，报文解析的结果
          NO
        
        */
        enum HTTP_CODE {NO_REQUEST, GET_REQUEST, BAD_REQUEST, NO_RESOUSE};

        static int m_epollfd; // 所有的sockt的事件都被注册到同一个epoll
        static int m_user_count; // 统计用户数量
        static const int READ_BUFFER_SIZE = 2048;
        static const int WRITE_BUFFER_SIZE = 2048;
        http_conn() {

        }
        ~http_conn() {

        }
        void process(); // 处理客户端请求， 响应，找到资源。拼接程详细信息
        void init(int sockfd,const sockaddr_in &addr); // 初始化接受的连接
        void close_conn(); // 关闭连接
        bool read(); // 非阻塞 读写
        bool write(); 
    private:
        int m_sockfd; // 该HTTP链接的socket的
        sockaddr_in m_address; // 通信socket地址
        char m_read_buf[READ_BUFFER_SIZE]; // 读数组
        int m_read_idx; // 表示读的最后的位置是哪里

};

#endif