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

	// ĳ���� ���
	for (PROTOCOL::ObjectInfo info : instance->_myCharacterList) 
		AddSlotList(info);
}

void ULobbyWidget::AddSlotList(PROTOCOL::ObjectInfo info)
{
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	// ĳ���� ���� ����
	UCharacterSlotWidget* characterSlot = Cast<UCharacterSlotWidget>(CreateWidget(GetWorld(), instance->_uiManager->CharacterSlotClass));
	if (IsValid(characterSlot)) {

		// ĳ�� ���� ���� �� ĳ���� ���� ����Ʈ�� �߰�
		CharacterSlotList->AddChild(characterSlot);
		int32 slotNum = CharacterSlotList->GetChildIndex(characterSlot);
		characterSlot->Init(this, info, slotNum);
	}
}

void ULobbyWidget::OnSlotClicked(int32 slotNum)
{
	// ��û �ߺ� ����
	if (isRequested == true)
		return;
	isRequested = true;

	// ��ĳ�� ����
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	instance->_myCharacterInfo = &instance->_myCharacterList[slotNum];

	// ��Ŷ
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

