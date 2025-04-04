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
	// �ߺ� ��û ���� - ��Ȱ��ȭ
	// RespawnBtn->SetIsEnabled(false);

	// ��Ŷ ��û
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	
	// �÷����ϴ� ĳ������ �̸����� ������ ��û
	PROTOCOL::C_ENTER_ROOM toPkt;
	toPkt.set_roomnum(1);
	toPkt.mutable_object()->set_name(instance->_myCharacterInfo->name());
	toPkt.set_isrespawn(true);
	auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
	instance->_netSession->Send(sendBuffer);
}














