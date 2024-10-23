// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueQuestList.generated.h"

class UButton;
class UTextBlock;
UCLASS()
class MMOCLIENT_API UDialogueQuestList : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void Init();
	void Close();
	
	void OnSlotClicked(class UQuestData* questData);

public:
	UPROPERTY(meta = (BindWidget))
		class UScrollBox* QuestLists;

public:
	class UDialogueWidget* ownerUI;
};
