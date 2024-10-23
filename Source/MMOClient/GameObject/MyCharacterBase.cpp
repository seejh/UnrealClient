// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterBase.h"
#include"../MyGameInstance.h"
#include"../MyPlayerController.h"
#include"../ObjectManager.h"

// Sets default values
AMyCharacterBase::AMyCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyCharacterBase::OnAttack()
{
	PlayAttackAnim();
}

void AMyCharacterBase::OnAttacked()
{
	PlayAttackedAnim();
}

void AMyCharacterBase::OnDead()
{
	PlayDeadAnim();

	/*GetWorldTimerManager().SetTimer(_timerHandle, FTimerDelegate::CreateLambda(
		[this]() {
			SetActorActive(false);
		}
	), 2, false);*/
}

void AMyCharacterBase::UpdateHP()
{

}

void AMyCharacterBase::SetActorActive(bool flag)
{
	// Ȱ��ȭ
	if (flag == true) {
		// �ִϸ��̼� Ȱ��ȭ
		//GetMesh()->bPauseAnims = false;

		// ��Ʈ�ѷ� Ȱ��ȭ
		SpawnDefaultController();

		// 
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

		// �浹 Ȱ��ȭ
		SetActorEnableCollision(true);

		// ���� �ΰ��� Ȱ��ȭ
		SetActorHiddenInGame(false);
	}

	// ��Ȱ��ȭ
	else {
		// �ִϸ��̼� ��Ȱ��ȭ
		//GetMesh()->bPauseAnims = true;

		// �浹 ��Ȱ��ȭ
		SetActorEnableCollision(false);

		// ���� �ΰ��� ��Ȱ��ȭ
		SetActorHiddenInGame(true);

		// ��Ʈ�ѷ� ����
		UnPossessed();
	}
}

// Called every frame
void AMyCharacterBase::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

