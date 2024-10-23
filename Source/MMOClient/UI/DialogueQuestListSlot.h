// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueQuestListSlot.generated.h"


class UDialogueQuestList;
class UQuestData;
UCLASS()
class MMOCLIENT_API UDialogueQuestListSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void Init(UDialogueQuestList* OwnerUI, UQuestData* QuestData);
	
	UFUNCTION()
		void OnBtnClicked();
public:
	UPROPERTY(meta = (BindWidget))
		class UButton* Btn;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Txt;
public:
	class UDialogueQuestList* ownerUI;
	UQuestData* questData;
};
