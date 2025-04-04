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

	// �� UI�� ���� ���� ����
	bIsFocusable = true;

	// �����ν��Ͻ� ���� ����
	_ownerInstance = Cast<UMyGameInstance>(GetGameInstance());
	
	// ù�⵿�̸�
	if (isFirst == true) {
		// �� �κ��丮�� �̷�� UI ��ü�� �̸� ���� (����, �������� ��)
		for (int i = 0; i < MAX_INVEN_SIZE; i++) {
			// ���� ����
			UInvenSlotWidget* slotWidget = Cast<UInvenSlotWidget>(CreateWidget(GetWorld(), _ownerInstance->_uiManager->InventorySlotClass));
			if (IsValid(slotWidget) == false) {
				UE_LOG(LogTemp, Error, TEXT("UInventoryWidget::Init() Error - Create InvenSlot"));
				return;
			}

			// �κ��� ���� �߰�
			InventoryGrid->AddChild(slotWidget);

			// ���� �ʱ�ȭ
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
	// �ش� ������ ���� ã��
	UInvenSlotWidget* slotWidget = Cast<UInvenSlotWidget>(InventoryGrid->GetChildAt(slot));
	if (IsValid(slotWidget) == false)
		return;

	// ������Ʈ
	slotWidget->UpdateUI();
}

void UInventoryWidget::UpdateDamageText()
{
	// �Ѱ��ݷ� ǥ�� (�Ѱ� = ���ݰ� + ������ ��)

	FString damageStr("");

	// ���� ��
	damageStr.Append(FString::FromInt(_ownerInstance->_myCharacterInfo->stat().damage()));
	
	// ���� ��
	int32 itemDamage = 0;
	for (auto p : _ownerInstance->inventory) {

		// �κ����� �����̸� ���� ������ ������ Ž�� - ���� �ʿ�(Ž������)
		if (p.Value->itemData->itemType == EItemType::ITEM_TYPE_WEAPON && p.Value->itemDB.equipped == true) {
			itemDamage += Cast<UWeaponData>(p.Value->itemData)->damage;
			break;
		}
	}

	// ���������� �߰��Ǵ� ���ݷ��� ������ �׳� ���ݸ� ǥ���� (�ڿ� �������� + �� ���̰�)
	if (itemDamage != 0) {
		damageStr.Append(" + ");
		damageStr.Append(FString::FromInt(itemDamage));
	}

	// 
	DamageText->SetText(FText::FromString(damageStr));
}

void UInventoryWidget::UpdateArmorText()
{
	// �ѹ��� ǥ�� (�ѹ� = ���� �� + �����۵� ��, �ٸ� ���� �⺻���ݿ� ���� ����)

	FString armorStr("0");

	// �� ��
	int32 itemArmor = 0;
	for (auto p : _ownerInstance->inventory) {

		// �κ����� ���̸� ���� ������ ������ Ž��
		if (p.Value->itemData->itemType == EItemType::ITEM_TYPE_ARMOR && p.Value->itemDB.equipped == true) 
			itemArmor += Cast<UArmorData>(p.Value->itemData)->defence;
	}

	// ���������� �߰��Ǵ� ������ ������ �׳� ���ݸ� ǥ���� (�ڿ� �������� + �� ���̰�)
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



