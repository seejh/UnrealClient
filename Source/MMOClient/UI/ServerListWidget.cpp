// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerListWidget.h"
#include<Components/WrapBox.h>
#include"ServerListSlotWidget.h"
#include"../MyGameInstance.h"
#include"../UIManager.h"

void UServerListWidget::SetServerList(TArray<FServerInfo> serverList)
{
	// �ʱ�ȭ
	ServerListBox->ClearChildren();

	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	// ���� ������ ���� �� ����
	for (FServerInfo serverInfo : serverList) {
		UServerListSlotWidget* serverItem = Cast<UServerListSlotWidget>(CreateWidget(GetWorld(), instance->_uiManager->ServerListSlotClass));
		if (IsValid(serverItem) == false)
			continue;

		serverItem->SetServerInfo(serverInfo);
		ServerListBox->AddChild(serverItem);
	}
}
