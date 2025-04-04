// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include"MyCharacterBase.h"

#include "MyMonster.generated.h"

UCLASS()
class MMOCLIENT_API AMyMonster : public AMyCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyMonster();
	void MoveToLocation(FVector location);	

	// override
	virtual void OnAttack() override;
	virtual void OnAttacked() override;
	virtual void OnDead() override;
	virtual void UpdateHP() override;
	virtual void SetActorActive(bool flag) override;


	UPROPERTY(VisibleAnywhere)
		class UWidgetComponent* HPBar;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
