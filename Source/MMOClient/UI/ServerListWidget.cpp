// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerListWidget.h"
#include<Components/WrapBox.h>
#include"ServerListSlotWidget.h"
#include"../MyGameInstance.h"
#include"../UIManager.h"

void UServerListWidget::SetServerList(TArray<FServerInfo> serverList)
{
	// 초기화
	ServerListBox->ClearChildren();

	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	// 서버 아이템 생성 및 설정
	for (FServerInfo serverInfo : serverList) {
		UServerListSlotWidget* serverItem = Cast<UServerListSlotWidget>(CreateWidget(GetWorld(), instance->_uiManager->ServerListSlotClass));
		if (IsValid(serverItem) == false)
			continue;

		serverItem->SetServerInfo(serverInfo);
		ServerListBox->AddChild(serverItem);
	}
}
