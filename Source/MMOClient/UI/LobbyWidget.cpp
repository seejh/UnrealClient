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


void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateBtn->OnReleased.AddDynamic(this, &ULobbyWidget::OnCreateBtnClicked);
}

void ULobbyWidget::Init()
{
	//
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	// 캐릭터 목록
	for (PROTOCOL::ObjectInfo info : instance->_myCharacterList) {
		AddSlotList(info);
	}
}

void ULobbyWidget::AddSlotList(PROTOCOL::ObjectInfo info)
{
	//
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	// 캐릭터 슬롯 생성
	UCharacterSlotWidget* characterSlot = Cast<UCharacterSlotWidget>(CreateWidget(GetWorld(), instance->CharacterSlotWidgetClass));
	if (IsValid(characterSlot)) {

		// 캐릭 슬롯, 캐릭터 슬롯 리스트에 추가
		CharacterSlotList->AddChild(characterSlot);

		// 슬롯 세팅
		int32 slotNum = CharacterSlotList->GetChildIndex(characterSlot);
		characterSlot->Init(this, info, slotNum);
	}
}

void ULobbyWidget::OnSlotClicked(int32 slotNum)
{
	if (isRequested == true)
		return;
	
	// 요청 중복 방지
	isRequested = true;

	// 내캐릭 설정
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	instance->_myCharacterInfo = &instance->_myCharacterList[slotNum];

	instance->TryEnterRoom();
}

void ULobbyWidget::OnCreateBtnClicked()
{
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	
	PopUpWidget = Cast<UCreateCharacterWidget>(CreateWidget(GetWorld(), instance->CreateCharacterWidgetClass));
	PopUpWidget->AddToViewport();
}

