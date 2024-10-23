// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include<../Data/GameStruct.h>

#include "ServerListWidget.generated.h"

class UWrapBox;
UCLASS()
class MMOCLIENT_API UServerListWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UWrapBox* ServerListBox;

public:
	void SetServerList(TArray<FServerInfo> serverList);
};
