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
	short _id;
	short _size;
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
bool Handle_S_LOGIN(UMyGameInstance* instance, PROTOCOL::S_Login fromPkt);
bool Handle_S_ENTER_ROOM(UMyGameInstance* instance, PROTOCOL::S_Enter_Room fromPkt);
bool Handle_S_LEAVE_ROOM(UMyGameInstance* instance, PROTOCOL::S_Leave_Room fromPkt);
bool Handle_S_SPAWN(UMyGameInstance* instance, PROTOCOL::S_Spawn fromPkt);
bool Handle_S_DESPAWN(UMyGameInstance* instance, PROTOCOL::S_DeSpawn fromPkt);
bool Handle_S_ITEMLIST(UMyGameInstance* instance, PROTOCOL::S_ItemList fromPkt);
bool Handle_S_MOVE(UMyGameInstance* instance, PROTOCOL::S_Move fromPkt);
bool Handle_S_SKILL(UMyGameInstance* instance, PROTOCOL::S_Skill fromPkt);
bool Handle_S_CHAT(UMyGameInstance* instance, PROTOCOL::S_Chat fromPkt);
bool Handle_S_CHANGEHP(UMyGameInstance* instance, PROTOCOL::S_ChangeHp fromPkt);
bool Handle_S_DIE(UMyGameInstance* instance, PROTOCOL::S_Die fromPkt);
bool Handle_S_EQUIPITEM(UMyGameInstance* instance, PROTOCOL::S_EquipItem fromPkt);
bool Handle_S_USEITEM(UMyGameInstance* instance, PROTOCOL::S_UseItem fromPkt);
bool Handle_S_ADDITEM(UMyGameInstance* instance, PROTOCOL::S_AddItem fromPkt);
bool Handle_S_UPDATEITEM(UMyGameInstance* instance, PROTOCOL::S_UpdateItem fromPkt);
bool Handle_S_REMOVEITEM(UMyGameInstance* instance, PROTOCOL::S_RemoveItem fromPkt);
bool Handle_S_CHANGE_STAT(UMyGameInstance* instance, PROTOCOL::S_ChangeStat fromPkt);
bool Handle_S_CREATE_PLAYER(UMyGameInstance* instance, PROTOCOL::S_CreatePlayer fromPkt);
bool Handle_S_ADD_EXP(UMyGameInstance* instance, PROTOCOL::S_AddExp fromPkt);
bool Handle_S_LEVEL_UP(UMyGameInstance* instance, PROTOCOL::S_LevelUp fromPkt);
bool Handle_S_ADD_QUEST(UMyGameInstance* instance, PROTOCOL::S_AddQuest fromPkt);
bool Handle_S_QUESTLIST(UMyGameInstance* instance, PROTOCOL::S_QuestList fromPkt);
bool Handle_S_REMOVE_QUEST(UMyGameInstance* instance, PROTOCOL::S_RemoveQuest fromPkt);
bool Handle_S_COMPLETE_QUEST(UMyGameInstance* instance, PROTOCOL::S_CompleteQuest fromPkt);
bool Handle_S_UPDATE_QUEST(UMyGameInstance* instance, PROTOCOL::S_UpdateQuest fromPkt);

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
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_Login toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_Enter_Room toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_Leave_Room toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_Spawn toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_Move toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_Skill toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_Chat toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_EquipItem toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_UseItem toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_CreatePlayer toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_AddQuest toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_RemoveQuest toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_CompleteQuest toPkt);
	TSharedPtr<FSendBuffer> MakeSendBuffer(PROTOCOL::C_UpdateQuest toPkt);

private:
	/*---------------------------------------------------------------------------------------------
		HandlePacket(Recv) - Private
	---------------------------------------------------------------------------------------------*/
	template<typename PacketType>
	bool HandlePacket(TFunction<bool(UMyGameInstance*, PacketType)> func, UMyGameInstance* instance, uint8* buffer, int len) {
		PacketType pkt;
		pkt.ParseFromArray(buffer + sizeof(PacketHeader), len);

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
		int packetSize = static_cast<int>(pkt.ByteSizeLong());
		int totalSize = packetSize + sizeof(PacketHeader);

		TSharedPtr<FSendBuffer> sendBuffer = MakeShareable(new FSendBuffer(totalSize));

		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		new(header)PacketHeader();

		header->_id = pktId;
		header->_size = packetSize;

		if (!pkt.SerializeToArray(sendBuffer->Buffer() + sizeof(PacketHeader), header->_size)) {
			UE_LOG(LogTemp, Error, TEXT("packetHandler makePacket Error"));
			return nullptr;
		}

		return sendBuffer;
	}
public:
	TFunction<bool(UMyGameInstance*, uint8*, int)> _packetHandleFuncs[PacketCount];
	TQueue<TFunction<void()>, EQueueMode::Spsc> _packetQueue;
};
