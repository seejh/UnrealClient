// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"

#include"InvenSlotWidget.h"
#include"../MyGameInstance.h"
#include<Components/WrapBox.h>
#include<Components/TextBlock.h>
#include"../Data/GameStruct.h"
#include"../Data/DataManager.h"


void UInventoryWidget::Init()
{
	// 
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	if (IsValid(instance) == false) 
		return;

	// UI 객체 생성 (슬롯, 슬롯툴팁)
	for (int i = 0; i < 30; i++) {
		// 슬롯 생성
		UInvenSlotWidget* slotWidget = Cast<UInvenSlotWidget>(CreateWidget(GetWorld(), instance->InvenSlotWidgetClass));
		if (IsValid(slotWidget) == false) 
			return;

		// UI 인벤토리 그리드에 슬롯 추가
		InventoryGrid->AddChild(slotWidget);

		// 슬롯 위젯 인덱스 설정 및 내부 툴팁 생성
		slotWidget->Init(InventoryGrid->GetChildIndex(slotWidget));
	}


	// c++ 인벤의 아이템들을 생성된 UI 객체에 설정
	for (UItem* item : instance->_inventory) {
		UInvenSlotWidget* slotWidget = Cast<UInvenSlotWidget>(InventoryGrid->GetChildAt(item->itemDB.slot));
		slotWidget->SetItem(item);
	}

	UpdateDamageText();
	UpdateArmorText();
}

void UInventoryWidget::SetInvenSlot(UItem* item)
{
	// 아이템 슬롯 찾아서
	UInvenSlotWidget* slotWidget = Cast<UInvenSlotWidget>(InventoryGrid->GetChildAt(item->itemDB.slot));
	if (IsValid(slotWidget) == false)
		return;

	// 슬롯에 아이템 설정
	slotWidget->SetItem(item);
}

void UInventoryWidget::UpdateDamageText()
{
	//
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	if (IsValid(instance) == false)
		return;

	//
	FString damageStr("");

	// 스텟 대미지 + 아이템 대미지
	// 스텟
	damageStr.Append(FString::FromInt(instance->_myCharacterInfo->stat().damage()));

	// 아이템
	int32 itemDamage = 0;
	for (UItem* invenItem : instance->_inventory) {
		// 무기 & 장착
		if (invenItem->itemData->itemType == EItemType::ITEM_TYPE_WEAPON && invenItem->itemDB.equipped == true) {
			itemDamage += Cast<UWeaponData>(invenItem->itemData)->damage;
			break;
		}
	}

	// 아이템 공격력이 있으면
	if (itemDamage != 0) {
		damageStr.Append(" + ");
		damageStr.Append(FString::FromInt(itemDamage));
	}

	// 대미지 텍스트 설정
	DamageText->SetText(FText::FromString(damageStr));
}

void UInventoryWidget::UpdateArmorText()
{
	//
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	if (IsValid(instance) == false)
		return;

	//
	FString armorStr("0");

	// 스텟 아머 + 아이템 아머
	// 스텟 - 기본 방어 현재 0

	// 아이템
	int32 itemArmor = 0;
	for (UItem* invenItem : instance->_inventory) {
		// 방어구 & 장착
		if (invenItem->itemData->itemType == EItemType::ITEM_TYPE_ARMOR && invenItem->itemDB.equipped == true) 
			itemArmor += Cast<UArmorData>(invenItem->itemData)->defence;
	}

	// 아이템 방어력이 있으면
	if (itemArmor != 0) {
		armorStr.Append(" + ");
		armorStr.Append(FString::FromInt(itemArmor));
	}

	// 아머 텍스트 설정
	ArmorText->SetText(FText::FromString(armorStr));
	
}

UInvenSlotWidget* UInventoryWidget::GetSlotAt(int32 slot)
{
	return Cast<UInvenSlotWidget>(InventoryGrid->GetChildAt(slot));
}

