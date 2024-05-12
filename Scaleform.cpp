#include "F4SE_common/BranchTrampoline.h"
#include "F4SE/ScaleformMovie.h"
#include "F4SE/ScaleformValue.h"
#include "F4SE/ScaleformState.h"
#include "F4SE/ScaleformLoader.h"
#include "F4SE/ScaleformTranslator.h"
#include "F4SE/ScaleformCallbacks.h"
#include "F4SE/GameCamera.h"
#include "F4SE/GameMenus.h"
#include "f4se/GameHandle.h"
#include "HookUtil.h"

#include <memory>
#include <string>
#include <sstream>

#include "Scaleform.h"
#include "GameData.h"
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

#pragma warning (push)
#pragma warning (disable : 4200)
struct Runtime_RTTIType
{
	void	* typeInfo;
	UInt64	data;
	char	name[0];
};

struct Runtime_RTTILocator
{
	UInt32		sig, offset, cdOffset;
	UInt32		typeDesc;
	UInt32		classDesc;
};
#pragma warning (pop)


template<UInt64 VTBL>
const void * VTBL_TO_RTTI()
{
	Runtime_RTTILocator	** vtbl = reinterpret_cast<RTTILocator **>(VTBL);
	Runtime_RTTILocator	* rtti = vtbl[-1];
	UInt64 typeDesc = rtti->typeDesc;
	RelocPtr<Runtime_RTTIType> type(typeDesc);
	return type.GetPtr();
}

#define DEFINE_CLASS_VTABLE(className, offset)		\
	RelocAddr<uintptr_t> className##_vtbl(offset);

#define RELOC_CLASS_VTABLE(className, ...)			\
	className##_vtbl = sig_scan_processor("VTABLE_"#className, __VA_ARGS__).GetOffset(); 

#define GET_CLASS_VTABLE(className)			\
	className##_vtbl.GetUIntPtr()
	
RelocPtr<Runtime_RTTIType>			Runtime_RTTI_TESFullName(0x02D29EB0); // 02D29EB0
RelocPtr<Runtime_RTTIType>			Runtime_RTTI_TESForm(0x02D27678);

using _VisitPerks = void(*)(Actor*, HasPerkVisitor&);
RelocAddr<_VisitPerks> VisitPerks(0x0C0BC30); // sub_140C0BC30

using _CalcMagicEffect = void(*)(MagicItem*, MagicItem::EffectItem*, float& magnitude, float& duration);
RelocAddr<_CalcMagicEffect> CalcMagicEffect(0x0A96ED0);///V1.10 CalaPerkEffect in this funtion.BC2D70  //48 85 C9 0F 84 ? ? ? ? 48 89 5C 24 ? 55 56

using _GetRestoredHPPercent = float(*)(MagicItem*, MagicItem::EffectItem*);
RelocAddr<_GetRestoredHPPercent> GetRestoredHPPercent(0x1000DA0); ///V1.10.26 //48 8B C4 53 48 83 EC 40 0F 29 70 E8

using _GetModifiedValueName = void(*)(MagicItem::EffectItem*, MagicItem::EffectItem::Description&);
RelocAddr<_GetModifiedValueName> GetModifiedValueName(0x0ACF980); ///V1.10.26 C0AE60 //48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B F2 48 8B E9 48 8D 15 ? ? ? ?

using _GetActorValueHolder = ActorValueInfo**(*)();
RelocAddr<_GetActorValueHolder> GetActorValueCollector(0x020E9F0); //E8 ? ? ? ? 4C 8B 46 58 48 8D 4E 58 48 8B 90 ? ? ? ? 41 FF 50 08 0F 57 C9

using _GetChargeKW = BGSKeyword*(*)();
RelocAddr<_GetChargeKW> GetChargeKW(0x03200B0); /// sub_1403200B0

using _GeHealthtKW = BGSKeyword*(*)();
RelocAddr<_GeHealthtKW> GeHealthtKW(0x0320110); /// sub_140320110

//using _ReleaseSimpleColllector = void(*)(BGSInventoryItem::SimpleDataCollector&);
//RelocAddr<_ReleaseSimpleColllector>	ReleaseSimpleColllector(0x153F730); /// sub_14153F730

using _GetWeaponRange = float(*)(ObjectInstanceData&);
RelocAddr<_GetWeaponRange> GetWeaponRange(0x0679C30);/// sub_140679C30

using _GetWeaponAccuracy = float(*)(ObjectInstanceData&, Actor*);
RelocAddr<_GetWeaponAccuracy> GetWeaponAccuracy(0x0679D10); /// sub_140679D10

using _GetWeaponSpeedDesc = const char * (*)(UInt32);
RelocAddr<_GetWeaponSpeedDesc>	GetWeaponSpeedDesc(0x0425810); /// sub_140425810

using _GetStringFromHash = const char *(*)(UInt64 &);
RelocAddr<_GetStringFromHash> GetStringFromHash(0x02FCD00); /// sub_1402FCD00

using _CalcInstanceData = ObjectInstanceData *(*)(ObjectInstanceData & out, TESForm *, TBO_InstanceData *);
RelocAddr<_CalcInstanceData>	CalcInstanceData(0x03E29D0); /// sub_1403E29D0

using _RegisterMenuOpenCloseEvent = void(*)(BSTEventDispatcher<MenuOpenCloseEvent> & dispatcher, BSTEventSink<MenuOpenCloseEvent> * pHandler);
RelocAddr <_RegisterMenuOpenCloseEvent>	RegisterMenuOpenCloseEvent(0x0612530);// sub_140612530

//RelocAddr <float *>    fNewBatteryCapacity(0x3761848);
//
//RelocPtr <BGSKeyword *>	pSyringerAmmoKW(0x591B350);///V1.10.26 ObjectTypeSyringerAmmo [KYWD:0018506F] 48 8B 15 ? ? ? ? 49 8D 8D ? ? ? ? 4C 89 6C 24 ?

