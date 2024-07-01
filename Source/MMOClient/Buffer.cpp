// Fill out your copyright notice in the Description page of Project Settings.


#include "Buffer.h"


/*----------------------------------------
    RecvBuffer
----------------------------------------*/
FRecvBuffer::FRecvBuffer(int32 size) : _size(size), _readPos(0), _writePos(0)
{
    _capacity = _size * 10;
    _buffer = new uint8[_capacity];
}

FRecvBuffer::~FRecvBuffer()
{
    delete _buffer;
}

bool FRecvBuffer::OnRead(int32 len)
{
    if (DataSize() < len) 
        return false;

    _readPos += len;

    return true;
}

bool FRecvBuffer::OnWrite(int32 len)
{
    if (FreeSize() < len) return false;

    _writePos += len;

    return true;
}

void FRecvBuffer::Reset()
{
    const int dataSize = DataSize();

    if (dataSize == 0) {
        _readPos = _writePos = 0;
        return;
    }
    
    if (FreeSize() < _size) {
        ::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
        _writePos = dataSize;
        _readPos = 0;
    }
}

uint8* FRecvBuffer::BufferReadPos()
{
    return &_buffer[_readPos];
}

uint8* FRecvBuffer::BufferWritePos()
{
    return &_buffer[_writePos];
}

int32 FRecvBuffer::DataSize()
{
    return _writePos - _readPos;
}

int32 FRecvBuffer::FreeSize()
{
    return _capacity - _writePos;
}

/*-------------------------------
    SendBuffer
--------------------------------*/
FSendBuffer::FSendBuffer(int size) : _size(size)
{
    _buffer = new uint8[_size];
}

FSendBuffer::~FSendBuffer()
{
    delete _buffer;
}

uint8* FSendBuffer::Buffer()
{
    return _buffer;
}

int FSendBuffer::Size()
{
    return _size;
}
