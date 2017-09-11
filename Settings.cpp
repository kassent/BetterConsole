#include <memory>
#include "Settings.h"

string  Settings::BASEINFO_ID = "ID";
string  Settings::BASEINFO_BASEID = "baseID";
string  Settings::BASEINFO_TYPE = "type";
string  Settings::BASEINFO_LOCATE = "locate";
string  Settings::BASEINFO_DEFINE = "define";
string  Settings::BASEINFO_LASTBASECHANGE = "lastBaseChange";
string  Settings::EXTRAINFO_ID = "ID";
string  Settings::EXTRAINFO_BASEID = "baseID";
string  Settings::EXTRAINFO_TYPE = "type";
string  Settings::EXTRAINFO_NAME = "name";
string  Settings::EXTRAINFO_MOD = "mod";
string  Settings::EXTRAINFO_RACE = "race";
string	Settings::EXTRAINFO_RACEID = "raceID";
string  Settings::EXTRAINFO_SEX = "sex";
string  Settings::EXTRAINFO_LEVEL = "level";
string  Settings::EXTRAINFO_MINLEVEL = "minLevel";
string  Settings::EXTRAINFO_MAXLEVEL = "maxLevel";
string  Settings::EXTRAINFO_PCLVMULT = "PCLevelMult";
string  Settings::EXTRAINFO_UNIQUE = "isUnique";
string  Settings::EXTRAINFO_ESSENTIAL = "isEssential";
string  Settings::EXTRAINFO_PROTECTED = "isProtected";
string  Settings::EXTRAINFO_INVULNERABLE = "isInvulnerable";
string  Settings::EXTRAINFO_MEMORY = "memory";
string	Settings::EXTRAINFO_TRUE = "true";
string	Settings::EXTRAINFO_FALSE = "false";
string	Settings::EXTRAINFO_NULL = Settings::EXTRAINFO_NULL.c_str();
string	Settings::EXTRAINFO_MALE = "male";
string	Settings::EXTRAINFO_FEMALE = "female";
string  Settings::POSITIONINFO = "positon";
string  Settings::POSITIONINFO_POSX = "POS-X";
string  Settings::POSITIONINFO_POSY = "POS-Y";
string  Settings::POSITIONINFO_POSZ = "POS-Z";
string  Settings::POSITIONINFO_ROTX = "ROT-X";
string  Settings::POSITIONINFO_ROTY = "ROT-Y";
string  Settings::POSITIONINFO_ROTZ = "ROT-Z";
string  Settings::CELLINFO = "cell";
string  Settings::CELLINFO_ISINTERIOR = "isInterior";
string  Settings::LOCATIONINFO = "location";
string  Settings::WORLDSPACEINFO = "worldspace";
string  Settings::ENCOUNTERZONEINFO = "encounterZone";
string	Settings::ENCOUNTERZONEINFO_MINLEVEL = "minLevel";
string	Settings::ENCOUNTERZONEINFO_MAXLEVEL = "maxLevel";
string	Settings::ENCOUNTERZONEINFO_CURRENTLEVEL = "currentLevel";
string  Settings::ENCOUNTERZONEINFO_NEVERRESET = "neverReset";
string  Settings::ENCOUNTERZONEINFO_DISABLEBOUNDARY = "disableBoundary";
string  Settings::ENCOUNTERZONEINFO_MATCHPCLEVEL = "matchPCLevel";
string  Settings::ENCOUNTERZONEINFO_ISWORKSHOP = "isWorkshop";
string  Settings::ENCOUNTERZONEINFO_LOCATIONID = "locationID";
string  Settings::PERKINFO = "perks";
string  Settings::PERKINFO_LEVEL = "level";
string  Settings::PERKINFO_RANK = "rank";
string  Settings::FACTIONINFO = "factions";
string  Settings::FACTIONINFO_RANK = "rank";
string	Settings::FACTIONINFO_MERCHANTFACTION = "isMerchantFaction";
string	Settings::FACTIONINFO_MERCHANTCONTAINERID = "merchantContainerID";
string  Settings::EFFECTINFO = "activeEffects";
string  Settings::EFFECTINFO_ELAPSED = "elapsed";
string  Settings::EFFECTINFO_DURATION = "duration";
string  Settings::EFFECTINFO_MAGNITUDE = "magnitude";
string  Settings::EFFECTINFO_INACTIVE = "Inactive";
string  Settings::EFFECTINFO_DETRIMENTAL = "detrimental";
string  Settings::EFFECTINFO_MODIFIEDAV = "modifiedActorValue";
string  Settings::EFFECTINFO_SPELLNAME = "spellName";
string  Settings::EFFECTINFO_SPELLID = "spellID";
string  Settings::EFFECTINFO_SPELLMOD = "spellMod";
string  Settings::EFFECTINFO_SPELLTYPE = "spellType";
string  Settings::AVINFO = "actorValues";
string  Settings::AVINFO_TYPE = "type";
string  Settings::AVINFO_BASEVALUE = "baseValue";
string  Settings::AVINFO_CURRENTVALUE = "currentValue";
string  Settings::EQUIPMENTINFO = "equipments";
string  Settings::EQUIPMENTINFO_SLOTINDEX = "slotIndex";
string  Settings::EQUIPMENTINFO_HEALTH = "health";
string  Settings::EQUIPMENTINFO_CHARGE = "charge";
string  Settings::EQUIPMENTINFO_CLASS = "class";
string  Settings::EQUIPMENTINFO_AMMO = "ammo";
string  Settings::EQUIPMENTINFO_SPEED = "speed";
string  Settings::EQUIPMENTINFO_RANGE = "range";
string  Settings::EQUIPMENTINFO_ACCURACY = "accuracy";
string  Settings::EQUIPMENTINFO_VALUE = "value";
string  Settings::EQUIPMENTINFO_WEIGHT = "weight";
string  Settings::EQUIPMENTINFO_PHYSICALDAMAGE = "";
string  Settings::EQUIPMENTINFO_POISONDAMAGE = "";
string  Settings::EQUIPMENTINFO_FIREDAMAGE = "";
string  Settings::EQUIPMENTINFO_ENERGYDAMAGE = "";
string  Settings::EQUIPMENTINFO_FROSTDAMAGE = "";
string  Settings::EQUIPMENTINFO_RADDAMAGE = "";
string  Settings::EQUIPMENTINFO_PHYSICALRESIST = "";
string  Settings::EQUIPMENTINFO_POISONRESIST = "";
string  Settings::EQUIPMENTINFO_FIRERESIST = "";
string  Settings::EQUIPMENTINFO_ENERGYRESIST = "";
string  Settings::EQUIPMENTINFO_FROSTRESIST = "";
string  Settings::EQUIPMENTINFO_RADRESIST = "";

