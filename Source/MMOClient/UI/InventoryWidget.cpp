// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"

#include"InvenSlotWidget.h"
#include"../MyGameInstance.h"
#include<Components/WrapBox.h>
#include<Components/TextBlock.h>
#include"../Data/GameStruct.h"
#include"../DataManager.h"
#include"../UIManager.h"
#include"../MyPlayerController.h"

void UInventoryWidget::Init(bool isFirst)
{
	SetVisibility(ESlateVisibility::Hidden);

	// 이 UI를 닫을 때를 위해
	bIsFocusable = true;

	// 게임인스턴스 참조 설정
	_ownerInstance = Cast<UMyGameInstance>(GetGameInstance());
	
	// 첫기동이면
	if (isFirst == true) {
		// 이 인벤토리를 이루는 UI 객체들 미리 생성 (슬롯, 슬롯툴팁 등)
		for (int i = 0; i < MAX_INVEN_SIZE; i++) {
			// 슬롯 생성
			UInvenSlotWidget* slotWidget = Cast<UInvenSlotWidget>(CreateWidget(GetWorld(), _ownerInstance->_uiManager->InventorySlotClass));
			if (IsValid(slotWidget) == false) {
				UE_LOG(LogTemp, Error, TEXT("UInventoryWidget::Init() Error - Create InvenSlot"));
				return;
			}

			// 인벤에 슬롯 추가
			InventoryGrid->AddChild(slotWidget);

			// 슬롯 초기화
			slotWidget->Init(this, InventoryGrid->GetChildIndex(slotWidget));
		}
	}

	for (auto invenSlotUI : InventoryGrid->GetAllChildren()) 
		Cast<UInvenSlotWidget>(invenSlotUI)->SetItem();
}

void UInventoryWidget::Clear()
{
	for (int i = 0; i < MAX_INVEN_SIZE; i++)
		SetSlot(i);
}

UInvenSlotWidget* UInventoryWidget::GetSlotAt(int32 slot)
{
	return Cast<UInvenSlotWidget>(InventoryGrid->GetChildAt(slot));
}

void UInventoryWidget::SetSlot(int32 slot)
{
	UInvenSlotWidget* slotUI = Cast<UInvenSlotWidget>(InventoryGrid->GetChildAt(slot));
	if (IsValid(slotUI))
		slotUI->SetItem();
}

void UInventoryWidget::UpdateSlot(int32 slot)
{
	// 해당 슬롯의 위젯 찾고
	UInvenSlotWidget* slotWidget = Cast<UInvenSlotWidget>(InventoryGrid->GetChildAt(slot));
	if (IsValid(slotWidget) == false)
		return;

	// 업데이트
	slotWidget->UpdateUI();
}

void UInventoryWidget::UpdateDamageText()
{
	// 총공격력 표현 (총공 = 스텟공 + 아이템 공)

	FString damageStr("");

	// 스텟 공
	damageStr.Append(FString::FromInt(_ownerInstance->_myCharacterInfo->stat().damage()));
	
	// 무기 공
	int32 itemDamage = 0;
	for (auto p : _ownerInstance->inventory) {

		// 인벤에서 무기이며 현재 장착된 아이템 탐색 - 수정 필요(탐색때문)
		if (p.Value->itemData->itemType == EItemType::ITEM_TYPE_WEAPON && p.Value->itemDB.equipped == true) {
			itemDamage += Cast<UWeaponData>(p.Value->itemData)->damage;
			break;
		}
	}

	// 아이템으로 추가되는 공격력이 없으면 그냥 스텟만 표현됨 (뒤에 쓸데없이 + 안 붙이게)
	if (itemDamage != 0) {
		damageStr.Append(" + ");
		damageStr.Append(FString::FromInt(itemDamage));
	}

	// 
	DamageText->SetText(FText::FromString(damageStr));
}

void UInventoryWidget::UpdateArmorText()
{
	// 총방어력 표현 (총방 = 스텟 방 + 아이템들 방, 다만 현재 기본스텟에 방어는 없음)

	FString armorStr("0");

	// 방어구 방
	int32 itemArmor = 0;
	for (auto p : _ownerInstance->inventory) {

		// 인벤에서 방어구이며 현재 장착된 아이템 탐색
		if (p.Value->itemData->itemType == EItemType::ITEM_TYPE_ARMOR && p.Value->itemDB.equipped == true) 
			itemArmor += Cast<UArmorData>(p.Value->itemData)->defence;
	}

	// 아이템으로 추가되는 방어력이 없으면 그냥 스텟만 표현됨 (뒤에 쓸데없이 + 안 붙이게)
	if (itemArmor != 0) {
		armorStr.Append(" + ");
		armorStr.Append(FString::FromInt(itemArmor));
	}

	// 
	ArmorText->SetText(FText::FromString(armorStr));
}

FReply UInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	if (InKeyEvent.GetKey() == EKeys::I)
		_ownerInstance->_playerController->OpenInventory();

	return FReply::Handled();
}



