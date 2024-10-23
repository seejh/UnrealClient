// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStruct.h"
#include"../DataManager.h"
#include"../MyGameInstance.h"

/*
	참고 : 
	UObject 객체들의 override는 이런식으로 한다고 알고있음..
*/

void UItem::Init(UMyGameInstance* instance, int32 templateId)
{
	// 데이터매니저에서 아이템데이터(스펙) 조회, 여기 설정
	itemData = instance->_dataManager->_itemTable[templateId];
	if (IsValid(itemData) == false) {
		UE_LOG(LogTemp, Error, TEXT("UItem::Init() Error - No Match Item In DataManager"));
		return;
	}
}

void UConsumable::Init(UMyGameInstance* instance, int templateId)
{
	Super::Init(instance, templateId);
	
	// 중첩여부
	stackable = true;
}

void UArmor::Init(UMyGameInstance* instance, int templateId)
{
	Super::Init(instance, templateId);

	// 중첩여부
	stackable = false;
}

void UWeapon::Init(UMyGameInstance* instance, int templateId)
{
	Super::Init(instance, templateId);

	// 중첩 여부
	stackable = false;
}

void UItem::SetItemDB(PROTOCOL::ItemInfo itemInfo)
{
	// ItemDB 설정
	itemDB.itemDbId = itemInfo.itemdbid();
	itemDB.templateId = itemInfo.templateid();
	itemDB.count = itemInfo.count();
	itemDB.slot = itemInfo.slot();
	itemDB.equipped = itemInfo.equipped();
}

