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
	// 위젯이 생성될 때 딱 한 번 호출된다.
	virtual void NativeOnInitialized() override;

	//virtual void NativePreConstruct() override;

	// AddToViewport 시 호출된다. Viewport에 Add될 때 마다 불림
	//virtual void NativeConstruct() override;

	// RemoveFromParent할 때 호출, Viewport에서 Remove될 때 마다 불림
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
