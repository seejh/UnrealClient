// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/GameInstance.h"
#include"proto/Protocol3.pb.h"

#include "MyGameInstance.generated.h"


class ULoginWidget;
class ULobbyWidget;
class UInventoryWidget;
class UObjectManager;
class UDataManager;
class AMMOClientCharacter;
class AMyPlayerController;
class ALoginController;
class FNetSession;
class FServerPacketHandler;
class UItem;
class UMyHUDWidget;
UCLASS()
class MMOCLIENT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UMyGameInstance();
	virtual void Init() override;
	virtual void BeginDestroy() override;

	void EnterLobby();
	void TryEnterRoom();

	void HandleLogin(PROTOCOL::S_Login fromPkt);
	void HandleEnterRoom(PROTOCOL::ObjectInfo info);
	void HandleItemList(PROTOCOL::S_ItemList fromPkt);
	void HandleUseItem(PROTOCOL::S_UseItem fromPkt);
	void AddItem(PROTOCOL::ItemInfo itemInfo);
	void HandleChangeStat(PROTOCOL::ObjectInfo info);
	void HandleCreatePlayer(PROTOCOL::ObjectInfo info);

	void HandleAddExp(int32 exp);;

public:
	// 매니저, 컨트롤러, 핸들러
	UPROPERTY()
	UDataManager* _dataManager;

	TSharedPtr<FNetSession> _netSession;
	TSharedPtr<FServerPacketHandler> _packetHandler;
	AMyPlayerController* _gameController;
	ALoginController* _loginController;

	// 위젯
	TSubclassOf<ULoginWidget> LoginWidgetClass;
	TSubclassOf<UUserWidget> LobbyWidgetClass;
	TSubclassOf<UUserWidget> CharacterSlotWidgetClass;
	TSubclassOf<UUserWidget> CreateCharacterWidgetClass;
	TSubclassOf<UUserWidget> InventoryWidgetClass;
	TSubclassOf<UUserWidget> InvenSlotWidgetClass;
	TSubclassOf<UUserWidget> SlotTooltipWidgetClass;
	TSubclassOf<UUserWidget> RespawnWidgetClass;
	TSubclassOf<UUserWidget> MyHUDWidgetClass;
	UMyHUDWidget* _hudWidget;
	UUserWidget* _nowWidget;

	// 내 정보
	// 내 캐릭 리스트
	TArray<PROTOCOL::ObjectInfo> _myCharacterList;

	// 인벤
	UPROPERTY()
	TArray<UItem*> _inventory;
	
	// 인게임 정보
	PROTOCOL::ObjectInfo* _myCharacterInfo;  // 일단....내 플레이어 메모리가 사라질 때가 위험
	AMMOClientCharacter* _myCharacter;
	bool _isMeAlive = true;

	// 쿨타임
	float _lastSkill0 = 0;

	// 접속 상태
	PROTOCOL::PlayerServerState _playerState = PROTOCOL::PlayerServerState::SERVER_STATE_NONE;
};
