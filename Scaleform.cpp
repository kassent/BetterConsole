#include "Scaleform.h"
#include "GameData.h"
#include "F4SE/ScaleformMovie.h"
#include "F4SE/ScaleformValue.h"
#include "F4SE/ScaleformState.h"
#include "F4SE/ScaleformLoader.h"
#include "F4SE/ScaleformTranslator.h"
#include "F4SE/GameCamera.h"

#include <memory>
#include <string>

#include "Settings.h"

std::vector<std::string> FormTypes =
{
	"NONE",
	"TES4",
	"GRUP",
	"GMST",
	"KYWD",
	"LCRT",
	"AACT",
	"TRNS",
	"CMPO",
	"TXST",
	"MICN",
	"GLOB",
	"DMGT",
	"CLAS",
	"FACT",
	"HDPT",
	"EYES",
	"RACE",
	"SOUN",
	"ASPC",
	"SKIL",
	"MGEF",
	"SCPT",
	"LTEX",
	"ENCH",
	"SPEL",
	"SCRL",
	"ACTI",
	"TACT",
	"ARMO",
	"BOOK",
	"CONT",
	"DOOR",
	"INGR",
	"LIGH",
	"MISC",
	"STAT",
	"SCOL",
	"MSTT",
	"GRAS",
	"TREE",
	"FLOR",
	"FURN",
	"WEAP",
	"AMMO",
	"NPC_",
	"LVLN",
	"KEYM",
	"ALCH",
	"IDLM",
	"NOTE",
	"PROJ",
	"HAZD",
	"BNDS",
	"SLGM",
	"TERM",
	"LVLI",
	"WTHR",
	"CLMT",
	"SPGD",
	"RFCT",
	"REGN",
	"NAVI",
	"CELL",
	"REFR",
	"ACHR",
	"PMIS",
	"PARW",
	"PGRE",
	"PBEA",
	"PFLA",
	"PCON",
	"PBAR",
	"PHZD",
	"WRLD",
	"LAND",
	"NAVM",
	"TLOD",
	"DIAL",
	"INFO",
	"QUST",
	"IDLE",
	"PACK",
	"CSTY",
	"LSCR",
	"LVSP",
	"ANIO",
	"WATR",
	"EFSH",
	"TOFT",
	"EXPL",
	"DEBR",
	"IMGS",
	"IMAD",
	"FLST",
	"PERK",
	"BPTD",
	"ADDN",
	"AVIF",
	"CAMS",
	"CPTH",
	"VTYP",
	"MATT",
	"IPCT",
	"IPDS",
	"ARMA",
	"ECZN",
	"LCTN",
	"MESG",
	"RGDL",
	"DOBJ",
	"DFOB",
	"LGTM",
	"MUSC",
	"FSTP",
	"FSTS",
	"SMBN",
	"SMQN",
	"SMEN",
	"DLBR",
	"MUST",
	"DLVW",
	"WOOP",
	"SHOU",
	"EQUP",
	"RELA",
	"SCEN",
	"ASTP",
	"OTFT",
	"ARTO",
	"MATO",
	"MOVT",
	"SNDR",
	"DUAL",
	"SNCT",
	"SOPM",
	"COLL",
	"CLFM",
	"REVB",
	"PKIN",
	"RFGP",
	"AMDL",
	"LAYR",
	"COBJ",
	"OMOD",
	"MSWP",
	"ZOOM",
	"INNR",
	"KSSM",
	"AECH",
	"SCCO",
	"AORU",
	"SCSN",
	"STAG",
	"NOCM",
	"LENS",
	"LSPR",
	"GDRY",
	"OVIS"
};

std::vector<std::string> ActorValueTypes = 
{
	"DERIVED ATTRIBUTE",
	"SPECIAL",
	"SKILL",
	"AI ATTRIBUTE",
	"RESISTANCE",
	"CONDITION",
	"UNKNOWN",
	"INT VALUE",
	"VARIABLE",
	"RESOURCE",
	"UNKNOWN"
};

std::vector<std::string> WeaponTypes =
{
	"UNARMED",
	"ONE-HANDED SWORD",
	"ONE-HANDED DAGGER",
	"ONE-HANDED AXE",
	"ONE-HANDED MACE",
	"TWO-HANDED SWORD",
	"TWO-HANDED AXE",
	"BOW",
	"STAFF",
	"GUN",
	"GRENADE",
	"MINE",
	"UNDEFINED"
};


template<typename T>
inline void Register(GFxValue * dst, const char * name, T value)
{

}

template<>
inline void Register(GFxValue * dst, const char * name, SInt32 value)
{
	GFxValue	fxValue;
	fxValue.SetInt(value);
	dst->SetMember(name, &fxValue);
}

template<>
inline void Register(GFxValue * dst, const char * name, UInt32 value)
{
	GFxValue	fxValue;
	fxValue.SetUInt(value);
	dst->SetMember(name, &fxValue);
}

template<>
inline void Register(GFxValue * dst, const char * name, double value)
{
	GFxValue	fxValue;
	fxValue.SetNumber(value);
	dst->SetMember(name, &fxValue);
}

template<>
inline void Register(GFxValue * dst, const char * name, bool value)
{
	GFxValue	fxValue;
	fxValue.SetBool(value);
	dst->SetMember(name, &fxValue);
}

inline void RegisterUnmanagedString(GFxValue * dst, const char * name, const char * str)
{
	GFxValue	fxValue;
	fxValue.SetString(str);
	dst->SetMember(name, &fxValue);
}

inline void RegisterString(GFxValue * dst, GFxMovieRoot * view, const char * name, const char * str)
{
	GFxValue	fxValue;
	view->CreateString(&fxValue, str);
	dst->SetMember(name, &fxValue);
}


void BetterConsole_GetBaseData::Invoke(Args * args)
{
	TESObjectREFR* pRef = nullptr;
	LookupREFRByHandle(g_consoleHandle, &pRef);
	if (pRef != nullptr)
	{
		args->movie->movieRoot->CreateObject(args->result);
		std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);

		TESForm* pBaseForm = pRef->baseForm;

		//RelocAddr<BSExtraData*(*)(ExtraDataList*, UInt8 type)> GetExtraData = 0x436A0; // 5B    V1.10.26
		BSExtraData* pExtraData = pRef->extraDataList->GetExtraData(kExtraData_LeveledCreature);
		if (pExtraData != nullptr)
		{
			auto leveledBaseForm = *reinterpret_cast<TESForm**>((uintptr_t)pExtraData + 0x18);
			pBaseForm = (leveledBaseForm != nullptr) ? leveledBaseForm : pRef->baseForm;
		}

		sprintf_s(sResult.get(), MAX_PATH, "%08X", pRef->formID);

		RegisterString(args->result, args->movie->movieRoot, "refFormID", sResult.get());
		RegisterString(args->result, args->movie->movieRoot, "refFormType", FormTypes[pRef->formType].c_str());

		sprintf_s(sResult.get(), MAX_PATH, "%08X", pBaseForm->formID);

		RegisterString(args->result, args->movie->movieRoot, "baseFormID", sResult.get());
		RegisterString(args->result, args->movie->movieRoot, "baseFormType", FormTypes[pBaseForm->formType].c_str());

		const char* pRefName = CALL_MEMBER_FN(pRef, GetReferenceName)();
		auto pExtraTextData = static_cast<ExtraTextDisplayData*>(pRef->extraDataList->GetExtraData(kExtraData_TextDisplayData));
		if (pExtraTextData != nullptr)
		{
			pRefName = pExtraTextData->name.c_str();
		}
		if (pRefName != nullptr && pRefName[0])
		{
			RegisterString(args->result, args->movie->movieRoot, "refName", pRefName);
		}

		TESFullName* pFullName = DYNAMIC_CAST(pBaseForm, TESForm, TESFullName);
		if (pFullName != nullptr && strlen(pFullName->name.c_str()))
		{
			RegisterString(args->result, args->movie->movieRoot, "baseName", pFullName->GetFullName());
		}

		auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(pBaseForm);
		const char* baseDefineModName = (pModInfo != nullptr) ? pModInfo->name : "undefined";
		RegisterString(args->result, args->movie->movieRoot, "baseDefineModName", baseDefineModName);

		pModInfo = (*g_dataHandler)->GetLocatedModInfo(pRef);
		const char* refDefineModName = (pModInfo != nullptr) ? pModInfo->name : "undefined";
		RegisterString(args->result, args->movie->movieRoot, "refDefineModName", refDefineModName);

		sprintf_s(sResult.get(), MAX_PATH, "%016I64X", (uintptr_t)pRef);
		RegisterString(args->result, args->movie->movieRoot, "refMemory", sResult.get());

		pModInfo = (*g_dataHandler)->GetLastChangeModInfo(pBaseForm);

		RegisterString(args->result, args->movie->movieRoot, "baseLastChangeModName", (pModInfo != nullptr) ? pModInfo->name : "undefined");

		pRef->handleRefObject.DecRefHandle();
	}
}

RelocAddr<void(*)(Actor*, HasPerkVisitor&)> DumpPerks = 0xDA5BE0; //48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8D 99 ? ? ? ? 48 8B F9 48 8B F2 48 8B CB E8 ? ? ? ? 48 8B 8F ? ? ? ?

void BetterConsole_GetPerkData::Invoke(Args * args)
{
	TESObjectREFR* pRef = nullptr;
	LookupREFRByHandle(g_consoleHandle, &pRef);
	if (pRef != nullptr && pRef->formType == kFormType_ACHR)
	{
		args->movie->movieRoot->CreateArray(args->result);

		Actor* actor = static_cast<Actor*>(pRef);
		
		if (actor->middleProcess != nullptr)
		{
			HasPerkVisitor visitor;
			DumpPerks(actor, visitor);
			for (auto& pEntry : visitor.result)
			{
				GFxValue object;
				args->movie->movieRoot->CreateObject(&object);
				TESFullName* pFullName = DYNAMIC_CAST(pEntry.perk, BGSPerk, TESFullName);
				if (pFullName != nullptr && strlen(pFullName->name.c_str()))
				{
					RegisterString(&object, args->movie->movieRoot, "text", pFullName->name.c_str());
				}
				else
				{
					RegisterString(&object, args->movie->movieRoot, "text", Settings::EXTRAINFO_NULL.c_str());
				}
				Register<UInt32>(&object, "filterFlag", 2);
				args->result->PushBack(&object);
			}
		}
	}
	if (pRef != nullptr)
	{
		pRef->handleRefObject.DecRefHandle();
	}
}


void BetterConsole_WriteLog::Invoke(Args * args)
{
	ASSERT(args->numArgs >= 1);
	ASSERT(args->args[0].GetType() == GFxValue::kType_String);
#ifdef _DEBUG
	_MESSAGE(args->args[0].GetString());
#endif
}


