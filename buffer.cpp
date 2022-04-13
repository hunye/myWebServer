#include "buffer.h"

Buffer::Buffer(int initBuffersize):buffer_(initBuffersize), readPos_(0), writePos_(0){}
size_t Buffer::writeableBytes() const
{
    // |begin()----readPos_*******writePos_ ---------end()|
    return buffer_.size()-writePos_;
}
size_t Buffer::readableBytes() const
{
    return writePos_ - readPos_;
}
size_t Buffer::readBytes() const
{
    return readPos_;
}
const char* Buffer::curReadPtr() const
{
    return BeginPtr_()+readPos_;
}
const char* Buffer::curWritePtrConst() const
{
    return BeginPtr_()+writePos_;
}
char* Buffer::curWritePtr()
{
    return BeginPtr_()+writePos_;
}
void Buffer::updateReadPtr(size_t len)
{
    assert(len<=readableBytes());
    readPos_+=len;
}
void Buffer::updateReadPtrUntilEnd(const char* end)
{
    assert(end>=curReadPtr());
    updateReadPtr(end-curReadPtr());
}
void Buffer::updateWritePtr(size_t len)
{
    assert(len<=writeableBytes());
    writePos_+=len;
}
void Buffer::initPtr()
{
    bzero(&buffer_[0], buffer_.size());
    readPos_=0;
    writePos_=0;
}
void Buffer::allocateSpace(size_t len)
{
    // |begin()----readPos_*******writePos_ ---------end()|
    if(writeableBytes()+readBytes()<len)//剩余空间不够len, 剩余空间包括头和尾，特别是头
    {
        buffer_.resize(writePos_+len+1);
    }
    else//剩余控件有len，将buffer移动到begin()
    {
        size_t readable=readableBytes();
        std::copy(BeginPtr_()+readPos_, BeginPtr_()+writePos_, BeginPtr_());
        readPos_=0;
        writePos_=readable;
        assert(readable==readableBytes());
    }
}
void Buffer::ensureWriteable(size_t len)
{
    if(writeableBytes()<len)
    {
        allocateSpace(len);
    }
    assert(writeableBytes()>=len);
}
void Buffer::append(const char* str, size_t len){
    assert(str);
    ensureWriteable(len);
    std::copy(str, str+len, curWritePtr());
    updateWritePtr(len);
}
void Buffer::append(const std::string& str)
{
    //str.data(): string to char array
    append(str.data(), str.length());
}
void Buffer::append(const void* data, size_t len)
{
    assert(data);
    append(static_cast<const char*>(data), len);
}
void Buffer::append(const Buffer& buffer)
{
    append(buffer.curReadPtr(), buffer.readableBytes());
}
ssize_t Buffer::readFd(int fd, int* Errno){
    char buff[65535];
    struct iovec iov[2];
    const size_t writeable=writeableBytes();

    iov[0].iov_base=BeginPtr_()+writePos_;
    iov[0].iov_len=writeable;
    iov[1].iov_base=buff;
    iov[1].iov_len=sizeof(buff);

    const ssize_t len=readv(fd, iov, 2);//先存在buffer_里，然后存到buff数组里
    if(len<0){
        //std::cout<<"从fd读取数据失败"<<std::endl;
        *Errno=errno;
    }
    else if(static_cast<size_t>(len)<=writeable)//如果buffer_内存充足
    {
        writePos_+=len;
    }
    else//如果buffer_内存不充足
    {
        writePos_=buffer_.size();
        append(buff, len-writeable);
    }
}
std::string Buffer::AlltoStr()
{
    std::string str(curReadPtr(), readableBytes());
    initPtr();
    return str;
}
char * Buffer::BeginPtr_()
{
    return &*buffer_.begin();
}
const char* Buffer::BeginPtr_() const
{
    return &*buffer_.begin();
}