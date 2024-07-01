// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenSlotWidget.h"
#include"SlotTooltipWidget.h"
#include"../Data/GameStruct.h"
#include"../ServerPacketHandler.h"
#include"../NetSession.h"

FString UInvenSlotWidget::NoneThumbnailPath = "/Game/Icons/Slot_EmptyWhite";

void UInvenSlotWidget::Init(int32 SlotIndex)
{
	// ���� �ε��� ����
	slotIndex = SlotIndex;

	// ���� ���� ����
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	slotTooltipWidget = Cast<USlotTooltipWidget>(CreateWidget(GetWorld(), instance->SlotTooltipWidgetClass));

	// ������ �⺻
	slotThumbnail->SetBrushFromTexture(Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *NoneThumbnailPath)));
}

void UInvenSlotWidget::SetItem(UItem* Item)
{
	// ������ ����
	item = Item;

	// ���� ������Ʈ
	UpdateWidget();
}

void UInvenSlotWidget::UpdateWidget()
{
	// 1. �������� ���� ��
	if (IsValid(item) == false || IsValid(item->itemData) == false) {
		// ������
		slotThumbnail->SetBrushFromTexture(Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *NoneThumbnailPath)));

		// ����
		slotStatus->SetText(FText::FromString(""));

		// ���� ���ε� ����
		tooltipBtn->SetToolTip(nullptr);

		return;
	}

	// 2. �������� ���� ��
	// ������
	slotThumbnail->SetBrushFromTexture(item->itemData->thumbnail);
	
	// ���� - ������ Ÿ�Կ� ����
	// NONE
	if (item->itemData->itemType == EItemType::ITEM_TYPE_NONE) {
		return;
	}
	
	// �Һ��
	else if (item->itemData->itemType == EItemType::ITEM_TYPE_CONSUMABLE) {
		slotStatus->SetText(FText::FromString(FString::FromInt(item->itemDB.count)));
	}
	
	// ����(����, ��)
	else if (item->itemData->itemType == EItemType::ITEM_TYPE_WEAPON ||
		item->itemData->itemType == EItemType::ITEM_TYPE_ARMOR) {

		if (item->itemDB.equipped) {
			slotStatus->SetText(FText::FromString("E"));
		}
		else {
			slotStatus->SetText(FText::FromString(""));
		}
	}

	// ���� ������Ʈ
	slotTooltipWidget->UpdateToolTip(item);

	// ���� ���ε�
	tooltipBtn->SetToolTip(slotTooltipWidget);
}

void UInvenSlotWidget::RequestUseItem()
{
	// ������ ������ �н�
	if (IsValid(item) == false || IsValid(item->itemData) == false)
		return;

	// ������ ��� ��û�� ���� ���̸�
	if (isRequested == true)
		return;

	// ����� - �����ʿ�
	if (isUseTimeOn == false)
		return;

	// ������ ��� ��û �÷��� ����
	isRequested = true;
	isUseTimeOn = false;

	GetWorld()->GetTimerManager().SetTimer(useTimer, FTimerDelegate::CreateLambda(
		[this]() {
			isUseTimeOn = true;
		}
	), 1.5f, false);

	//
	PROTOCOL::C_UseItem toPkt;
	toPkt.set_itemdbid(item->itemDB.itemDbId);
	toPkt.set_use(!item->itemDB.equipped);

	// ����
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
	instance->_netSession->Send(sendBuffer);
}
