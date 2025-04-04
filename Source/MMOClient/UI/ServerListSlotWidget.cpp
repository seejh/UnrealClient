// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerListSlotWidget.h"
#include<Components/TextBlock.h>
#include"../MyGameInstance.h"
#include"../NetSession.h"
#include"../ServerPacketHandler.h"

void UServerListSlotWidget::ConnectToGameServer()
{
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	if (IsValid(instance) == false) {
		UE_LOG(LogTemp, Error, TEXT("No GameInstance Error"));
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("UServerListSlotWidget::ConnectToGameServer - %s, %d"), *_serverInfo.IpAddress, _serverInfo.Port);

	// 해당 서버의 ip와 port로 소켓 접속
	if (instance->_netSession->Connect(_serverInfo.IpAddress, _serverInfo.Port)) {
		// 소켓 연결 성공, C_Login 패킷 전송(토큰)
		PROTOCOL::C_LOGIN loginPkt;
		loginPkt.set_tokenstring(TCHAR_TO_ANSI(*(instance->_token)));

		auto sendBuffer = instance->_packetHandler->MakeSendBuffer(loginPkt);
		instance->_netSession->Send(sendBuffer);

		instance->_playerState = PROTOCOL::PlayerServerState::SERVER_STATE_LOGIN;
	}
	else {
		// 소켓 연결 실패
		UE_LOG(LogTemp, Error, TEXT("UServerListSlotWidget::ConnectToGameServer() Failed."));
	}
}

void UServerListSlotWidget::SetServerInfo(FServerInfo serverInfo)
{
	_serverInfo = serverInfo;

	ServerName->SetText(FText::FromString(_serverInfo.Name));
	BusyScore->SetText(FText::FromString(FString::FromInt(_serverInfo.BusyScore)));
}
