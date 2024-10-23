// Fill out your copyright notice in the Description page of Project Settings.


#include "SlotTooltipWidget.h"
#include<Components/TextBlock.h>
#include"../Data/GameStruct.h"


void USlotTooltipWidget::UpdateToolTip(UItem* item)
{
	// 아이템 명
	name->SetText(FText::FromString(item->itemData->name));

	// 설명
	description->SetText(FText::FromString(item->itemData->description));

	// 디테일
	FString txt("");

	// 무기
	if (item->itemData->itemType == EItemType::ITEM_TYPE_WEAPON) {
		txt.Append("Damage : ")
			.Append(FString::FromInt(Cast<UWeaponData>(item->itemData)->damage));

		details->SetText(FText::FromString(txt));
	}

	// 방어구
	else if (item->itemData->itemType == EItemType::ITEM_TYPE_ARMOR) {
		txt.Append("Defence : ")
			.Append(FString::FromInt(Cast<UArmorData>(item->itemData)->defence));

		details->SetText(FText::FromString(txt));
	}

	// 소모품
	else if (item->itemData->itemType == EItemType::ITEM_TYPE_CONSUMABLE) {
		txt.Append("Recovery : ")
			.Append(FString::FromInt(Cast<UConsumableData>(item->itemData)->recovery));
		
		details->SetText(FText::FromString(txt));
	}
}
