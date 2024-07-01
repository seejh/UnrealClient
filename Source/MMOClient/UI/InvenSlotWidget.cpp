// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenSlotWidget.h"
#include"SlotTooltipWidget.h"
#include"../Data/GameStruct.h"
#include"../ServerPacketHandler.h"
#include"../NetSession.h"

FString UInvenSlotWidget::NoneThumbnailPath = "/Game/Icons/Slot_EmptyWhite";

void UInvenSlotWidget::Init(int32 SlotIndex)
{
	// 슬롯 인덱스 설정
	slotIndex = SlotIndex;

	// 툴팁 위젯 생성
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	slotTooltipWidget = Cast<USlotTooltipWidget>(CreateWidget(GetWorld(), instance->SlotTooltipWidgetClass));

	// 섬네일 기본
	slotThumbnail->SetBrushFromTexture(Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *NoneThumbnailPath)));
}

void UInvenSlotWidget::SetItem(UItem* Item)
{
	// 아이템 설정
	item = Item;

	// 위젯 업데이트
	UpdateWidget();
}

void UInvenSlotWidget::UpdateWidget()
{
	// 1. 아이템이 없을 때
	if (IsValid(item) == false || IsValid(item->itemData) == false) {
		// 섬네일
		slotThumbnail->SetBrushFromTexture(Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *NoneThumbnailPath)));

		// 상태
		slotStatus->SetText(FText::FromString(""));

		// 툴팁 바인드 해제
		tooltipBtn->SetToolTip(nullptr);

		return;
	}

	// 2. 아이템이 있을 때
	// 섬네일
	slotThumbnail->SetBrushFromTexture(item->itemData->thumbnail);
	
	// 상태 - 아이템 타입에 따라
	// NONE
	if (item->itemData->itemType == EItemType::ITEM_TYPE_NONE) {
		return;
	}
	
	// 소비류
	else if (item->itemData->itemType == EItemType::ITEM_TYPE_CONSUMABLE) {
		slotStatus->SetText(FText::FromString(FString::FromInt(item->itemDB.count)));
	}
	
	// 장비류(무기, 방어구)
	else if (item->itemData->itemType == EItemType::ITEM_TYPE_WEAPON ||
		item->itemData->itemType == EItemType::ITEM_TYPE_ARMOR) {

		if (item->itemDB.equipped) {
			slotStatus->SetText(FText::FromString("E"));
		}
		else {
			slotStatus->SetText(FText::FromString(""));
		}
	}

	// 툴팁 업데이트
	slotTooltipWidget->UpdateToolTip(item);

	// 툴팁 바인드
	tooltipBtn->SetToolTip(slotTooltipWidget);
}

void UInvenSlotWidget::RequestUseItem()
{
	// 아이템 없으면 패스
	if (IsValid(item) == false || IsValid(item->itemData) == false)
		return;

	// 아이템 사용 요청이 진행 중이면
	if (isRequested == true)
		return;

	// 사용쿨 - 수정필요
	if (isUseTimeOn == false)
		return;

	// 아이템 사용 요청 플래그 설정
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

	// 전송
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
	instance->_netSession->Send(sendBuffer);
}
