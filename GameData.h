#pragma once
#include "F4SE/GameData.h"
#include "F4SE/GameReferences.h"
#include "F4SE/GameAPI.h"
#include "F4SE/GameRTTI.h"
#include "F4SE/GameExtraData.h"


#include <map>
#include <vector>

class ActiveEffect;

template <typename T>
struct SimpleList
{
	struct Node
	{
		T			item;
		Node		* next;
	};
	Node			node;

};
class TESReactionForm : public BaseFormComponent
{
public:
	struct RelationData						// XNAM
	{
		TESFaction	* faction;				// 00
		UInt32		modifier;				// 04
		UInt32		groupCombatReaction;	// 08 - Enemy / Friend / Ally
	};

	virtual ~TESReactionForm();

	// @members
	SimpleList<RelationData*>		relations;	// 08
	UInt8							unk0C;		// 18    ini'd to 14
	UInt8							pad0D[3];	// 0D
};
STATIC_ASSERT(sizeof(TESReactionForm) == 0x20);

//0x100
class TESFaction : public TESForm,
				   public TESFullName,
				   public TESReactionForm
				   
{
public:
	enum FactionFlags
	{
		kFlag_HiddenFromPC = 1 << 0,		//     1
		kFlag_SpecialCombat = 1 << 1,		//     2
		kFlag_TrackCrime = 1 << 6,		//    40
		kFlag_IgnoreMurder = 1 << 7,		//    80
		kFlag_IgnoreAssault = 1 << 8,		//   100
		kFlag_IgnorePickpocket = 1 << 9,		//   200
		kFlag_IgnoreStealing = 1 << 10,		//   400
		kFlag_DoReportCrimes = 1 << 11,		//   800
		kFlag_CrimeGoldDefaults = 1 << 12,		//  1000
		kFlag_IgnoreTrespass = 1 << 13,		//  2000
		kFlag_Vender = 1 << 14,		//  4000
		kFlag_CanBeOwner = 1 << 15,		//  8000
		kFlag_IgnoreWerewolf = 1 << 16		// 10000
	};

	// 2C
	struct CrimeValues								// CRVA
	{
		TESObjectREFR	* exteriorJailMarker;		// 00 (60) JAIL 60
		TESObjectREFR	* followerWaitMarker;		// 04 (68) WAIT
		TESObjectREFR	* stolenGoodsContainer;		// 08 (70) STOL
		TESObjectREFR	* playerInventoryContainer;	// 0C (78) PLCN
		BGSListForm		* sharedCrimeFactionList;	// 10 (80) CRGR
		BGSOutfit		* jailOutfit;				// 14 (88) JOUT
		bool			arrest;						// 18 (90)
		bool			attackOnSight;				// 19 (91)
		UInt16			murder;						// 1A (92)
		UInt16			assault;					// 1C (94)
		UInt16			trespass;					// 1E (96)
		UInt16			pickpocket;					// 20 (98)
		UInt16			pad22;						// 22 (9A)
		float			stealMult;					// 24 (9C)
		UInt16			escape;						// 28 (A0)
		UInt16			werewolf;					// 2A (A2)
		UInt32			padA4;						// A4
	};

	// 20
	struct VendorValues							// VENV
	{
		UInt16			startHour;				// 00	//A8
		UInt16			endHour;				// 02
		UInt32			radius;					// 04
		bool			buyStolenItems;			// 08	//B0
		bool			tradeEverything;		// 09
		bool			buyNonStolenItems;		// 0A
		void			* packageLocation;		// 10   //B8
		Condition		* condition;			// 18	//C0
		BGSListForm		* buySellList;			// 20	//C8
		TESObjectREFR	* merchantContainer;	// 28	//D0
		UInt32			unk1C;					// 30	//D8
	};

	// 8
	struct Data80
	{
		UInt64	unk0;	// 0
		UInt64	unk4;	// 4
	};

	// @members
	UInt64			unk50;			// 50  
	UInt32			factionFlags;	// 58 - DATA
	CrimeValues		crimeValues;	// 60 - CRVA
	VendorValues	vendorValues;	// A8 - VENV
	Data80			unk80;			// E0
	UInt32			unk88;			// F0
	UInt32			unk8C;			// F4
	float			unk90;			// F8
	UInt32			unk94;			// FC
};
STATIC_ASSERT(offsetof(TESFaction, factionFlags) == 0x58);
STATIC_ASSERT(offsetof(TESFaction, crimeValues) == 0x60);
STATIC_ASSERT(offsetof(TESFaction, vendorValues) == 0xA8);
STATIC_ASSERT(offsetof(TESFaction::VendorValues, merchantContainer) == 0xD0 - 0xA8);
STATIC_ASSERT(sizeof(TESFaction) == 0x100);

