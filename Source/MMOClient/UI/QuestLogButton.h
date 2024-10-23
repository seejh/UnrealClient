// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestLogButton.generated.h"

class UQuestLogContentWidget;
UCLASS()
class MMOCLIENT_API UQuestLogButton : public UUserWidget
{
	GENERATED_BODY()
public:
	// ������ ������ �� �� �� �� ȣ��ȴ�.
	virtual void NativeOnInitialized() override;

	//virtual void NativePreConstruct() override;

	// AddToViewport �� ȣ��ȴ�. Viewport�� Add�� �� ���� �Ҹ�
	//virtual void NativeConstruct() override;

	// RemoveFromParent�� �� ȣ��, Viewport���� Remove�� �� ���� �Ҹ�
	//virtual void NativeDestruct() override;

	UFUNCTION()
		void OnBtnClicked();

public:
	UPROPERTY(meta = (BindWidget))
		class UButton* Btn;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Txt;
public:
	class UQuestLogWidget* ownerUI;
	UQuestLogContentWidget* contentUI;
};
