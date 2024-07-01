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
	// ��������(�κ�����), �� ������ �ε���
	ownerLobbyWidget = lobbyWidget;
	slotNum = slotNumber;

	// ����
	FString levelStr("Lv.");
	levelStr.Append(FString::FromInt(info.stat().level()));
	Level->SetText(FText::FromString(levelStr));

	// �̸�
	FString nameStr("");
	nameStr.Append(info.name().c_str());
	Name->SetText(FText::FromString(nameStr));

	// ���ݷ�
	FString damageStr("");
	damageStr.Append(FString::FromInt(info.stat().damage()));
	Damage->SetText(FText::FromString(damageStr));

	// ü��
	FString hpStr("");
	hpStr.Append(FString::FromInt(info.stat().maxhp()));
	HP->SetText(FText::FromString(hpStr));

	// ����ġ
	FString expStr("");
	expStr.Append(FString::FromInt(info.stat().exp()));
	EXP->SetText(FText::FromString(expStr));
}

void UCharacterSlotWidget::OnEnterBtnClicked()
{
	ownerLobbyWidget->OnSlotClicked(slotNum);
}
