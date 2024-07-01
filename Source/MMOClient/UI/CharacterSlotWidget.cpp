// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSlotWidget.h"
#include"LobbyWidget.h"

void UCharacterSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EnterBtn->OnReleased.AddDynamic(this, &UCharacterSlotWidget::OnEnterBtnClicked);
}

void UCharacterSlotWidget::Init(ULobbyWidget* lobbyWidget, PROTOCOL::ObjectInfo info, int32 slotNumber)
{
	// 소유위젯(로비위젯), 이 슬롯의 인덱스
	ownerLobbyWidget = lobbyWidget;
	slotNum = slotNumber;

	// 레벨
	FString levelStr("Lv.");
	levelStr.Append(FString::FromInt(info.stat().level()));
	Level->SetText(FText::FromString(levelStr));

	// 이름
	FString nameStr("");
	nameStr.Append(info.name().c_str());
	Name->SetText(FText::FromString(nameStr));

	// 공격력
	FString damageStr("");
	damageStr.Append(FString::FromInt(info.stat().damage()));
	Damage->SetText(FText::FromString(damageStr));

	// 체력
	FString hpStr("");
	hpStr.Append(FString::FromInt(info.stat().maxhp()));
	HP->SetText(FText::FromString(hpStr));

	// 경험치
	FString expStr("");
	expStr.Append(FString::FromInt(info.stat().exp()));
	EXP->SetText(FText::FromString(expStr));
}

void UCharacterSlotWidget::OnEnterBtnClicked()
{
	ownerLobbyWidget->OnSlotClicked(slotNum);
}
