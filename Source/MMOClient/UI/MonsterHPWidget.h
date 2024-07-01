// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include<Components/ProgressBar.h>

#include "MonsterHPWidget.generated.h"

/**
 * 
 */
UCLASS()
class MMOCLIENT_API UMonsterHPWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UProgressBar* HPBar;

	void SetHPBar(float percent);
public:
};
