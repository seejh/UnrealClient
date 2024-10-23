// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include"../Data/GameStruct.h"

#include "ServerListSlotWidget.generated.h"


class UTextBlock;
UCLASS()
class MMOCLIENT_API UServerListSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		void ConnectToGameServer();

public:
	void SetServerInfo(FServerInfo serverInfo);

public:
	UPROPERTY(meta = (BindWidget))
		UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* BusyScore;

	FServerInfo _serverInfo;
};
