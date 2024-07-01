// Fill out your copyright notice in the Description page of Project Settings.


#include "NetSession.h"

#include<Sockets.h>
#include<Runtime/Networking/Public/Interfaces/IPv4/IPv4Address.h>
#include<Runtime/Networking/Public/Interfaces/IPv4/IPv4Endpoint.h>
#include<SocketSubsystem.h>

#include"MyGameInstance.h"
#include"MyPlayerController.h"
#include"Buffer.h"
#include"ServerPacketHandler.h"

/*--------------------------------------
    NetSession
---------------------------------------*/
FNetSession::FNetSession(UMyGameInstance* instance) : _instance(instance)
{
    _recvBuffer = MakeShareable(new FRecvBuffer(2000));
}

FNetSession::~FNetSession()
{
    
}

bool FNetSession::Init()
{
    return true;
}

uint32 FNetSession::Run()
{
    while (_isRun.Load()) {
        Recv();
    }

    _socket->Shutdown(ESocketShutdownMode::ReadWrite);

    return 0;
}

void FNetSession::Stop()
{
    _isRun.Store(false);
}

void FNetSession::Exit()
{
    UE_LOG(LogTemp, Error, TEXT("NetSession::Exit"))
    _thread->WaitForCompletion();
    _thread->Kill();
    delete _thread;
    _thread = nullptr;
}

bool FNetSession::Connect()
{
    _socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT(""), false);

    FIPv4Address ipAddr;
    FIPv4Address::Parse(TEXT("127.0.0.1"), ipAddr);
    FIPv4Endpoint endPoint = FIPv4Endpoint(ipAddr, 7777);

    //_socket->SetNoDelay(true);
    _socket->SetNonBlocking(true);
    _socket->Connect(*endPoint.ToInternetAddr());

    ESocketConnectionState state = _socket->GetConnectionState();
    if (state != ESocketConnectionState::SCS_Connected) {
        UE_LOG(LogTemp, Error, TEXT("Socket Connect Error"));
        return false;
    }

    // 연결 성공
    UE_LOG(LogTemp, Error, TEXT("Socket Connect OK"));
    
    // recv 가동 (Runnable)
    _isRun.Store(true);
    _thread = FRunnableThread::Create(this, TEXT(""));

    return true;
}

void FNetSession::Send(TSharedPtr<FSendBuffer> sendBuffer)
{
    int32 sendBytes;
    _socket->Send(sendBuffer->Buffer(), sendBuffer->Size(), sendBytes);
    if (sendBytes <= 0) {
        // TODO : error
        UE_LOG(LogTemp, Error, TEXT("send Error Len <= 0"));
    }
}

void FNetSession::Recv()
{
    int32 recvBytes;
    _socket->Recv(_recvBuffer->BufferWritePos(), _recvBuffer->FreeSize(), recvBytes);

    // 받은게 하나라도 있으면
    if (recvBytes > 0) {
        
        // RecvBuffer에 기록
        if (!_recvBuffer->OnWrite(recvBytes)) {
            // Disconnect - RecvBuffer Write Overflow
            return;
        }

        while (true) {
            // 패킷 헤더 추출도 못하나?
            if (_recvBuffer->DataSize() < sizeof(PacketHeader)) {
                break;
            }

            // 덜 받아서 패킷을 완성할 수가 없나?
            PacketHeader* header = reinterpret_cast<PacketHeader*>(_recvBuffer->BufferReadPos());
            if (_recvBuffer->DataSize() < header->_size + sizeof(PacketHeader)) {
                break;
            }

            int packetBodySize = header->_size;
            int totalPacketSize = packetBodySize + sizeof(PacketHeader);
            _instance->_packetHandler->HandlePacket(_instance, _recvBuffer->BufferReadPos(), packetBodySize);
            _recvBuffer->OnRead(totalPacketSize);
        }
        
        _recvBuffer->Reset();
    }
}
