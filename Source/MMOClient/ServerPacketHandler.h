// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include"proto/Protocol3.pb.h"
#include"Buffer.h"

#include "CoreMinimal.h"

/*----------------------------
	PacketHeader
----------------------------*/
class PacketHeader {
public:
	short _size;
	short _id;
};

enum {
	PacketCount = 3000,
};

/*-------------------------------------------------------------------
	HandleFuncs
--------------------------------------------------------------------*/
class AMyPlayerController;
class UMyGameInstance;

bool Handle_INVALID(UMyGameInstance* instance, uint8* buffer, int len);
bool Handle_S_LOGIN(UMyGameInstance* instance, PROTOCOL::S_LOGIN fromPkt);
bool Handle_S_ENTER_ROOM(UMyGameInstance* instance, PROTOCOL::S_ENTER_ROOM fromPkt);
bool Handle_S_LEAVE_ROOM(UMyGameInstance* instance, PROTOCOL::S_LEAVE_ROOM fromPkt);
bool Handle_S_SPAWN(UMyGameInstance* instance, PROTOCOL::S_SPAWN fromPkt);
bool Handle_S_DESPAWN(UMyGameInstance* instance, PROTOCOL::S_DESPAWN fromPkt);
bool Handle_S_ITEMLIST(UMyGameInstance* instance, PROTOCOL::S_ITEMLIST fromPkt);
bool Handle_S_MOVE(UMyGameInstance* instance, PROTOCOL::S_MOVE fromPkt);
bool Handle_S_SKILL(UMyGameInstance* instance, PROTOCOL::S_SKILL fromPkt);
bool Handle_S_CHAT(UMyGameInstance* instance, PROTOCOL::S_CHAT fromPkt);
bool Handle_S_CHANGEHP(UMyGameInstance* instance, PROTOCOL::S_CHANGE_HP fromPkt);
bool Handle_S_DIE(UMyGameInstance* instance, PROTOCOL::S_DIE fromPkt);
bool Handle_S_EQUIPITEM(UMyGameInstance* instance, PROTOCOL::S_EQUIP_ITEM fromPkt);
bool Handle_S_USEITEM(UMyGameInstance* instance, PROTOCOL::S_USE_ITEM fromPkt);
bool Handle_S_ADDITEM(UMyGameInstance* instance, PROTOCOL::S_ADD_ITEM fromPkt);
bool Handle_S_UPDATEITEM(UMyGameInstance* instance, PROTOCOL::S_UPDATE_ITEM fromPkt);
bool Handle_S_REMOVEITEM(UMyGameInstance* instance, PROTOCOL::S_REMOVE_ITEM fromPkt);
bool Handle_S_CHANGE_STAT(UMyGameInstance* instance, PROTOCOL::S_CHANGE_STAT fromPkt);
bool Handle_S_CREATE_PLAYER(UMyGameInstance* instance, PROTOCOL::S_CREATE_PLAYER fromPkt);
bool Handle_S_ADD_EXP(UMyGameInstance* instance, PROTOCOL::S_ADD_EXP fromPkt);
bool Handle_S_LEVEL_UP(UMyGameInstance* instance, PROTOCOL::S_LEVEL_UP fromPkt);
bool Handle_S_ADD_QUEST(UMyGameInstance* instance, PROTOCOL::S_ADD_QUEST fromPkt);
bool Handle_S_QUESTLIST(UMyGameInstance* instance, PROTOCOL::S_QUESTLIST fromPkt);
bool Handle_S_REMOVE_QUEST(UMyGameInstance* instance, PROTOCOL::S_REMOVE_QUEST fromPkt);
bool Handle_S_COMPLETE_QUEST(UMyGameInstance* instance, PROTOCOL::S_COMPLETE_QUEST fromPkt);
bool Handle_S_UPDATE_QUEST(UMyGameInstance* instance, PROTOCOL::S_UPDATE_QUEST fromPkt);

/*----------------------------------------------------
	ServerPacketHandler
-----------------------------------------------------*/
class MMOCLIENT_API FServerPacketHandler
{
public:
	// TEST, 빈 큐로 와야함
	bool FlushPacket(TQueue<TFunction<void()>, EQueueMode::Spsc>& queue);
	TFunction<void()> PopPacket();
	bool IsPacketQueueEmpty();

	void Init();

	/*---------------------------------------------------------------------------------------------
		HandlePacket(Recv) - Public
	---------------------------------------------------------------------------------------------*/
	bool HandlePacket(UMyGameInstance* instance, uint8* buffer, int len);
	
	/*---------------------------------------------------------------------------------------------
		MakeSendBuffer(Send) - Public
	---------------------------------------------------------------------------------------------*/
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_LOGIN toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_ENTER_ROOM toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_LEAVE_ROOM toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_SPAWN toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_MOVE toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_SKILL toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_CHAT toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_EQUIP_ITEM toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_USE_ITEM toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_CREATE_PLAYER toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_ADD_QUEST toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_REMOVE_QUEST toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_COMPLETE_QUEST toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_UPDATE_QUEST toPkt);

private:
	/*---------------------------------------------------------------------------------------------
		HandlePacket(Recv) - Private
	---------------------------------------------------------------------------------------------*/
	template<typename PacketType>
	bool HandlePacket(TFunction<bool(UMyGameInstance*, PacketType)> func, UMyGameInstance* instance, uint8* buffer, int len) {
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		// 패킷큐에 push, 다른데로 뺄 생각 해야댐
		TFunction<void()> callback = [func, instance, pkt]() { func(instance, pkt); };
		_packetQueue.Enqueue(callback);
		
		return true;

		//return func(instance, pkt);
	}

	/*---------------------------------------------------------------------------------------------
		MakeSendBuffer(Send) - Private
	---------------------------------------------------------------------------------------------*/
	template<typename Pkt>
	TSharedPtr<FSendBuffer> MakeSendBuffer(Pkt pkt, int pktId) {
		int dataSize = static_cast<int>(pkt.ByteSizeLong());
		int totalSize = dataSize + sizeof(PacketHeader);

		TSharedPtr<FSendBuffer> sendBuffer = MakeShareable(new FSendBuffer(totalSize));

		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		new(header)PacketHeader();
		header->_id = pktId;
		header->_size = totalSize;

		if (!pkt.SerializeToArray(sendBuffer->Buffer() + sizeof(PacketHeader), dataSize)) {
			UE_LOG(LogTemp, Error, TEXT("packetHandler makePacket Error"));
			return nullptr;
		}

		return sendBuffer;
	}
public:
	TFunction<bool(UMyGameInstance*, uint8*, int)> _packetHandleFuncs[PacketCount];
	TQueue<TFunction<void()>, EQueueMode::Spsc> _packetQueue;
};
