// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include<Runtime/Online/HTTP/Public/Http.h>
#include"Data/GameStruct.h"

class UMyGameInstance;
class MMOCLIENT_API FWebManager
{
public:
	// 
	void Init(UMyGameInstance* instance, FString url);
	FHttpRequestRef MakeRequest();

	// ฟไรป
	void RequestPost(FLoginAccountReq reqModel, TFunction<void(FLoginAccountRes)> callback);

	
private:
	inline FString SerializeJsonObject(TSharedRef<FJsonObject>& jsonObject);
	inline TSharedPtr<FJsonObject> DeserializeJsonObject(FString jsonString);

public:
	UMyGameInstance* _ownerInstance;
	FString _url;
};
