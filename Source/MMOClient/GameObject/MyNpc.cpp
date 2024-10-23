// Fill out your copyright notice in the Description page of Project Settings.


#include "MyNpc.h"
#include"../MyAIController.h"
#include"../MyGameInstance.h"
#include"../UIManager.h"
#include"../UI/InteractWidget.h"
#include"../UI/MyHUDWidget.h"
#include"../UI/DialogueWidget.h"
#include"../DataManager.h"
#include"../ObjectsManager.h"
#include"../QuestManager.h"
#include<Components/BoxComponent.h>
#include"MMOClientCharacter.h"

// Sets default values
AMyNpc::AMyNpc()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	AIControllerClass = AMyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::Spawned;

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	Collider->SetupAttachment(RootComponent, NAME_None);
	Collider->SetGenerateOverlapEvents(true); // Overlap �浹 �̺�Ʈ�� ������ ������ ����(�������� �ʾƵ� �⺻�� true)
	Collider->SetBoxExtent(FVector(1.f, 1.f, 1.f), false);
	// Collider->SetCollisionProfileName(TEXT("Trigger"), false);
}

void AMyNpc::ComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// if (OtherActor == nullptr || OtherActor == this || OtherComp == nullptr) return;
	
	// �� ĳ���Ϳ� �������� �Ͼ ���� �ƴ϶�� �н�
	AMMOClientCharacter* overlapped = Cast<AMMOClientCharacter>(OtherActor);
	if (IsValid(overlapped) == false || overlapped->_objectId != Cast<UMyGameInstance>(GetGameInstance())->_myCharacterInfo->objectid())
		return;
	
	// MainUI(HUD)�� InteractUI ���� 
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	Cast<UMyHUDWidget>(instance->_uiManager->_mainUI)->InteractUI->SetVisibility(ESlateVisibility::Visible);

	// ���� ��ȣ�ۿ� ������Ʈ �� NPC�� ����
	instance->_nowInteractable = this;
}

void AMyNpc::ComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// �� ĳ���Ϳ� �������� �Ͼ ���� �ƴ϶�� �н�
	AMMOClientCharacter* overlapped = Cast<AMMOClientCharacter>(OtherActor);
	if (IsValid(overlapped) == false || overlapped->_objectId != Cast<UMyGameInstance>(GetGameInstance())->_myCharacterInfo->objectid())
		return;

	// MainUI(HUD)�� InteractUI �ݰ�
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	Cast<UMyHUDWidget>(instance->_uiManager->_mainUI)->InteractUI->SetVisibility(ESlateVisibility::Hidden);

	// ���� ��ȣ�ۿ� ������Ʈ ����
	instance->_nowInteractable = nullptr;
}

void AMyNpc::Interact()
{
	UE_LOG(LogTemp, Error, TEXT("Interact"));
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	UMyHUDWidget* mainUI = Cast<UMyHUDWidget>(instance->_uiManager->_mainUI);
	if (IsValid(mainUI) == false)
		return;

	// ���ͷ�ƮUI �ݰ�
	mainUI->InteractUI->SetVisibility(ESlateVisibility::Hidden);

	PROTOCOL::ObjectInfo* info = instance->_objectsManager->GetObjectInfoById(_objectId);
	if (info == nullptr)
		return;

	// ���ͷ�Ʈ ������Ʈ
	// ��ȣ�ۿ� ����Ʈ ��ü ������Ʈ -> ������ ��ȣ�ۿ� ��Ŷ ���� -> ��ȭ UI ���½� �Ϸᰡ�� ����Ʈ�� ���
	auto infoPtr = instance->_objectsManager->_objectInfos.Find(_objectId);
	instance->_questManager->InteractUpdateReq(_objectId, infoPtr->typetemplateid());

	// ��ȭUI
	mainUI->DialogueUI->Open(info->typetemplateid());
}


// Called when the game starts or when spawned
void AMyNpc::BeginPlay()
{
	Super::BeginPlay();
	
	Collider->OnComponentBeginOverlap.AddDynamic(this, &AMyNpc::ComponentBeginOverlap);
	Collider->OnComponentEndOverlap.AddDynamic(this, &AMyNpc::ComponentEndOverlap);
}

// Called every frame
void AMyNpc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMyNpc::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