class BetterConsole_GetBaseData : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args * args)
	{
		// TESObjectREFR * pRef = nullptr;
		NiPointer<TESObjectREFR> pRef;
		LookupREFRByHandle(*g_consoleHandle, pRef);
		if (pRef != nullptr)
		{
			args->movie->movieRoot->CreateObject(args->result);
			std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);
			TESForm * pBaseForm = pRef->baseForm;
			BSExtraData * pExtraData = pRef->extraDataList->GetExtraData(kExtraData_LeveledCreature);
			if (pExtraData != nullptr)
			{
				auto * leveledBaseForm = *reinterpret_cast<TESForm**>((uintptr_t)pExtraData + 0x18);
				pBaseForm = (leveledBaseForm != nullptr) ? leveledBaseForm : pRef->baseForm;
			}
			sprintf_s(sResult.get(), MAX_PATH, "%08X", pRef->formID);
			RegisterString(args->result, args->movie->movieRoot, "refFormID", sResult.get());
			RegisterString(args->result, args->movie->movieRoot, "refFormType", FormTypes[pRef->formType].c_str());
			sprintf_s(sResult.get(), MAX_PATH, "%08X", pBaseForm->formID);
			RegisterString(args->result, args->movie->movieRoot, "baseFormID", sResult.get());
			RegisterString(args->result, args->movie->movieRoot, "baseFormType", FormTypes[pBaseForm->formType].c_str());

			const char * pRefName = pRef->GetReferenceName();
			auto pExtraTextData = static_cast<ExtraTextDisplayData*>(pRef->extraDataList->GetExtraData(kExtraData_TextDisplayData));
			if (pExtraTextData != nullptr)
			{
				pRefName = pExtraTextData->GetReferenceName(pBaseForm)->c_str();
			}
			if (pRefName && pRefName[0])
			{
				RegisterString(args->result, args->movie->movieRoot, "refName", pRefName);
			}
			TESFullName * pFullName = (TESFullName *)Runtime_DynamicCast_Internal(pBaseForm, 0, Runtime_RTTI_TESForm.GetPtr(), Runtime_RTTI_TESFullName.GetPtr(), 0);
			if (pFullName && strlen(pFullName->name.c_str()))
			{
				RegisterString(args->result, args->movie->movieRoot, "baseName", pFullName->GetFullName());
			}
			auto * pModInfo = (*g_dataHandler)->GetLocatedModInfo(pBaseForm);
			const char * baseDefineModName = (pModInfo != nullptr) ? pModInfo->name : "undefined";
			RegisterString(args->result, args->movie->movieRoot, "baseDefineModName", baseDefineModName);

			pModInfo = (*g_dataHandler)->GetLocatedModInfo(pRef);
			const char * refDefineModName = (pModInfo != nullptr) ? pModInfo->name : "undefined";
			RegisterString(args->result, args->movie->movieRoot, "refDefineModName", refDefineModName);

			sprintf_s(sResult.get(), MAX_PATH, "%016I64X", (uintptr_t)pRef.get());
			RegisterString(args->result, args->movie->movieRoot, "refMemory", sResult.get());

			pModInfo = (*g_dataHandler)->GetLastChangeModInfo(pBaseForm);

			RegisterString(args->result, args->movie->movieRoot, "baseLastChangeModName", (pModInfo != nullptr) ? pModInfo->name : "undefined");

			// pRef->handleRefObject.DecRefHandle();
		}
	}
};

class BetterConsole_GetExtraData : public GFxFunctionHandler
{
public:

