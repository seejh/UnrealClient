// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include<Input/Reply.h>

#include "QuestLogWidget.generated.h"


/*------------------------------------------------------------------
	1. ����Ʈ �Ŵ����� ����
	2. ����Ʈ �Ŵ��� ���� �긦 ����Ʈ �Ŵ���ó�� ���

	�ϴ� 1�� ������� �ϴ� �ɷ� =>
	����Ʈ �Ŵ������� ����Ǵ� ������ ����� ������ UI�� ����Ʈ���� ����ȭ

	����Ʈ �Ŵ��������� ����Ʈ ��ȣ�� �Ϸ������� ����.
	����Ʈ ��ȣ�� �����ϸ� �Ϸ�, ���� ���� �������� �ʴ´�.
	�ٸ� ���� UI���� ǥ��� �����ؾ� �Ѵ�.



	3 ������ ������ �ִ� ����Ʈ�� �Ϸ��Ѵ�
	�������� ����Ʈ���� �Ϸ�� ����Ʈ ����Ʈ�� �̵�
	RemoveQuest�� �����ϰ� AddQuest�� �����ϸ�?

-------------------------------------------------------------------*/
class UQuest;
UCLASS()
class MMOCLIENT_API UQuestLogWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	// �� ������ �� �Ҹ��� �ڵ�
	// ����Ʈ UI�� Ȱ��ȭ�� �� ����Ʈ �Ŵ����κ��� ����Ʈ ������ �ε�
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
