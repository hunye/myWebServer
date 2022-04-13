#include "epoller.h"
// 如果监听的文件描述符数量超过 size, 则内核会自动扩容。
// 而现在 size 已经没有这种语义了，但是调用者调用时 size 依然必须大于 0，以保证后向兼容性。
Epoller::Epoller(int maxEvent):epollerFd_(epoll_create(512)), events_(maxEvent){
    assert(epollerFd_>=0 && events_.size()>0);
}

Epoller::~Epoller(){
    close(epollerFd_);
}

bool Epoller::addFd(int fd, uint32_t events){
    if(fd<0) return false;
    epoll_event ev={0};
    ev.data.fd=fd;
    ev.events=events;
    return 0==epoll_ctl(epollerFd_, EPOLL_CTL_ADD, fd, &ev);
}
bool Epoller::modFd(int fd, u_int32_t events){
    if(fd<0) return false;
    epoll_event ev={0};
    ev.data.fd=fd;
    ev.events=events;
    return 0==epoll_ctl(epollerFd_, EPOLL_CTL_MOD, fd, &ev);
}
bool Epoller::delFd(int fd){
    if(fd<0) return false;
    epoll_event ev={0};
    return 0==epoll_ctl(epollerFd_, EPOLL_CTL_DEL, fd, &ev);
}
int Epoller::wait(int timeoutMs){
    return epoll_wait(epollerFd_, &events_[0], static_cast<int>(events_.size()), timeoutMs);
}
int Epoller::getEventFd(size_t i)const{
    assert(i<events_.size() && i>=0);
    return events_[i].data.fd;
}
uint32_t Epoller::getEvents(size_t i)const{
    assert(i<events_.size() && i>=0);
    return events_[i].events;
}