	static void	CollectPositionData(GFxValue& object, GFxMovieRoot * root, TESObjectREFR *  pRef)
	{
		if (object.GetType() == GFxValue::kType_Object && root != nullptr)
		{
			GFxValue value;
			root->CreateArray(&value); //extraData
			std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);

			GFxValue posX;
			root->CreateObject(&posX);
			sprintf_s(sResult.get(), MAX_PATH, "%s: ", Settings::POSITIONINFO_POSX.c_str());
			RegisterString(&posX, root, "key", sResult.get());
			sprintf_s(sResult.get(), MAX_PATH, "%.2f", pRef->pos.x);
			RegisterString(&posX, root, "value", sResult.get());
			Register<UInt32>(&posX, "filterFlag", 2);
			value.PushBack(&posX);

			GFxValue posY;
			root->CreateObject(&posY);
			sprintf_s(sResult.get(), MAX_PATH, "%s: ", Settings::POSITIONINFO_POSY.c_str());
			RegisterString(&posY, root, "key", sResult.get());
			sprintf_s(sResult.get(), MAX_PATH, "%.2f", pRef->pos.y);
			RegisterString(&posY, root, "value", sResult.get());
			Register<UInt32>(&posY, "filterFlag", 2);
			value.PushBack(&posY);

			GFxValue posZ;
			root->CreateObject(&posZ);
			sprintf_s(sResult.get(), MAX_PATH, "%s: ", Settings::POSITIONINFO_POSZ.c_str());
			RegisterString(&posZ, root, "key", sResult.get());
			sprintf_s(sResult.get(), MAX_PATH, "%.2f", pRef->pos.z);
			RegisterString(&posZ, root, "value", sResult.get());
			Register<UInt32>(&posZ, "filterFlag", 2);
			value.PushBack(&posZ);

			GFxValue rotX;
			root->CreateObject(&rotX);
			sprintf_s(sResult.get(), MAX_PATH, "%s: ", Settings::POSITIONINFO_ROTX.c_str());
			RegisterString(&rotX, root, "key", sResult.get());
			sprintf_s(sResult.get(), MAX_PATH, "%.2f", pRef->rot.x);
			RegisterString(&rotX, root, "value", sResult.get());
			Register<UInt32>(&rotX, "filterFlag", 2);
			value.PushBack(&rotX);

			GFxValue rotY;
			root->CreateObject(&rotY);
			sprintf_s(sResult.get(), MAX_PATH, "%s: ", Settings::POSITIONINFO_ROTY.c_str());
			RegisterString(&rotY, root, "key", sResult.get());
			sprintf_s(sResult.get(), MAX_PATH, "%.2f", pRef->rot.y);
			RegisterString(&rotY, root, "value", sResult.get());
			Register<UInt32>(&rotY, "filterFlag", 2);
			value.PushBack(&rotY);

			GFxValue rotZ;
			root->CreateObject(&rotZ);
			sprintf_s(sResult.get(), MAX_PATH, "%s: ", Settings::POSITIONINFO_ROTZ.c_str());
			RegisterString(&rotZ, root, "key", sResult.get());
			sprintf_s(sResult.get(), MAX_PATH, "%.2f", pRef->rot.z);
			RegisterString(&rotZ, root, "value", sResult.get());
			Register<UInt32>(&rotZ, "filterFlag", 2);
			value.PushBack(&rotZ);

			object.SetMember("extraInfo", &value);
		}
	}

	static void	CollectPerkData(GFxValue& object, GFxMovieRoot * root, Actor *  actor, UInt32&	count)
	{
		if (object.GetType() == GFxValue::kType_Object && actor->formType == kFormType_ACHR && root != nullptr)
		{
			if (actor->middleProcess != nullptr)
			{
				HasPerkVisitor visitor;
				VisitPerks(actor, visitor);
				count = visitor.result.size();
				if (!count)	return;
				GFxValue value;
				root->CreateArray(&value); //extraData
				std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);
				for (auto& pEntry : visitor.result)
				{
					GFxValue perkData;
					root->CreateObject(&perkData);
					TESFullName * pFullName = (TESFullName *)Runtime_DynamicCast_Internal(pEntry.perk, 0, Runtime_RTTI_TESForm.GetPtr(), Runtime_RTTI_TESFullName.GetPtr(), 0);
					RegisterString(&perkData, root, "text", (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->GetFullName() : Settings::EXTRAINFO_NULL.c_str());
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

	static void	CollectFactionData(GFxValue& object, GFxMovieRoot * root, Actor *  actor, UInt32&	count)
	{
		if (object.GetType() == GFxValue::kType_Object && actor->formType == kFormType_ACHR && root != nullptr)
		{
			std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);
			std::map<TESFaction*, SInt8>	collector;
			BSExtraData * pExtraData = actor->extraDataList->GetExtraData(kExtraData_FactionChanges);
			if (pExtraData != nullptr)
			{
				auto * pFactionChanges = static_cast<ExtraFactionChanges*>(pExtraData);
				for (uintptr_t i = 0; i < pFactionChanges->factions.count; ++i)
				{
					const auto factionInfo = pFactionChanges->factions[i];
					collector.insert({ factionInfo.faction, factionInfo.rank });
				}
			}
			TESNPC * npc = static_cast<TESNPC*>(actor->baseForm);
			for (uintptr_t i = 0; i < npc->actorData.factions.count; ++i)
			{
				const auto factionInfo = npc->actorData.factions[i];
				collector.insert({ factionInfo.faction, factionInfo.rank });
			}

			count = collector.size();
			GFxValue value;
			root->CreateArray(&value); //extraData

			for (const auto& info : collector)
			{
				GFxValue factionData;
				root->CreateObject(&factionData);
				TESFullName * pFullName = (TESFullName *)Runtime_DynamicCast_Internal(info.first, 0, Runtime_RTTI_TESForm.GetPtr(), Runtime_RTTI_TESFullName.GetPtr(), 0);
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

	static void	CollectEffectData(GFxValue& object, GFxMovieRoot * root, Actor *  actor, UInt32&	count)
	{
		if (object.GetType() == GFxValue::kType_Object && actor->formType == kFormType_ACHR && root != nullptr)
		{
			auto * pMaigicTarget = &actor->magicTarget;
			BSSimpleList<ActiveEffect*>	* pEffectList = pMaigicTarget->GetEffectList();

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
						auto * pEffectSetting = pEffect->effect->mgef;
						if (pEffectSetting != nullptr)
						{
							++count;
							GFxValue effectData;
							root->CreateObject(&effectData);
							TESFullName * pFullName = (TESFullName *)Runtime_DynamicCast_Internal(pEffectSetting, 0, Runtime_RTTI_TESForm.GetPtr(), Runtime_RTTI_TESFullName.GetPtr(), 0);
							RegisterString(&effectData, root, "text", (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->GetFullName() : Settings::EXTRAINFO_NULL.c_str());
							if (!(pEffect->flags & ActiveEffect::kFlag_Inactive))
								Register<UInt32>(&effectData, "filterFlag", 2);
							else
								Register<UInt32>(&effectData, "filterFlag", 1);

							GFxValue extraInfo;
							root->CreateArray(&extraInfo); //extraData

							GFxValue effectName;
							root->CreateObject(&effectName);
							sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->GetFullName() : Settings::EXTRAINFO_NULL.c_str());
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
							sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EFFECTINFO_DETRIMENTAL.c_str(), (pEffectSetting->effectType & EffectSetting::kEffectType_Detrimental) ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
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
								pFullName = (TESFullName *)Runtime_DynamicCast_Internal(pEffect->item, 0, Runtime_RTTI_TESForm.GetPtr(), Runtime_RTTI_TESFullName.GetPtr(), 0);
								sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EFFECTINFO_SPELLNAME.c_str(), (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->GetFullName() : Settings::EXTRAINFO_NULL.c_str());
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

	static void	CollectValueData(GFxValue& object, GFxMovieRoot * root, Actor *  actor, UInt32&	count)
	{
		if (object.GetType() == GFxValue::kType_Object && actor->formType == kFormType_ACHR && root != nullptr)
		{
			std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);

			struct AVInfo
			{
				ActorValueInfo		* pAVI;
				float				base;
				float				current;
			};
			std::map<std::string, AVInfo> actorValueDict;

			for (uint32_t i = 0; i < actor->actorValueData.count; ++i)
			{
				auto data = actor->actorValueData[i];
				TESForm * pForm = LookupFormByID(data.avFormID);
				if (pForm != nullptr && pForm->formType == kFormType_AVIF)
				{
					auto * pAVI = static_cast<ActorValueInfo*>(pForm);
					float currentValue = actor->actorValueOwner.GetValue(pAVI);
					if (data.value != 0.0f || currentValue != 0.0f)
					{
						const char * actorValueName = pAVI->GetDisplayName();
						if (!actorValueName || !actorValueName[0]) {
							actorValueName = pAVI->avName;
						}
						if (!actorValueName || !actorValueName[0]) {
							actorValueName = pAVI->GetFullName();
						}
						if (!actorValueName || !actorValueName[0]) {
							continue;
						}
						//if (!actorValueName || !actorValueName[0]) {
						//	static char formId[9] = { 0 }; 
						//	sprintf_s(formId, "%08X", pForm->formID); // 使用 sprintf_s 格式化字符串并补零
						//	actorValueName = formId;
						//}
						if (nullptr != actorValueName) {
							actorValueDict.insert({ actorValueName,{ pAVI, data.value, currentValue } });
						}
					}
				}
			}
			if (!actorValueDict.size())
				return;

			GFxValue value;
			root->CreateArray(&value); //extraData

									   //sort by string.A-Z
			for (const auto& map : actorValueDict)
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

	static void	ColllectInventoryData(GFxValue& arrObj, GFxMovieRoot * root, Actor * actor, TESForm * pBaseForm)
	{
		auto pInvList = actor->inventoryList;
		std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);
		bool result = false;
		if (pInvList != nullptr)
		{
			BSReadLocker inventoryLocker(&pInvList->inventoryLock);

			for (size_t i = 0; i < pInvList->items.count; ++i)
			{
				BGSInventoryItem & item = pInvList->items[i];
				if (item.form != pBaseForm)	continue;
				result = true;
				if (item.stack != nullptr)
				{
					item.stack->Visit([&](BGSInventoryItem::Stack * stack)->bool
					{
						if (!(stack->flags & BGSInventoryItem::Stack::kFlagEquipped))
							return true;

						ExtraDataList * pExtraDataList = stack->extraData;
						TBO_InstanceData * pExtraInstanceData = nullptr;
						const char * pName = nullptr;
						if (pExtraDataList != nullptr)
						{
							pExtraInstanceData = pExtraDataList->GetExtraInstanceData();
							auto * pExtraName = static_cast<ExtraTextDisplayData*>(pExtraDataList->GetExtraData(kExtraData_TextDisplayData));
							pName = (pExtraName != nullptr) ? pExtraName->GetReferenceName(item.form)->c_str() : nullptr;
						}
						if (!pName)
						{
							TESFullName * pFullName = (TESFullName *)Runtime_DynamicCast_Internal(pBaseForm, 0, Runtime_RTTI_TESForm.GetPtr(), Runtime_RTTI_TESFullName.GetPtr(), 0);
							pName = (pFullName != nullptr) ? pFullName->GetFullName() : nullptr;
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

						ObjectInstanceData	objInst{ nullptr, nullptr };
						CalcInstanceData(objInst, pBaseForm, pExtraInstanceData);

						switch (pBaseForm->formType)
						{
						case FormType::kFormType_ARMO:
						{
							TESObjectARMO * pArmor = static_cast<TESObjectARMO*>(pBaseForm);
							auto * pArmorInst = (TESObjectARMO::InstanceData*)((objInst.data != nullptr) ? objInst.data : pArmor->GetInstanceData());///V1.10
							BGSInventoryItem::SimpleDataCollector resistInfo;
							item.CollectResistData(stack, resistInfo, 1.0f);
							for (size_t i = 0; i < resistInfo.count; ++i)
							{
								if (resistInfo.data[i].value != 0.0f)
								{
									GFxValue itemResist;
									root->CreateObject(&itemResist);
									sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::RESISTTYPES[resistInfo.data[i].type].c_str(), resistInfo.data[i].value);
									RegisterString(&itemResist, root, "text", sResult.get());
									Register<UInt32>(&itemResist, "filterFlag", 2);
									arrObj.PushBack(&itemResist);
								}
							}
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
							auto * pWeaponInst = (TESObjectWEAP::InstanceData*)((objInst.data != nullptr) ? objInst.data : pWeapon->GetInstanceData()); //V1.10.26

							if (pWeaponInst != nullptr) {
								GFxValue weaponType;
								root->CreateObject(&weaponType);
								sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EQUIPMENTINFO_CLASS.c_str(), (pWeaponInst->weaponType > TESObjectWEAP::InstanceData::kWeaponType_Mine) ? WeaponTypes[WeaponTypes.size() - 1].c_str() : WeaponTypes[pWeaponInst->weaponType].c_str());
								RegisterString(&weaponType, root, "text", sResult.get());
								Register<UInt32>(&weaponType, "filterFlag", 2);
								arrObj.PushBack(&weaponType);

								TESAmmo * pAmmo = (pWeaponInst != nullptr) ? pWeaponInst->ammo : pWeapon->weapData.ammo;
								if (pAmmo != nullptr)
								{
									bool isJunk = pAmmo->IsJunk();
									GFxValue itemAmmo;
									root->CreateObject(&itemAmmo);
									sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EQUIPMENTINFO_AMMO.c_str(), GetStringFromHash(pAmmo->crcString));
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

									float range = GetWeaponRange(objInst);

									GFxValue itemRange;
									root->CreateObject(&itemRange);
									sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::EQUIPMENTINFO_RANGE.c_str(), range);
									RegisterString(&itemRange, root, "text", sResult.get());
									Register<UInt32>(&itemRange, "filterFlag", 2);
									arrObj.PushBack(&itemRange);

									float accuracy = GetWeaponAccuracy(objInst, actor);

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
									GFxValue itemSpeed;
									root->CreateObject(&itemSpeed);
									sprintf_s(sResult.get(), MAX_PATH, "%s: %d", Settings::EQUIPMENTINFO_SPEED.c_str(), attackSpeed);
									RegisterString(&itemSpeed, root, "text", sResult.get());
									Register<UInt32>(&itemSpeed, "filterFlag", 2);
									arrObj.PushBack(&itemSpeed);
								}
								BGSInventoryItem::SimpleDataCollector damageInfo;
								item.CollectDamageData(stack, damageInfo);
								for (size_t i = 0; i < damageInfo.count; ++i)
								{
									if (damageInfo.data[i].value != 0.0f)
									{
										GFxValue itemDamage;
										root->CreateObject(&itemDamage);
										sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::DAMAGETYPES[damageInfo.data[i].type].c_str(), damageInfo.data[i].value);
										RegisterString(&itemDamage, root, "text", sResult.get());
										Register<UInt32>(&itemDamage, "filterFlag", 2);
										arrObj.PushBack(&itemDamage);
									}
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
		}
		if (!result)
		{
			TESFullName * pFullName = (TESFullName *)Runtime_DynamicCast_Internal(pBaseForm, 0, Runtime_RTTI_TESForm.GetPtr(), Runtime_RTTI_TESFullName.GetPtr(), 0);

			GFxValue itemName;
			root->CreateObject(&itemName);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->GetFullName() : Settings::EXTRAINFO_NULL.c_str());
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

	static void	CollectEquipmentData(GFxValue& object, GFxMovieRoot * root, Actor * actor, UInt32& count)
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
				GFxValue pExtraInfo;
				root->CreateArray(&pExtraInfo);
				for (size_t slotIndex = 0; slotIndex < ActorEquipData::kMaxSlots; ++slotIndex)
				{
					auto * pForm = equipData->slots[slotIndex].item;
					if (pForm != nullptr)
					{
						++count;
						GFxValue	equipmentData;
						root->CreateObject(&equipmentData);
						TESFullName* pFullName = (TESFullName *)Runtime_DynamicCast_Internal(pForm, 0, Runtime_RTTI_TESForm.GetPtr(), Runtime_RTTI_TESFullName.GetPtr(), 0);
						sprintf_s(sResult.get(), MAX_PATH, "%s(%s)", (pFullName != nullptr) ? pFullName->name.c_str() : "", FormTypes[pForm->formType].c_str());
						RegisterString(&equipmentData, root, "text", sResult.get());
						Register<UInt32>(&equipmentData, "filterFlag", 2);

						GFxValue extraInfo;
						root->CreateArray(&extraInfo); //extraData

						if (pForm->formType == FormType::kFormType_ARMA)
						{
							GFxValue equipmentName;
							root->CreateObject(&equipmentName);
							sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->GetFullName() : Settings::EXTRAINFO_NULL.c_str());
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
						pExtraInfo.PushBack(&equipmentData);
					}
				}
				if (pExtraInfo.GetArraySize())
				{
					object.SetMember("extraInfo", &pExtraInfo);
				}
			}
		}
	}

	virtual void Invoke(Args * args)
	{
		NiPointer<TESObjectREFR> pRef;
		LookupREFRByHandle(*g_consoleHandle, pRef);
		if (pRef != nullptr)
		{
			args->movie->movieRoot->CreateArray(args->result);
			std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);

			GFxValue name;
			args->movie->movieRoot->CreateObject(&name);
			const char * pRefName = pRef->GetReferenceName();
			sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), (pRefName != nullptr && strlen(pRefName)) ? pRefName : Settings::EXTRAINFO_NULL.c_str());
			RegisterString(&name, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&name, "filterFlag", 2);
			args->result->PushBack(&name);

			GFxValue refID;
			args->movie->movieRoot->CreateObject(&refID);
			sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EXTRAINFO_ID.c_str(), pRef->formID);
			RegisterString(&refID, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&refID, "filterFlag", 2);
			args->result->PushBack(&refID);

			TESForm* pBaseForm = pRef->baseForm;
			BSExtraData* pExtraData = pRef->extraDataList->GetExtraData(kExtraData_LeveledCreature);
			if (pExtraData != nullptr)
			{
				auto * leveledBaseForm = *reinterpret_cast<TESForm**>((uintptr_t)pExtraData + 0x18);
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

			if (pRef->formType == kFormType_ACHR)
			{
				Actor * actor = static_cast<Actor*>(pRef.get());
				TESNPC * npc = static_cast<TESNPC*>(pBaseForm);
				TESFullName * pFullName = (TESFullName *)Runtime_DynamicCast_Internal(actor->race, 0, Runtime_RTTI_TESForm.GetPtr(), Runtime_RTTI_TESFullName.GetPtr(), 0);
				if (pFullName != nullptr && strlen(pFullName->name.c_str()))
				{
					GFxValue race;
					args->movie->movieRoot->CreateObject(&race);
					sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_RACE.c_str(), (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->GetFullName() : Settings::EXTRAINFO_NULL.c_str());
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
				auto * pAVs = GetActorValueCollector();
				ActorValueInfo*	pHealth = *reinterpret_cast<ActorValueInfo**>(pAVs + 0x1B);
				sprintf_s(sResult.get(), MAX_PATH, "HP: %d", static_cast<UInt32>(actor->actorValueOwner.GetValue(pHealth)));
				RegisterString(&health, args->movie->movieRoot, "text", sResult.get());
				Register<UInt32>(&health, "filterFlag", 2);
				args->result->PushBack(&health);

				TESForm * pForm = LookupFormByID(0x2D5); //action points
				if (pForm != nullptr && pForm->formType == kFormType_AVIF)
				{
					auto * pAV = static_cast<ActorValueInfo*>(pForm);
					GFxValue AP;
					args->movie->movieRoot->CreateObject(&AP);
					sprintf_s(sResult.get(), MAX_PATH, "AP: %d", static_cast<UInt32>(actor->actorValueOwner.GetValue(pAV)));
					RegisterString(&AP, args->movie->movieRoot, "text", sResult.get());
					Register<UInt32>(&AP, "filterFlag", 2);
					args->result->PushBack(&AP);
				}

				GFxValue carryWeight;
				args->movie->movieRoot->CreateObject(&carryWeight);
				float fCarryWeight = pRef->GetCarryWeight();
				sprintf_s(sResult.get(), MAX_PATH, "%s: %.2f", Settings::EXTRAINFO_CARRYWEIGHT.c_str(), fCarryWeight);
				RegisterString(&carryWeight, args->movie->movieRoot, "text", sResult.get());
				Register<UInt32>(&carryWeight, "filterFlag", 2);
				args->result->PushBack(&carryWeight);
			}

			if (pRef->formType == kFormType_ACHR)
			{
				TESNPC * npc = static_cast<TESNPC*>(pBaseForm);

				GFxValue flags;
				args->movie->movieRoot->CreateObject(&flags);
				sprintf_s(sResult.get(), MAX_PATH, "%s >", Settings::FLAGINFO.c_str());
				RegisterString(&flags, args->movie->movieRoot, "text", sResult.get());
				Register<UInt32>(&flags, "filterFlag", 2);

				GFxValue extraInfo;
				args->movie->movieRoot->CreateArray(&extraInfo);

				GFxValue isUnique;
				args->movie->movieRoot->CreateObject(&isUnique);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::FLAGINFO_UNIQUE.c_str(), npc->actorData.flags.unique ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
				RegisterString(&isUnique, args->movie->movieRoot, "text", sResult.get());
				Register<UInt32>(&isUnique, "filterFlag", 2);
				extraInfo.PushBack(&isUnique);

				GFxValue isEssential;
				args->movie->movieRoot->CreateObject(&isEssential);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::FLAGINFO_ESSENTIAL.c_str(), npc->actorData.flags.essential ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
				RegisterString(&isEssential, args->movie->movieRoot, "text", sResult.get());
				Register<UInt32>(&isEssential, "filterFlag", 2);
				extraInfo.PushBack(&isEssential);

				GFxValue isProtected;
				args->movie->movieRoot->CreateObject(&isProtected);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::FLAGINFO_PROTECTED.c_str(), npc->actorData.flags.protect ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
				RegisterString(&isProtected, args->movie->movieRoot, "text", sResult.get());
				Register<UInt32>(&isProtected, "filterFlag", 2);
				extraInfo.PushBack(&isProtected);

				GFxValue isInvulnerable;
				args->movie->movieRoot->CreateObject(&isInvulnerable);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::FLAGINFO_INVULNERABLE.c_str(), npc->actorData.flags.invulnerable ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
				RegisterString(&isInvulnerable, args->movie->movieRoot, "text", sResult.get());
				Register<UInt32>(&isInvulnerable, "filterFlag", 2);
				extraInfo.PushBack(&isInvulnerable);

				flags.SetMember("extraInfo", &extraInfo);
				args->result->PushBack(&flags);
			}


			GFxValue position;
			args->movie->movieRoot->CreateObject(&position);
			sprintf_s(sResult.get(), MAX_PATH, "%s >", Settings::POSITIONINFO.c_str());
			RegisterString(&position, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&position, "filterFlag", 2);
			CollectPositionData(position, args->movie->movieRoot, pRef);
			args->result->PushBack(&position);

			TESObjectCELL * pCell = pRef->parentCell;  //extra
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
				sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), strlen(pCell->fullName.name.c_str()) ? pCell->GetFullName() : Settings::EXTRAINFO_NULL.c_str());
				RegisterString(&cellName, args->movie->movieRoot, "text", sResult.get());
				Register<UInt32>(&cellName, "filterFlag", 2);
				extraInfo.PushBack(&cellName);

				GFxValue cellID;
				args->movie->movieRoot->CreateObject(&cellID);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %08X", Settings::EXTRAINFO_ID.c_str(), pCell->formID);
				RegisterString(&cellID, args->movie->movieRoot, "text", sResult.get());
				Register<UInt32>(&cellID, "filterFlag", 2);
				extraInfo.PushBack(&cellID);

				GFxValue cellIsInterior;
				args->movie->movieRoot->CreateObject(&cellIsInterior);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::CELLINFO_ISINTERIOR.c_str(), (pCell->flags_2 & TESObjectCELL::kFlag_IsInterior) ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
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

			auto * pLocation = pRef->GetCurrentLocation(); //extra V1.10.20
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
				sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), strlen(pLocation->fullName.name.c_str()) ? pLocation->GetFullName() : Settings::EXTRAINFO_NULL.c_str());
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

			auto * pWorld = pRef->GetWorldspace();  //extra
			if (pWorld != nullptr)
			{
				TESFullName* pFullName = (TESFullName *)Runtime_DynamicCast_Internal(pWorld, 0, Runtime_RTTI_TESForm.GetPtr(), Runtime_RTTI_TESFullName.GetPtr(), 0);

				GFxValue worldSpace;
				args->movie->movieRoot->CreateObject(&worldSpace);
				sprintf_s(sResult.get(), MAX_PATH, "%s >", Settings::WORLDSPACEINFO.c_str());
				RegisterString(&worldSpace, args->movie->movieRoot, "text", sResult.get());
				Register<UInt32>(&worldSpace, "filterFlag", 2);

				GFxValue extraInfo;
				args->movie->movieRoot->CreateArray(&extraInfo);

				GFxValue worldName;
				args->movie->movieRoot->CreateObject(&worldName);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::EXTRAINFO_NAME.c_str(), strlen(pFullName->name.c_str()) ? pFullName->GetFullName() : Settings::EXTRAINFO_NULL.c_str());
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

			auto * pZone = pRef->GetEncounterZone();
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
				sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::ENCOUNTERZONEINFO_NEVERRESET.c_str(), (pZone->flags_2 & BGSEncounterZone::kFlag_NeverResets) ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
				RegisterString(&zoneNeverReset, args->movie->movieRoot, "text", sResult.get());
				Register<UInt32>(&zoneNeverReset, "filterFlag", 2);
				extraInfo.PushBack(&zoneNeverReset);

				GFxValue zoneDisableBoundary;
				args->movie->movieRoot->CreateObject(&zoneDisableBoundary);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::ENCOUNTERZONEINFO_DISABLEBOUNDARY.c_str(), (pZone->flags_2 & BGSEncounterZone::kFlag_DisableCombatBoundary) ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
				RegisterString(&zoneDisableBoundary, args->movie->movieRoot, "text", sResult.get());
				Register<UInt32>(&zoneDisableBoundary, "filterFlag", 2);
				extraInfo.PushBack(&zoneDisableBoundary);

				GFxValue zoneMatchPCLevel;
				args->movie->movieRoot->CreateObject(&zoneMatchPCLevel);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::ENCOUNTERZONEINFO_MATCHPCLEVEL.c_str(), (pZone->flags_2 & BGSEncounterZone::kFlag_MatchPCBelowMinimumLevel) ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
				RegisterString(&zoneMatchPCLevel, args->movie->movieRoot, "text", sResult.get());
				Register<UInt32>(&zoneMatchPCLevel, "filterFlag", 2);
				extraInfo.PushBack(&zoneMatchPCLevel);

				GFxValue zoneWorkshop;
				args->movie->movieRoot->CreateObject(&zoneWorkshop);
				sprintf_s(sResult.get(), MAX_PATH, "%s: %s", Settings::ENCOUNTERZONEINFO_ISWORKSHOP.c_str(), (pZone->flags_2 & BGSEncounterZone::kFlag_Workshop) ? Settings::EXTRAINFO_TRUE.c_str() : Settings::EXTRAINFO_FALSE.c_str());
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

			if (pRef->formType == kFormType_ACHR)
			{
				Actor * actor = static_cast<Actor*>(pRef.get());
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

			}

			GFxValue memory;
			args->movie->movieRoot->CreateObject(&memory);
			sprintf_s(sResult.get(), MAX_PATH, "%s: 0x%I64X", Settings::EXTRAINFO_MEMORY.c_str(), (uintptr_t)pRef.get());
			RegisterString(&memory, args->movie->movieRoot, "text", sResult.get());
			Register<UInt32>(&memory, "filterFlag", 2);
			args->result->PushBack(&memory);
		}
	}
};
												   //0xF0
class MenuOpenCloseHandler : public BSTEventSink<MenuOpenCloseEvent>
{
public:
	virtual ~MenuOpenCloseHandler() { };
	virtual	EventResult	ReceiveEvent(MenuOpenCloseEvent * evn, void * dispatcher) override
	{
		static BSFixedString console("Console");
		if (evn->menuName == console && evn->isOpen)
		{
			GFxValue dispatchEvent;
			GFxValue eventArgs[3];
			IMenu * pConsole = (*g_ui)->GetMenu(console);
			auto * movieRoot = pConsole->movie->movieRoot;
			movieRoot->CreateString(&eventArgs[0], "OnConsoleOpen");
			eventArgs[1].SetBool(true);
			eventArgs[2].SetBool(false);
			movieRoot->CreateObject(&dispatchEvent, "flash.events.Event", eventArgs, 3);
			movieRoot->Invoke("root.dispatchEvent", nullptr, &dispatchEvent, 1);
		}
		return kEvent_Continue;
	};

	static void Register()
	{
		if ((*g_ui) != nullptr)
		{
			static auto * pHandler = new MenuOpenCloseHandler();
			RegisterMenuOpenCloseEvent((*g_ui)->menuOpenCloseEventSource, pHandler);
		}
	}
};

class Console : public GameMenuBase
{
public:
	using	FnPickNext = bool(__thiscall Console::*)(void* pData);
	static	FnPickNext	 PickNext_Original;

	bool	PickNext_Hook(void * pData)
	{
		GFxValue result(true);
		if (stage.IsDisplayObject())
		{
			stage.Invoke("canScrollMouse", &result, nullptr, 0);
		}
		if (!result.GetBool())
		{
			return false;
		}
		return (this->*PickNext_Original)(pData);
	}

	static const char * GetReferenceName(TESObjectREFR * pRef)
	{
		return pRef->GetReferenceName();
	}

	static void InitHook()
	{
		PickNext_Original = HookUtil::SafeWrite64(RELOC_RUNTIME_ADDR("48 8D 05 ? ? ? ? 48 89 06 4C 8D 46 ? 33 C9", 0, 3, 7) + 8 * 0xF, &PickNext_Hook); //V1.10.26 //48 8D 05 ? ? ? ? BD ? ? ? ? F3 0F 11 44 24 ? //02DAE938
		g_branchTrampoline.Write6Call(RELOC_RUNTIME_ADDR("FF 90 ? ? ? ? 48 89 74 24 ? 4C 8D 05 ? ? ? ?"), (uintptr_t)GetReferenceName); //FF 90 ? ? ? ? 48 89 74 24 ? 4C 89 7C 24 ?
	}
};
Console::FnPickNext			Console::PickNext_Original = nullptr;


void F4SEMessageHandler(F4SEMessagingInterface::Message* msg)
{
	if (msg->type == F4SEMessagingInterface::kMessage_GameLoaded)
	{
		MenuOpenCloseHandler::Register();
	}
}

bool ScaleformCallback(GFxMovieView * view, GFxValue * value)
{
	GFxMovieRoot * movieRoot = view->movieRoot;
	if (movieRoot)
	{
		GFxValue loaderInfo;
		if (movieRoot->GetVariable(&loaderInfo, "root.loaderInfo.url"))
		{
			std::string sResult = loaderInfo.GetString();
			if (sResult.find("Console.swf") != std::string::npos)
			{
				RegisterFunction<BetterConsole_GetBaseData>(value, view->movieRoot, "GetBaseData");
				RegisterFunction<BetterConsole_GetExtraData>(value, view->movieRoot, "GetExtraData");
			}
		}
	}
	return true;
}



void InitRuntimeData()
{
	RELOC_GLOBAL_VAL(Runtime_RTTI_TESFullName, "4C 8D 0D ? ? ? ? 4C 8D 05 ? ? ? ? 89 7C 24 ? 33 D2 48 8B D9", 0, 3, 7); // 1.10.980

	RELOC_GLOBAL_VAL(Runtime_RTTI_TESForm, "4C 8D 05 ? ? ? ? 89 7C 24 ? 33 D2 48 8B D9", 0, 3, 7); // 1.10.980
	RELOC_GLOBAL_VAL(Runtime_DynamicCast_Internal, "E8 ? ? ? ? 48 8B F8 48 85 C0 0F 84 ? ? ? ? 48 8B 80 ? ? ? ?", 0, 1, 5);// 1.10.980

	RELOC_MEMBER_FN(BSFixedString, ctor, "E8 ? ? ? ? 48 8B 55 ? 48 8D 4D ? 49 8B 5D ? 45 33 C0 49 8B FE", 0, 1, 5); // 1.10.980
	RELOC_MEMBER_FN(BSFixedString, Set, "E8 ? ? ? ? E9 ? ? ? ? 33 DB 48 8D 54 24 ? 45 8B C1", 0, 1, 5); // 1.10.980
	RELOC_MEMBER_FN(BSFixedString, Release, "E8 ? ? ? ? 48 8B 1E 8B 43 ? C1 E8 ? A8 ? 75 ?", 0, 1, 5); // 1.10.980
	RELOC_MEMBER_FN(BSFixedString, ctor_w, "E8 ? ? ? ? 48 8B 7D ? 48 8D 15 ? ? ? ?", 0, 1, 5); // 1.10.980
	RELOC_MEMBER_FN(BSFixedString, Set_w, "48 81 EC ? ? ? ? 41 0F B6 F0 4C 8B EA E8 ? ? ? ? 41 0F B7 4D ?", -0x1B); // 1.10.980

	RELOC_MEMBER_FN(Heap, Allocate, "E8 ? ? ? ? 4C 8B D0 48 89 05 ? ? ? ? 48 F7 D8 41 8B C5", 0, 1, 5); // 1.10.980
	RELOC_MEMBER_FN(Heap, Free, "E8 ? ? ? ? 45 89 7D ? 41 8B EF", 0, 1, 5); // 1.10.980

	RELOC_MEMBER_FN(GFxValue::ObjectInterface, SetMember, "4C 8D 4D ? 3C ? 4C 8D 05 ? ? ? ?", 0x14, 1, 5); // new
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, HasMember, "E8 ? ? ? ? 84 C0 74 ? 49 8B CF E8 ? ? ? ? 8B 4B ?", 0, 1, 5); // new
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, GetMember, "E8 ? ? ? ? 84 C0 74 ? 48 8B 55 ? 44 8B CB", 0, 1, 5); // new
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, Invoke, "E8 ? ? ? ? EB ? 8B 07 39 05 ? ? ? ?", 0, 1, 5); // new
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, PushBack, "40 53 48 83 EC ? 48 8B 41 ? 4D 8B C8 48 8B DA 4C 8D 44 24 ? 49 8B D1"); // new
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, GetArraySize, "E8 ? ? ? ? 3B F8 0F 82 ? ? ? ? 4C 8D 9C 24 ? ? ? ?", 0, 1, 5);
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, AddManaged_Internal, "E8 ? ? ? ? 48 8D 44 24 ? EB ?", 0, 1, 5);
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, ReleaseManaged_Internal, "E8 ? ? ? ? 48 C7 03 ? ? ? ? 8B 46 ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(GFxValue::ObjectInterface, SetMember, "4C 8D 4D ? 3C ? 4C 8D 05 ? ? ? ?", 0x14, 1, 5); // new
	RELOC_MEMBER_FN_EX(GFxValue::ObjectInterface, AddManaged_Internal, "E8 ? ? ? ? 48 8D 44 24 ? EB ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(GFxValue::ObjectInterface, ReleaseManaged_Internal, "E8 ? ? ? ? 48 C7 03 ? ? ? ? 8B 46 ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(UI, IsMenuOpen, "E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 48 8B 3D ? ? ? ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(ExtraDataList, GetExtraData, "E8 ? ? ? ? 48 85 C0 74 ? 4C 8B 70 ? EB ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(ExtraDataList, GetExtraInstanceData, "E8 ? ? ? ? 48 89 44 24 ? 48 8B D8 48 85 C0 75 ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(ExtraDataList, GetCurrentHealth, "48 8B DA 48 8B F9 E8 ? ? ? ? 48 8B D0 48 8B CB", -0xA);
	RELOC_MEMBER_FN_EX(ExtraDataList, GetCharge, "E8 ? ? ? ? 0F 28 F0 48 8B CD F3 0F 5F 35 ? ? ? ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(ExtraDataList, GetEffectiveMod, "48 83 EC ? B2 ? 48 C7 44 24 ? ? ? ? ?"); //  sub_1409ED630 item card logic
	RELOC_MEMBER_FN_EX(ExtraDataList, GetExtraEnchantmentItem, "E8 ? ? ? ? 48 8B D0 48 8D 8D ? ? ? ? E8 ? ? ? ? 48 8B 4D ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(ExtraTextDisplayData, GetReferenceName, "48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC ? 48 8D 79 ?");

	RELOC_MEMBER_FN(TESObjectREFR, GetReferenceName, "48 8B E9 0F 29 74 24 ? 48 8B 89 ? ? ? ?", -0x12);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetReferenceName, "48 8B E9 0F 29 74 24 ? 48 8B 89 ? ? ? ?", -0x12);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetCurrentLocation, "83 79 ? ? 48 8B D9 75 ? 48 8B 05 ? ? ? ?", -0x6);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetEncounterZone, "E8 ? ? ? ? 48 85 C0 75 ? 49 8B 86 ? ? ? ? 48 85 C0", 0, 1, 5);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetWorldspace, "48 83 EC ? 48 8B 81 ? ? ? ? 48 85 C0 75 ? 48 8B 01");

	RELOC_MEMBER_FN_EX(TESObjectREFR, GetInventoryWeight, "E8 ? ? ? ? 0F 2F C6 41 0F 43 DC", 0, 1, 5);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetCarryWeight, "E8 ? ? ? ? F3 0F 5D C6 48 8B 74 24 ?", 0, 1, 5);

	//DEFINE_MEMBER_FN(GetInventoryWeight, float, 0x004B3B20);
	//DEFINE_MEMBER_FN(GetCarryWeight, float, 0x00BEACF0);

	RELOC_MEMBER_FN_EX(SimpleLock, LockThis, "E8 ? ? ? ? 41 8B 16 48 8D 4C 24 ? 45 33 C0 33 F6", 0, 1, 5);
	RELOC_MEMBER_FN_EX(SimpleLock, ReleaseThis, "E8 ? ? ? ? 41 0F B6 C6 48 83 C4 ? 41 5E 5F C3", 0, 1, 5);
	RELOC_MEMBER_FN_EX(BSReadWriteLock, LockForRead, "E8 ? ? ? ? 41 8B 16 48 8D 4C 24 ? 45 33 C0 33 F6", 0, 1, 5);
	RELOC_MEMBER_FN_EX(BSReadWriteLock, UnlockRead, "E8 ? ? ? ? 41 0F B6 C6 48 83 C4 ? 41 5E 5F C3", 0, 1, 5);
	RELOC_MEMBER_FN_EX(BSReadWriteLock, LockForWrite, "E8 ? ? ? ? 48 8B 85 ? ? ? ? 48 8D 4D ? 48 83 C0 ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(BSReadWriteLock, UnlockWrite, "E8 ? ? ? ? 48 8B 44 24 ? 48 8B 7C 24 ? 48 8B 5C 24 ?", 0, 1, 5);


	RELOC_MEMBER_FN_EX(ActorValueInfo, GetDisplayName, "E8 ? ? ? ? 48 8B F0 48 85 C0 0F 84 ? ? ? ? 66 41 39 7E ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(BGSInventoryItem, CollectDamageData, "E8 ? ? ? ? 44 8B 85 ? ? ? ? 41 0F 28 F2", 0, 1, 5);
	RELOC_MEMBER_FN_EX(BGSInventoryItem, CollectResistData, "48 8B C4 F3 0F 11 58 ? 56");
	RELOC_MEMBER_FN_EX(BGSInventoryItem::SimpleDataCollector, Release, "40 53 48 83 EC ? 48 8B 51 ? 48 8B D9 48 85 D2 74 ? 48 8B 09 E8 ? ? ? ?");

	RELOC_MEMBER_FN_EX(TESDescription, Get, "E8 ? ? ? ? 48 8B 93 ? ? ? ? 48 8D 45 ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(TESObjectWEAP, GetShootSpeed, "E8 ? ? ? ? 0F 28 F8 48 85 FF 74 ? 48 85 DB", 0, 1, 5);
	RELOC_MEMBER_FN_EX(TESObjectWEAP, GetAttackSpeed, "E8 ? ? ? ? 8B D8 48 85 FF 74 ? 48 8B CF", 0, 1, 5);

	RELOC_MEMBER_FN_EX(TESObjectARMO, GetFullHealth, "E8 ? ? ? ? 85 C0 74 ? F3 0F 10 4F ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(TESAmmo, IsJunk, "E8 ? ? ? ? 84 C0 74 ? 41 8B F6", 0, 1, 5);

	RELOC_MEMBER_FN_EX(Actor, GetItemCount, "40 53 48 83 EC ? 48 8B DA 48 85 D2 74 ? 48 8B 81 ? ? ? ?");
	RELOC_MEMBER_FN_EX(Actor, HasPerk, "E8 ? ? ? ? 48 8B 0D ? ? ? ? 45 33 C0 48 8B D5", 0, 1, 5);
	RELOC_MEMBER_FN_EX(Actor, GetLevel, "E8 ? ? ? ? 0F B7 D0 F3 0F 2C 05 ? ? ? ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(TESForm, GetWeight, "40 53 48 83 EC ? 0F 29 74 24 ? 48 8B D9 48 85 D2 74 ? 48 8B 02");
	RELOC_MEMBER_FN_EX(TESForm, GetValue, "E8 ? ? ? ? 40 84 ED 74 ? 0F AF 43 ?", 0, 1, 5);

	RELOC_GLOBAL_VAL(CalculateCRC32_64, "48 89 5C 24 ? 48 89 7C 24 ? 41 8B C0 48 8D 3D ? ? ? ?"); // new
	RELOC_GLOBAL_VAL(CalculateCRC32_32, "41 8B C0 44 8B CA 4C 33 C8");
	RELOC_GLOBAL_VAL(LookupREFRByHandle, "E8 ? ? ? ? 48 8B BD ? ? ? ? 48 8B 0B", 0, 1, 5);
	RELOC_GLOBAL_VAL(LookupFormByID, "E8 ? ? ? ? 48 8B F8 48 85 C0 74 ? 80 78 ? ? 75 ? 48 8D 48 ?", 0, 1, 5);
	RELOC_GLOBAL_VAL(EvaluationConditions, "48 8B C4 53 57 48 81 EC ? ? ? ? 48 8B 19");

	RELOC_GLOBAL_VAL(CalcMagicEffect, "48 85 C9 0F 84 ? ? ? ? 48 89 5C 24 ? 55 56"); // new
	RELOC_GLOBAL_VAL(GetRestoredHPPercent, "48 8B C4 53 48 83 EC ? 0F 29 70 ? 48 8B DA"); 
	RELOC_GLOBAL_VAL(GetModifiedValueName, "57 41 56 41 57 48 83 EC ? 0F B7 42 ?", -0xF);
	RELOC_GLOBAL_VAL(GetActorValueCollector, "E8 ? ? ? ? 48 8B 5C 24 ? 48 8B 88 ? ? ? ? 48 89 0D ? ? ? ?", 0, 1, 5); // new detail: sub_14020E8F0
	RELOC_GLOBAL_VAL(GetChargeKW, "E8 ? ? ? ? 48 8B D0 48 C7 44 24 ? ? ? ? ? 45 33 C9 45 33 C0", 0, 1, 5);
	RELOC_GLOBAL_VAL(GeHealthtKW, "E8 ? ? ? ? 48 8B D0 49 8D 8C 24 ? ? ? ? 45 33 C0", 0, 1, 5);
	// RELOC_GLOBAL_VAL(ReleaseSimpleColllector, "40 53 48 83 EC ? 48 8B 51 ? 48 8B D9 48 85 D2 74 ? 48 8B 09 E8 ? ? ? ?");

	RELOC_GLOBAL_VAL(GetWeaponRange, "40 53 48 83 EC ? 48 83 39 ? 48 8B D9 0F 57 C9"); 
	RELOC_GLOBAL_VAL(GetWeaponAccuracy, "40 53 48 83 EC ? 0F 29 74 24 ? 48 8B D9 E8 ? ? ? ? 48 8B 4B ?");
	RELOC_GLOBAL_VAL(GetWeaponSpeedDesc, "E8 ? ? ? ? 48 8B D0 48 8D 8D ? ? ? ? E8 ? ? ? ? 4C 8B C0", 0, 1, 5); // this function in main logic.
	RELOC_GLOBAL_VAL(GetStringFromHash, "E8 ? ? ? ? 4C 8B F8 48 85 C0 74 ? 80 38 ?", 0, 1, 5);
	RELOC_GLOBAL_VAL(VisitPerks, "E8 ? ? ? ? 48 8B 44 24 ? 48 85 C0 74 ? 0F B6 40 ?", 0, 1, 5);
	RELOC_GLOBAL_VAL(CalcInstanceData, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 89 11 49 8B F8");
	RELOC_GLOBAL_VAL(RegisterMenuOpenCloseEvent, "E8 ? ? ? ? B0 ? E9 ? ? ? ? 4D 85 FF", 0, 1, 5);

	RELOC_GLOBAL_VAL(g_consoleHandle, "8B 1D ? ? ? ? 45 8B C7", 0, 2, 6);
	RELOC_GLOBAL_VAL(g_dataHandler, "48 8B 05 ? ? ? ? 8B 13", 0, 3, 7); // new 
	RELOC_GLOBAL_VAL(g_scaleformHeap, "48 8B 0D ? ? ? ? 48 8B 01 FF 50 ? 48 83 EF ? 48 83 EE ? 75 ? 48 8B 55 ?", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_scaleformManager, "48 8B 0D ? ? ? ? 48 8B 49 ? E8 ? ? ? ? 48 8B 45 ?", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_mainHeap, "48 8D 0D ? ? ? ? 45 33 C0 49 8B D6 E8 ? ? ? ? 4C 8B D0", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_player, "48 3B 1D ? ? ? ? 75 ? E8 ? ? ? ? 84 C0", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_playerCamera, "48 8B 0D ? ? ? ? 88 05 ? ? ? ?", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_ui, "48 8B 0D ? ? ? ? 89 1C 2E", 0, 3, 7);

	RELOC_GLOBAL_VAL(g_menuStackLock, "48 8D 0D ? ? ? ? 48 89 45 ? 44 8B 38", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_menuTableLock, "48 8D 0D ? ? ? ? E8 ? ? ? ? 4D 8B 85 ? ? ? ?", 0, 3, 7);

	Console::InitHook();
}