void	CollectPositionData		(GFxValue& object, GFxMovieRoot * root, Actor *  actor)
{
	if (object.GetType() == GFxValue::kType_Object && actor->formType == kFormType_ACHR && root != nullptr)
	{
		GFxValue value;
		root->CreateArray(&value); //extraData
		std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);
		
		GFxValue posX;
		root->CreateObject(&posX);
		sprintf_s(sResult.get(), MAX_PATH, "%s: ", Settings::POSITIONINFO_POSX.c_str());
		RegisterString(&posX, root, "key", sResult.get());
		sprintf_s(sResult.get(), MAX_PATH, "%.2f", actor->pos.x);
		RegisterString(&posX, root, "value", sResult.get());
		Register<UInt32>(&posX, "filterFlag", 2);
		value.PushBack(&posX);

		GFxValue posY;
		root->CreateObject(&posY);
		sprintf_s(sResult.get(), MAX_PATH, "%s: ", Settings::POSITIONINFO_POSY.c_str());
		RegisterString(&posY, root, "key", sResult.get());
		sprintf_s(sResult.get(), MAX_PATH, "%.2f", actor->pos.y);
		RegisterString(&posY, root, "value", sResult.get());
		Register<UInt32>(&posY, "filterFlag", 2);
		value.PushBack(&posY);

		GFxValue posZ;
		root->CreateObject(&posZ);
		sprintf_s(sResult.get(), MAX_PATH, "%s: ", Settings::POSITIONINFO_POSZ.c_str());
		RegisterString(&posZ, root, "key", sResult.get());
		sprintf_s(sResult.get(), MAX_PATH, "%.2f", actor->pos.z);
		RegisterString(&posZ, root, "value", sResult.get());
		Register<UInt32>(&posZ, "filterFlag", 2);
		value.PushBack(&posZ);

		GFxValue rotX;
		root->CreateObject(&rotX);
		sprintf_s(sResult.get(), MAX_PATH, "%s: ", Settings::POSITIONINFO_ROTX.c_str());
		RegisterString(&rotX, root, "key", sResult.get());
		sprintf_s(sResult.get(), MAX_PATH, "%.2f", actor->rot.x);
		RegisterString(&rotX, root, "value", sResult.get());
		Register<UInt32>(&rotX, "filterFlag", 2);
		value.PushBack(&rotX);

		GFxValue rotY;
		root->CreateObject(&rotY);
		sprintf_s(sResult.get(), MAX_PATH, "%s: ", Settings::POSITIONINFO_ROTY.c_str());
		RegisterString(&rotY, root, "key", sResult.get());
		sprintf_s(sResult.get(), MAX_PATH, "%.2f", actor->rot.y);
		RegisterString(&rotY, root, "value", sResult.get());
		Register<UInt32>(&rotY, "filterFlag", 2);
		value.PushBack(&rotY);

		GFxValue rotZ;
		root->CreateObject(&rotZ);
		sprintf_s(sResult.get(), MAX_PATH, "%s: ", Settings::POSITIONINFO_ROTZ.c_str());
		RegisterString(&rotZ, root, "key", sResult.get());
		sprintf_s(sResult.get(), MAX_PATH, "%.2f", actor->rot.z);
		RegisterString(&rotZ, root, "value", sResult.get());
		Register<UInt32>(&rotZ, "filterFlag", 2);
		value.PushBack(&rotZ);

		object.SetMember("extraInfo", &value);
	}
}

void	CollectPerkData			(GFxValue& object, GFxMovieRoot * root, Actor *  actor, UInt32&	count)
{
	if (object.GetType() == GFxValue::kType_Object && actor->formType == kFormType_ACHR && root != nullptr)
	{
		//RelocAddr<void(*)(Actor*, HasPerkVisitor&)> DumpPerks = 0xDA4140; ///V1.10
		if (actor->middleProcess != nullptr)
		{
			HasPerkVisitor visitor;
			DumpPerks(actor, visitor);
			count = visitor.result.size();
			if (!count)	return;
			GFxValue value;
			root->CreateArray(&value); //extraData
			std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);
			for (auto& pEntry : visitor.result)
			{
				GFxValue perkData;
				root->CreateObject(&perkData);
				TESFullName* pFullName = DYNAMIC_CAST(pEntry.perk, BGSPerk, TESFullName);
				RegisterString(&perkData, root, "text", (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->name.c_str() : Settings::EXTRAINFO_NULL.c_str());
				Register<UInt32>(&perkData, "filterFlag", 2);

				GFxValue extraInfo;
				root->CreateArray(&extraInfo);

				GFxValue perkName;
				root->CreateObject(&perkName);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->name.c_str() : Settings::EXTRAINFO_NULL.c_str());
				RegisterString(&perkName, root, "text", sResult.get());
				Register<UInt32>(&perkName, "filterFlag", 2);
				extraInfo.PushBack(&perkName);

				GFxValue perkID;
				root->CreateObject(&perkID);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EXTRAINFO_ID.c_str(), pEntry.perk->formID);
				RegisterString(&perkID, root, "text", sResult.get());
				Register<UInt32>(&perkID, "filterFlag", 2);
				extraInfo.PushBack(&perkID);

				GFxValue perkLevel;
				root->CreateObject(&perkLevel);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %d", Settings::PERKINFO_LEVEL.c_str(), pEntry.perk->perkLevel);
				RegisterString(&perkLevel, root, "text", sResult.get());
				Register<UInt32>(&perkLevel, "filterFlag", 2);
				extraInfo.PushBack(&perkLevel);

				GFxValue perkRank;
				root->CreateObject(&perkRank);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %d", Settings::PERKINFO_RANK.c_str(), pEntry.rank);
				RegisterString(&perkRank, root, "text", sResult.get());
				Register<UInt32>(&perkRank, "filterFlag", 2);
				extraInfo.PushBack(&perkRank);

				GFxValue perkMod;
				root->CreateObject(&perkMod);
				auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(pEntry.perk);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_MOD.c_str(), (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL.c_str());
				RegisterString(&perkMod, root, "text", sResult.get());
				Register<UInt32>(&perkMod, "filterFlag", 2);
				extraInfo.PushBack(&perkMod);

				perkData.SetMember("extraInfo", &extraInfo);
				value.PushBack(&perkData);
			}
			object.SetMember("extraInfo", &value);
		}
	}
}

void	CollectFactionData		(GFxValue& object, GFxMovieRoot * root, Actor *  actor, UInt32&	count)
{
	if (object.GetType() == GFxValue::kType_Object && actor->formType == kFormType_ACHR && root != nullptr)
	{
		std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);
		std::map<TESFaction*, SInt8>	collector;
		BSExtraData* pExtraData = actor->extraDataList->GetExtraData(kExtraData_FactionChanges);
		if (pExtraData != nullptr)
		{
			auto pFactionChanges = static_cast<ExtraFactionChanges*>(pExtraData);
			for (uintptr_t i = 0; i < pFactionChanges->factions.count; ++i)
			{
				const auto factionInfo = pFactionChanges->factions[i];
				collector.insert({ factionInfo.faction, factionInfo.rank });
			}
		}
		TESNPC* npc = static_cast<TESNPC*>(actor->baseForm);
		for (uintptr_t i = 0; i < npc->actorData.factions.count; ++i)
		{
			const auto factionInfo = npc->actorData.factions[i];
			//SInt8 rank = (*RelocPtr<Actor*>(0x59628C0) != actor || ((*(UInt32*)(((uintptr_t)factionInfo.faction + 0x58))) & (1 << 2))) ? factionInfo.rank : -2;
			collector.insert({ factionInfo.faction, factionInfo.rank });
		}

		count = collector.size();
		GFxValue value;
		root->CreateArray(&value); //extraData

		for (const auto& info : collector)
		{
			GFxValue factionData;
			root->CreateObject(&factionData);
			TESFullName* pFullName = DYNAMIC_CAST(info.first, TESFaction, TESFullName);
			RegisterString(&factionData, root, "text", (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->name.c_str() : Settings::EXTRAINFO_NULL.c_str());
			Register<UInt32>(&factionData, "filterFlag", 2);

			GFxValue extraInfo;
			root->CreateArray(&extraInfo); //extraData

			GFxValue factionID;
			root->CreateObject(&factionID);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EXTRAINFO_ID.c_str(), info.first->formID);
			RegisterString(&factionID, root, "text", sResult.get());
			Register<UInt32>(&factionID, "filterFlag", 2);
			extraInfo.PushBack(&factionID);

			GFxValue factionName;
			root->CreateObject(&factionName);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->name.c_str() : Settings::EXTRAINFO_NULL.c_str());
			RegisterString(&factionName, root, "text", sResult.get());
			Register<UInt32>(&factionName, "filterFlag", 2);
			extraInfo.PushBack(&factionName);

			GFxValue factionRank;
			root->CreateObject(&factionRank);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %d", Settings::FACTIONINFO_RANK.c_str(), info.second);
			RegisterString(&factionRank, root, "text", sResult.get());
			Register<UInt32>(&factionRank, "filterFlag", 2);
			extraInfo.PushBack(&factionRank);

			if (info.first->vendorValues.merchantContainer)
			{
				//Vender	
				GFxValue factionVendor;
				root->CreateObject(&factionVendor);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::FACTIONINFO_MERCHANTFACTION.c_str(), (info.first->factionFlags & TESFaction::kFlag_Vender) ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
				RegisterString(&factionVendor, root, "text", sResult.get());
				Register<UInt32>(&factionVendor, "filterFlag", 2);
				extraInfo.PushBack(&factionVendor);

				GFxValue factionVendorContainerID;
				root->CreateObject(&factionVendorContainerID);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::FACTIONINFO_MERCHANTCONTAINERID.c_str(), info.first->vendorValues.merchantContainer->formID);
				RegisterString(&factionVendorContainerID, root, "text", sResult.get());
				Register<UInt32>(&factionVendorContainerID, "filterFlag", 2);
				extraInfo.PushBack(&factionVendorContainerID);
			}

			GFxValue factionMod;
			root->CreateObject(&factionMod);
			auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(info.first);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_MOD.c_str(), (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL.c_str());
			RegisterString(&factionMod, root, "text", sResult.get());
			Register<UInt32>(&factionMod, "filterFlag", 2);
			extraInfo.PushBack(&factionMod);

			factionData.SetMember("extraInfo", &extraInfo);
			value.PushBack(&factionData);
		}

		object.SetMember("extraInfo", &value);
	}
}

