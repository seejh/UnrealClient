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
	// 활성화
	if (flag == true) {
		// 애니메이션 활성화
		//GetMesh()->bPauseAnims = false;

		// 컨트롤러 활성화
		SpawnDefaultController();

		// 
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

		// 충돌 활성화
		SetActorEnableCollision(true);

		// 액터 인게임 활성화
		SetActorHiddenInGame(false);
	}

	// 비활성화
	else {
		// 애니메이션 비활성화
		//GetMesh()->bPauseAnims = true;

		// 충돌 비활성화
		SetActorEnableCollision(false);

		// 액터 인게임 비활성화
		SetActorHiddenInGame(true);

		// 컨트롤러 해제
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

