// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

enum {
	MAX_INVEN_SIZE = 30,
};

/*------------------------------------------------------------------------
	InventoryGrid(UWrapBox)�� ����UI���� ��Ƽ� ���
	����UI�� �̸� �����س��� ���, ���� ��ȣ ������ ����
	
-------------------------------------------------------------------------*/
class UTextBlock;
class UInvenSlotWidget;
class UItem;
UCLASS()
class MMOCLIENT_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// �κ��丮�� ����UI �̸� ����, ���Կ� c++�κ� �����۵� ����
	UFUNCTION(BlueprintCallable)
	void Init(bool isFirst);

	// �����Ǿ� �ִ� ����ĭ���� �����ϸ鼭 ���빰�鸸 ����
	void Clear();

	UInvenSlotWidget* GetSlotAt(int32 slot);

	// C++ ������ ������Ʈ �� �ش� ����UI ������Ʈ
	void SetSlot(int32 slot);
	void UpdateSlot(int32 slot);

	void UpdateDamageText();
	void UpdateArmorText();

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UWrapBox* InventoryGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* DamageText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ArmorText;

public:
	class UMyGameInstance* _ownerInstance;
};
