// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include"../MyGameInstance.h"
#include"../Proto/Protocol3.pb.h"

#include "Engine/DataTable.h"
#include "GameStruct.generated.h"


/*---------------------------------------------------------------
	아이템 관련
---------------------------------------------------------------*/
/*---------------------------------------------------------------
	아이템 열거형 (ITEM TYPE)
---------------------------------------------------------------*/
UENUM(BlueprintType)
enum class EItemType : uint8 {
	ITEM_TYPE_NONE = 0,
	ITEM_TYPE_WEAPON,
	ITEM_TYPE_ARMOR,
	ITEM_TYPE_CONSUMABLE,
};

UENUM(BlueprintType)
enum class EWeaponType : uint8 {
	WEAPON_TYPE_NONE = 0,
	WEAPON_TYPE_SWORD,
	WEAPON_TYPE_BOW,
};

UENUM(BlueprintType)
enum class EArmorType : uint8 {
	ARMOR_TYPE_NONE = 0,
	ARMOR_TYPE_HELMET,
	ARMOR_TYPE_ARMOR,
	ARMOR_TYPE_BOOTS,
};

UENUM(BlueprintType)
enum class EConsumableType : uint8 {
	CONSUMABLE_TYPE_NONE = 0,
	CONSUMABLE_TYPE_POTION,
};


/*---------------------------------------------------------------
	아이템데이터 - 스펙 정보
	id, 명칭, 타입, 세부정보(ex.무기타입, 공격력)
---------------------------------------------------------------*/
UCLASS()
class UItemData : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType itemType;
};

UCLASS()
class UWeaponData : public UItemData {
	GENERATED_BODY()
public:
	UWeaponData() { itemType = EItemType::ITEM_TYPE_WEAPON; }
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType weaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 damage;
};

UCLASS()
class UArmorData : public UItemData {
	GENERATED_BODY()
public:
	UArmorData() { itemType = EItemType::ITEM_TYPE_ARMOR; }
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EArmorType armorType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 defence;
};

UCLASS()
class UConsumableData : public UItemData {
	GENERATED_BODY()
public:
	UConsumableData() { itemType = EItemType::ITEM_TYPE_CONSUMABLE; }
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EConsumableType consumableType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 maxCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 recovery;
};

/*--------------------------------------------------------------------------
	아이템DB - 소지 정보
	itemdbid, templateid, 수량, 슬롯, 장착여부
--------------------------------------------------------------------------*/
USTRUCT(BlueprintType)
struct FItemDB {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 itemDbId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 templateId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 count;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 slot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool equipped;
};

/*--------------------------------------------------------------------------
	아이템 - 스펙 + 소지
--------------------------------------------------------------------------*/
class UMyGameInstance;
UCLASS()
class UItem : public UObject {
	GENERATED_BODY()
public:
	void SetItemDB(PROTOCOL::ItemInfo itemInfo);

	UFUNCTION(BlueprintCallable)
	void Init(UMyGameInstance* instance, int32 templateId);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FItemDB itemDB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UItemData* itemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool stackable;
};

UCLASS()
class UWeapon : public UItem {
	GENERATED_BODY()
public:
	void Init(UMyGameInstance* instance, int templateId);
};

UCLASS()
class UArmor : public UItem {
	GENERATED_BODY()
public:
	void Init(UMyGameInstance* instance, int templateId);
};

UCLASS()
class UConsumable : public UItem {
	GENERATED_BODY()
public:
	void Init(UMyGameInstance* instance, int templateId);
};

/*------------------------------------------------------------------
	스텟 관련
------------------------------------------------------------------*/
USTRUCT(BlueprintType)
struct FStatData {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	int32 level;

	UPROPERTY(BlueprintReadWrite)
	int32 totalExp;

	UPROPERTY(BlueprintReadWrite)
	int32 maxHp;

	UPROPERTY(BlueprintReadWrite)
	int32 damage;

	UPROPERTY(BlueprintReadWrite)
	float speed;

	UPROPERTY(BlueprintReadWrite)
	int32 attackDistance;

	UPROPERTY(BlueprintReadWrite)
	int32 noticeDistance;

	UPROPERTY(BlueprintReadWrite)
	int32 returnDistance;

	UPROPERTY(BlueprintReadWrite)
	int32 attackCoolTime;
};

/*------------------------------------------------------------------
	몬스터 관련
------------------------------------------------------------------*/
USTRUCT(BlueprintType)
struct FRewardData {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	int32 itemDropRate;

	UPROPERTY(BlueprintReadWrite)
	int32 itemId;

	UPROPERTY(BlueprintReadWrite)
	int32 count;
};

USTRUCT(BlueprintType)
struct FMonsterData {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	int32 id;

	UPROPERTY(BlueprintReadWrite)
	FString name;

	UPROPERTY(BlueprintReadWrite)
	FStatData statData;

	UPROPERTY(BlueprintReadWrite)
	TArray<FRewardData> rewardDatas;
};

/*------------------------------------------------------------------
	스킬 관련
------------------------------------------------------------------*/
UENUM(BlueprintType)
enum class ESkillType :uint8 {
	SKILL_TYPE_NONE = 0,
	SKILL_TYPE_AUTO,
	SKILL_TYPE_PROJECTILE,
};

USTRUCT(BlueprintType)
struct FSkillData {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	int32 id;

	UPROPERTY(BlueprintReadWrite)
	FString name;

	UPROPERTY(BlueprintReadWrite)
	float coolTime;

	UPROPERTY(BlueprintReadWrite)
	int32 damage;

	UPROPERTY(BlueprintReadWrite)
	ESkillType skillType;
};