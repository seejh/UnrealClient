// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// #include"../MyGameInstance.h"
#include"../Proto/Protocol3.pb.h"

#include "Engine/DataTable.h"
#include "GameStruct.generated.h"


/*------------------------------------------------------------------
	USTRUCT�� �� �������� �����ͷ� ����� �� ����.
	
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
	WebPacket
------------------------------------------------------------------*/

// ���� oauth
USTRUCT(BlueprintType)
struct FGoogleLoginRequestPacket {
	GENERATED_BODY()
public:
	UPROPERTY()
		FString ID;
	UPROPERTY()
		FString PW;
	UPROPERTY()
		bool ReturnSecureToken;
};

USTRUCT(BlueprintType)
struct FGoogleLoginResponsePacket {
	GENERATED_BODY()
public:
	UPROPERTY()
		FString Kind;
	UPROPERTY()
		FString LocalId;
	UPROPERTY()
		FString Email;
	UPROPERTY()
		FString DisplayName;
	UPROPERTY()
		FString IdToken;
	UPROPERTY()
		bool Registered;
	UPROPERTY()
		FString RefreshToken;
	UPROPERTY()
		FString ExpiresIn;
};

// ����

USTRUCT(BlueprintType)
struct FServerInfo {
	GENERATED_BODY()
public:
	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString IpAddress;
	
	UPROPERTY()
	int32 Port;

	UPROPERTY()
	int32 BusyScore;
};

// TEST WEB
USTRUCT(BlueprintType)
struct FCreateAccountPacketReq {
	GENERATED_BODY()
public:
	UPROPERTY()
	FString AccountName;
	
	UPROPERTY()
	int32 AccountPw;
};

USTRUCT(BlueprintType)
struct FCreateAccountPacketRes {
	GENERATED_BODY()
public:
	UPROPERTY()
	bool CreateOk;
};

USTRUCT(BlueprintType)
struct FLoginAccountReq {
	GENERATED_BODY()
public:
	UPROPERTY()
	FString AccountName;

	UPROPERTY()
	FString AccountPw;
};

USTRUCT(BlueprintType)
struct FLoginAccountRes {
	GENERATED_BODY()
public:
	UPROPERTY()
	bool LoginOk;

	UPROPERTY()
	FString AccountName;

	UPROPERTY()
	FString Token;

	UPROPERTY()
	TArray<FServerInfo> serverList;
};

USTRUCT(BlueprintType)
struct FLoginFirebasePacketReq {
	GENERATED_BODY()
public:
	UPROPERTY()
		FString Token;
};

USTRUCT(BlueprintType)
struct FLoginFirebasePacketRes {
	GENERATED_BODY()
public:
	UPROPERTY()
		bool LoginOK;
	UPROPERTY()
		FString JwtAccessToken;
};

/*---------------------------------------------------------------
	������ ����
---------------------------------------------------------------*/
/*---------------------------------------------------------------
	������ ������ (ITEM TYPE)
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
	CONSUMABLE_TYPE_HP_POTION,
	CONSUMABLE_TYPE_MP_POTION,
};


/*---------------------------------------------------------------
	�����۵����� - ���� ����
	id, ��Ī, Ÿ��, ��������(ex.����Ÿ��, ���ݷ�)
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
	������DB - ���� ����
	itemdbid, templateid, ����, ����, ��������
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
	������ - ���� + ����
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
	���� ����
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
	���� ����
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
	FString assetPath;

	UPROPERTY(BlueprintReadWrite)
	FStatData statData;

	UPROPERTY(BlueprintReadWrite)
	TArray<FRewardData> rewardDatas;
};

/*------------------------------------------------------------------
	��ų ����
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

/*------------------------------------------------------------------
	����Ʈ ����
------------------------------------------------------------------*/
UENUM(BlueprintType)
enum class EQuestType : uint8 {
	QUEST_TYPE_INTERACT,
	QUEST_TYPE_KILL,
	QUEST_TYPE_COLLECT,
};

UCLASS()
class UQuestData : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY()
		int32 id;
	UPROPERTY()
		FString name;
	UPROPERTY()
		FString description;


	// ����Ʈ Ÿ��, ��ǥ��, ��ǥġ
	UPROPERTY()
		EQuestType type;
	UPROPERTY()
		int32 objectiveId;
	UPROPERTY()
		int32 quantity;

	//
	UPROPERTY()
		int32 QuestGiver;
	UPROPERTY()
		int32 QuestEnder;
	UPROPERTY()
		int32 dialogueId;
	UPROPERTY()
		int32 dialogueEndId;

	// ����
	UPROPERTY()
		int32 rewardExp;
	UPROPERTY()
		int32 rewardMoney;
	UPROPERTY()
		TMap<int32, int32> rewardItems;
};

/*------------------------------------------------------------------
	NPC ����
------------------------------------------------------------------*/
USTRUCT(BlueprintType)
struct FDialogue {
	GENERATED_BODY()
public:
	UPROPERTY()
		int32 dialogueId;
	UPROPERTY()
		TArray<FString> Talks;
	UPROPERTY()
		TArray<FString> TalkEnds;
};

UENUM(BlueprintType)
enum class ENpcType : uint8 {
	NPC_TYPE_NONE = 0,
	NPC_TYPE_NORMAL,
	NPC_TYPE_MERCHANT,
};

UCLASS()
class UNpcData : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY()
		int32 id;
	UPROPERTY()
		FString name;
	UPROPERTY()
		ENpcType npcType;
	UPROPERTY()
		FString greeting;
	UPROPERTY()
		TMap<int32, FDialogue> dialogues;
	UPROPERTY()
		TArray<int> quests;
};

/*------------------------------------------------------------------
	NPC ��ȭ ����
------------------------------------------------------------------*/


UCLASS()
class UNpcDialogueData : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY()
		int32 npcId;
	UPROPERTY()
		TMap<int32, FDialogue> dialogues;
};

