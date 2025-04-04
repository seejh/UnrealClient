// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenSlotWidget.h"
#include<Components/Image.h>
#include<Components/TextBlock.h>
#include<Components/Button.h>

#include"InventoryWidget.h"
#include"SlotTooltipWidget.h"
#include"../Data/GameStruct.h"
#include"../ServerPacketHandler.h"
#include"../NetSession.h"
#include"../MyGameInstance.h"
#include"../UIManager.h"

FString UInvenSlotWidget::NoneThumbnailPath = "/Game/Icons/Slot_EmptyWhite";

void UInvenSlotWidget::Init(UInventoryWidget* ownerInvenWidget, int32 SlotIndex)
{
	// �κ��丮 ���� ����
	_ownerWidget = ownerInvenWidget;

	// ���� �ε��� ����
	slotIndex = SlotIndex;

	// ���� ���� ���� - UPROPERTY�� �̰� �����ʿ䰡 �����ʳ� -> üũ
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	slotTooltipUI = Cast<USlotTooltipWidget>(CreateWidget(GetWorld(), instance->_uiManager->SlotTooltipClass));
		
	// �󽽷� �ؽ��� �ε�, ���� ������ �󽽷����� ����
	emptySlotTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *NoneThumbnailPath));
	slotThumbnail->SetBrushFromTexture(emptySlotTexture);
}

void UInvenSlotWidget::SetItem()
{
	// ������ ���� (GameInstance�� C++ �κ��丮���� ����)
	auto itemPtr = Cast<UMyGameInstance>(GetGameInstance())->inventory.Find(slotIndex);
	if (itemPtr == nullptr)
		item = nullptr;
	else
		item = (*itemPtr);

	// ���� ������Ʈ
	UpdateUI();
}

void UInvenSlotWidget::SetRequestTimer(bool flag)
{
	// ��Ŷ ��û ����
	if (flag == true) {
		// ��Ŷ ��û ���� ��ȯ
		isRequested = true;

		// ���� ���� ����
		GetWorld()->GetTimerManager().SetTimer(requestTimer, FTimerDelegate::CreateLambda(
			[this]() {isRequested = false; }
		), 5.0f, false);
	}
	// ��Ŷ ��û ���� �ƴ�
	else {
		// ��Ŷ ��û ���� �ƴ�
		isRequested = false;

		// ���� ���� ���� ���
		GetWorld()->GetTimerManager().ClearTimer(requestTimer);
	}
}

void UInvenSlotWidget::SetUseTimer()
{
	// ������ ��� ��Ÿ�� ����
	isUseTime = false;

	// ������ ��� ��Ÿ�� ���� ����
	GetWorld()->GetTimerManager().SetTimer(useTimer, FTimerDelegate::CreateLambda(
		[this]() {isUseTime = true; }
	), 2.0f, false);
}

void UInvenSlotWidget::UpdateUI()
{
	// ������ ���� (�����ǰų�, ���ʿ� ���� ���)
	if (IsValid(item) == false || IsValid(item->itemData) == false) {
		// ������
		slotThumbnail->SetBrushFromTexture(emptySlotTexture);

		// ����
		slotStatus->SetText(FText::FromString(""));

		// ����
		tooltipBtn->SetToolTip(nullptr);
	}

	// ������ ����
	else {
		// ������
		slotThumbnail->SetBrushFromTexture(item->itemData->thumbnail);

		// ���� - �Һ�, ���
		// �Һ��
		if (item->itemData->itemType == EItemType::ITEM_TYPE_CONSUMABLE)
			slotStatus->SetText(FText::FromString(FString::FromInt(item->itemDB.count)));
		
		// ����
		else {
			// ����
			if (item->itemDB.equipped == true)
				slotStatus->SetText(FText::FromString("E"));
			else
				slotStatus->SetText(FText::FromString(""));
		}

		// ����
		slotTooltipUI->UpdateToolTip(item);
		tooltipBtn->SetToolTip(slotTooltipUI);
	}

	// ����UI���� ���ݷ�, ���� ����
	_ownerWidget->UpdateArmorText();
	_ownerWidget->UpdateDamageText();
}

void UInvenSlotWidget::RequestUseItem()
{
	// �������� ������ ���
	if (IsValid(item) == false || IsValid(item->itemData) == false)
		return;

	// ������ ��� ��û�� ���� ���̸� ���
	// if (isRequested == true) return;

	// ��û �� ����
	// SetRequestTimer(true);
	
	// �Ҹ�ǰ�̸� ���
	if (item->itemData->itemType == EItemType::ITEM_TYPE_CONSUMABLE) {
		// ������� ���� �ʾ����� ��� (���� �ϵ��ڵ�)
		// if (isUseTime == false) return;

		// ������ ��� ��Ÿ�� ����
		// SetUseTimer();

		// ��� ��Ŷ
		PROTOCOL::C_USE_ITEM toPkt;
		toPkt.set_slot(item->itemDB.slot);
		toPkt.set_use(true);

		// ��û
		UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
		auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
		instance->_netSession->Send(sendBuffer);
	}
	
	// ���ǰ�̸� ����/����
	else {
		// ���� ��Ŷ
		PROTOCOL::C_EQUIP_ITEM toPkt;
		toPkt.set_slot(item->itemDB.slot);
		toPkt.set_equip(!item->itemDB.equipped);

		// ��û
		UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
		auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
		instance->_netSession->Send(sendBuffer);
	}
}
