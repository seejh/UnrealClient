// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"

#include<Components/EditableTextBox.h>
#include<Components/TextBlock.h>
#include<Components/Button.h>

#include<Kismet/KismetSystemLibrary.h>
#include<Kismet/GameplayStatics.h>
#include"../MyGameInstance.h"
#include"../Proto/Protocol3.pb.h"

#include"../NetSession.h"
#include"../ServerPacketHandler.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 로그인 위젯 생성 시, 게임 인스턴스에 연결
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	instance->_nowWidget = this;

	// 로그인 버튼 기능 생성
	LoginBtn->OnReleased.AddDynamic(this, &ULoginWidget::OnClickedLogin);
}

void ULoginWidget::OnClickedLogin()
{
	// UI에서 ID, PW 추출해서 
	FString id(IdEditableTextBox->GetText().ToString());
	FString pw(PwEditableTextBox->GetText().ToString());

	// 로그인 패킷
	PROTOCOL::C_Login toPkt;
	toPkt.set_id(TCHAR_TO_ANSI(*id));
	toPkt.set_pw(TCHAR_TO_ANSI(*pw));
 
	// 전송
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	TSharedPtr<FSendBuffer> sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
	instance->_netSession->Send(sendBuffer);

	// 5초 지나도 반응 없으면 원상 복구
	LoginBtn->SetIsEnabled(false);
	GetWorld()->GetTimerManager().SetTimer(_handle, FTimerDelegate::CreateLambda(
		[this]() {
			LoginBtn->SetIsEnabled(true);
		}
	), 5, false);
}
