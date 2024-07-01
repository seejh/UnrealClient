// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"


class UTextBlock;
class UInvenSlotWidget;
class UWrapBox;
class UItem;
UCLASS()
class MMOCLIENT_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Init();

	void SetInvenSlot(UItem* item);
	void UpdateDamageText();
	void UpdateArmorText();

	UInvenSlotWidget* GetSlotAt(int32 slot);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UWrapBox* InventoryGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* DamageText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ArmorText;
};