std::vector<string>	Settings::DAMAGETYPES = {
	"DAMAGE",
	"PHYSICAL DAMAGE",
	"POISON DAMAGE",
	"FIRE DAMAGE",
	"ENERGY DAMAGE",
	"FROST DAMAGE",
	"RADIATION DAMAGE"
};
std::vector<string>	Settings::RESISTTYPES = {
	"RESIST",
	"PHYSICAL RESIST",
	"POISON RESIST",
	"FIRE RESIST",
	"ENERGY RESIST",
	"FROST RESIST",
	"RADIATION RESIST"
};

#define LoadSetting(key)	\
	GetPrivateProfileString(settingsSection, #key, "", sResult.get(), MAX_PATH, configFile); \
	Settings::##key## =  sResult.get();														 \
	_MESSAGE("%s=%s", #key, sResult.get());													

#define LoadDamageSetting(key, index)	\
	GetPrivateProfileString(settingsSection, #key, "", sResult.get(), MAX_PATH, configFile); \
	Settings::DAMAGETYPES[##index##] =  sResult.get();										 \
	_MESSAGE("%s=%s", #key, sResult.get());

#define LoadResistSetting(key, index)	\
	GetPrivateProfileString(settingsSection, #key, "", sResult.get(), MAX_PATH, configFile); \
	Settings::RESISTTYPES[##index##] =  sResult.get();										 \
	_MESSAGE("%s=%s", #key, sResult.get());

void Settings::LoadSettings()
{
	constexpr char* configFile = "Data\\F4se\\Plugins\\BetterConsole_Translation.ini";
	constexpr char* settingsSection = "Translations";

	std::unique_ptr<char[]> sResult(new char[MAX_PATH]);

	LoadSetting(EXTRAINFO_ID);
	LoadSetting(EXTRAINFO_BASEID);
	LoadSetting(EXTRAINFO_TYPE);
	LoadSetting(EXTRAINFO_NAME);
	LoadSetting(EXTRAINFO_MOD);
	LoadSetting(EXTRAINFO_RACE);
	LoadSetting(EXTRAINFO_RACEID);
	LoadSetting(EXTRAINFO_SEX);
	LoadSetting(EXTRAINFO_LEVEL);
	LoadSetting(EXTRAINFO_MINLEVEL);
	LoadSetting(EXTRAINFO_MAXLEVEL);
	LoadSetting(EXTRAINFO_PCLVMULT);
	LoadSetting(EXTRAINFO_UNIQUE);
	LoadSetting(EXTRAINFO_ESSENTIAL);
	LoadSetting(EXTRAINFO_PROTECTED);
	LoadSetting(EXTRAINFO_INVULNERABLE);
	LoadSetting(EXTRAINFO_MEMORY);
	LoadSetting(EXTRAINFO_TRUE);
	LoadSetting(EXTRAINFO_FALSE);
	LoadSetting(EXTRAINFO_NULL);
	LoadSetting(EXTRAINFO_MALE);
	LoadSetting(EXTRAINFO_FEMALE);
	LoadSetting(POSITIONINFO);
	LoadSetting(POSITIONINFO_POSX);
	LoadSetting(POSITIONINFO_POSY);
	LoadSetting(POSITIONINFO_POSZ);
	LoadSetting(POSITIONINFO_ROTX);
	LoadSetting(POSITIONINFO_ROTY);
	LoadSetting(POSITIONINFO_ROTZ);
	LoadSetting(CELLINFO);
	LoadSetting(CELLINFO_ISINTERIOR);
	LoadSetting(LOCATIONINFO);
	LoadSetting(WORLDSPACEINFO);
	LoadSetting(ENCOUNTERZONEINFO);
	LoadSetting(ENCOUNTERZONEINFO_MINLEVEL);
	LoadSetting(ENCOUNTERZONEINFO_MAXLEVEL);
	LoadSetting(ENCOUNTERZONEINFO_CURRENTLEVEL)
	LoadSetting(ENCOUNTERZONEINFO_NEVERRESET);
	LoadSetting(ENCOUNTERZONEINFO_DISABLEBOUNDARY);
	LoadSetting(ENCOUNTERZONEINFO_MATCHPCLEVEL);
	LoadSetting(ENCOUNTERZONEINFO_ISWORKSHOP);
	LoadSetting(ENCOUNTERZONEINFO_LOCATIONID);
	LoadSetting(PERKINFO);
	LoadSetting(PERKINFO_LEVEL);
	LoadSetting(PERKINFO_RANK);
	LoadSetting(FACTIONINFO);
	LoadSetting(FACTIONINFO_RANK);
	LoadSetting(FACTIONINFO_MERCHANTFACTION);
	LoadSetting(FACTIONINFO_MERCHANTCONTAINERID);
	LoadSetting(EFFECTINFO);
	LoadSetting(EFFECTINFO_ELAPSED);
	LoadSetting(EFFECTINFO_DURATION);
	LoadSetting(EFFECTINFO_MAGNITUDE);
	LoadSetting(EFFECTINFO_INACTIVE);
	LoadSetting(EFFECTINFO_DETRIMENTAL);
	LoadSetting(EFFECTINFO_MODIFIEDAV);
	LoadSetting(EFFECTINFO_SPELLNAME);
	LoadSetting(EFFECTINFO_SPELLID);
	LoadSetting(EFFECTINFO_SPELLMOD);
	LoadSetting(EFFECTINFO_SPELLTYPE);
	LoadSetting(AVINFO);
	LoadSetting(AVINFO_TYPE);
	LoadSetting(AVINFO_BASEVALUE);
	LoadSetting(AVINFO_CURRENTVALUE);
	LoadSetting(EQUIPMENTINFO);
	LoadSetting(EQUIPMENTINFO_SLOTINDEX);
	LoadSetting(EQUIPMENTINFO_HEALTH);
	LoadSetting(EQUIPMENTINFO_CHARGE);
	LoadSetting(EQUIPMENTINFO_CLASS);
	LoadSetting(EQUIPMENTINFO_AMMO);
	LoadSetting(EQUIPMENTINFO_SPEED);
	LoadSetting(EQUIPMENTINFO_RANGE);
	LoadSetting(EQUIPMENTINFO_ACCURACY);
	LoadSetting(EQUIPMENTINFO_VALUE);
	LoadSetting(EQUIPMENTINFO_WEIGHT);

	LoadDamageSetting(EQUIPMENTINFO_PHYSICALDAMAGE, 1);
	LoadDamageSetting(EQUIPMENTINFO_POISONDAMAGE, 2);
	LoadDamageSetting(EQUIPMENTINFO_FIREDAMAGE, 3);
	LoadDamageSetting(EQUIPMENTINFO_ENERGYDAMAGE, 4);
	LoadDamageSetting(EQUIPMENTINFO_FROSTDAMAGE, 5);
	LoadDamageSetting(EQUIPMENTINFO_RADDAMAGE, 6);

	LoadResistSetting(EQUIPMENTINFO_PHYSICALRESIST, 1);
	LoadResistSetting(EQUIPMENTINFO_POISONRESIST, 2);
	LoadResistSetting(EQUIPMENTINFO_FIRERESIST, 3);
	LoadResistSetting(EQUIPMENTINFO_ENERGYRESIST, 4);
	LoadResistSetting(EQUIPMENTINFO_FROSTRESIST, 5);
	LoadResistSetting(EQUIPMENTINFO_RADRESIST, 6);
}