void	CollectEffectData		(GFxValue& object, GFxMovieRoot * root, Actor *  actor, UInt32&	count)
{
	if (object.GetType() == GFxValue::kType_Object && actor->formType == kFormType_ACHR && root != nullptr)
	{
		auto pMaigicTarget = &actor->magicTarget;
		auto fnGetEffectList = (BSSimpleList<ActiveEffect*> * (**)(MagicTarget*))(*(uintptr_t*)pMaigicTarget + 0x38);
		BSSimpleList<ActiveEffect*>	* pEffectList = (*fnGetEffectList)(pMaigicTarget);

		if (pEffectList != nullptr && pEffectList->count)
		{
			++pEffectList->refCount;
			GFxValue value;
			root->CreateArray(&value); //extraData
			std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);
			for (size_t i = 0; i < pEffectList->count; ++i)
			{
				auto pEffect = pEffectList->items[i];
				if (pEffect != nullptr)
				{
					auto pEffectSetting = pEffect->effect->mgef;
					if (pEffectSetting != nullptr)
					{
						++count;
						GFxValue effectData;
						root->CreateObject(&effectData);
						TESFullName* pFullName = DYNAMIC_CAST(pEffectSetting, EffectSetting, TESFullName);
						RegisterString(&effectData, root, "text", (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->name.c_str() : Settings::EXTRAINFO_NULL.c_str());
						if (!(pEffect->flags & ActiveEffect::kFlag_Inactive))
						{
							Register<UInt32>(&effectData, "filterFlag", 2);
						}
						else
						{
							Register<UInt32>(&effectData, "filterFlag", 1);
						}

						GFxValue extraInfo;
						root->CreateArray(&extraInfo); //extraData

						GFxValue effectName;
						root->CreateObject(&effectName);
						sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->name.c_str() : Settings::EXTRAINFO_NULL.c_str());
						RegisterString(&effectName, root, "text", sResult.get());
						Register<UInt32>(&effectName, "filterFlag", 2);
						extraInfo.PushBack(&effectName);

						GFxValue effectID;
						root->CreateObject(&effectID);
						sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EXTRAINFO_ID.c_str(), pEffectSetting->formID);
						RegisterString(&effectID, root, "text", sResult.get());
						Register<UInt32>(&effectID, "filterFlag", 2);
						extraInfo.PushBack(&effectID);

						GFxValue effectMod;
						root->CreateObject(&effectMod);
						auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(pEffectSetting);
						sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_MOD.c_str(), (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL.c_str());
						RegisterString(&effectMod, root, "text", sResult.get());
						Register<UInt32>(&effectMod, "filterFlag", 2);
						extraInfo.PushBack(&effectMod);

						GFxValue elapsed;
						root->CreateObject(&elapsed);
						sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::EFFECTINFO_ELAPSED.c_str(), pEffect->elapsed);
						RegisterString(&elapsed, root, "text", sResult.get());
						Register<UInt32>(&elapsed, "filterFlag", 2);
						extraInfo.PushBack(&elapsed);

						GFxValue duration;
						root->CreateObject(&duration);
						sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::EFFECTINFO_DURATION.c_str(), pEffect->duration);
						RegisterString(&duration, root, "text", sResult.get());
						Register<UInt32>(&duration, "filterFlag", 2);
						extraInfo.PushBack(&duration);

						GFxValue magnitude;
						root->CreateObject(&magnitude);
						sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::EFFECTINFO_MAGNITUDE.c_str(), pEffect->magnitude);
						RegisterString(&magnitude, root, "text", sResult.get());
						Register<UInt32>(&magnitude, "filterFlag", 2);
						extraInfo.PushBack(&magnitude);

						GFxValue inactive;
						root->CreateObject(&inactive);
						sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EFFECTINFO_INACTIVE.c_str(), (pEffect->flags & ActiveEffect::kFlag_Inactive) ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
						RegisterString(&inactive, root, "text", sResult.get());
						Register<UInt32>(&inactive, "filterFlag", 2);
						extraInfo.PushBack(&inactive);

						GFxValue detrimental;
						root->CreateObject(&detrimental);
						sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EFFECTINFO_DETRIMENTAL.c_str(), (pEffectSetting->flags & EffectSetting::kEffectType_Detrimental) ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
						RegisterString(&detrimental, root, "text", sResult.get());
						Register<UInt32>(&detrimental, "filterFlag", 2);
						extraInfo.PushBack(&detrimental);

						if (pEffectSetting->actorValInfoD8)
						{
							const char * pName = pEffectSetting->actorValInfoD8->GetDisplayName();
							if (!pName || !pName[0])
							{
								pName = pEffectSetting->actorValInfoD8->avName;
							}
							if (pName && pName[0])
							{
								GFxValue modifiedActorValue;
								root->CreateObject(&modifiedActorValue);
								sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EFFECTINFO_MODIFIEDAV.c_str(), pName);
								RegisterString(&modifiedActorValue, root, "text", sResult.get());
								Register<UInt32>(&modifiedActorValue, "filterFlag", 2);
								extraInfo.PushBack(&modifiedActorValue);
							}
						}

						if (pEffect->item)
						{
							GFxValue spellName;
							root->CreateObject(&spellName);
							pFullName = DYNAMIC_CAST(pEffect->item, MagicItem, TESFullName);
							sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EFFECTINFO_SPELLNAME.c_str(), (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->name.c_str() : Settings::EXTRAINFO_NULL.c_str());
							RegisterString(&spellName, root, "text", sResult.get());
							Register<UInt32>(&spellName, "filterFlag", 2);
							extraInfo.PushBack(&spellName);

							GFxValue spellID;
							root->CreateObject(&spellID);
							sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EFFECTINFO_SPELLID.c_str(), pEffect->item->formID);
							RegisterString(&spellID, root, "text", sResult.get());
							Register<UInt32>(&spellID, "filterFlag", 2);
							extraInfo.PushBack(&spellID);

							GFxValue spellType;
							root->CreateObject(&spellType);
							sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EFFECTINFO_SPELLTYPE.c_str(), FormTypes[pEffect->item->formType].c_str());
							RegisterString(&spellType, root, "text", sResult.get());
							Register<UInt32>(&spellType, "filterFlag", 2);
							extraInfo.PushBack(&spellType);

							GFxValue spellMod;
							root->CreateObject(&spellMod);
							pModInfo = (*g_dataHandler)->GetLocatedModInfo(pEffect->item);
							sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EFFECTINFO_SPELLMOD.c_str(), (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL.c_str());
							RegisterString(&spellMod, root, "text", sResult.get());
							Register<UInt32>(&spellMod, "filterFlag", 2);
							extraInfo.PushBack(&spellMod);
						}

						effectData.SetMember("extraInfo", &extraInfo);
						value.PushBack(&effectData);
					}
				}
			}
			object.SetMember("extraInfo", &value);
			--pEffectList->refCount;
		}
	}
}

void	CollectValueData		(GFxValue& object, GFxMovieRoot * root, Actor *  actor, UInt32&	count)
{
	if (object.GetType() == GFxValue::kType_Object && actor->formType == kFormType_ACHR && root != nullptr)
	{
		std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);

		struct AVInfo
		{
			ActorValueInfo*		pAVI;
			float				base;
			float				current;
		};
		std::map<std::string, AVInfo> dataBase;

		for (uint32_t i = 0; i < actor->actorValueData.count; ++i)
		{
			auto data = actor->actorValueData[i];
			TESForm* pForm = LookupFormByID(data.avFormID);
			if (pForm != nullptr && pForm->formType == kFormType_AVIF)
			{
				auto pAVI = static_cast<ActorValueInfo*>(pForm);
				float currentValue = actor->actorValueOwner.GetValue(pAVI);
				if (data.value != 0.0f || currentValue != 0.0f)
				{
					const char * actorValueName = pAVI->GetDisplayName();
					if (!actorValueName || !actorValueName[0])
					{
						actorValueName = pAVI->avName;
					}
					dataBase.insert({ actorValueName,{ pAVI, data.value, currentValue } });
				}
			}
		}
		if (!dataBase.size())
			return;

		GFxValue value;
		root->CreateArray(&value); //extraData

		//sort by string.A-Z
		for (const auto& map : dataBase)
		{
			++count;
			auto pAVI = map.second.pAVI;

			GFxValue actorValueData;
			root->CreateObject(&actorValueData);
			sprintf_s(sResult.get(), MAX_PATH, "%s", map.first.c_str());
			RegisterString(&actorValueData, root, "text", sResult.get());
			Register<UInt32>(&actorValueData, "filterFlag", 2);

			GFxValue extraInfo;
			root->CreateArray(&extraInfo); //extraData

			GFxValue avName;
			root->CreateObject(&avName);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), map.first.c_str());
			RegisterString(&avName, root, "text", sResult.get());
			Register<UInt32>(&avName, "filterFlag", 2);
			extraInfo.PushBack(&avName);

			GFxValue avType;
			root->CreateObject(&avType);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::AVINFO_TYPE.c_str(), ActorValueTypes[pAVI->type].c_str());
			RegisterString(&avType, root, "text", sResult.get());
			Register<UInt32>(&avType, "filterFlag", 2);
			extraInfo.PushBack(&avType);

			GFxValue avID;
			root->CreateObject(&avID);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EXTRAINFO_ID.c_str(), pAVI->formID);
			RegisterString(&avID, root, "text", sResult.get());
			Register<UInt32>(&avID, "filterFlag", 2);
			extraInfo.PushBack(&avID);

			GFxValue avBase;
			root->CreateObject(&avBase);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::AVINFO_BASEVALUE.c_str(), map.second.base);
			RegisterString(&avBase, root, "text", sResult.get());
			Register<UInt32>(&avBase, "filterFlag", 2);
			extraInfo.PushBack(&avBase);

			GFxValue avCurrent;
			root->CreateObject(&avCurrent);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::AVINFO_CURRENTVALUE.c_str(), map.second.current);
			RegisterString(&avCurrent, root, "text", sResult.get());
			Register<UInt32>(&avCurrent, "filterFlag", 2);
			extraInfo.PushBack(&avCurrent);

			GFxValue avMod;
			root->CreateObject(&avMod);
			auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(pAVI);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_MOD.c_str(), (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL.c_str());
			RegisterString(&avMod, root, "text", sResult.get());
			Register<UInt32>(&avMod, "filterFlag", 2);
			extraInfo.PushBack(&avMod);

			actorValueData.SetMember("extraInfo", &extraInfo);
			value.PushBack(&actorValueData);
		}

		object.SetMember("extraInfo", &value);
	}
}

