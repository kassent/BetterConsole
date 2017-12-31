#include "GameData.h"
#include "F4SE_common/F4SE_version.h"
#include "F4SE_common/Relocation.h"
#include "F4SE_common/SafeWrite.h"

//RelocAddr<ActorValueInfoHolder*>	g_actorValues(0x585B270);

uintptr_t HasPerkVisitor::Accept(Entry* pEntry) //bool, don't use bool,VS2015 compiler seems has a bug.
{
	if (pEntry != nullptr && pEntry->perk != nullptr)
	{
		result.push_back(Entry{ pEntry->perk, pEntry->rank });
	}
	return 1;
}

