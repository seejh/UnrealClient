// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LoginController.generated.h"


class UMyGameInstance;


UCLASS()
class MMOCLIENT_API ALoginController : public APlayerController
{
	GENERATED_BODY()
public:
	ALoginController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UMyGameInstance* _ownerInstance;
};