void	ColllectInventoryData	(GFxValue& arrObj, GFxMovieRoot * root, Actor * actor, TESForm * pBaseForm)
{
	auto pInvList = actor->inventoryList;
	std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);
	bool result = false;
	if (pInvList != nullptr)
	{
		pInvList->inventoryLock.Lock();
		for (size_t i = 0; i < pInvList->items.count; ++i)
		{
			BGSInventoryItem item = pInvList->items[i];
			if (item.form != pBaseForm)	continue;
			result = true;
			if (item.stack != nullptr)
			{
				item.stack->Visit([&](BGSInventoryItem::Stack * stack)->bool
				{
					if (!(stack->flags & BGSInventoryItem::Stack::kFlagEquipped))	return true;

					struct InstanceData
					{
						TESForm					* form;
						TBO_InstanceData		* data;
					};
					RelocAddr<InstanceData *(*)(InstanceData& out, TESForm *, TBO_InstanceData *)> CalcInstanceData(0x2F7A30); ///48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 89 11 49 8B F8

					ExtraDataList * pExtraDataList = stack->extraData;
					TBO_InstanceData * pExtraInstanceData = nullptr;
					const char * pName = nullptr;
					if (pExtraDataList != nullptr)
					{
						pExtraInstanceData = pExtraDataList->GetExtraInstanceData();
						auto pExtraName = static_cast<ExtraTextDisplayData*>(pExtraDataList->GetExtraData(kExtraData_TextDisplayData));
						pName = (pExtraName != nullptr) ? pExtraName->name.c_str() : nullptr;
					}
					if (!pName)
					{
						TESFullName* pFullName = DYNAMIC_CAST(pBaseForm, TESForm, TESFullName);
						pName = (pFullName != nullptr) ? pFullName->name.c_str() : nullptr;
					}

					GFxValue itemName;
					root->CreateObject(&itemName);
					sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), pName);
					RegisterString(&itemName, root, "text", sResult.get());
					Register<UInt32>(&itemName, "filterFlag", 2);
					arrObj.PushBack(&itemName);

					GFxValue itemID;
					root->CreateObject(&itemID);
					sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EXTRAINFO_ID.c_str(), pBaseForm->formID);
					RegisterString(&itemID, root, "text", sResult.get());
					Register<UInt32>(&itemID, "filterFlag", 2);
					arrObj.PushBack(&itemID);

					GFxValue itemType;
					root->CreateObject(&itemType);
					sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_TYPE.c_str(), FormTypes[pBaseForm->formType].c_str());
					RegisterString(&itemType, root, "text", sResult.get());
					Register<UInt32>(&itemType, "filterFlag", 2);
					arrObj.PushBack(&itemType);

					InstanceData	objInst{ nullptr, nullptr };
					CalcInstanceData(objInst, pBaseForm, pExtraInstanceData);

					switch (pBaseForm->formType)
					{
					case FormType::kFormType_ARMO:
					{
						TESObjectARMO* pArmor = static_cast<TESObjectARMO*>(pBaseForm);
						auto pArmorInst = (TESObjectARMO::InstanceData*)((objInst.data != nullptr) ? objInst.data : pArmor->GetInstanceData());///V1.10

						struct ResistInfo
						{
							UInt32	type;
							float	resist;
						};
						SimpleCollector<ResistInfo>		resistInfo{ 0, nullptr, 0, 0 };
						RelocAddr<void(*)(BGSInventoryItem&, BGSInventoryItem::Stack *, SimpleCollector<ResistInfo>	&, float)> CollectResistInfo = 0xC0AB30; ///V1.10.26 //48 8B C4 F3 0F 11 58 ? 48 89 48 08 55 41 54
						CollectResistInfo(item, stack, resistInfo, 1.0f);
						for (size_t i = 0; i < resistInfo.count; ++i)
						{
							if (resistInfo.items[i].resist != 0.0f)
							{
								GFxValue itemResist;
								root->CreateObject(&itemResist);
								sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::RESISTTYPES[resistInfo.items[i].type].c_str(), resistInfo.items[i].resist);
								RegisterString(&itemResist, root, "text", sResult.get());
								Register<UInt32>(&itemResist, "filterFlag", 2);
								arrObj.PushBack(&itemResist);
							}
						}
						if (resistInfo.items != nullptr)
						{
							RelocAddr<void(*)(SimpleCollector<ResistInfo>&)>	Release = 0x1B10F10; ///V1.10.26 ////40 53 48 83 EC 20 48 8B 51 08 48 8B D9 48 85 D2 74 1A
							Release(resistInfo);
						}
						RelocAddr<BGSKeyword*(*)()> GeHealthtKW = 0x1E2A10; ///V1.10.26 //E8 ? ? ? ? 4C 8B C3 48 8B D0 48 8B CF FF 56 08 48 8B 74 24 ?
						BGSKeyword * pKW = GeHealthtKW(); 
						if (pArmor->keywordForm.keywordBase.HasKeyword(pKW))
						{
							UInt32 fullHealth = pArmor->GetFullHealth(pArmorInst);///V1.10.26
							UInt32 currentHealth = (pExtraDataList != nullptr) ? static_cast<UInt32>(pExtraDataList->GetCurrentHealth(pArmor)) : fullHealth;	 ///V1.10.26		

							GFxValue itemHealth;
							root->CreateObject(&itemHealth);
							sprintf_s(sResult.get(), MAX_PATH, "%s: %d/%d", Settings::EQUIPMENTINFO_HEALTH.c_str(), currentHealth, fullHealth);
							RegisterString(&itemHealth, root, "text", sResult.get());
							Register<UInt32>(&itemHealth, "filterFlag", 2);
							arrObj.PushBack(&itemHealth);
						}
						break;
					}
					case FormType::kFormType_AMMO:
					{
						auto pAmmo = static_cast<TESAmmo*>(pBaseForm);
						RelocAddr<BGSKeyword*(*)()> GetChargeKW = 0x1E29B0; ///V1.10.26 //E8 ? ? ? ? 33 FF 45 33 C9 45 33 C0
						BGSKeyword * pKW = GetChargeKW(); //Charge keyword.
						if (pAmmo->keywordForm.keywordBase.HasKeyword(pKW))
						{
							UInt32 currentCharge = pExtraDataList->GetCharge();
							GFxValue itemCharge;
							root->CreateObject(&itemCharge);
							sprintf_s(sResult.get(), MAX_PATH, "%s: %d/%d", Settings::EQUIPMENTINFO_CHARGE.c_str(), currentCharge, 100);
							RegisterString(&itemCharge, root, "text", sResult.get());
							Register<UInt32>(&itemCharge, "filterFlag", 2);
							arrObj.PushBack(&itemCharge);
						}
						break;
					}
					case FormType::kFormType_WEAP:
					{
						TESObjectWEAP * pWeapon = static_cast<TESObjectWEAP*>(pBaseForm);
						auto pWeaponInst = (TESObjectWEAP::InstanceData*)((objInst.data != nullptr) ? objInst.data : pWeapon->GetInstanceData()); //V1.10.26

						if (pWeaponInst != nullptr) {
							GFxValue weaponType;
							root->CreateObject(&weaponType);
							sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EQUIPMENTINFO_CLASS.c_str(), (pWeaponInst->weaponType > TESObjectWEAP::InstanceData::kWeaponType_Mine) ? WeaponTypes[WeaponTypes.size() - 1].c_str() : WeaponTypes[pWeaponInst->weaponType].c_str());
							RegisterString(&weaponType, root, "text", sResult.get());
							Register<UInt32>(&weaponType, "filterFlag", 2);
							arrObj.PushBack(&weaponType);

							TESAmmo* pAmmo = (pWeaponInst != nullptr) ? pWeaponInst->ammo : pWeapon->weapData.ammo;
							if (pAmmo != nullptr)
							{
								bool isJunk = pAmmo->IsJunk();
								RelocAddr<const char *(*)(UInt64 &)> GetCRCString = 0x1B2E10; ///V1.10.26 			//E8 ? ? ? ? 45 33 C0 48 8B CF 48 8B D0 E8 ? ? ? ? 48 8D 54 24 ? 48 8B CF E8 ? ? ? ? 48 8B 7C 24 ? 48 85 FF 74 2E 83 3D ? ? ? ? ? 74 13 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? small
								GFxValue itemAmmo;
								root->CreateObject(&itemAmmo);
								sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EQUIPMENTINFO_AMMO.c_str(), GetCRCString(pAmmo->crcString));
								RegisterString(&itemAmmo, root, "text", sResult.get());
								Register<UInt32>(&itemAmmo, "filterFlag", 2);
								arrObj.PushBack(&itemAmmo);
							}
							///V1.10
							if (pWeapon->weapData.weaponType > TESObjectWEAP::InstanceData::kWeaponType_TwoHandedAxe)
							{
								float shotSpeed = pWeapon->GetShootSpeed(pWeaponInst);///V1.10.26

								GFxValue itemSpeed;
								root->CreateObject(&itemSpeed);
								sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::EQUIPMENTINFO_SPEED.c_str(), shotSpeed);
								RegisterString(&itemSpeed, root, "text", sResult.get());
								Register<UInt32>(&itemSpeed, "filterFlag", 2);
								arrObj.PushBack(&itemSpeed);

								RelocAddr<float(*)(InstanceData&)> GetRange = 0x661C50;///V1.10.26  40 53 48 83 EC 20 48 83 39 00 0F 57 C9
								float range = GetRange(objInst);

								GFxValue itemRange;
								root->CreateObject(&itemRange);
								sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::EQUIPMENTINFO_RANGE.c_str(), range);
								RegisterString(&itemRange, root, "text", sResult.get());
								Register<UInt32>(&itemRange, "filterFlag", 2);
								arrObj.PushBack(&itemRange);

								RelocAddr<float(*)(InstanceData&, Actor*)> GetAccuracy = 0x661D30; ///40 53 48 83 EC 30 0F 29 74 24 ? 48 8B D9 E8 ? ? ? ? 48 8B 4B 08
								float accuracy = GetAccuracy(objInst, actor);

								GFxValue itemAccuracy;
								root->CreateObject(&itemAccuracy);
								sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::EQUIPMENTINFO_ACCURACY.c_str(), accuracy);
								RegisterString(&itemAccuracy, root, "text", sResult.get());
								Register<UInt32>(&itemAccuracy, "filterFlag", 2);
								arrObj.PushBack(&itemAccuracy);
							}
							else
							{
								UInt32 attackSpeed = pWeapon->GetAttackSpeed();
								//RelocAddr<const char * (*)(UInt32)>	GetWeaponSpeedDesc = 0x34D330; ///V1.10.26
								GFxValue itemSpeed;
								root->CreateObject(&itemSpeed);
								sprintf_s(sResult.get(), MAX_PATH, "%s: %d", Settings::EQUIPMENTINFO_SPEED.c_str(), attackSpeed);
								RegisterString(&itemSpeed, root, "text", sResult.get());
								Register<UInt32>(&itemSpeed, "filterFlag", 2);
								arrObj.PushBack(&itemSpeed);
							}

							struct DamageInfo
							{
								UInt32	type;
								float	damage;
							};

							SimpleCollector<DamageInfo>		damageInfo{ 0, nullptr, 0, 0 };
							RelocAddr<void(*)(BGSInventoryItem&, BGSInventoryItem::Stack *, SimpleCollector<DamageInfo>&)> CollectDamageInfo = 0xC0A6D0; ///V1.10.26 //48 8B C4 48 89 50 10 55 56 41 54
							CollectDamageInfo(item, stack, damageInfo);
							for (size_t i = 0; i < damageInfo.count; ++i)
							{
								if (damageInfo.items[i].damage != 0.0f)
								{
									GFxValue itemDamage;
									root->CreateObject(&itemDamage);
									sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::DAMAGETYPES[damageInfo.items[i].type].c_str(), damageInfo.items[i].damage);
									RegisterString(&itemDamage, root, "text", sResult.get());
									Register<UInt32>(&itemDamage, "filterFlag", 2);
									arrObj.PushBack(&itemDamage);
								}
							}
							if (damageInfo.items != nullptr)
							{
								RelocAddr<void(*)(SimpleCollector<DamageInfo>&)>	Release = 0x1B10F10; ///V1.10.26 //40 53 48 83 EC 20 48 8B 51 08 48 8B D9 48 85 D2 74 1A
								Release(damageInfo);
							}
						}
						break;
					}
					}

					if (objInst.data != nullptr)
					{
						objInst.data->DecRefHandle();
						objInst.data = nullptr;
					}

					UInt32 value = pBaseForm->GetValue(pExtraDataList);

					GFxValue itemValue;
					root->CreateObject(&itemValue);
					sprintf_s(sResult.get(), MAX_PATH, "%s: %d.00", Settings::EQUIPMENTINFO_VALUE.c_str(), value);
					RegisterString(&itemValue, root, "text", sResult.get());
					Register<UInt32>(&itemValue, "filterFlag", 2);
					arrObj.PushBack(&itemValue);

					float weight = pBaseForm->GetWeight(objInst.data);
					weight = actor->CalcFormWeight(pBaseForm, stack, weight, nullptr);

					GFxValue itemWeight;
					root->CreateObject(&itemWeight);
					sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::EQUIPMENTINFO_WEIGHT.c_str(), weight);
					RegisterString(&itemWeight, root, "text", sResult.get());
					Register<UInt32>(&itemWeight, "filterFlag", 2);
					arrObj.PushBack(&itemWeight);

					return false;
				});
			}
		}
		pInvList->inventoryLock.Release();
	}
	if (!result)
	{
		TESFullName* pFullName = DYNAMIC_CAST(pBaseForm, TESForm, TESFullName);

		GFxValue itemName;
		root->CreateObject(&itemName);
		sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->name.c_str() : Settings::EXTRAINFO_NULL.c_str());
		RegisterString(&itemName, root, "text", sResult.get());
		Register<UInt32>(&itemName, "filterFlag", 2);
		arrObj.PushBack(&itemName);

		GFxValue itemID;
		root->CreateObject(&itemID);
		sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EXTRAINFO_ID.c_str(), pBaseForm->formID);
		RegisterString(&itemID, root, "text", sResult.get());
		Register<UInt32>(&itemID, "filterFlag", 2);
		arrObj.PushBack(&itemID);

		GFxValue itemType;
		root->CreateObject(&itemType);
		sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_TYPE.c_str(), FormTypes[pBaseForm->formType].c_str());
		RegisterString(&itemType, root, "text", sResult.get());
		Register<UInt32>(&itemType, "filterFlag", 2);
		arrObj.PushBack(&itemType);

		float weight = pBaseForm->GetWeight(nullptr);
		GFxValue itemWeight;
		root->CreateObject(&itemWeight);
		sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::EQUIPMENTINFO_WEIGHT.c_str(), weight);
		RegisterString(&itemWeight, root, "text", sResult.get());
		Register<UInt32>(&itemWeight, "filterFlag", 2);
		arrObj.PushBack(&itemWeight);
	}
}

