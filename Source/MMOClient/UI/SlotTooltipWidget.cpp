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
	FString text("");

	// 디테일 - 무기
	if (item->itemData->itemType == EItemType::ITEM_TYPE_WEAPON) {
		// 대미지 추출 및 텍스트 세팅
		text += "Damage : ";
		text += FString::FromInt(Cast<UWeaponData>(item->itemData)->damage);

		// 설정
		details->SetText(FText::FromString(text));
	}

	// 디테일 - 방어구
	else if (item->itemData->itemType == EItemType::ITEM_TYPE_ARMOR) {
		// 방어도 추출 및 텍스트 세팅
		text += "Defence : ";
		text += FString::FromInt(Cast<UArmorData>(item->itemData)->defence);

		// 설정
		details->SetText(FText::FromString(text));
	}

	// 디테일 - 소모품
	else if (item->itemData->itemType == EItemType::ITEM_TYPE_CONSUMABLE) {
		// 회복력 추출 및 텍스트 세팅
		text += "Recovery : ";
		text += FString::FromInt(Cast<UConsumableData>(item->itemData)->recovery);

		// 설정
		details->SetText(FText::FromString(text));
	}
}
