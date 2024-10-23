// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include"GameObject/MMOClientCharacter.h"
#include"UI/MyHUDWidget.h"
#include"Proto/Protocol3.pb.h"

#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/*-------------------------------------------------------------------------------
	PlayerInfo
-------------------------------------------------------------------------------*/
class ActorInfo {
public:

	//FVector GetVelocity();
	//void SetPos(PROTOCOL::Position pos);
public:
	PROTOCOL::ObjectInfo info;
	AActor* _ownerActor = nullptr;
};

/*---------------------------------------------------------------------------------------------
	MyPlayerController
---------------------------------------------------------------------------------------------*/
class UMyGameInstance;
class FObjectManager;
UCLASS()
class MMOCLIENT_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMyPlayerController();

	void Init(bool isFirst);

	/*---------------------------------------------------------------------------
		기타
	---------------------------------------------------------------------------*/
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void ShowMouseCursor(bool isShow);

	/*---------------------------------------------------------------------------
		외부 입력
	--------------------------------------------------------------------------*/
	void SpawnObject(PROTOCOL::S_Spawn& spawnPkt);
	void DeSpawnObject(PROTOCOL::S_DeSpawn& despawnPkt);
	void MoveUpdate(PROTOCOL::ObjectInfo info);
	void ChangeHP(PROTOCOL::S_ChangeHp fromPkt);
	void Skill(PROTOCOL::S_Skill& skillPkt);
	void Die(PROTOCOL::S_Die fromPkt);
	void PlayerChat(PROTOCOL::S_Chat fromPkt);
	void SystemChat(FString chatMessage);

	/*---------------------------------------------------------------------------
		개인 플레이어 입력
	---------------------------------------------------------------------------*/
	void SendMyPos();
	void MyPlayerAttack(TArray<int32>& playerArray);
	void MyPlayerChat(FString& chatMessage);

protected:
	virtual void SetupInputComponent() override;
public:
	void Interact();
	void MouseCursorOnOff();
	void OpenInventory();
	void OpenQuest();

public:
	
	FTimerHandle _timerHandle_myPosSend;
	UMyGameInstance* _ownerInstance;
	
	int64 _lastSendTime = 0;
};