void	CollectEquipmentData	(GFxValue& object, GFxMovieRoot * root, Actor * actor, UInt32& count)
{
	if (object.GetType() == GFxValue::kType_Object && actor->formType == kFormType_ACHR && root != nullptr)
	{
		ActorEquipData * equipData = actor->equipData;
		if (actor->formID == 0x14 && *g_playerCamera && (*g_playerCamera)->cameraState == (*g_playerCamera)->cameraStates[PlayerCamera::kCameraState_FirstPerson])
		{
			equipData = static_cast<PlayerCharacter*>(actor)->playerEquipData;
		}
		if (equipData != nullptr)
		{
			std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);
			GFxValue * pExtraInfo = nullptr;

			for (size_t slotIndex = 0; slotIndex < ActorEquipData::kMaxSlots; ++slotIndex)
			{
				auto pForm = equipData->slots[slotIndex].item;
				if (pForm != nullptr/* && pForm->formType != FormType::kFormType_ARMA*/)
				{
					++count;
					if (!pExtraInfo)
					{
						pExtraInfo = new GFxValue();
						root->CreateArray(pExtraInfo);
					}
					GFxValue	equipmentData;
					root->CreateObject(&equipmentData);
					TESFullName* pFullName = DYNAMIC_CAST(pForm, TESForm, TESFullName);
					sprintf_s(sResult.get(), MAX_PATH, "%s(%s)", (pFullName != nullptr) ? pFullName->name.c_str() : "", FormTypes[pForm->formType].c_str());
					RegisterString(&equipmentData, root, "text", sResult.get());
					Register<UInt32>(&equipmentData, "filterFlag", 2);

					GFxValue extraInfo;
					root->CreateArray(&extraInfo); //extraData

					if (pForm->formType == FormType::kFormType_ARMA)
					{
						GFxValue equipmentName;
						root->CreateObject(&equipmentName);
						sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->name.c_str() : Settings::EXTRAINFO_NULL.c_str());
						RegisterString(&equipmentName, root, "text", sResult.get());
						Register<UInt32>(&equipmentName, "filterFlag", 2);
						extraInfo.PushBack(&equipmentName);

						GFxValue equipmentID;
						root->CreateObject(&equipmentID);
						sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EXTRAINFO_ID.c_str(), pForm->formID);
						RegisterString(&equipmentID, root, "text", sResult.get());
						Register<UInt32>(&equipmentID, "filterFlag", 2);
						extraInfo.PushBack(&equipmentID);

						GFxValue equipmentType;
						root->CreateObject(&equipmentType);
						sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_TYPE.c_str(), FormTypes[pForm->formType].c_str());
						RegisterString(&equipmentType, root, "text", sResult.get());
						Register<UInt32>(&equipmentType, "filterFlag", 2);
						extraInfo.PushBack(&equipmentType);
					}
					else
					{
						ColllectInventoryData(extraInfo, root, actor, pForm);
					}
					GFxValue equipmentSlot;
					root->CreateObject(&equipmentSlot);
					sprintf_s(sResult.get(), MAX_PATH, "%s: %d", Settings::EQUIPMENTINFO_SLOTINDEX.c_str(), static_cast<int>(slotIndex));
					RegisterString(&equipmentSlot, root, "text", sResult.get());
					Register<UInt32>(&equipmentSlot, "filterFlag", 2);
					extraInfo.PushBack(&equipmentSlot);

					GFxValue equipmentMod;
					root->CreateObject(&equipmentMod);
					auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(pForm);
					sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_MOD.c_str(), (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL.c_str());
					RegisterString(&equipmentMod, root, "text", sResult.get());
					Register<UInt32>(&equipmentMod, "filterFlag", 2);
					extraInfo.PushBack(&equipmentMod);

					equipmentData.SetMember("extraInfo", &extraInfo);
					pExtraInfo->PushBack(&equipmentData);
				}
			}
			if (pExtraInfo != nullptr)
			{
				object.SetMember("extraInfo", pExtraInfo);
				delete pExtraInfo;
				pExtraInfo = nullptr;
			}
		}
	}
}

#ifdef DEBUG
	UInt32 * value = reinterpret_cast<UInt32*>((uintptr_t)(pObjectExtraInstance->data->forms) + pObjectExtraInstance->data->blockSize);
	if (*value >> 24 != 0xFF)
	{
		UInt32	iOffset = *value & 0xFFFFFF;
		UInt32	iCount = *(value + 1) >> 3 & 0x1FFFFF;
		if (iCount)
		{
			for (size_t i = 0; i < iCount; ++i)
			{
				auto * pMod = reinterpret_cast<BGSObjectInstanceExtra::Data::Form *>((uintptr_t)(pObjectExtraInstance->data->forms) + i * sizeof(BGSObjectInstanceExtra::Data::Form));
				auto * pObjectMod = (BGSMod::Attachment::Mod *)LookupFormByID(pMod->formId);
				if (pObjectMod != nullptr && pObjectMod->formType == FormType::kFormType_OMOD/* && pObjectMod->flags & TESForm::kModFlag_0x10 && !(pObjectMod->flags & TESForm::kModFlag_0x80)*/)
				{
					TESFullName& pFullName = pObjectMod->fullName;
					_MESSAGE("fullName : %s", pObjectMod->fullName.name.c_str());

					TESDescription * pDescription = DYNAMIC_CAST(pObjectMod, TESForm, TESDescription);
					if (pDescription != nullptr)
					{
						BSString str;
						CALL_MEMBER_FN(pDescription, Get)(&str, nullptr);
						_MESSAGE("DESC: %s", str.Get());
					}
				}
			}
		}
	}
#endif


#ifdef DEBUG

