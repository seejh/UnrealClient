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
	Collider->SetGenerateOverlapEvents(true); // Overlap 충돌 이벤트를 감지할 것인지 설정(설정하지 않아도 기본이 true)
	Collider->SetBoxExtent(FVector(1.f, 1.f, 1.f), false);
	// Collider->SetCollisionProfileName(TEXT("Trigger"), false);
}

void AMyNpc::ComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// if (OtherActor == nullptr || OtherActor == this || OtherComp == nullptr) return;
	
	// 내 캐릭터와 오버랩이 일어난 것이 아니라면 패스
	AMMOClientCharacter* overlapped = Cast<AMMOClientCharacter>(OtherActor);
	if (IsValid(overlapped) == false || overlapped->_objectId != Cast<UMyGameInstance>(GetGameInstance())->_myCharacterInfo->objectid())
		return;
	
	// MainUI(HUD)에 InteractUI 띄우고 
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	Cast<UMyHUDWidget>(instance->_uiManager->_mainUI)->InteractUI->SetVisibility(ESlateVisibility::Visible);

	// 현재 상호작용 오브젝트 이 NPC로 설정
	instance->_nowInteractable = this;
}

void AMyNpc::ComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 내 캐릭터와 오버랩이 일어난 것이 아니라면 패스
	AMMOClientCharacter* overlapped = Cast<AMMOClientCharacter>(OtherActor);
	if (IsValid(overlapped) == false || overlapped->_objectId != Cast<UMyGameInstance>(GetGameInstance())->_myCharacterInfo->objectid())
		return;

	// MainUI(HUD)에 InteractUI 닫고
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	Cast<UMyHUDWidget>(instance->_uiManager->_mainUI)->InteractUI->SetVisibility(ESlateVisibility::Hidden);

	// 현재 상호작용 오브젝트 제거
	instance->_nowInteractable = nullptr;
}

void AMyNpc::Interact()
{
	UE_LOG(LogTemp, Error, TEXT("Interact"));
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	UMyHUDWidget* mainUI = Cast<UMyHUDWidget>(instance->_uiManager->_mainUI);
	if (IsValid(mainUI) == false)
		return;

	// 인터랙트UI 닫고
	mainUI->InteractUI->SetVisibility(ESlateVisibility::Hidden);

	PROTOCOL::ObjectInfo* info = instance->_objectsManager->GetObjectInfoById(_objectId);
	if (info == nullptr)
		return;

	// 인터랙트 업데이트
	// 상호작용 퀘스트 자체 업데이트 -> 서버에 상호작용 패킷 전송 -> 대화 UI 오픈시 완료가능 퀘스트로 등록
	auto infoPtr = instance->_objectsManager->_objectInfos.Find(_objectId);
	instance->_questManager->InteractUpdateReq(_objectId, infoPtr->typetemplateid());

	// 대화UI
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

