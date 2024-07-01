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

	// 버튼 이벤트
	CreateBtn->OnReleased.AddDynamic(this, &UCreateCharacterWidget::OnCreateBtnClicked);
	CancelBtn->OnReleased.AddDynamic(this, &UCreateCharacterWidget::OnCancelBtnClicked);
}

void UCreateCharacterWidget::OnCreateBtnClicked()
{
	// 텍스트
	FString str = Name->GetText().ToString();

	// 패킷 생성
	PROTOCOL::C_CreatePlayer toPkt;
	toPkt.set_name(TCHAR_TO_ANSI(*str));
	
	// 비활성화
	SetIsEnabled(false);

	// 전송
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
	instance->_netSession->Send(sendBuffer);
}

void UCreateCharacterWidget::OnCancelBtnClicked()
{
	// 본인 제거
	RemoveFromParent();
}
