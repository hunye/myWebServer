#include "HTTPconnection.h"

const char* HTTPconnection::srcDir;
std::atomic<int> HTTPconnection::userCount;

HTTPconnection::HTTPconnection(){
    fd_=-1;
    addr_={0};
    isClose_=true;
};

HTTPconnection::~HTTPconnection(){
    closeHTTPConn();
};

void HTTPconnection::initHTTPConn(int fd, const sockaddr_in& addr){
    assert(fd>0);
    userCount++;
    addr_=addr;
    fd_=fd;
    writeBuffer_.initPtr();
    readBuffer_.initPtr();
    isClose_=false;
}

void HTTPconnection::closeHTTPConn(){
    //TODO: reponse_
    if(isClose_==false){
        isClose_=true;
        userCount--;
        close(fd_);
    }

}