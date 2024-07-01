// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlotTooltipWidget.generated.h"

class UTextBlock;
class UItem;
UCLASS()
class MMOCLIENT_API USlotTooltipWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateToolTip(UItem* item);
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* name;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* description;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* details;
};
