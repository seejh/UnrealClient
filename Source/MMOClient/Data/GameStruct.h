// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// #include"../MyGameInstance.h"
#include"../Proto/Protocol3.pb.h"

#include "Engine/DataTable.h"
#include "GameStruct.generated.h"


/*------------------------------------------------------------------
	USTRUCT는 값 유형으로 포인터로 사용할 수 없다.
	
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
	WebPacket
------------------------------------------------------------------*/

// 구글 oauth
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

// 기존

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
	CONSUMABLE_TYPE_HP_POTION,
	CONSUMABLE_TYPE_MP_POTION,
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
	FString assetPath;

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

/*------------------------------------------------------------------
	퀘스트 관련
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


	// 퀘스트 타입, 목표물, 목표치
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

	// 보상
	UPROPERTY()
		int32 rewardExp;
	UPROPERTY()
		int32 rewardMoney;
	UPROPERTY()
		TMap<int32, int32> rewardItems;
};

/*------------------------------------------------------------------
	NPC 관련
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
	NPC 대화 관련
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

