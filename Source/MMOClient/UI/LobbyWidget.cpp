// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

#include<Components/Button.h>
#include<Components/TextBlock.h>
#include<Components/HorizontalBox.h>

#include"CharacterSlotWidget.h"
#include"CreateCharacterWidget.h"
#include<Kismet/GameplayStatics.h>
#include"../MyGameInstance.h"
#include"../NetSession.h"
#include"../ServerPacketHandler.h"
#include"../UIManager.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CreateBtn->OnReleased.AddDynamic(this, &ULobbyWidget::OnCreateBtnClicked);

	// 
	Init();
}

void ULobbyWidget::Init()
{
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	// 캐릭터 목록
	for (PROTOCOL::ObjectInfo info : instance->_myCharacterList) 
		AddSlotList(info);
}

void ULobbyWidget::AddSlotList(PROTOCOL::ObjectInfo info)
{
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	// 캐릭터 슬롯 생성
	UCharacterSlotWidget* characterSlot = Cast<UCharacterSlotWidget>(CreateWidget(GetWorld(), instance->_uiManager->CharacterSlotClass));
	if (IsValid(characterSlot)) {

		// 캐릭 슬롯 세팅 및 캐릭터 슬롯 리스트에 추가
		CharacterSlotList->AddChild(characterSlot);
		int32 slotNum = CharacterSlotList->GetChildIndex(characterSlot);
		characterSlot->Init(this, info, slotNum);
	}
}

void ULobbyWidget::OnSlotClicked(int32 slotNum)
{
	// 요청 중복 방지
	if (isRequested == true)
		return;
	isRequested = true;

	// 내캐릭 설정
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	instance->_myCharacterInfo = &instance->_myCharacterList[slotNum];

	// 패킷
	PROTOCOL::C_Enter_Room toPkt;
	toPkt.set_roomnum(1);
	toPkt.mutable_object()->set_name(instance->_myCharacterInfo->name());

	auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
	instance->_netSession->Send(sendBuffer);
}

void ULobbyWidget::OnCreateBtnClicked()
{
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	
	PopUpWidget = Cast<UCreateCharacterWidget>(CreateWidget(GetWorld(), instance->_uiManager->CreateCharacterClass));
	PopUpWidget->AddToViewport();
}

