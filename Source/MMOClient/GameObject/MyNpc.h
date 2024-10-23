// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include"MyCharacterBase.h"

#include "GameFramework/Character.h"
#include "MyNpc.generated.h"

class UBoxComponent;

UCLASS()
class MMOCLIENT_API AMyNpc : public AMyCharacterBase
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	AMyNpc();


	UFUNCTION()
	void ComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void ComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex);

	// InteractUI닫고, 해당 상호작용 진행
	// UFUNCTION()
	void Interact();
public:
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* Collider;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
