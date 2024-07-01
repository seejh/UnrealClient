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

	// UI ��ü ���� (����, ��������)
	for (int i = 0; i < 30; i++) {
		// ���� ����
		UInvenSlotWidget* slotWidget = Cast<UInvenSlotWidget>(CreateWidget(GetWorld(), instance->InvenSlotWidgetClass));
		if (IsValid(slotWidget) == false) 
			return;

		// UI �κ��丮 �׸��忡 ���� �߰�
		InventoryGrid->AddChild(slotWidget);

		// ���� ���� �ε��� ���� �� ���� ���� ����
		slotWidget->Init(InventoryGrid->GetChildIndex(slotWidget));
	}


	// c++ �κ��� �����۵��� ������ UI ��ü�� ����
	for (UItem* item : instance->_inventory) {
		UInvenSlotWidget* slotWidget = Cast<UInvenSlotWidget>(InventoryGrid->GetChildAt(item->itemDB.slot));
		slotWidget->SetItem(item);
	}

	UpdateDamageText();
	UpdateArmorText();
}

void UInventoryWidget::SetInvenSlot(UItem* item)
{
	// ������ ���� ã�Ƽ�
	UInvenSlotWidget* slotWidget = Cast<UInvenSlotWidget>(InventoryGrid->GetChildAt(item->itemDB.slot));
	if (IsValid(slotWidget) == false)
		return;

	// ���Կ� ������ ����
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

	// ���� ����� + ������ �����
	// ����
	damageStr.Append(FString::FromInt(instance->_myCharacterInfo->stat().damage()));

	// ������
	int32 itemDamage = 0;
	for (UItem* invenItem : instance->_inventory) {
		// ���� & ����
		if (invenItem->itemData->itemType == EItemType::ITEM_TYPE_WEAPON && invenItem->itemDB.equipped == true) {
			itemDamage += Cast<UWeaponData>(invenItem->itemData)->damage;
			break;
		}
	}

	// ������ ���ݷ��� ������
	if (itemDamage != 0) {
		damageStr.Append(" + ");
		damageStr.Append(FString::FromInt(itemDamage));
	}

	// ����� �ؽ�Ʈ ����
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

	// ���� �Ƹ� + ������ �Ƹ�
	// ���� - �⺻ ��� ���� 0

	// ������
	int32 itemArmor = 0;
	for (UItem* invenItem : instance->_inventory) {
		// �� & ����
		if (invenItem->itemData->itemType == EItemType::ITEM_TYPE_ARMOR && invenItem->itemDB.equipped == true) 
			itemArmor += Cast<UArmorData>(invenItem->itemData)->defence;
	}

	// ������ ������ ������
	if (itemArmor != 0) {
		armorStr.Append(" + ");
		armorStr.Append(FString::FromInt(itemArmor));
	}

	// �Ƹ� �ؽ�Ʈ ����
	ArmorText->SetText(FText::FromString(armorStr));
	
}

UInvenSlotWidget* UInventoryWidget::GetSlotAt(int32 slot)
{
	return Cast<UInvenSlotWidget>(InventoryGrid->GetChildAt(slot));
}

