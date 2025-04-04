// Fill out your copyright notice in the Description page of Project Settings.


#include "BotPlayer.h"
#include"../MyAIController.h"

// Sets default values
ABotPlayer::ABotPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 디폴트 AI 컨트롤러
	AIControllerClass = AMyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::Spawned;

	// 디폴트 메시
	//ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("SkeletalMesh'/Game/ParagonShinbi/Characters/Heroes/Shinbi/Meshes/Shinbi.Shinbi'"));
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("SkeletalMesh'/Game/ParagonCrunch/Characters/Heroes/Crunch/Meshes/Crunch.Crunch'"));
	//if (tempMesh.Succeeded()) {
	//	if (GetMesh() == nullptr)
	//		UE_LOG(LogTemp, Error, TEXT("No GetMesh"));

	//	GetMesh()->SetSkeletalMesh(tempMesh.Object);
	//	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	//}
	//else {
	//	UE_LOG(LogTemp, Error, TEXT("ABotPlayer() Error - BotPlayer, NoMesh"));
	//}

	//// 애니메이션 블루프린트 모드
	//GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	//// 애니메이션 블루프린트 
	////ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimObj(TEXT("AnimBlueprint'/Game/ParagonShinbi/Characters/Heroes/Shinbi/Shinbi_AnimBlueprint.Shinbi_AnimBlueprint'"));
	//// static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimObj(TEXT("AnimBlueprint'/Game/ParagonCrunch/Characters/Heroes/Crunch/Crunch_AnimBlueprint.Crunch_AnimBlueprint'"));
	//static ConstructorHelpers::FClassFinder<UAnimInstance> AnimObj(TEXT("AnimBlueprint'/Game/ParagonCrunch/Characters/Heroes/Crunch/Crunch_AnimBlueprint.Crunch_AnimBlueprint_C'"));
	//if (AnimObj.Succeeded()) {
	//	// GetMesh()->SetAnimInstanceClass(AnimObj.Object->GeneratedClass);
	//	GetMesh()->SetAnimInstanceClass(AnimObj.Class);
	//}
	//else {
	//	UE_LOG(LogTemp, Error, TEXT("ABotPlayer() Error - BotPlayer, No AnimBlueprint"));
	//	return;
	//}
}

void ABotPlayer::MovetoLocation(FVector location)
{
	AMyAIController* controller = Cast<AMyAIController>(GetController());
	if (IsValid(controller)) {
		EPathFollowingRequestResult::Type result = controller->MoveToLocation(location);
		if (result == EPathFollowingRequestResult::Failed)
			UE_LOG(LogTemp, Error, TEXT("MovetoLocation Failed"));
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ABotPlayer::MoveToLocation() Error - Invalid Controller"));
	}
}

void ABotPlayer::OnAttack()
{
	Super::OnAttack();
}

void ABotPlayer::OnAttacked()
{
	Super::OnAttacked();
}

void ABotPlayer::OnDead()
{
	Super::OnDead();
}

void ABotPlayer::UpdateHP()
{

}

void ABotPlayer::SetActorActive(bool flag)
{
}

void ABotPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//if (MeshAsset) {
	//	USkeletalMeshComponent* characterMesh = GetMesh();
	//	characterMesh->SetSkeletalMesh(MeshAsset);
	//}
}

// Called when the game starts or when spawned
void ABotPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABotPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABotPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

