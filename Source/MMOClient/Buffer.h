// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


/*-------------------------------
	RecvBuffer
-------------------------------*/
class MMOCLIENT_API FRecvBuffer {
public:
	FRecvBuffer(int32 size = 1000);
	~FRecvBuffer();

	bool OnRead(int32 len);
	bool OnWrite(int32 len);
	void Reset();

	uint8* BufferReadPos();
	uint8* BufferWritePos();

	int32 DataSize();
	int32 FreeSize();
public:
	uint8* _buffer;
	int32 _capacity;
	int32 _size;
	int32 _readPos;
	int32 _writePos;
};

/*------------------------------
	SendBuffer
------------------------------*/
class MMOCLIENT_API FSendBuffer {
public:
	FSendBuffer(int size);
	~FSendBuffer();

	uint8* Buffer();
	int Size();
public:
	uint8* _buffer;
	int _size;
};