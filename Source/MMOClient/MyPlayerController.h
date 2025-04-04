// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include"GameObject/MMOClientCharacter.h"
#include"UI/MyHUDWidget.h"
#include"Proto/Protocol3.pb.h"

#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"



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
		상속
	---------------------------------------------------------------------------*/
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	/*---------------------------------------------------------------------------
		외부 입력
	--------------------------------------------------------------------------*/
	void SpawnObject(PROTOCOL::S_SPAWN& spawnPkt);
	void DeSpawnObject(PROTOCOL::S_DESPAWN& despawnPkt);
	void MoveUpdate(PROTOCOL::ObjectInfo info);
	void ChangeHP(PROTOCOL::S_CHANGE_HP fromPkt);
	void Skill(PROTOCOL::S_SKILL& skillPkt);
	void Die(PROTOCOL::S_DIE fromPkt);
	void PlayerChat(PROTOCOL::S_CHAT fromPkt);
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
	// 입력
	void Interact();
	void MouseCursor();
	void OpenInventory();
	void OpenQuest();

public:
	
	FTimerHandle _timerHandle_myPosSend;
	UMyGameInstance* _ownerInstance;
	
	int64 _lastSendTime = 0;
	float t = 0;
};
