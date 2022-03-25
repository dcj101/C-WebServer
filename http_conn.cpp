#include "http_conn.h"
int http_conn::m_epollfd = -1; // 所有的sockt的事件都被注册到同一个epoll
int http_conn::m_user_count = 0;  // 统计用户数量


// 设置文件描述符非阻塞
void setnonblocking(int fd) {
    int old_flag = fcntl(fd,F_GETFL);
    int new_flag = old_flag | O_NONBLOCK;
    fcntl(fd,F_SETFL,new_flag);
}

// 向epoll 中添加需要监听的文件描述符
void addfd(int epollfd, int fd, bool one_shot) {
    epoll_event event;
    event.data.fd = fd;
    // event.events = EPOLLIN | EPOLLRDHUP; // 设置边缘触发 HUP 挂起异常断开可以挂起
    event.events = EPOLLIN | EPOLLRDHUP | EPOLLET; // 设置边缘触发 HUP 挂起异常断开可以挂起
    
    if(one_shot) { // 假如你的一个socket被一个线程读取了数据去处理了，
   // 但是突然现在这个socket又可以读了，可能线程池又分配了新的线程的处理这个socket的，出现了
   //一个socket被两个线程处理 
        event.events | EPOLLONESHOT;
    }
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
    // 设置文件描述符非阻塞
    setnonblocking(fd);
}

void removefd(int epollfd,int fd) {
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}


// 修改文件描述符
void modfd(int epollfd, int fd, int ev) {
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLONESHOT | EPOLLRDHUP;
    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&event);
}

void http_conn::init(int sockfd,const sockaddr_in &addr)  {
    m_sockfd = sockfd;
    m_address = addr;

    // 端口复用
    int reuse = 1;
    setsockopt(m_sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
    
    // 添加到epoll 对象中
    addfd(m_epollfd,sockfd, true);
    m_user_count ++; // 总用户++;
}
// 关闭连接
void http_conn::close_conn() {
    if(m_sockfd != -1) { // 还未关闭
        removefd(m_epollfd, m_sockfd);
        m_sockfd = -1;
        m_user_count --;
    }
}

bool http_conn::read()  {
    // printf("read\n");
    if(m_read_idx >= READ_BUFFER_SIZE) {
        printf("buff 不够");
        return false;
    }
    // 读取到的字节
    int byte_read = 0;
    while(true) {
        byte_read = recv(m_sockfd,m_read_buf + m_read_idx, READ_BUFFER_SIZE - m_read_idx,0);
        if(byte_read == -1) {
            if( errno == EAGAIN || errno == EWOULDBLOCK) {
                //没数据读啦
                break;
            }
            return false;
        } else if(byte_read == 0) {
            // 对方关闭连接
            return false;
        }
        m_read_idx += byte_read;
    } 
    printf("读取到的数据: %s\n\n\n",m_read_buf);
    return true;
}
bool http_conn::write() {
    printf("write\n");
    return true;
} 
// 有线程池中的工作线程调用的， 这是处理的http请求入口函数
void http_conn::process() {
    // 解析http 请求
    // process_read();
    printf("http解析\n");
    // 生成响应

}