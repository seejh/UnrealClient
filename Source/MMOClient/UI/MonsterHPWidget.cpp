// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterHPWidget.h"

void UMonsterHPWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMonsterHPWidget::SetHPBar(float percent)
{
	HPBar->SetPercent(percent);
}

