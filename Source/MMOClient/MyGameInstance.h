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
	// �Ŵ���, ��Ʈ�ѷ�, �ڵ鷯
	UPROPERTY()
	UDataManager* _dataManager;

	TSharedPtr<FNetSession> _netSession;
	TSharedPtr<FServerPacketHandler> _packetHandler;
	AMyPlayerController* _gameController;
	ALoginController* _loginController;

	// ����
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

	// �� ����
	// �� ĳ�� ����Ʈ
	TArray<PROTOCOL::ObjectInfo> _myCharacterList;

	// �κ�
	UPROPERTY()
	TArray<UItem*> _inventory;
	
	// �ΰ��� ����
	PROTOCOL::ObjectInfo* _myCharacterInfo;  // �ϴ�....�� �÷��̾� �޸𸮰� ����� ���� ����
	AMMOClientCharacter* _myCharacter;
	bool _isMeAlive = true;

	// ��Ÿ��
	float _lastSkill0 = 0;

	// ���� ����
	PROTOCOL::PlayerServerState _playerState = PROTOCOL::PlayerServerState::SERVER_STATE_NONE;
};
