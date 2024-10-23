// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlotTooltipWidget.generated.h"

class UTextBlock;
UCLASS()
class MMOCLIENT_API USlotTooltipWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateToolTip(class UItem* item);
public:
	UPROPERTY(meta = (BindWidget))
		UTextBlock* name;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* description;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* details;
};
