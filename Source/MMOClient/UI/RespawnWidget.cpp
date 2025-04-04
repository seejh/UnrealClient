// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnWidget.h"
#include <Components/Button.h>
#include"../MyGameInstance.h"
#include"../NetSession.h"
#include"../ServerPacketHandler.h"

void URespawnWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RespawnBtn->OnReleased.AddDynamic(this, &URespawnWidget::OnRespawnBtnClicked);
}

void URespawnWidget::Init()
{
	SetVisibility(ESlateVisibility::Hidden);

	RespawnBtn->SetIsEnabled(true);
}

void URespawnWidget::OnRespawnBtnClicked()
{
	// 중복 요청 방지 - 비활성화
	// RespawnBtn->SetIsEnabled(false);

	// 패킷 요청
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	
	// 플레이하던 캐릭터의 이름으로 재입장 요청
	PROTOCOL::C_ENTER_ROOM toPkt;
	toPkt.set_roomnum(1);
	toPkt.mutable_object()->set_name(instance->_myCharacterInfo->name());
	toPkt.set_isrespawn(true);
	auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
	instance->_netSession->Send(sendBuffer);
}














