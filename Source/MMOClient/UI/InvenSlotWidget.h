// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include<Components/Image.h>
#include<Components/TextBlock.h>
#include<Components/Button.h>
#include"SlotTooltipWidget.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InvenSlotWidget.generated.h"


class UItem;
class USlotTooltipWidget;
UCLASS()
class MMOCLIENT_API UInvenSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(int32 SlotIndex);
	void SetItem(UItem* Item);

	UFUNCTION(BlueprintCallable)
	void UpdateWidget();

	UFUNCTION(BlueprintCallable)
	void RequestUseItem();

public:
	//UPROPERTY(BlueprintReadWrite)
	UItem* item = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* slotThumbnail = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* slotStatus;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* tooltipBtn;

	UPROPERTY(BlueprintReadWrite)
	USlotTooltipWidget* slotTooltipWidget;

	UPROPERTY(BlueprintReadWrite)
	int32 slotIndex;
public:
	FTimerHandle useTimer;

	bool isRequested;
	bool isUseTimeOn = true;

	static FString NoneThumbnailPath;
};
