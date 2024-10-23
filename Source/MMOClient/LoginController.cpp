// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginController.h"

#include"MyGameInstance.h"
#include"ServerPacketHandler.h"
#include"UI/LoginWidget.h"
#include"UIManager.h"

ALoginController::ALoginController()
{
}

void ALoginController::BeginPlay()
{
	Super::BeginPlay();

	// GameInstance�� ���� ����
	_ownerInstance = Cast<UMyGameInstance>(GetGameInstance());
	_ownerInstance->_loginController = this;

	// �α�
	UE_LOG(LogTemp, Error, TEXT("LoginController::BeginPlay"));

	// ƽ �õ� - ���� ��Ȱ��ȭ
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
}

void ALoginController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// ���� ���� ����, �α��� ���¿��� �Ѵ�.
	if (_ownerInstance->_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_CONNECTED
		|| _ownerInstance->_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_LOGIN) {

		// ��Ŷ ť���� ��Ŷ Ȯ���ϰ� ������
		if (!_ownerInstance->_packetHandler->_packetQueue.IsEmpty()) {
			//
			TFunction<void()> func;
			_ownerInstance->_packetHandler->_packetQueue.Dequeue(func);
			
			// ��Ŷ ����
			func();
		}
	}
}

void ALoginController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UE_LOG(LogTemp, Error, TEXT("LoginController::EndPlay"));
}

