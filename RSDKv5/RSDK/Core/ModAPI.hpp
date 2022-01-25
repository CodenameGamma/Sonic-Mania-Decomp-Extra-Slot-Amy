#ifndef MOD_API_H
#define MOD_API_H

#if RETRO_USE_MOD_LOADER
#include <vector>
#include <string>
#include <map>
#include "tinyxml2.h"

#include <functional>
#endif

namespace RSDK
{

#if RETRO_USE_MOD_LOADER
typedef enum {
    MODCB_GAME_STARTUP,
    MODCB_STAGELOAD,
    MODCB_ONUPDATE,
    MODCB_ONLATEUPDATE,
    MODCB_ONSTATICUPDATE,
    MODCB_ONDRAW,
    MODCB_STAGEUNLOAD,
    MODCB_MAX,
} ModCallbackEvents;

typedef enum {
    SUPER_UPDATE,
    SUPER_LATEUPDATE,
    SUPER_STATICUPDATE,
    SUPER_DRAW,
    SUPER_CREATE,
    SUPER_STAGELOAD,
    SUPER_EDITORDRAW,
    SUPER_EDITORLOAD,
    SUPER_SERIALIZE
} ModSuper;

typedef enum {
    ModTable_RegisterGlobals,
    ModTable_RegisterObject,
    ModTable_RegisterObjectSTD,
    ModTable_GetGlobals,
    ModTable_Super,
    ModTable_LoadModInfo,
    ModTable_GetModPath,
    ModTable_GetModCount,
    ModTable_GetModIDByIndex,
    ModTable_ForeachModID,
    ModTable_AddModCallback,
    ModTable_AddModCallbackSTD,
    ModTable_AddPublicFunction,
    ModTable_GetPublicFunction,
    ModTable_GetSettingsBool,
    ModTable_GetSettingsInt,
    ModTable_GetSettingsString,
    ModTable_SetSettingsBool,
    ModTable_SetSettingsInt,
    ModTable_SetSettingsString,
    ModTable_SaveSettings,
    ModTable_GetConfigBool,
    ModTable_GetConfigInt,
    ModTable_GetConfigString,
    ModTable_ForeachConfig,
    ModTable_ForeachConfigCategory,
    ModTable_GetObject,
    ModTable_RegisterAchievement,
    ModTable_GetAchievementInfo,
    ModTable_GetAchievementIndexByID,
    ModTable_GetAchievementCount,
    ModTable_Max
} ModFunctionTable;

typedef void (*ModCallback)(void *);
typedef std::function<void(void *)> ModCallbackSTD;

struct ModPublicFunctionInfo {
    std::string name;
    void *ptr;
};

typedef bool (*modLink)(RSDK::GameInfo *, const char *);
typedef std::function<bool(RSDK::GameInfo *, const char *)> modLinkSTD;
typedef const char *(*langSetup)(RSDK::GameInfo *, const char *);
typedef modLink (*newLangLink)(const char *, const char *, int32 *);

struct ModInfo {
    std::string name;
    std::string desc;
    std::string author;
    std::string version;
    std::map<std::string, std::string> fileMap;
    std::vector<ModPublicFunctionInfo> functionList;
    std::string id;
    bool active;
    std::vector<modLinkSTD> linkModLogic;
    void (*unloadMod)() = NULL;
    std::map<std::string, std::map<std::string, std::string>> settings;
    std::map<std::string, std::map<std::string, std::string>> config;
    const char *language = NULL;
};

extern std::vector<ModInfo> modList;
extern int32 activeMod;
extern std::vector<ModCallbackSTD> modCallbackList[MODCB_MAX];

extern void *modFunctionTable[ModTable_Max];
extern int32 currentObjectID;

extern ModInfo *currentMod;

inline void setActiveMod(int32 id) { activeMod = id; }

void initModAPI();
void unloadMods();
void loadMods();
bool32 loadMod(ModInfo *info, std::string modsPath, std::string folder, bool32 active);
void saveMods();

void sortMods();

void RunModCallbacks(int32 callbackID, void *data);

// Mod API
void ModRegisterGlobalVariables(const char *globalsPath, void **globals, uint32 size);
void ModRegisterObject(Object **structPtr, const char *name, uint32 entitySize, uint32 objectSize, void (*update)(void), void (*lateUpdate)(void),
                       void (*staticUpdate)(void), void (*draw)(void), void (*create)(void *), void (*stageLoad)(void), void (*editorDraw)(void),
                       void (*editorLoad)(void), void (*serialize)(void), const char *inherited);
void ModRegisterObject_STD(Object **structPtr, const char *name, uint32 entitySize, uint32 objectSize, std::function<void(void)> update,
                           std::function<void(void)> lateUpdate, std::function<void(void)> staticUpdate, std::function<void(void)> draw,
                           std::function<void(void *)> create, std::function<void(void)> stageLoad, std::function<void(void)> editorDraw,
                           std::function<void(void)> editorLoad, std::function<void(void)> serialize, const char *inherited);
void *GetGlobals();

bool32 LoadModInfo(const char *folder, TextInfo *name, TextInfo *description, TextInfo *version, bool32 *active);
void GetModPath(const char *id, TextInfo *result);
int32 GetModCount(bool32 active);
const char *GetModIDByIndex(uint32 index);
bool32 ForeachModID(TextInfo *id);

void AddModCallback(int32 callbackID, ModCallback callback);
void AddModCallback_STD(int32 callbackID, ModCallbackSTD callback);
void AddPublicFunction(const char *functionName, void *functionPtr);
void *GetPublicFunction(const char *folder, const char *functionName);

bool32 GetSettingsBool(const char *id, const char *key, bool32 fallback);
int32 GetSettingsInteger(const char *id, const char *key, int32 fallback);
void GetSettingsString(const char *id, const char *key, TextInfo *result, const char *fallback);

void SetSettingsBool(const char *key, bool32 val);
void SetSettingsInteger(const char *key, int32 val);
void SetSettingsString(const char *key, TextInfo *val);

void SaveSettings();

bool32 GetConfigBool(const char *key, bool32 fallback);
int32 GetConfigInteger(const char *key, int32 fallback);
void GetConfigString(const char *key, TextInfo *result, const char *fallback);
bool32 ForeachConfig(TextInfo *textInfo);
bool32 ForeachConfigCategory(TextInfo *textInfo);

void Super(int32 objectID, ModSuper callback, void *data);

Object *GetObject(const char *name);

void RegisterAchievement(const char *identifier, const char *name, const char *desc);
void GetAchievementInfo(uint32 id, TextInfo *name, TextInfo *description, TextInfo *identifer, bool32 *achieved);
int GetAchievementIndexByID(const char *id);
int GetAchievementCount();
#endif

} // namespace RSDK

#endif // !MOD_API_H
