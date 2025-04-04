// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/GameInstance.h"
#include"proto/Protocol3.pb.h"
#include"Data/GameStruct.h"
#include "MyGameInstance.generated.h"

class UQuestManager;
class ULoginWidget;
class ULobbyWidget;
class UInventoryWidget;
class UDataManager;
class AMMOClientCharacter;
class AMyNpc;
class AMyPlayerController;
class ALoginController;
class FNetSession;
class FServerPacketHandler;
class UItem;
class UMyHUDWidget;
class FWebManager;
class UFirebaseManager;
class UUIManager;
class UObjectsManager;
UCLASS()
class MMOCLIENT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UMyGameInstance();
	virtual void Init() override;
	virtual void BeginDestroy() override;

	// 
	void AddItem(PROTOCOL::ItemInfo itemInfo);
	void UpdateItem(PROTOCOL::ItemInfo itemInfo);

	// ����Ŷ
	void Handle_Login(PROTOCOL::S_LOGIN fromPkt);
	void Handle_CreateAccount(FCreateAccountPacketRes pkt);

	// ��Ŷ
	void Handle_EnterRoom(PROTOCOL::ObjectInfo info);
	void Handle_LeaveRoom(PROTOCOL::S_LEAVE_ROOM fromPkt);
	void Handle_ItemList(PROTOCOL::S_ITEMLIST fromPkt);
	void Handle_UseItem(PROTOCOL::S_USE_ITEM fromPkt);
	void Handle_EquipItem(PROTOCOL::S_EQUIP_ITEM fromPkt);
	void Handle_RemoveItem(PROTOCOL::S_REMOVE_ITEM fromPkt);
	void Handle_AddItem(PROTOCOL::S_ADD_ITEM fromPkt);
	void Handle_UpdateItem(PROTOCOL::S_UPDATE_ITEM fromPkt);
	void Handle_ChangeStat(PROTOCOL::ObjectInfo info);
	void Handle_CreatePlayer(PROTOCOL::ObjectInfo info);
	void Handle_AddExp(int32 exp);;
	void Handle_Spawn(PROTOCOL::S_SPAWN fromPkt);
	void Handle_DeSpawn(PROTOCOL::S_DESPAWN fromPkt);
	void Handle_Skill(PROTOCOL::S_SKILL fromPkt);
	void Handle_Chat(PROTOCOL::S_CHAT fromPkt);
	void Handle_ChangeHp(PROTOCOL::S_CHANGE_HP fromPkt);
	void Handle_Die(PROTOCOL::S_DIE fromPkt);
	void Handle_AddQuest(PROTOCOL::QuestInfo questInfo);
	void Handle_RemoveQuest(PROTOCOL::S_REMOVE_QUEST fromPkt);
	void Handle_CompleteQuest(PROTOCOL::S_COMPLETE_QUEST fromPkt);
	void Handle_UpdateQuest(PROTOCOL::S_UPDATE_QUEST fromPkt);

	// ���� �ʿ�
	PROTOCOL::PFVector FVectorToPFV(FVector fv);
	PROTOCOL::PFVector FRotatorToPFV(FRotator fr);
	FVector PFVtoFVector(PROTOCOL::PFVector pfv);
	FRotator PFVtoFRoator(PROTOCOL::PFVector pfv);
	

public:
	// �Ŵ���, ��Ʈ�ѷ�, �ڵ鷯
	UPROPERTY()
		UDataManager* _dataManager;
	UPROPERTY()
		UUIManager* _uiManager;
	UPROPERTY()
		UObjectsManager* _objectsManager;
	
	TSharedPtr<FNetSession> _netSession;
	TSharedPtr<FServerPacketHandler> _packetHandler;
	FWebManager* _webManager;
	AMyPlayerController* _playerController;
	ALoginController* _loginController;
	UFirebaseManager* _firebaseManager;
	
	UPROPERTY()
		TMap<int32, UItem*> inventory;
	UPROPERTY()
		UQuestManager* _questManager;


	// ���� ��ȣ�ۿ� ����
	AMyNpc* _nowInteractable;

	// ������, �ǻ��(���������, )
	bool _isRelease;

	// ���� �� ���� : �ϴ� ����
	FString _token;
	int32 _accountid;
	
	// �� ����
	// �� ĳ�� ����Ʈ
	TArray<PROTOCOL::ObjectInfo> _myCharacterList;


	// �ΰ��� ����
	PROTOCOL::ObjectInfo* _myCharacterInfo;  // �ϴ�....�� �÷��̾� �޸𸮰� ����� ���� ����
	AMMOClientCharacter* _myCharacter;
	bool _isMeAlive = true;

	// ��Ÿ��
	float _lastSkill0 = 0;

	// ���� ����
	PROTOCOL::PlayerServerState _playerState = PROTOCOL::PlayerServerState::SERVER_STATE_NONE;
};
