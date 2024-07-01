// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerPacketHandler.h"

#include"MyPlayerController.h"
#include"MyGameInstance.h"

/*---------------------------------------------------------------------------------------------
	실제로 패킷을 처리하는 부분 (전역함수)
---------------------------------------------------------------------------------------------*/
bool Handle_INVALID(UMyGameInstance* instance, uint8* buffer, int len)
{
	UE_LOG(LogTemp, Error, TEXT("Handle_INVALID"));
	return true;
}

bool Handle_S_LOGIN(UMyGameInstance* instance, PROTOCOL::S_Login fromPkt)
{
	instance->HandleLogin(fromPkt);

	return true;
}

// 본인(로그인, 종료)
bool Handle_S_ENTER_ROOM(UMyGameInstance* instance, PROTOCOL::S_Enter_Room fromPkt)
{
	if (fromPkt.success()) 
		instance->HandleEnterRoom(fromPkt.object());

	return true;
}

bool Handle_S_LEAVE_ROOM(UMyGameInstance* instance, PROTOCOL::S_Leave_Room fromPkt)
{
	// instance->_gameController->HandleDespawn();
	
	return true;
}

bool Handle_S_SPAWN(UMyGameInstance* instance, PROTOCOL::S_Spawn fromPkt)
{
	instance->_gameController->HandleSpawn(fromPkt);

	return true;
}

bool Handle_S_DESPAWN(UMyGameInstance* instance, PROTOCOL::S_DeSpawn fromPkt)
{
	instance->_gameController->HandleDespawn(fromPkt);
	
	return true;
}

bool Handle_S_ITEMLIST(UMyGameInstance* instance, PROTOCOL::S_ItemList fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("S_ItemList : %d"), fromPkt.items_size());
	
	for (int i = 0; i < fromPkt.items_size(); i++) {
		instance->AddItem(fromPkt.items(i));
	}

	return true;
}

bool Handle_S_MOVE(UMyGameInstance* instance, PROTOCOL::S_Move fromPkt)
{
	instance->_gameController->MoveUpdate(fromPkt.object());
	
	return true;
}

bool Handle_S_SKILL(UMyGameInstance* instance, PROTOCOL::S_Skill fromPkt)
{
	instance->_gameController->HandleSkill(fromPkt);

	return false;
}

bool Handle_S_CHAT(UMyGameInstance* instance, PROTOCOL::S_Chat fromPkt)
{
	instance->_gameController->HandleChat(fromPkt);

	return true;
}

bool Handle_S_CHANGEHP(UMyGameInstance* instance, PROTOCOL::S_ChangeHp fromPkt)
{
	instance->_gameController->HandleChangeHp(fromPkt);

	return false;
}

bool Handle_S_DIE(UMyGameInstance* instance, PROTOCOL::S_Die fromPkt)
{
	instance->_gameController->HandleDie(fromPkt);

	return true;
}

bool Handle_S_USEITEM(UMyGameInstance* instance, PROTOCOL::S_UseItem fromPkt)
{
	instance->HandleUseItem(fromPkt);

	return true;
}

bool Handle_S_ADDITEM(UMyGameInstance* instance, PROTOCOL::S_AddItem fromPkt)
{
	for (int i = 0; i < fromPkt.items_size(); i++)
		instance->AddItem(fromPkt.items(i));

	return true;
}

// 장비 변경, 레벨업, 
bool Handle_S_CHANGE_STAT(UMyGameInstance* instance, PROTOCOL::S_ChangeStat fromPkt)
{
	instance->HandleChangeStat(fromPkt.object());

	return true;
}

bool Handle_S_CREATE_PLAYER(UMyGameInstance* instance, PROTOCOL::S_CreatePlayer fromPkt)
{
	instance->HandleCreatePlayer(fromPkt.object());
	
	return false;
}

bool Handle_S_ADD_EXP(UMyGameInstance* instance, PROTOCOL::S_AddExp fromPkt)
{
	instance->HandleAddExp(fromPkt.exp());

	return true;
}

bool Handle_S_LEVEL_UP(UMyGameInstance* instance, PROTOCOL::S_LevelUp fromPkt)
{
	instance->HandleChangeStat(fromPkt.info());
	
	// TODO : 레벨업 이펙트
	instance->_hudWidget->PlayLevelUpWidgetAnim();

	return true;
}



/*----------------------------------------------------
	ServerPacketHandler
-----------------------------------------------------*/
bool FServerPacketHandler::FlushPacket(TQueue<TFunction<void()>, EQueueMode::Spsc>& queue)
{
	if (_packetQueue.IsEmpty())
		return false;

	Swap(queue, _packetQueue);

	return true;
}

TFunction<void()> FServerPacketHandler::PopPacket()
{
	TFunction<void()> func = nullptr;

	_packetQueue.Dequeue(func);

	return func;
}

bool FServerPacketHandler::IsPacketQueueEmpty()
{
	return _packetQueue.IsEmpty();
}

void FServerPacketHandler::Init()
{
	for (int i = 0; i < PacketCount; i++) {
		_packetHandleFuncs[i] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
			return Handle_INVALID(instance, buffer, len);
		};
	}

	_packetHandleFuncs[PROTOCOL::MsgId::S_LOGIN] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_Login>(Handle_S_LOGIN, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_ENTER_ROOM] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_Enter_Room>(Handle_S_ENTER_ROOM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_LEAVE_ROOM] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_Leave_Room>(Handle_S_LEAVE_ROOM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_SPAWN] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_Spawn>(Handle_S_SPAWN, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_DESPAWN] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_DeSpawn>(Handle_S_DESPAWN, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_ITEM_LIST] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_ItemList>(Handle_S_ITEMLIST, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_MOVE] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_Move>(Handle_S_MOVE, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_SKILL] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_Skill>(Handle_S_SKILL, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_CHAT] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_Chat>(Handle_S_CHAT, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_CHANGE_HP] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_ChangeHp>(Handle_S_CHANGEHP, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_DIE] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_Die>(Handle_S_DIE, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_USEITEM] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_UseItem>(Handle_S_USEITEM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_ADD_ITEM] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_AddItem>(Handle_S_ADDITEM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_CHANGE_STAT] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_ChangeStat>(Handle_S_CHANGE_STAT, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_CREATE_PLAYER] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_CreatePlayer>(Handle_S_CREATE_PLAYER, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_ADD_EXP] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_AddExp>(Handle_S_ADD_EXP, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_LEVEL_UP] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_LevelUp>(Handle_S_LEVEL_UP, instance, buffer, len);
	};
}

/*---------------------------------------------------------------------------------------------
	HandlePacket - public
---------------------------------------------------------------------------------------------*/
bool FServerPacketHandler::HandlePacket(UMyGameInstance* instance, uint8* buffer, int len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	
	return _packetHandleFuncs[header->_id](instance, buffer, len);
}

/*---------------------------------------------------------------------------------------------
	 MakeSendBuffer - public
---------------------------------------------------------------------------------------------*/
TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_Login toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_LOGIN);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_Enter_Room toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_ENTER_ROOM);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_Leave_Room toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_LEAVE_ROOM);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_Spawn toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_SPAWN);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_Move toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_MOVE);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_Skill toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_SKILL);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_Chat toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_CHAT);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_UseItem toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_USEITEM);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_CreatePlayer toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_CREATE_PLAYER);
}
