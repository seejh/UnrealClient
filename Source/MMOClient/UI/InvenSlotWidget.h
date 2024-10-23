// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InvenSlotWidget.generated.h"


/*------------------------------------------------------------------------------
	UMyGameInstance에 있는 C++ 인벤토리에 들어가 있는 아이템(UItem*)를 참조하며
	해당 아이템 외부에서 변경 후 이 위젯을 업데이트(UI) 하는 방식으로 사용
------------------------------------------------------------------------------*/
class UItem;
class UInventoryWidget;
UCLASS()
class MMOCLIENT_API UInvenSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 상위컨테이너참조, 슬롯인덱스설정, 툴팁위젯생성, 기본섬네일적용
	void Init(UInventoryWidget* owner, int32 SlotIndex);
	void SetItem();

	// 타이머 그냥 활용 안하는걸로
	void SetRequestTimer(bool flag);
	void SetUseTimer();

	UFUNCTION(BlueprintCallable)
	void UpdateUI();

	UFUNCTION(BlueprintCallable)
	void RequestUseItem();

public:
	//UPROPERTY(BlueprintReadWrite)
	UItem* item = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UImage* slotThumbnail = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* slotStatus;

	UPROPERTY(meta = (BindWidget))
	class UButton* tooltipBtn;

	UPROPERTY()
	class USlotTooltipWidget* slotTooltipUI;

	UPROPERTY()
	int32 slotIndex;

	UPROPERTY()
	class UTexture2D* emptySlotTexture;

public:
	// 
	UInventoryWidget* _ownerWidget;

	// 타이머
	FTimerHandle useTimer;
	FTimerHandle requestTimer;
	bool isRequested;
	bool isUseTime = true;

	static FString NoneThumbnailPath;
};
