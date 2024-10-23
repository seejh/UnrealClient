// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class AMyPlayerController;
class UMyGameInstance;
class FRecvBuffer;
class FSendBuffer;
class MMOCLIENT_API FNetSession : FRunnable
{
public:
	FNetSession(UMyGameInstance* instance);
	~FNetSession();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	bool Connect(FString ip, int32 port);
	void Send(TSharedPtr<FSendBuffer> sendBuffer);
	void Recv();
public:
	FSocket* _socket;
	FRunnableThread* _thread;

	UMyGameInstance* _instance;

	TSharedPtr<FRecvBuffer> _recvBuffer;
	TAtomic<bool> _isRun = false;
};
