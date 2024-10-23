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

	// 웹패킷
	void Handle_Login(PROTOCOL::S_Login fromPkt);
	void Handle_CreateAccount(FCreateAccountPacketRes pkt);

	// 패킷
	void Handle_EnterRoom(PROTOCOL::ObjectInfo info);
	void Handle_LeaveRoom(PROTOCOL::S_Leave_Room fromPkt);
	void Handle_ItemList(PROTOCOL::S_ItemList fromPkt);
	void Handle_UseItem(PROTOCOL::S_UseItem fromPkt);
	void Handle_EquipItem(PROTOCOL::S_EquipItem fromPkt);
	void Handle_RemoveItem(PROTOCOL::S_RemoveItem fromPkt);
	void Handle_AddItem(PROTOCOL::S_AddItem fromPkt);
	void Handle_UpdateItem(PROTOCOL::S_UpdateItem fromPkt);
	void Handle_ChangeStat(PROTOCOL::ObjectInfo info);
	void Handle_CreatePlayer(PROTOCOL::ObjectInfo info);
	void Handle_AddExp(int32 exp);;
	void Handle_Spawn(PROTOCOL::S_Spawn fromPkt);
	void Handle_DeSpawn(PROTOCOL::S_DeSpawn fromPkt);
	void Handle_Skill(PROTOCOL::S_Skill fromPkt);
	void Handle_Chat(PROTOCOL::S_Chat fromPkt);
	void Handle_ChangeHp(PROTOCOL::S_ChangeHp fromPkt);
	void Handle_Die(PROTOCOL::S_Die fromPkt);
	void Handle_AddQuest(PROTOCOL::QuestInfo questInfo);
	void Handle_RemoveQuest(PROTOCOL::S_RemoveQuest fromPkt);
	void Handle_CompleteQuest(PROTOCOL::S_CompleteQuest fromPkt);
	void Handle_UpdateQuest(PROTOCOL::S_UpdateQuest fromPkt);

public:
	// 매니저, 컨트롤러, 핸들러
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


	// 현재 상호작용 액터
	AMyNpc* _nowInteractable;

	// 게임 전 정보 : 일단 여기
	FString _token;
	int32 _accountid;
	
	// 내 정보
	// 내 캐릭 리스트
	TArray<PROTOCOL::ObjectInfo> _myCharacterList;


	// 인게임 정보
	PROTOCOL::ObjectInfo* _myCharacterInfo;  // 일단....내 플레이어 메모리가 사라질 때가 위험
	AMMOClientCharacter* _myCharacter;
	bool _isMeAlive = true;

	// 쿨타임
	float _lastSkill0 = 0;

	// 접속 상태
	PROTOCOL::PlayerServerState _playerState = PROTOCOL::PlayerServerState::SERVER_STATE_NONE;
};
