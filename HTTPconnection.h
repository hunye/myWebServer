#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H


#include<assert.h>
#include<sys/uio.h> //readv, writev
#include<iostream>
#include<arpa/inet.h> //sockaddr_in
#include<sys/types.h>

#include "buffer.h"
#include "HTTPrequest.h"
#include "HTTPresponse.h"

class HTTPconnection{
public:
    HTTPconnection();
    ~HTTPconnection();

    void initHTTPConn(int socketFd, const sockaddr_in& addr);
    
    //关闭http连接的接口
    void closeHTTPConn();
    //处理http连接接口 主要分为resquest解析和response生成
    bool handleHTTPConn();

    //其他方法
    const char* getIP() const;
    int getPort() const;
    sockaddr_in getAddr() const;

    int writeBytes(){
        return iov_[1].iov_len+iov_[0].iov_len;
    }

    // bool isKeepAlive() const{

    // }

    static bool isET;
    static const char* srcDir;
    static std::atomic<int> userCount;
    
private:
    int fd_; //http 连接描述符
    struct sockaddr_in addr_;
    bool isClose_;

    int iovCnt_;
    struct iovec iov_[2];

    Buffer readBuffer_;
    Buffer writeBuffer_;

    HTTPrequest request_;
    HTTPresponse response_;
};

#endif