//C0C590
void DumpInventory(Actor* actor)
{
	auto pInvList = actor->inventoryList;
	if (pInvList != nullptr)
	{
		pInvList->inventoryLock.Lock();
		for (size_t i = 0; i < pInvList->items.count; ++i)
		{
			BGSInventoryItem item = pInvList->items[i];
			TESForm* pBaseForm = item.form;

			if (item.stack != nullptr)
			{
				item.stack->Visit([&](BGSInventoryItem::Stack * stack)->bool
				{
					struct InstanceData
					{
						TESForm					* form;
						TBO_InstanceData		* data;
					};
					RelocAddr<InstanceData *(*)(InstanceData& out, TESForm *, TBO_InstanceData *)> CalcInstanceData = 0x2F7A40; //V1.10

					ExtraDataList * pExtraDataList = stack->extraData;
					TBO_InstanceData * pExtraInstanceData = nullptr;
					const char * pName = nullptr;
					if (pExtraDataList != nullptr)
					{
						RelocAddr<TBO_InstanceData *(*)(ExtraDataList *)> GetExtraInstanceData = 0x8A430; //V1.10
						pExtraInstanceData = GetExtraInstanceData(pExtraDataList);
						auto pExtraName = static_cast<ExtraTextDisplayData*>(pExtraDataList->GetByType(kExtraData_TextDisplayData));
						pName = (pExtraName != nullptr) ? pExtraName->name.c_str() : nullptr;
					}
					if (!pName)
					{
						TESFullName* pFullName = DYNAMIC_CAST(pBaseForm, TESForm, TESFullName);
						pName = (pFullName != nullptr) ? pFullName->name.c_str() : nullptr;
					}
					_MESSAGE("formID: %08X	count: %d	name: %s	type: %s", pBaseForm->formID, stack->count, pName, FormTypes[pBaseForm->formType].c_str());
					InstanceData	objInst{ nullptr, nullptr };
					CalcInstanceData(objInst, pBaseForm, pExtraInstanceData);

					switch (pBaseForm->formType)
					{
						case FormType::kFormType_ARMO:
						{
							_MESSAGE(">>>>>>>>>>>>>>>>>>>[ARMO]");
							TESObjectARMO* pArmor = static_cast<TESObjectARMO*>(pBaseForm);
							auto pArmorInst = (TESObjectARMO::InstanceData*)((objInst.data != nullptr) ? objInst.data : pArmor->GetInstanceData());//V1.10

							struct ResistInfo
							{
								UInt32	type;
								float	resist;
							};
							SimpleCollector<ResistInfo>		resistInfo{ 0, nullptr, 0, 0 };
							RelocAddr<void(*)(BGSInventoryItem&, BGSInventoryItem::Stack *, SimpleCollector<ResistInfo>	&, float)> CollectResistInfo = 0xC090D0; //1.10
							CollectResistInfo(item, stack, resistInfo, 1.0f);

							for (size_t i = 0; i < resistInfo.count; ++i)
							{
								_MESSAGE("%d = %.2f", resistInfo.items[i].type, resistInfo.items[i].resist);
							}
							if (resistInfo.items != nullptr)
							{
								RelocAddr<void(*)(SimpleCollector<ResistInfo>&)>	Release = 0x1B0F430; //V1.10
								Release(resistInfo);
							}
							RelocAddr<BGSKeyword*(*)()> GeHealthtKW = 0x1E2A20; //V1.10
							BGSKeyword * pKW = GeHealthtKW(); //Health keyword.
							if (pArmor->keywordForm.keywordBase.HasKeyword(pKW))
							{
								UInt32 fullHealth = CALL_MEMBER_FN(pArmor, GetFullHealth)(pArmorInst);///V1.10		
								UInt32 currentHealth = (pExtraDataList != nullptr) ? static_cast<UInt32>(CALL_MEMBER_FN(pExtraDataList, GetCurrentHealth)(pArmor)) : fullHealth;	 ///V1.10			
								_MESSAGE("curHealth: %d		fullHealth: %d", currentHealth, fullHealth);
							}

							TESEnchantableForm * pEnchantableForm = DYNAMIC_CAST(pArmor, TESForm, TESEnchantableForm);
							if (pEnchantableForm != nullptr)
							{
								if (pArmorInst != nullptr)
								{
									tArray<EnchantmentItem*>* pEnchantmentItems = pArmorInst->GetEnchantmentItems();
									if (pEnchantmentItems != nullptr)
									{
										for (size_t i = 0; i < pEnchantmentItems->count; ++i)
										{
											auto pEnchantmentItem = pEnchantmentItems->operator[](i);
											TESFullName* pFullName = DYNAMIC_CAST(pEnchantableForm, EnchantmentItem, TESFullName);
											if (pFullName != nullptr)
											{
												_MESSAGE("armorEnchantmentName: %s", pFullName->name.c_str());
											}
											for (size_t i = 0; i < pEnchantmentItem->effectItems.count; ++i)
											{
												auto pEffectItem = pEnchantmentItem->effectItems[i];
												_MESSAGE("mag: %.2f   area: %d   duration: %d   cost: %.2f", pEffectItem->magnitude, pEffectItem->area, pEffectItem->duration, pEffectItem->cost);
												if (pEffectItem->mgef && pEffectItem->mgef->actorValInfoD8)
												{
													_MESSAGE("AvName: %s  mgef: %.2f", CALL_MEMBER_FN(pEffectItem->mgef->actorValInfoD8, GetDisplayName)(), pEffectItem->magnitude);
												}
											}
										}
									}
								}
								EnchantmentItem* pEnchantmentItem = (pEnchantableForm->enchantment != nullptr) ? pEnchantableForm->enchantment : CALL_MEMBER_FN(pExtraDataList, GetExtraEnchantmentItem)();
								if (pEnchantmentItem)
								{
									TESFullName* pFullName = DYNAMIC_CAST(pEnchantableForm, EnchantmentItem, TESFullName);
									if (pFullName != nullptr)
									{
										_MESSAGE("basearmorEnchantmentName: %s", pFullName->name.c_str());
									}
									for (size_t i = 0; i < pEnchantmentItem->effectItems.count; ++i)
									{
										auto pEffectItem = pEnchantmentItem->effectItems[i];
										_MESSAGE("mag: %.2f   area: %d   duration: %d   cost: %.2f", pEffectItem->magnitude, pEffectItem->area, pEffectItem->duration, pEffectItem->cost);
										if (pEffectItem->mgef && pEffectItem->mgef->actorValInfoD8)
										{
											_MESSAGE("AvName: %s  mgef: %.2f", CALL_MEMBER_FN(pEffectItem->mgef->actorValInfoD8, GetDisplayName)(), pEffectItem->magnitude);
										}
									}
								}
							}
							//V1.10
							RelocAddr<BSExtraData*(*)(ExtraDataList*, UInt8 type)> GetExtraData = 0x436A0; // 5B
							BSExtraData* pExtraData = GetExtraData(pExtraDataList, kExtraData_ObjectInstance);
							BGSObjectInstanceExtra* pObjectExtraInstance = static_cast<BGSObjectInstanceExtra*>(pExtraData);
							if (pObjectExtraInstance != nullptr && pObjectExtraInstance->data && pObjectExtraInstance->data->forms != nullptr)
							{
								for (UInt32 i = 0; i < pObjectExtraInstance->data->blockSize / sizeof(BGSObjectInstanceExtra::Data::Form); ++i)
								{
									BGSMod::Attachment::Mod * pObjectMod = (BGSMod::Attachment::Mod *)Runtime_DynamicCast(LookupFormByID(pObjectExtraInstance->data->forms[i].formId), RTTI_TESForm, RTTI_BGSMod__Attachment__Mod);
									if (pObjectMod != nullptr && pObjectMod->formType == FormType::kFormType_OMOD/* && pObjectMod->flags & TESForm::kModFlag_0x10 && !(pObjectMod->flags & TESForm::kModFlag_0x80)*/)
									{
										TESFullName* pFullName = DYNAMIC_CAST(pObjectMod, TESForm, TESFullName);
										if (pFullName != nullptr)
										{
											_MESSAGE("fullName : %s", pObjectMod->fullName.name.c_str());
										}
										TESDescription * pDescription = DYNAMIC_CAST(pObjectMod, TESForm, TESDescription);
										if (pDescription != nullptr)
										{
											BSString str;
											CALL_MEMBER_FN(pDescription, Get)(&str, nullptr);//V1.10
											_MESSAGE("DESC: %s", str.Get());
										}
									}
								}
							}
							break;
						}
						case FormType::kFormType_AMMO:
						{
							_MESSAGE(">>>>>>>>>>>>>>>>>>>[AMMO]");
							auto pAmmo = static_cast<TESAmmo*>(pBaseForm);

							RelocAddr<BGSKeyword*(*)()> GetChargeKW = 0x1E29C0; //V1.10
							BGSKeyword * pKW = GetChargeKW(); //Charge keyword.
							if (pAmmo->keywordForm.keywordBase.HasKeyword(pKW))
							{
								float currentCharge = CALL_MEMBER_FN(stack->extraData, GetCharge)();
								_MESSAGE("curCharge: %.2f		fullCharge: %.2f", currentCharge * 100.0f, 100.0f);
							}
							break;
						}//V1.10
						case FormType::kFormType_WEAP:
						{
							_MESSAGE(">>>>>>>>>>>>>>>>>>>[WEAP]");
							TESObjectWEAP * pWeapon = static_cast<TESObjectWEAP*>(pBaseForm);
							auto pWeaponInst = (TESObjectWEAP::InstanceData*)((objInst.data != nullptr) ? objInst.data : pWeapon->GetInstanceData());

							if (pWeaponInst != nullptr){
								TESAmmo* pAmmo = (pWeaponInst != nullptr) ? pWeaponInst->ammo : pWeapon->weapData.ammo;
								if (pAmmo != nullptr)
								{
									bool isJunk = CALL_MEMBER_FN(pAmmo, IsJunk)();
									RelocAddr<const char *(*)(UInt64 &)> GetCRCString = 0x1B2E20; //V1.10
									if (isJunk && (*g_player)->unkD80)
									{
										_MESSAGE("JUNK: %s", GetCRCString(pAmmo->crcString));
									}
									else
									{
										const char * sResult = GetCRCString(pAmmo->crcString);
										UInt32 iCount = CALL_MEMBER_FN((*g_player), GetItemCount)(pAmmo);
										_MESSAGE("Ammo: %s Count: %d", sResult, iCount);
									}
								}
								///V1.10
								if (pWeapon->weapData.weaponType > TESObjectWEAP::InstanceData::kWeaponType_TwoHandedAxe)  //weapon type
								{
									float shotSpeed = CALL_MEMBER_FN(pWeapon, GetShootSpeed)(pWeaponInst);///V1.10
									RelocAddr<float(*)(InstanceData&)> GetRange = 0x661C50;///V1.10
									float range = GetRange(objInst);
									RelocAddr<float(*)(InstanceData&, Actor*)> GetAccuracy = 0x661D30; ///V1.10
									float accuracy = GetAccuracy(objInst, (*g_player));
									_MESSAGE("shotSpeed: %.2f  range: %.2f  accuracy: %.2f", shotSpeed, range, accuracy);
								}
								else
								{
									RelocAddr<UInt32(*)(TESObjectWEAP*)> GetWeaponClass = 0x34D150; ///V1.10
									UInt32 weaponClass = GetWeaponClass(pWeapon);
									RelocAddr<UInt32(*)(UInt32)>	GetWeaponSpeed = 0x34D340; ///V1.10
									UInt32 speed = GetWeaponSpeed(weaponClass);
									_MESSAGE("WeaponSpeed: %d", speed);
								}

								struct DamageInfo
								{
									UInt32	type;
									float	damage;
								};
								SimpleCollector<DamageInfo>		damageInfo{ 0, nullptr, 0, 0 };
								RelocAddr<void(*)(BGSInventoryItem&, BGSInventoryItem::Stack *, SimpleCollector<DamageInfo>&)> CollectDamageInfo = 0xC08C70; ///V1.10
								CollectDamageInfo(item, stack, damageInfo);
								for (size_t i = 0; i < damageInfo.count; ++i)
								{
									_MESSAGE("[DAMAGE] type: %d damage:  %.2f", damageInfo.items[i].type, damageInfo.items[i].damage);
								}
								if (damageInfo.items != nullptr)
								{
									RelocAddr<void(*)(SimpleCollector<DamageInfo>&)>	Release = 0x1B0F430; ///V1.10
									Release(damageInfo);
								}
							}
							break;
						}
						case FormType::kFormType_MISC:
						{
							_MESSAGE(">>>>>>>>>>>>>>>>>>>[MISC]");
							TESObjectMISC* pMisc = static_cast<TESObjectMISC*>(pBaseForm);
							auto pComponents = pMisc->components;
							for (size_t i = 0; pComponents != nullptr && i < pComponents->count; ++i)
							{
								auto pItem = pComponents->operator[](i);
								TESFullName * pFullName = DYNAMIC_CAST(pItem.component, BGSComponent, TESFullName);
								if (pFullName != nullptr){
									_MESSAGE("[MISC] fullName: %s  count: %d  formID: %08X", pFullName->GetFullName(), pItem.count, pItem.component->formID);
								}
							}
							break;
						}
						case FormType::kFormType_ALCH:
						{
							_MESSAGE(">>>>>>>>>>>>>>>>>>>[ALCH]");
							RelocPtr<BGSKeyword*>	pHealingKW = 0x59016D0;///V1.10
							AlchemyItem* pAlchemy = static_cast<AlchemyItem*>(pBaseForm);
							if (!pAlchemy->keywordForm.keywordBase.HasKeyword(*pHealingKW))
							{
								float HP = 0.0f;
								for (size_t i = 0; i < pAlchemy->effectItems.count; ++i)
								{
									auto pEffectItem = pAlchemy->effectItems[i];
									if (pEffectItem->mgef != nullptr && !(pEffectItem->mgef->flags & EffectSetting::kEffectType_HideInUI)	\
										&& EvaluationConditions(&pEffectItem->conditions, *g_player, *g_player)								\
										&& EvaluationConditions(&pEffectItem->mgef->conditions, *g_player, *g_player))
									{
										RelocAddr<void(*)(MagicItem*, MagicItem::EffectItem*, float& magnitude, float& duration)> CalcMagicEffect = 0xBC2D20;///V1.10 CalaPerkEffect in this funtion.
										float magnitude = 0.0f, duration = 0.0f;
										CalcMagicEffect(pAlchemy, pEffectItem, magnitude, duration);
										_MESSAGE("[ALCH] mag: %.2f dur: %.2f", magnitude, duration);

										RelocAddr<float(*)(MagicItem*, MagicItem::EffectItem*)> GetRestoredHPPercent = 0x12BF4B0; ///V1.10
										HP += GetRestoredHPPercent(pAlchemy, pEffectItem);

										MagicItem::EffectItem::Description valueName{ nullptr, 0, 0, 0, 0 };;
										RelocAddr<void(*)(MagicItem::EffectItem*, MagicItem::EffectItem::Description&)> GetValueName = 0xC0AE10; ///V1.10
										GetValueName(pEffectItem, valueName);
										_MESSAGE("[VALU] %s", valueName.pDescription);

										MagicItem::EffectItem::Description description{ nullptr, 0, 0, 0, 0 };
										if (!valueName.pDescription || !valueName.pDescription[0])
										{
											CALL_MEMBER_FN(pEffectItem, CalcDescription)(description, "", "", magnitude, duration); ///V1.10
											_MESSAGE("[DESC] %s", description.pDescription);
										}
										const char * pValueName = (valueName.pDescription != nullptr && valueName.pDescription[0]) ? valueName.pDescription : nullptr;
										const char * pDesctiption = (!pValueName) ? description.pDescription : nullptr;

										bool showAsDescription = true;
										if (pDesctiption && (pDesctiption[0] == 0x25 || pDesctiption[0] == 0x24)) //% 37 $ 36
										{
											pDesctiption = description.pDescription + 1;
											showAsDescription = false;
										}

										bool showAsPercent = false;
										if (pEffectItem->mgef->archetype == EffectSetting::kArchetype_ValueAndParts)
										{
											showAsPercent = true;
										}
										else
										{
											RelocAddr<ActorValueInfo**(*)()> fnGetAVHolder = 0x006B1F0;
											auto pAVs = fnGetAVHolder();
											ActorValueInfo*	pXP = *reinterpret_cast<ActorValueInfo**>(pAVs + 0x1A);
											if (pEffectItem->mgef->actorValInfoD8 == pXP)
											{
												showAsPercent = 1;
											}
											else if (!pValueName && description.pDescription && (description.pDescription[0] == 0x25))
											{
												showAsPercent = 1;
											}
										}

										if (valueName.pDescription != nullptr)
										{
											RelocAddr<UInt32*>	heapAlignment = 0x38EB200; ///V1.10
											if (*heapAlignment != 2)
											{
												CALL_MEMBER_FN(g_mainHeap, ChangeAlignment)(reinterpret_cast<UInt32*>(heapAlignment.GetUIntPtr()));
											}
											Heap_Free(const_cast<char*>(valueName.pDescription));
										}

										if (description.pDescription != nullptr)
										{
											RelocAddr<UInt32*>	heapAlignment = 0x38EB200; ///V1.10
											if (*heapAlignment != 2)
											{
												CALL_MEMBER_FN(g_mainHeap, ChangeAlignment)(reinterpret_cast<UInt32*>(heapAlignment.GetUIntPtr()));
											}
											Heap_Free(const_cast<char*>(description.pDescription));
										}
									}
								}
								_MESSAGE("HP: %.2f", HP);
							}
							break;					
						}
						default:
						{

						}
					}

					UInt32 value = CALL_MEMBER_FN(pBaseForm, GetValue)(pExtraDataList);
					float weight = CALL_MEMBER_FN(pBaseForm, GetWeight)(objInst.data);
					weight = actor->CalcFormWeight(pBaseForm, stack, weight, nullptr);
					_MESSAGE("VALUE: %d WEIGHT: %.2f", value, weight);
					return true;
				});
			}
			else
			{

			}
		}
		pInvList->inventoryLock.Release();
	}
}

