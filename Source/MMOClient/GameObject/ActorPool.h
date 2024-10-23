// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include"../MyGameInstance.h"
#include"../MyPlayerController.h"

template<typename T>
class MMOCLIENT_API FActorPool
{
public:
	//
	static T* Pop(AMyPlayerController* controller, TSubclassOf<UObject> bpActor, PROTOCOL::ObjectInfo info) {
		//
		if (!controller)
			return nullptr;

		// 
		T* actor = nullptr;

		// 스폰 위치
		FVector loc(info.pos().locationx(), info.pos().locationy(), info.pos().locationz());
		FRotator rot(info.pos().rotationpitch(), info.pos().rotationyaw(), info.pos().rotationroll());
		
		// 여유분X
		if (_queue.IsEmpty() == true) {
			FActorSpawnParameters spawnParam;
			spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			actor = controller->GetWorld()->SpawnActor<T>(bpActor, loc, rot, spawnParam);
		}
		// 여유분O
		else {
			_queue.Dequeue(actor);
			actor->SetActorLocation(loc);
		}

		return actor;
	}

	static void Push(T* obj) {
		_queue.Enqueue(obj);
	}
public:
	static TQueue<T*> _queue;
};

template<typename T>
TQueue<T*> FActorPool<T>::_queue;