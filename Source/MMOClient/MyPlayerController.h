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

	/*---------------------------------------------------------------------------
		override
	---------------------------------------------------------------------------*/
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/*---------------------------------------------------------------------------
		외부 입력
	--------------------------------------------------------------------------*/
	void HandleSpawn(PROTOCOL::S_Spawn& spawnPkt);
	void HandleDespawn(PROTOCOL::S_DeSpawn& despawnPkt);
	void MoveUpdate(PROTOCOL::ObjectInfo info);
	void HandleChangeHp(PROTOCOL::S_ChangeHp fromPkt);
	void HandleSkill(PROTOCOL::S_Skill& skillPkt);
	void HandleDie(PROTOCOL::S_Die fromPkt);
	void HandleChat(PROTOCOL::S_Chat fromPkt);

	/*---------------------------------------------------------------------------
		개인 플레이어 입력
	---------------------------------------------------------------------------*/
	void SendMyPos();
	void MyPlayerAttack(TArray<int32>& playerArray);
	void MyPlayerChat(FString& chatMessage);
	
protected:
	void OpenInventory();
	void MouseCursorOnOff();
	virtual void SetupInputComponent() override;

public:
	TSubclassOf<UObject> _bpPlayer;
	TSubclassOf<UObject> _bpMonster;
	USkeletalMesh* _crunchMesh;
	USkeletalMesh* _blackCrunchMesh;

	FObjectManager* _objectManager;
public:
	
	FTimerHandle _timerHandle;
	UMyGameInstance* _ownerInstance;
	
	int64 _lastSendTime = 0;
};
