// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include<iostream>

#include "Blueprint/UserWidget.h"
#include "RespawnWidget.generated.h"

/**
 * 
 */
UCLASS()
class MMOCLIENT_API URespawnWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void Init();

	UFUNCTION(BlueprintCallable)
		void OnRespawnBtnClicked();
public:
	UPROPERTY(meta = (BindWidget))
		class UButton* RespawnBtn;

	std::string name;
};
