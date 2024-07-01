// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


template<typename T>
class MMOCLIENT_API FObjectPool
{
public:
	template<typename... Args>
	static T* Pop(Args&&... args) {
		//
		T* obj = nullptr;

		// ¿©À¯ºÐX
		if (_queue.IsEmpty() == true) {
			obj = reinterpret_cast<T*>(malloc(sizeof(T)));
			new(obj)T(std::forward<Args>(args)...);
		}

		// ¿©À¯ºÐO
		else {
			_queue.Dequeue(obj);
			if(obj != nullptr)
				new(obj)T(std::forward<Args>(args)...);
		}

		return obj;
	}

	static void Push(T* obj) {
		// ¼Ò¸ê
		obj->~T();

		// ¹Ý³³
		_queue.Enqueue(obj);
	}
public:
	static TQueue<T*> _queue;
};

template<typename T>
TQueue<T*> FObjectPool<T>::_queue;
