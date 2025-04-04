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
	// 인벤토리 위젯 참조
	_ownerWidget = ownerInvenWidget;

	// 슬롯 인덱스 설정
	slotIndex = SlotIndex;

	// 툴팁 위젯 생성 - UPROPERTY임 이거 만들필요가 없지않나 -> 체크
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	slotTooltipUI = Cast<USlotTooltipWidget>(CreateWidget(GetWorld(), instance->_uiManager->SlotTooltipClass));
		
	// 빈슬롯 텍스쳐 로드, 슬롯 섬네일 빈슬롯으로 설정
	emptySlotTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *NoneThumbnailPath));
	slotThumbnail->SetBrushFromTexture(emptySlotTexture);
}

void UInvenSlotWidget::SetItem()
{
	// 아이템 설정 (GameInstance에 C++ 인벤토리에서 참조)
	auto itemPtr = Cast<UMyGameInstance>(GetGameInstance())->inventory.Find(slotIndex);
	if (itemPtr == nullptr)
		item = nullptr;
	else
		item = (*itemPtr);

	// 위젯 업데이트
	UpdateUI();
}

void UInvenSlotWidget::SetRequestTimer(bool flag)
{
	// 패킷 요청 상태
	if (flag == true) {
		// 패킷 요청 상태 전환
		isRequested = true;

		// 상태 해제 예약
		GetWorld()->GetTimerManager().SetTimer(requestTimer, FTimerDelegate::CreateLambda(
			[this]() {isRequested = false; }
		), 5.0f, false);
	}
	// 패킷 요청 상태 아님
	else {
		// 패킷 요청 상태 아님
		isRequested = false;

		// 상태 해제 예약 취소
		GetWorld()->GetTimerManager().ClearTimer(requestTimer);
	}
}

void UInvenSlotWidget::SetUseTimer()
{
	// 아이템 사용 쿨타임 적용
	isUseTime = false;

	// 아이템 사용 쿨타임 해제 예약
	GetWorld()->GetTimerManager().SetTimer(useTimer, FTimerDelegate::CreateLambda(
		[this]() {isUseTime = true; }
	), 2.0f, false);
}

void UInvenSlotWidget::UpdateUI()
{
	// 아이템 없음 (삭제되거나, 애초에 없는 경우)
	if (IsValid(item) == false || IsValid(item->itemData) == false) {
		// 섬네일
		slotThumbnail->SetBrushFromTexture(emptySlotTexture);

		// 상태
		slotStatus->SetText(FText::FromString(""));

		// 툴팁
		tooltipBtn->SetToolTip(nullptr);
	}

	// 아이템 있음
	else {
		// 섬네일
		slotThumbnail->SetBrushFromTexture(item->itemData->thumbnail);

		// 상태 - 소비, 장비
		// 소비류
		if (item->itemData->itemType == EItemType::ITEM_TYPE_CONSUMABLE)
			slotStatus->SetText(FText::FromString(FString::FromInt(item->itemDB.count)));
		
		// 장비류
		else {
			// 상태
			if (item->itemDB.equipped == true)
				slotStatus->SetText(FText::FromString("E"));
			else
				slotStatus->SetText(FText::FromString(""));
		}

		// 툴팁
		slotTooltipUI->UpdateToolTip(item);
		tooltipBtn->SetToolTip(slotTooltipUI);
	}

	// 상위UI에서 공격력, 방어력 갱신
	_ownerWidget->UpdateArmorText();
	_ownerWidget->UpdateDamageText();
}

void UInvenSlotWidget::RequestUseItem()
{
	// 아이템이 없으면 취소
	if (IsValid(item) == false || IsValid(item->itemData) == false)
		return;

	// 아이템 사용 요청이 진행 중이면 취소
	// if (isRequested == true) return;

	// 요청 쿨 적용
	// SetRequestTimer(true);
	
	// 소모품이면 사용
	if (item->itemData->itemType == EItemType::ITEM_TYPE_CONSUMABLE) {
		// 사용쿨이 되지 않았으면 취소 (현재 하드코딩)
		// if (isUseTime == false) return;

		// 아이템 사용 쿨타임 적용
		// SetUseTimer();

		// 사용 패킷
		PROTOCOL::C_USE_ITEM toPkt;
		toPkt.set_slot(item->itemDB.slot);
		toPkt.set_use(true);

		// 요청
		UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
		auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
		instance->_netSession->Send(sendBuffer);
	}
	
	// 장비품이면 장착/해제
	else {
		// 장착 패킷
		PROTOCOL::C_EQUIP_ITEM toPkt;
		toPkt.set_slot(item->itemDB.slot);
		toPkt.set_equip(!item->itemDB.equipped);

		// 요청
		UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
		auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
		instance->_netSession->Send(sendBuffer);
	}
}
