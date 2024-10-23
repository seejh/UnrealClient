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
	RespawnBtn->SetIsEnabled(false);

	// 요청
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	
	// 패킷 - 걍 일단 여기서 처리
	PROTOCOL::C_Enter_Room toPkt;
	toPkt.set_roomnum(1);
	toPkt.mutable_object()->set_name(name);

	auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
	instance->_netSession->Send(sendBuffer);
}














