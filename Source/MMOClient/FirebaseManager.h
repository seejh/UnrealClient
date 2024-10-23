// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include<firebase/app.h>
#include<firebase/auth.h>

#include "CoreMinimal.h"
#include "FirebaseManager.generated.h"


/*
	future가 옳다고 가정하고 작성
	다른 방안 : FTimerHandle
*/
class UMyGameInstance;
UCLASS()
class MMOCLIENT_API UFirebaseManager : public UObject
{
	GENERATED_BODY()
public:
	bool Init(UMyGameInstance* instance);

	// 요청
	void Register(FString email, FString password);
	void Login(FString email, FString password);
	void LoginToken();
	//void SendTokenToMyWeb(FGoogleLoginResponsePacket resPkt);
	//void Delete();

	
public:
	firebase::App* _app;

	FTimerHandle _timerHandle;
	int32 _handleCount;
	UMyGameInstance* _ownerInstance;
};