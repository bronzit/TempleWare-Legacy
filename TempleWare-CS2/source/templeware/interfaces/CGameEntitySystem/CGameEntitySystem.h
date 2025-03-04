#pragma once

#include <cstdint>
#include "../../../cs2/entity/handle.h"
#include "../../../templeware/utils/memory/memorycommon.h"
#include "../../../templeware/utils/math/vector/vector.h"
#include "..\..\..\..\source\templeware\utils\schema\schema.h"
#include "..\..\..\..\source\templeware\utils\memory\vfunc\vfunc.h"

#include "..\..\..\cs2\entity\C_BaseEntity\C_BaseEntity.h"
#include "..\..\..\cs2\entity\C_CSPlayerPawn\C_CSPlayerPawn.h" 
#include "..\..\..\cs2\entity\CCSPlayerController\CCSPlayerController.h"

class CGameEntitySystem
{
public:
	template <typename T = C_BaseEntity>
	T* Get(int nIndex)
	{
		return reinterpret_cast<T*>(this->GetEntityByIndex(nIndex));
	}

	/// GetClientEntityFromHandle
	template <typename T = C_BaseEntity>
	T* Get(const CBaseHandle hHandle)
	{
		if (!hHandle.valid())
			return nullptr;

		return reinterpret_cast<T*>(this->GetEntityByIndex(hHandle.index()));
	}

	int GetHighestEntityIndex()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + 0x20F0);
	}


	C_CSPlayerPawn* get_entity(int index)
	{
		__int64 v2; // rcx
		__int64 v3; // r8
		__int64 result{}; // rax

		if ((unsigned int)index <= 0x7FFE
			&& (unsigned int)(index >> 9) <= 0x3F
			&& (v2 = *(std::uintptr_t*)(std::uintptr_t(this) + 8 * (index >> 9) + 16)) != 0
			&& (v3 = 120 * (index & 0x1FF), v3 + v2)
			&& (*(std::uintptr_t*)(v3 + v2 + 16) & 0x7FFF) == index)
		{
			result = *(std::uintptr_t*)(v3 + v2);
		}
		return reinterpret_cast<C_CSPlayerPawn*>(result);
	}


private:
	void* GetEntityByIndex(int nIndex);

};

class IGameResourceService
{
public:
	MEM_PAD(0x58);
	CGameEntitySystem* Instance;
};