// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStruct.h"
#include"../DataManager.h"
#include"../MyGameInstance.h"

/*
	���� : 
	UObject ��ü���� override�� �̷������� �Ѵٰ� �˰�����..
*/

void UItem::Init(UMyGameInstance* instance, int32 templateId)
{
	// �����͸Ŵ������� �����۵�����(����) ��ȸ, ���� ����
	itemData = instance->_dataManager->_itemTable[templateId];
	if (IsValid(itemData) == false) {
		UE_LOG(LogTemp, Error, TEXT("UItem::Init() Error - No Match Item In DataManager"));
		return;
	}
}

void UConsumable::Init(UMyGameInstance* instance, int templateId)
{
	Super::Init(instance, templateId);
	
	// ��ø����
	stackable = true;
}

void UArmor::Init(UMyGameInstance* instance, int templateId)
{
	Super::Init(instance, templateId);

	// ��ø����
	stackable = false;
}

void UWeapon::Init(UMyGameInstance* instance, int templateId)
{
	Super::Init(instance, templateId);

	// ��ø ����
	stackable = false;
}

void UItem::SetItemDB(PROTOCOL::ItemInfo itemInfo)
{
	// ItemDB ����
	itemDB.itemDbId = itemInfo.itemdbid();
	itemDB.templateId = itemInfo.templateid();
	itemDB.count = itemInfo.count();
	itemDB.slot = itemInfo.slot();
	itemDB.equipped = itemInfo.equipped();
}

