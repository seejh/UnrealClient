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

void URespawnWidget::OnRespawnBtnClicked()
{
	// �ߺ� ��û ���� - ��Ȱ��ȭ
	RespawnBtn->SetIsEnabled(false);

	// ��û
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	instance->TryEnterRoom();
}
