// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMonster.h"

#include"../UI/MonsterHPWidget.h"

#include"../MyAIController.h"

// Sets default values
AMyMonster::AMyMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	AIControllerClass = AMyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// HPBar 컴포넌트 생성
	HPBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBar"));
	HPBar->SetupAttachment(GetMesh());
	HPBar->SetRelativeLocation(FVector(0.f, 0.f, 260.f));

	// 컴포넌트에 위젯 블루프린트 연결
	static ConstructorHelpers::FClassFinder<UUserWidget> HPBarWidgetRef = TEXT("WidgetBlueprint'/Game/UI/WB_MonsterHPWidget.WB_MonsterHPWidget_C'");
	if (HPBarWidgetRef.Succeeded()) {
		HPBar->SetWidgetClass(HPBarWidgetRef.Class);
		HPBar->SetWidgetSpace(EWidgetSpace::Screen);
		HPBar->SetDrawSize(FVector2D(150.f, 15.f));
		HPBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AMyMonster::MoveToLocation(FVector location)
{
	// 
	//location.Z = 145.f;

	AMyAIController* controller = Cast<AMyAIController>(GetController());
	if (IsValid(controller)) {
		EPathFollowingRequestResult::Type result = controller->MoveToLocation(location);
		if (result == EPathFollowingRequestResult::Failed) 
			UE_LOG(LogTemp, Error, TEXT("MoveToLocation Failed"));
	}
}

void AMyMonster::OnAttack()
{
	Super::OnAttack();	
}

void AMyMonster::OnAttacked()
{
	Super::OnAttacked();
}

void AMyMonster::OnDead()
{
	Super::OnDead();
}

void AMyMonster::UpdateHP()
{
	Super::UpdateHP();

	UMonsterHPWidget* hpWidget = Cast<UMonsterHPWidget>(HPBar->GetWidget());
	if (IsValid(hpWidget)) 
		hpWidget->SetHPBar((float)info->stat().hp() / (float)info->stat().maxhp());
}

void AMyMonster::SetActorActive(bool flag)
{
	Super::SetActorActive(flag);
}

// Called when the game starts or when spawned
void AMyMonster::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