#endif

void BetterConsole_GetExtraData::Invoke(Args * args)
{
	TESObjectREFR* pRef = nullptr;
	LookupREFRByHandle(g_consoleHandle, &pRef);
	if (pRef != nullptr && pRef->formType == kFormType_ACHR)
	{
		args->movie->movieRoot->CreateArray(args->result);
		Actor* actor = static_cast<Actor*>(pRef);
		std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);

		GFxValue name;
		args->movie->movieRoot->CreateObject(&name);
		const char* pRefName = CALL_MEMBER_FN(pRef, GetReferenceName)();
		sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), (pRefName != nullptr && strlen(pRefName)) ? pRefName : Settings::EXTRAINFO_NULL.c_str());
		RegisterString(&name, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&name, "filterFlag", 2);
		args->result->PushBack(&name);

		GFxValue refID;
		args->movie->movieRoot->CreateObject(&refID);
		sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EXTRAINFO_ID.c_str(), actor->formID);
		RegisterString(&refID, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&refID, "filterFlag", 2);
		args->result->PushBack(&refID);

		TESForm* pBaseForm = pRef->baseForm;
		BSExtraData* pExtraData = pRef->extraDataList->GetExtraData(kExtraData_LeveledCreature);
		if (pExtraData != nullptr)
		{
			auto leveledBaseForm = *reinterpret_cast<TESForm**>((uintptr_t)pExtraData + 0x18);
			pBaseForm = (leveledBaseForm != nullptr) ? leveledBaseForm : pRef->baseForm;
		}

		GFxValue baseID;
		args->movie->movieRoot->CreateObject(&baseID);
		sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EXTRAINFO_BASEID.c_str(), pBaseForm->formID);
		RegisterString(&baseID, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&baseID, "filterFlag", 2);
		args->result->PushBack(&baseID);

		auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(pBaseForm);
		if (pModInfo != nullptr)
		{
			GFxValue mod;
			args->movie->movieRoot->CreateObject(&mod);
			const char* modName = (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL.c_str();
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_MOD.c_str(), modName);
			RegisterString(&mod, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&mod, "filterFlag", 2);
			args->result->PushBack(&mod);
		}

		TESFullName* pFullName = DYNAMIC_CAST(actor->race, TESRace, TESFullName);
		if (pFullName != nullptr && strlen(pFullName->name.c_str()))
		{
			GFxValue race;
			args->movie->movieRoot->CreateObject(&race);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_RACE.c_str(), (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->name.c_str() : Settings::EXTRAINFO_NULL.c_str());
			RegisterString(&race, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&race, "filterFlag", 2);
			args->result->PushBack(&race);
		}

		GFxValue raceID;
		args->movie->movieRoot->CreateObject(&raceID);
		sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EXTRAINFO_RACEID.c_str(), actor->race->formID);
		RegisterString(&raceID, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&raceID, "filterFlag", 2);
		args->result->PushBack(&raceID);

		TESNPC* npc = static_cast<TESNPC*>(pBaseForm);

		GFxValue sex;
		args->movie->movieRoot->CreateObject(&sex);
		sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_SEX.c_str(), npc->actorData.flags.female ? Settings::EXTRAINFO_FEMALE.c_str() : Settings::EXTRAINFO_MALE.c_str());
		RegisterString(&sex, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&sex, "filterFlag", 2);
		args->result->PushBack(&sex);

		GFxValue level;
		args->movie->movieRoot->CreateObject(&level);
		sprintf_s(sResult.get(), MAX_PATH, "%s: %d", Settings::EXTRAINFO_LEVEL.c_str(), actor->GetLevel());///V1.10
		RegisterString(&level, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&level, "filterFlag", 2);
		args->result->PushBack(&level);


		if (npc->actorData.flags.pcLevelMult)
		{
			GFxValue PCLevelMult;
			args->movie->movieRoot->CreateObject(&PCLevelMult);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::EXTRAINFO_PCLVMULT.c_str(), static_cast<float>(npc->actorData.level) / 1000);
			RegisterString(&PCLevelMult, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&PCLevelMult, "filterFlag", 2);
			args->result->PushBack(&PCLevelMult);

			GFxValue minLevel;
			args->movie->movieRoot->CreateObject(&minLevel);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %d", Settings::EXTRAINFO_MINLEVEL.c_str(), npc->actorData.minLevel);
			RegisterString(&minLevel, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&minLevel, "filterFlag", 2);
			args->result->PushBack(&minLevel);

			GFxValue maxLevel;
			args->movie->movieRoot->CreateObject(&maxLevel);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %d", Settings::EXTRAINFO_MAXLEVEL.c_str(), npc->actorData.maxLevel);
			RegisterString(&maxLevel, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&maxLevel, "filterFlag", 2);
			args->result->PushBack(&maxLevel);
		}

		GFxValue health;
		args->movie->movieRoot->CreateObject(&health);
		RelocAddr<ActorValueInfo**(*)()> GetActorValueHolder = 0x006B1F0; //E8 ? ? ? ? 48 8B C8 E8 ? ? ? ? 48 8B 0D ? ? ? ? E8 ? ? ? ? E8 ? ? ? ?
		auto pAVs = GetActorValueHolder();
		ActorValueInfo*	pHealth = *reinterpret_cast<ActorValueInfo**>(pAVs + 0x1B);
		sprintf_s(sResult.get(), MAX_PATH, "HP: %d", static_cast<UInt32>(actor->actorValueOwner.GetValue(pHealth)));
		RegisterString(&health, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&health, "filterFlag", 2);
		args->result->PushBack(&health);

		TESForm* pForm = LookupFormByID(0x2D5); //action points
		if (pForm != nullptr && pForm->formType == kFormType_AVIF)
		{
			auto pAV = static_cast<ActorValueInfo*>(pForm);
			GFxValue AP;
			args->movie->movieRoot->CreateObject(&AP);
			sprintf_s(sResult.get(), MAX_PATH, "AP: %d", static_cast<UInt32>(actor->actorValueOwner.GetValue(pAV)));
			RegisterString(&AP, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&AP, "filterFlag", 2);
			args->result->PushBack(&AP);
		}

		GFxValue isUnique;
		args->movie->movieRoot->CreateObject(&isUnique);
		sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_UNIQUE.c_str(), npc->actorData.flags.unique ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
		RegisterString(&isUnique, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&isUnique, "filterFlag", 2);
		args->result->PushBack(&isUnique);

		GFxValue isEssential;
		args->movie->movieRoot->CreateObject(&isEssential);
		sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_ESSENTIAL.c_str(), npc->actorData.flags.essential ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
		RegisterString(&isEssential, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&isEssential, "filterFlag", 2);
		args->result->PushBack(&isEssential);

		GFxValue isProtected;
		args->movie->movieRoot->CreateObject(&isProtected);
		sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_PROTECTED.c_str(), npc->actorData.flags.protect ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
		RegisterString(&isProtected, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&isProtected, "filterFlag", 2);
		args->result->PushBack(&isProtected);

		GFxValue isInvulnerable;
		args->movie->movieRoot->CreateObject(&isInvulnerable);
		sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_INVULNERABLE.c_str(), npc->actorData.flags.invulnerable ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
		RegisterString(&isInvulnerable, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&isInvulnerable, "filterFlag", 2);
		args->result->PushBack(&isInvulnerable);

		GFxValue position;
		args->movie->movieRoot->CreateObject(&position);
		sprintf_s(sResult.get(), MAX_PATH, "%s >", Settings::POSITIONINFO.c_str());
		RegisterString(&position, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&position, "filterFlag", 2);
		CollectPositionData(position, args->movie->movieRoot, actor);
		args->result->PushBack(&position);

		TESObjectCELL* pCell = actor->parentCell;  //extra
		if (pCell != nullptr)
		{
			GFxValue cell;
			args->movie->movieRoot->CreateObject(&cell);
			sprintf_s(sResult.get(), MAX_PATH, "%s >", Settings::CELLINFO.c_str());
			RegisterString(&cell, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&cell, "filterFlag", 2);

			GFxValue extraInfo;
			args->movie->movieRoot->CreateArray(&extraInfo);

			GFxValue cellName;
			args->movie->movieRoot->CreateObject(&cellName);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), strlen(pCell->fullName.name.c_str()) ? pCell->fullName.name.c_str() : Settings::EXTRAINFO_NULL.c_str());
			RegisterString(&cellName, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&cellName, "filterFlag", 2);
			extraInfo.PushBack(&cellName);

			GFxValue cellID;
			args->movie->movieRoot->CreateObject(&cellID);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EXTRAINFO_ID.c_str(), pCell->formID);
			RegisterString(&cellID, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&cellID, "filterFlag", 2);
			extraInfo.PushBack(&cellID);

			//IsInterior
			GFxValue cellIsInterior;
			args->movie->movieRoot->CreateObject(&cellIsInterior);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::CELLINFO_ISINTERIOR.c_str(), (pCell->flags & TESObjectCELL::kFlag_IsInterior) ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
			RegisterString(&cellIsInterior, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&cellIsInterior, "filterFlag", 2);
			extraInfo.PushBack(&cellIsInterior);

			GFxValue cellMod;
			args->movie->movieRoot->CreateObject(&cellMod);
			auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(pCell);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_MOD.c_str(), (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL.c_str());
			RegisterString(&cellMod, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&cellMod, "filterFlag", 2);
			extraInfo.PushBack(&cellMod);

			cell.SetMember("extraInfo", &extraInfo);
			args->result->PushBack(&cell);
		}		

		auto pLocation = actor->GetCurrentLocation(); //extra V1.10.20
		if (pLocation != nullptr)
		{
			GFxValue location;  
			args->movie->movieRoot->CreateObject(&location);
			sprintf_s(sResult.get(), MAX_PATH, "%s >", Settings::LOCATIONINFO.c_str());
			RegisterString(&location, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&location, "filterFlag", 2);

			GFxValue extraInfo;
			args->movie->movieRoot->CreateArray(&extraInfo);

			GFxValue locationName;
			args->movie->movieRoot->CreateObject(&locationName);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), strlen(pLocation->fullName.name.c_str()) ? pLocation->fullName.name.c_str() : Settings::EXTRAINFO_NULL.c_str());
			RegisterString(&locationName, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&locationName, "filterFlag", 2);
			extraInfo.PushBack(&locationName);

			GFxValue locationID;
			args->movie->movieRoot->CreateObject(&locationID);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EXTRAINFO_ID.c_str(), pLocation->formID);
			RegisterString(&locationID, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&locationID, "filterFlag", 2);
			extraInfo.PushBack(&locationID);

			GFxValue locationMod;
			args->movie->movieRoot->CreateObject(&locationMod);
			auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(pLocation);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_MOD.c_str(), (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL.c_str());
			RegisterString(&locationMod, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&locationMod, "filterFlag", 2);
			extraInfo.PushBack(&locationMod);

			location.SetMember("extraInfo", &extraInfo);
			args->result->PushBack(&location);
		}

		auto pWorld = CALL_MEMBER_FN(actor, GetWorldspace)();  //extra
		if (pWorld != nullptr)
		{
			TESFullName* pFullName = DYNAMIC_CAST(pWorld, TESWorldSpace, TESFullName);

			GFxValue worldSpace; 
			args->movie->movieRoot->CreateObject(&worldSpace);
			sprintf_s(sResult.get(), MAX_PATH, "%s >", Settings::WORLDSPACEINFO.c_str());
			RegisterString(&worldSpace, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&worldSpace, "filterFlag", 2);

			GFxValue extraInfo;
			args->movie->movieRoot->CreateArray(&extraInfo);

			GFxValue worldName;
			args->movie->movieRoot->CreateObject(&worldName);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), strlen(pFullName->name.c_str()) ? pFullName->name.c_str() : Settings::EXTRAINFO_NULL.c_str());
			RegisterString(&worldName, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&worldName, "filterFlag", 2);
			extraInfo.PushBack(&worldName);

			GFxValue worldID;
			args->movie->movieRoot->CreateObject(&worldID);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EXTRAINFO_ID.c_str(), pWorld->formID);
			RegisterString(&worldID, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&worldID, "filterFlag", 2);
			extraInfo.PushBack(&worldID);

			GFxValue worldMod;
			args->movie->movieRoot->CreateObject(&worldMod);
			auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(pWorld);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_MOD.c_str(), (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL.c_str());
			RegisterString(&worldMod, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&worldMod, "filterFlag", 2);
			extraInfo.PushBack(&worldMod);

			worldSpace.SetMember("extraInfo", &extraInfo);
			args->result->PushBack(&worldSpace);
		}

		auto pZone = actor->GetEncounterZone();
		if (pZone != nullptr)
		{
			GFxValue zone;
			args->movie->movieRoot->CreateObject(&zone);
			sprintf_s(sResult.get(), MAX_PATH, "%s >", Settings::ENCOUNTERZONEINFO.c_str());
			RegisterString(&zone, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&zone, "filterFlag", 2);

			GFxValue extraInfo;
			args->movie->movieRoot->CreateArray(&extraInfo);

			GFxValue zoneID;
			args->movie->movieRoot->CreateObject(&zoneID);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EXTRAINFO_ID.c_str(), pZone->formID);
			RegisterString(&zoneID, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&zoneID, "filterFlag", 2);
			extraInfo.PushBack(&zoneID);

			GFxValue zoneMinlevel;
			args->movie->movieRoot->CreateObject(&zoneMinlevel);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %d", Settings::ENCOUNTERZONEINFO_MINLEVEL.c_str(), pZone->minLevel);
			RegisterString(&zoneMinlevel, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&zoneMinlevel, "filterFlag", 2);
			extraInfo.PushBack(&zoneMinlevel);

			GFxValue zoneMaxlevel;
			args->movie->movieRoot->CreateObject(&zoneMaxlevel);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %d", Settings::ENCOUNTERZONEINFO_MAXLEVEL.c_str(), pZone->maxLevel);
			RegisterString(&zoneMaxlevel, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&zoneMaxlevel, "filterFlag", 2);
			extraInfo.PushBack(&zoneMaxlevel);

			GFxValue zoneCurlevel;
			args->movie->movieRoot->CreateObject(&zoneCurlevel);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %d", Settings::ENCOUNTERZONEINFO_CURRENTLEVEL.c_str(), pZone->curLevel);
			RegisterString(&zoneCurlevel, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&zoneCurlevel, "filterFlag", 2);
			extraInfo.PushBack(&zoneCurlevel);

			GFxValue zoneNeverReset;
			args->movie->movieRoot->CreateObject(&zoneNeverReset);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::ENCOUNTERZONEINFO_NEVERRESET.c_str(), (pZone->flags & BGSEncounterZone::kFlag_NeverResets) ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
			RegisterString(&zoneNeverReset, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&zoneNeverReset, "filterFlag", 2);
			extraInfo.PushBack(&zoneNeverReset);

			GFxValue zoneDisableBoundary;
			args->movie->movieRoot->CreateObject(&zoneDisableBoundary);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::ENCOUNTERZONEINFO_DISABLEBOUNDARY.c_str(), (pZone->flags & BGSEncounterZone::kFlag_DisableCombatBoundary) ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
			RegisterString(&zoneDisableBoundary, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&zoneDisableBoundary, "filterFlag", 2);
			extraInfo.PushBack(&zoneDisableBoundary);

			GFxValue zoneMatchPCLevel;
			args->movie->movieRoot->CreateObject(&zoneMatchPCLevel);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::ENCOUNTERZONEINFO_MATCHPCLEVEL.c_str(), (pZone->flags & BGSEncounterZone::kFlag_MatchPCBelowMinimumLevel) ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
			RegisterString(&zoneMatchPCLevel, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&zoneMatchPCLevel, "filterFlag", 2);
			extraInfo.PushBack(&zoneMatchPCLevel);

			GFxValue zoneWorkshop;
			args->movie->movieRoot->CreateObject(&zoneWorkshop);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::ENCOUNTERZONEINFO_ISWORKSHOP.c_str(), (pZone->flags & BGSEncounterZone::kFlag_Workshop) ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
			RegisterString(&zoneWorkshop, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&zoneWorkshop, "filterFlag", 2);
			extraInfo.PushBack(&zoneWorkshop);

			if (pZone->location != nullptr)
			{
				GFxValue zoneLocationID;
				args->movie->movieRoot->CreateObject(&zoneLocationID);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::ENCOUNTERZONEINFO_LOCATIONID.c_str(), pZone->location->formID);
				RegisterString(&zoneLocationID, args->movie->movieRoot, "text", sResult.get());
				Register<UInt32>(&zoneLocationID, "filterFlag", 2);
				extraInfo.PushBack(&zoneLocationID);
			}

			GFxValue zoneMod;
			args->movie->movieRoot->CreateObject(&zoneMod);
			auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(pZone);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_MOD.c_str(), (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL.c_str());
			RegisterString(&zoneMod, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&zoneMod, "filterFlag", 2);
			extraInfo.PushBack(&zoneMod);

			zone.SetMember("extraInfo", &extraInfo);
			args->result->PushBack(&zone);
		}

		GFxValue perks;
		args->movie->movieRoot->CreateObject(&perks);
		UInt32 perkCount = 0;
		CollectPerkData(perks, args->movie->movieRoot, actor, perkCount);
		sprintf_s(sResult.get(), MAX_PATH, "%s(%d) >", Settings::PERKINFO.c_str(), perkCount);
		RegisterString(&perks, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&perks, "filterFlag", 2);
		args->result->PushBack(&perks);

		GFxValue factions;
		args->movie->movieRoot->CreateObject(&factions);
		UInt32 factionCount = 0;
		CollectFactionData(factions, args->movie->movieRoot, actor, factionCount);
		sprintf_s(sResult.get(), MAX_PATH, "%s(%d) >", Settings::FACTIONINFO.c_str(), factionCount);
		RegisterString(&factions, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&factions, "filterFlag", 2);
		args->result->PushBack(&factions);

		GFxValue effects;
		args->movie->movieRoot->CreateObject(&effects);
		UInt32 effectCount = 0;
		CollectEffectData(effects, args->movie->movieRoot, actor, effectCount);
		sprintf_s(sResult.get(), MAX_PATH, "%s(%d) >", Settings::EFFECTINFO.c_str(), effectCount);
		RegisterString(&effects, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&effects, "filterFlag", 2);
		args->result->PushBack(&effects);

		GFxValue actorValues;
		UInt32 valueCount = 0;
		args->movie->movieRoot->CreateObject(&actorValues);
		CollectValueData(actorValues, args->movie->movieRoot, actor, valueCount);
		sprintf_s(sResult.get(), MAX_PATH, "%s(%d) >", Settings::AVINFO.c_str(), valueCount);
		RegisterString(&actorValues, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&actorValues, "filterFlag", 2);
		args->result->PushBack(&actorValues);

		GFxValue equipments;
		UInt32 equipmentCount = 0;
		args->movie->movieRoot->CreateObject(&equipments);
		CollectEquipmentData(equipments, args->movie->movieRoot, actor, equipmentCount);
		sprintf_s(sResult.get(), MAX_PATH, "%s(%d) >", Settings::EQUIPMENTINFO.c_str(), equipmentCount);
		RegisterString(&equipments, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&equipments, "filterFlag", 2);
		args->result->PushBack(&equipments);

		GFxValue memory;
		args->movie->movieRoot->CreateObject(&memory);
		sprintf_s(sResult.get(), MAX_PATH, "%s: 0x%I64X", Settings::EXTRAINFO_MEMORY.c_str(), (uintptr_t)pRef);
		RegisterString(&memory, args->movie->movieRoot, "text", sResult.get());
		Register<UInt32>(&memory, "filterFlag", 2);
		args->result->PushBack(&memory);
	}
	if (pRef != nullptr)
	{
		pRef->handleRefObject.DecRefHandle();
	}
}



