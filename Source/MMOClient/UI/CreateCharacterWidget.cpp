// Fill out your copyright notice in the Description page of Project Settings.


#include "CreateCharacterWidget.h"
#include<Components/EditableTextBox.h>
#include<Components/TextBlock.h>
#include<Components/Button.h>
#include"../MyGameInstance.h"
#include"../Proto/Protocol3.pb.h"
#include"../NetSession.h"
#include"../ServerPacketHandler.h"

void UCreateCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ��ư �̺�Ʈ
	CreateBtn->OnReleased.AddDynamic(this, &UCreateCharacterWidget::OnCreateBtnClicked);
	CancelBtn->OnReleased.AddDynamic(this, &UCreateCharacterWidget::OnCancelBtnClicked);
}

void UCreateCharacterWidget::OnCreateBtnClicked()
{
	// �ؽ�Ʈ
	FString str = Name->GetText().ToString();

	// ��Ŷ ����
	PROTOCOL::C_CreatePlayer toPkt;
	toPkt.set_name(TCHAR_TO_ANSI(*str));
	
	// ��Ȱ��ȭ
	SetIsEnabled(false);

	// ����
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
	instance->_netSession->Send(sendBuffer);
}

void UCreateCharacterWidget::OnCancelBtnClicked()
{
	// ���� ����
	RemoveFromParent();
}
