// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include"../Proto/Protocol3.pb.h"

#include "MyCharacterBase.generated.h"


UCLASS()
class MMOCLIENT_API AMyCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// 애니메이션 : 공격, 피격, 죽음
	UFUNCTION(BlueprintImplementableEvent)
		void PlayAttackAnim();
	UFUNCTION(BlueprintImplementableEvent)
		void PlayAttackedAnim();
	UFUNCTION(BlueprintImplementableEvent)
		void PlayDeadAnim();

	//
	virtual void OnAttack();
	virtual void OnAttacked();
	virtual void OnDead();
	virtual void UpdateHP();
	virtual void SetActorActive(bool flag);


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	FTimerHandle _timerHandle;

	int32 _objectId;
	// PROTOCOL::ObjectInfo info;
};
