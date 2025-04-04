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
#include"../WebManager.h"
#include"ServerListWidget.h"
#include"../FirebaseManager.h"
#include"../UIManager.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 로그인 위젯 생성 시, 게임 인스턴스에 연결
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	instance->_uiManager->_mainUI = this;
	
	serverListPopUp->SetVisibility(ESlateVisibility::Hidden);

	// 로그인 버튼 기능 생성
	LoginBtn->OnReleased.AddDynamic(this, &ULoginWidget::OnClickedLogin);
}

void ULoginWidget::OnClickedLogin()
{
	FString id(IdEditableTextBox->GetText().ToString());
	FString pw(PwEditableTextBox->GetText().ToString());


	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	if (IsValid(instance) == false) {
		UE_LOG(LogTemp, Error, TEXT("ULoginWidget::OnClickedLogin() Error - Invalid GameInstance"));
		return;
	}

	// 개발중 (웹서버 사용 X)
	if (instance->_isRelease == false) {
		FLoginAccountRes res;
		if (id.Compare("see") == 0 && pw.Compare("see") == 0)
			instance->_token = "1";
		else if (id.Compare("lee") == 0 && pw.Compare("lee") == 0)
			instance->_token = "2";
		else if (id.Compare("kim") == 0 && pw.Compare("kim") == 0)
			instance->_token = "3";
		else {
			UE_LOG(LogTemp, Error, TEXT("Invalid Account"));
			return;
		}

		// 서버 정보 가상으로 설정해서 UI로 전달
		FServerInfo serverInfo;
		serverInfo.BusyScore = 0;
		serverInfo.IpAddress = "127.0.0.1";
		serverInfo.Port = 7777;
		serverInfo.Name = "FirstServer";

		res.serverList.Add(serverInfo);

		// 서버선택 UI
		serverListPopUp->SetServerList(res.serverList);
		serverListPopUp->SetVisibility(ESlateVisibility::Visible);
	}
	
	// 실사용 (웹서버 사용)
	else {
		// 웹서버 패킷 생성
		FLoginAccountReq loginReq;
		loginReq.AccountName = id;
		loginReq.AccountPw = pw;

		// 전송
		instance->_webManager->RequestPost(loginReq, [this, instance](FLoginAccountRes res) {
			// 서버로부터 패킷 응답 처리
			if (res.LoginOk) {
				UE_LOG(LogTemp, Error, TEXT("LoginOk"));
			
				// 서버선택 UI
				serverListPopUp->SetServerList(res.serverList);
				serverListPopUp->SetVisibility(ESlateVisibility::Visible);
			
				// 토큰 세팅
				instance->_token = res.Token;
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("ULoginWidget::OnClickedLogin() Failed"));
			}
		});
	}
}
