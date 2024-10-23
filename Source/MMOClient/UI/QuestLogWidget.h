// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include<Input/Reply.h>

#include "QuestLogWidget.generated.h"


/*------------------------------------------------------------------
	1. 퀘스트 매니저와 연동
	2. 퀘스트 매니저 없이 얘를 퀘스트 매니저처럼 사용

	일단 1번 방식으로 하는 걸로 =>
	퀘스트 매니저에서 변경되는 정보를 여기로 보내서 UI와 퀘스트정보 동기화

	퀘스트 매니저에서는 퀘스트 번호로 일률적으로 관리.
	퀘스트 번호로 접근하며 완료, 진행 따로 구분하지 않는다.
	다만 여기 UI에서 표기는 구분해야 한다.



	3 기존에 가지고 있는 퀘스트를 완료한다
	진행중인 퀘스트에서 완료된 퀘스트 리스트로 이동
	RemoveQuest를 진행하고 AddQuest를 진행하면?

-------------------------------------------------------------------*/
class UQuest;
UCLASS()
class MMOCLIENT_API UQuestLogWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	// 룸 접속할 때 불리는 코드
	// 퀘스트 UI가 활성화될 때 퀘스트 매니저로부터 퀘스트 정보들 로드
	void Init();
	void Clear();

	void AddQuestUI(UQuest* quest);
	void RemoveQuestUI(UQuest* quest);

	void CompleteQuestUI(UQuest* quest);
	void UpdateUI(UQuest* quest);

	UFUNCTION()
	void OnBtnClicked(class UQuestLogContentWidget* contentWidget);

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
public:
	UPROPERTY(meta = (BindWidget))
		class UScrollBox* ProgressList;
	UPROPERTY(meta = (BindWidget))
		class UScrollBox* CompleteList;
	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* Switcher;

public:
	// class UMyGameInstance* instance;
};