class TESWorldSpace : public TESForm,
					  public TESFullName,
					  public TESTexture
{

};

class PerkVisitor
{
public:
	struct Entry
	{
		BGSPerk*	perk;
		UInt8		rank;
	};
	virtual bool Accept(Entry* pEntry) = 0;
};



class HasPerkVisitor
{
public:
	struct Entry
	{
		BGSPerk*	perk;
		UInt8		rank;
	};
	virtual uintptr_t Accept(Entry* pEntry);

	std::vector<Entry>				result;
};


class ExtraFactionChanges : public BSExtraData
{
public:
	enum { kExtraTypeID = kExtraData_FactionChanges };

	ExtraFactionChanges();

	virtual ~ExtraFactionChanges();

	struct FactionInfo
	{
		TESFaction*		faction;
		SInt8			rank;
	};

	UInt64					unk08;
	tArray<FactionInfo>		factions;
};
static_assert(sizeof(ExtraFactionChanges::FactionInfo) == 0x10, "ExtraFactionChanges::FactionInfo != 0x10");



class ActiveEffectReferenceEffectController
{
public:
	void			** _vtbl;	// 00 
	ActiveEffect	* effect;	// 08
};


class ActiveEffect
{
public:
	enum
	{
		kFlag_Inactive = 0x8000
	};

	void												** _vtbl;								// 00 
	UInt64												unk08;									// 08
	ActiveEffectReferenceEffectController				controller;								// 10
	UInt64												unk20[(0x38 - 0x20) >> 3];				// 20
	UInt32												casterHandle;							// 38
	NiNode												* niNode;								// 40  NiNode
	MagicItem											* item;									// 48
	MagicItem::EffectItem								* effect;								// 50  effectsettings???????????
	TESObjectREFR										* reference;							// 58  -0x110 MagicTarget; DYNAMIC_CAST
	UInt64												unk68;									// 60
	UInt64												unk70;									// 68
	UInt64												unk78;									// 70
	float												elapsed;								// 78
	float												duration;								// 7C
	float												magnitude;								// 80
	UInt32												flags;									// 84
	UInt32												unk88;									// 88
	UInt32												unk8C;									// 8C
	UInt32												effectNum;								// 90 - Somekind of counter used to determine whether the ActiveMagicEffect handle is valid
	UInt32												unk94;									// 94
	ActorValueInfo										* actorValue;							// 98 - Only seems to appear on value modifiers
	float												unkA0;									// A0  //float
	UInt32												unkA4;									// A4
};
static_assert(sizeof(ActiveEffect) == 0xA8, "sizeof(ActiveEffect) != 0xA8");



template <typename T>
class BSSimpleList
{
public:
	T*			items;	   // 00
	UInt64		unk08;
	UInt32		count;
	UInt32		unk14;
	UInt8		refCount;  // 18
};

struct ActorValueInfoHolder
{

};

template<typename T>
struct SimpleCollector
{
	UInt64		unk00;
	T			* items;
	UInt32		unk10;
	UInt64		count;
};

//extern RelocAddr<ActorValueInfoHolder*>	g_actorValues;



