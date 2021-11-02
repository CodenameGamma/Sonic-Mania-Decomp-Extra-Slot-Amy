#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#define minVal(a, b)                      ((a) < (b) ? (a) : (b))
#define maxVal(a, b)                      ((a) > (b) ? (a) : (b))
#define clampVal(value, minimum, maximum) (((value) < (minimum)) ? (minimum) : (((value) > (maximum)) ? (maximum) : (value)))
#define fabs(a)                           ((a) > 0 ? (a) : -(a))

#define setBit(value, set, pos) ((value) ^= (-(int32)(set) ^ (value)) & (1 << (pos)))
#define getBit(b, pos)          ((b) >> (pos) & 1)

#define intToVoid(x)   (void *)(size_t)(x)
#define floatToVoid(x) intToVoid(*(int32 *)&(x))
#define voidToInt(x)   (int32)(size_t)(x)
#define voidToFloat(x) *(float *)&(x)

#define unused(x) (void)x

typedef uint32 colour;
typedef uint32 color;

#define RETRO_USE_MOD_LOADER (1)

#if RETRO_USE_MOD_LOADER
typedef enum {
    MODCB_GAME_STARTUP,
    MODCB_STAGELOAD,
    MODCB_ONUPDATE,
    MODCB_ONLATEUPDATE,
    MODCB_ONSTATICUPDATE,
    MODCB_ONDRAW,
    MODCB_STAGEUNLOAD,
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

// Mod Table
typedef struct {
    void (*RegisterGlobals)(const char *globalsPath, void **globals, uint32 size);
    void *RegisterObject_FuncP;
    void *RegisterObject_STD;
    void *(*GetGlobals)(void);
    void (*Super)(int32 objectID, ModSuper callback, void *data);

    bool32 (*LoadModInfo)(const char *id, TextInfo *name, TextInfo *description, TextInfo *version, bool32 *active);
    void *AddModCallback_FuncP;
    void *AddModCallback_STD;
    void (*AddPublicFunction)(const char *functionName, void *functionPtr);
    void *(*GetPublicFunction)(const char *id, const char *functionName);
    const char *(*GetModPath)(const char *id);

    bool32 (*GetSettingsBool)(const char *id, const char *key, bool32 fallback);
    int32 (*GetSettingsInteger)(const char *id, const char *key, int32 fallback);
    void (*GetSettingsString)(const char *id, const char *key, TextInfo *result, const char *fallback);

    void (*SetSettingsBool)(const char *key, bool32 val);
    void (*SetSettingsInteger)(const char *key, int32 val);
    void (*SetSettingsString)(const char *key, TextInfo *val);

    void (*SaveSettings)(void);

    bool32 (*GetConfigBool)(const char *key, bool32 fallback);
    int32 (*GetConfigInteger)(const char *key, int32 fallback);
    void (*GetConfigString)(const char *key, TextInfo *result, const char *fallback);
    bool32 (*ForeachConfig)(TextInfo *textInfo);
    bool32 (*ForeachConfigCategory)(TextInfo *textInfo);

    Object *(*GetObject)(const char *name);
} ModFunctionTable;
#endif

#if RETRO_USE_PLUS
// Userdata Table
typedef struct {
    int32 (*GetUserLanguage)(void);
    bool32 (*GetConfirmButtonFlip)(void);
    void (*ExitGame)(void);
    void (*LaunchManual)(void);
    bool32 (*Unknown4)(uint8 inputID);
    bool32 (*CheckDLC)(GameDLC dlc);
    void (*ShowExtensionOverlay)(uint8 overlay);
#if RETRO_GAMEVER == VER_107
    void (*EGS_Checkout)(int32 a1);
    void (*ShowEncorePage)(int32 a1);
    void (*EGS_Unknown4)(int32 a1);
    void (*RegisterHIDDevice)(void);
#endif
    void (*UnlockAchievement)(const char *achName);
    void (*GetAchievementStatus)(void);
    void (*SetAchievementStatus)(int32 a1);
#if RETRO_GAMEVER == VER_107
    bool32 (*CheckAchievementsEnabled)(void);
    void (*GetAchievementNames)(TextInfo *names, int32 count);
#endif
    void (*LeaderboardsUnknown4)(void);
#if RETRO_GAMEVER == VER_107
    void (*EGS_LeaderboardUnknown1)(void);
#endif
    void (*FetchLeaderboard)(const char *name, int32 a2);
    void (*TrackScore)(const char *name, int32 score, void (*callback)(int32 status, int32 rank));
    int32 (*GetLeaderboardsStatus)(void);
    Vector2 (*LeaderboardEntryCount)(void);
    int32 (*GetLeaderboardUnknown2)(void);
    void (*Unknown12)(int32 a2, uint32 a3, int32 a4);
    void (*LeaderboardsUnknown8)(void);
    LeaderboardEntry *(*ReadLeaderboardEntry)(uint32 a1);
    void (*SetRichPresence)(int32, TextInfo *text);
    void (*TryTrackStat)(StatInfo *stat);
    int32 (*GetStatsStatus)(void);
    void (*SetStatsStatus)(int32 a1);
    void (*ClearPrerollErrors)(void);
    void (*TryAuth)(void);
    int32 (*GetUserAuthStatus)(void);
    bool32 (*GetUsername)(TextInfo *text);
    void (*TryInitStorage)(void);
    int32 (*UserStorageStatusUnknown1)(void);
    int32 (*GetSaveStatus)(void);
    void (*ClearUserStorageStatus)(void);
    void (*SetUserStorageStatus)(void);
    void (*SetSaveStatusOK)(void);
    void (*SetSaveStatusForbidden)(void);
    void (*SetSaveStatusError)(void);
    void (*SetUserStorageNoSave)(bool32 state);
    bool32 (*GetUserStorageNoSave)(void);
    void (*LoadUserFile)(const char *name, int32 *data, int32 size, void (*callback)(int32 status));                  // load user file from game dir
    void (*SaveUserFile)(const char *name, int32 *data, int32 size, void (*callback)(int32 status), bool32 compress); // save user file to game dir
    void (*DeleteUserFile)(const char *filename, void (*callback)(int32 status));                                 // delete user file from game dir
    uint16 (*InitUserDB)(const char *name, ...);
    uint16 (*LoadUserDB)(const char *filename, void (*callback)(int32 status));
    void (*SaveUserDB)(uint16 tableID, void (*callback)(int32 status));
    void (*ClearUserDB)(uint16 tableID);
    void (*ClearAllUserDBs)(void);
    void (*SetupSortedUserDBRowIDs)(uint16 tableID);
    bool32 (*GetUserDBRowsChanged)(uint16 tableID);
    void (*Unknown33)(uint16 tableID, int32 type, const char *name, void *value);
    void (*SortDBRows)(uint16 tableID, int32 type, const char *name, bool32 flag);
    int32 (*GetSortedUserDBRowCount)(uint16 tableID);
    int32 (*GetSortedUserDBRowID)(uint16 tableID, uint16 row);
    int32 (*AddUserDBRow)(uint16 tableID);
    void (*SetUserDBValue)(uint16 tableID, int32 row, int32 type, const char *name, void *value);
    void (*GetUserDBValue)(uint16 tableID, int32 row, int32 type, const char *name, void *value);
    uint32 (*GetUserDBRowUUID)(uint16 tableID, uint16 row);
    int32 (*GetUserDBByID)(uint16 tableID, uint32 uuid);
    void (*GetUserDBCreationTime)(uint16 tableID, uint16 row, char *buffer, uint32 sizeInBytes, const char *format);
    void (*RemoveDBRow)(uint16 tableID, uint16 row);
    void (*RemoveAllDBRows)(uint16 tableID);
    // count: 59
} APIFunctionTable;
#endif

// Function Table
typedef struct {
    void (*InitGlobalVariables)(void **globals, int32 size);
    void (*RegisterObject)(Object **structPtr, const char *name, uint32 entitySize, uint32 objectSize, void (*update)(void), void (*lateUpdate)(void),
                           void (*staticUpdate)(void), void (*draw)(void), void (*create)(void *), void (*stageLoad)(void), void (*editorDraw)(void),
                           void (*editorLoad)(void), void (*serialize)(void));
#if RETRO_USE_PLUS
    void (*RegisterObjectContainer)(void **structPtr, const char *name, uint32 objectSize);
#endif
    bool32 (*GetActiveEntities)(uint16 group, Entity **entity);
    bool32 (*GetEntities)(uint16 type, Entity **entity);
    void (*BreakForeachLoop)(void);
    void (*SetEditableVar)(uint8 type, const char *name, uint8 object, int32 storeOffset);
    void *(*GetEntityByID)(uint16 slotID);
    int32 (*GetEntityID)(void *entityPtr);
    int32 (*GetEntityCount)(uint16 type, bool32 isActive);
    int32 (*GetDrawListRef)(uint8 layerID, uint16 entityID);
    void *(*GetDrawListRefPtr)(uint8 layerID, uint16 entityID);
    void (*ResetEntityPtr)(void *entity, uint16 type, void *data);
    void (*ResetEntitySlot)(uint16 slotID, uint16 type, void *data);
    Entity *(*CreateEntity)(uint16 type, void *data, int32 x, int32 y);
    void (*CopyEntity)(void *destEntity, void *srcEntity, bool32 clearSrcEntity);
    bool32 (*CheckOnScreen)(void *entity, Vector2 *range);
    bool32 (*CheckPosOnScreen)(Vector2 *pos, Vector2 *range);
    void (*AddDrawListRef)(uint8 layer, uint16 entityID);
    void (*SwapDrawListEntries)(uint8 layer, uint16 entryStart, uint16 entryEnd, uint16 count);
    void (*SetDrawLayerProperties)(uint8 layer, bool32 sorted, void (*callback)(void));
    void (*SetScene)(const char *categoryName, const char *sceneName);
    void (*SetGameMode)(uint8 mode);
#if RETRO_USE_PLUS
    void (*SetHardResetFlag)(bool32 set);
#endif
    bool32 (*CheckValidScene)(void);
    int32 (*CheckStageFolder)(const char *folderName);
    int32 (*LoadScene)(void);
    int32 (*GetObjectIDByName)(const char *name);
    void (*ClearCameras)(void);
    void (*AddCamera)(Vector2 *pos, int32 offsetX, int32 offsetY, bool32 worldRelative);
#if !RETRO_USE_PLUS
    void *(*GetAPIFunction)(const char *funcName);
#endif
    int32 (*GetSettingsValue)(int32 id);
    void (*SetSettingsValue)(int32 id, int32 val);
    void (*UpdateWindow)(void);
    int32 (*Sin1024)(int32 angle);
    int32 (*Cos1024)(int32 angle);
    int32 (*Tan1024)(int32 angle);
    int32 (*ASin1024)(int32 angle);
    int32 (*ACos1024)(int32 angle);
    int32 (*Sin512)(int32 angle);
    int32 (*Cos512)(int32 angle);
    int32 (*Tan512)(int32 angle);
    int32 (*ASin512)(int32 angle);
    int32 (*ACos512)(int32 angle);
    int32 (*Sin256)(int32 angle);
    int32 (*Cos256)(int32 angle);
    int32 (*Tan256)(int32 angle);
    int32 (*ASin256)(int32 angle);
    int32 (*ACos256)(int32 angle);
    int32 (*Rand)(int32 min, int32 max);
    int32 (*Random)(int32 min, int32 max, int32 *randKey);
    void (*SetRandKey)(int32 key);
    uint8 (*ATan2)(int32 x, int32 y);
    void (*SetIdentityMatrix)(Matrix *matrix);
    void (*MatrixMultiply)(Matrix *dest, Matrix *matrixA, Matrix *matrixB);
    void (*MatrixTranslateXYZ)(Matrix *matrix, int32 x, int32 y, int32 z, bool32 setIdentity);
    void (*MatrixScaleXYZ)(Matrix *matrix, int32 scaleX, int32 scaleY, int32 scaleZ);
    void (*MatrixRotateX)(Matrix *matrix, int32 angle);
    void (*MatrixRotateY)(Matrix *matrix, int32 angle);
    void (*MatrixRotateZ)(Matrix *matrix, int32 angle);
    void (*MatrixRotateXYZ)(Matrix *matrix, int32 angleX, int32 angleY, int32 angleZ);
    void (*MatrixInverse)(Matrix *dest, Matrix *matrix);
    void (*MatrixCopy)(Matrix *matDest, Matrix *matSrc);
    void (*SetText)(TextInfo *textInfo, const char *text, uint32 size);
    void (*CopyString)(TextInfo *dst, TextInfo *src);
    void (*PrependText)(TextInfo *info, const char *text);
    void (*AppendString)(TextInfo *info, TextInfo *str);
    void (*AppendText)(TextInfo *info, const char *text);
    void (*LoadStrings)(TextInfo *dst, const char *path, int32);
    bool32 (*SplitStringList)(TextInfo *list, TextInfo *strings, int32 start, int32 end);
    void (*GetCString)(char *text, TextInfo *info);
    bool32 (*StringCompare)(TextInfo *strA, TextInfo *strB, bool32 exactMatch);
    void (*GetDisplayInfo)(int32 *displayID, int32 *width, int32 *height, int32 *refreshRate, TextInfo *text);
    void (*GetWindowSize)(int32 *width, int32 *height);
    int32 (*SetScreenSize)(uint8 screenID, uint16 width, uint16 height);
    void (*SetClipBounds)(uint8 screenID, int32 x1, int32 y1, int32 x2, int32 y2);
#if RETRO_USE_PLUS
    void (*SetScreenSplitVerticies)(int8 p2_1, int8 p2_2, int8 p3_1, int8 p3_2, int8 p3_3);
#endif
    int16 (*LoadSpriteSheet)(const char *path, Scopes scope);
    void (*SetLookupTable)(uint16 *tablePtr);
    void (*SetPaletteMask)(uint32 maskColour);
    void (*SetPaletteEntry)(uint8 paletteID, uint8 index, uint32 colour);
    uint32 (*GetPaletteEntry)(uint8 paletteID, uint8 index);
    void (*SetActivePalette)(uint8 newActivePal, int32 startLine, int32 endLine);
    void (*CopyPalette)(uint8 sourcePalette, uint8 srcPaletteStart, uint8 destinationPalette, uint8 destPaletteStart, uint16 count);
#if RETRO_USE_PLUS
    void (*LoadPalette)(uint8 bank, const char *path, uint16 rowFlags);
#endif
    void (*RotatePalette)(uint8 palID, uint8 startIndex, uint8 endIndex, bool32 right);
    void (*SetLimitedFade)(uint8 destPaletteID, uint8 srcPaletteA, uint8 srcPaletteB, uint16 blendAmount, int32 startIndex, int32 endIndex);
#if RETRO_USE_PLUS
    void (*BlendColours)(uint8 paletteID, uint8 *coloursA, uint8 *coloursB, int32 alpha, int32 index, int32 count);
#endif
    void (*DrawRect)(int32 x, int32 y, int32 width, int32 height, uint32 colour, int32 alpha, InkEffects inkEffect, bool32 screenRelative);
    void (*DrawLine)(int32 x1, int32 y1, int32 x2, int32 y2, uint32 colour, int32 alpha, InkEffects inkEffect, bool32 screenRelative);
    void (*DrawCircle)(int32 x, int32 y, int32 radius, uint32 colour, int32 alpha, InkEffects inkEffect, bool32 screenRelative);
    void (*DrawCircleOutline)(int32 x, int32 y, int32 innerRadius, int32 outerRadius, uint32 colour, int32 alpha, InkEffects inkEffect, bool32 screenRelative);
    void (*DrawQuad)(Vector2 *verticies, int32 vertCount, int32 r, int32 g, int32 b, int32 alpha, InkEffects inkEffect);
    void (*DrawBlendedQuad)(Vector2 *verticies, colour *vertColours, int32 vertCount, int32 alpha, InkEffects inkEffect);
    void (*DrawSprite)(Animator *animator, Vector2 *position, bool32 screenRelative);
    void (*DrawDeformedSprite)(uint16 sheet, InkEffects inkEffect, bool32 screenRelative);
    void (*DrawText)(Animator *animator, Vector2 *position, TextInfo *info, int32 startCharID, int32 endCharID, Alignments align, int32 spacing, int32 a8,
                     Vector2 *charPos, bool32 ScreenRelative);
    void (*DrawTile)(uint16 *tileInfo, int32 countX, int32 countY, Vector2 *position, Vector2 *offset, bool32 screenRelative);
    void (*CopyTile)(void);
    void (*DrawAniTiles)(uint16 sheetID, uint16 tileIndex, uint16 srcX, uint16 srcY, uint16 width, uint16 height);
    void (*FillScreen)(uint32 colour, int32 alphaR, int32 alphaG, int32 alphaB);
    uint16 (*LoadMesh)(const char *filename, uint8 scope);
    uint16 (*Create3DScene)(const char *identifier, uint16 faceCount, uint8 scope);
    void (*Prepare3DScene)(uint16 index);
    void (*SetDiffuseColour)(uint16 index, int32 x, int32 y, int32 z);
    void (*SetDiffuseIntensity)(uint16 index, int32 x, int32 y, int32 z);
    void (*SetSpecularIntensity)(uint16 index, int32 x, int32 y, int32 z);
    void (*AddModelTo3DScene)(uint16 modelIndex, uint16 sceneIndex, uint8 type, Matrix *mat1, Matrix *mat2, colour colour);
    void (*SetModelAnimation)(uint16 modelAnim, Animator *animator, int16 animSpeed, uint8 loopIndex, bool32 forceApply, uint16 frameID);
    void (*AddMeshFrameTo3DScene)(uint16 modelID, uint16 sceneID, Animator *animator, uint8 drawMode, Matrix *mat1, Matrix *mat, colour colour);
    void (*Draw3DScene)(uint16 index);
    uint16 (*LoadSpriteAnimation)(const char *path, Scopes scope);
    uint16 (*CreateSpriteAnimation)(const char *filename, uint32 frameCount, uint32 animCount, Scopes scope);
    void (*SetSpriteAnimation)(uint16 spriteIndex, uint16 animationID, Animator *animator, bool32 forceApply, int16 frameID);
    void (*EditSpriteAnimation)(uint16 spriteIndex, uint16 animID, const char *name, int32 frameOffset, uint16 frameCount, int16 animSpeed,
                                uint8 loopIndex, uint8 rotationFlag);
    void (*SetSpriteString)(uint16 spriteIndex, uint16 animID, TextInfo *info);
    void *(*GetSpriteAnimation)(uint16 sprIndex, const char *name);
    SpriteFrame *(*GetFrame)(uint16 sprIndex, uint16 anim, int32 frame);
    Hitbox *(*GetHitbox)(Animator *animator, uint8 hitboxID);
    int16 (*GetFrameID)(Animator *animator);
    int32 (*GetStringWidth)(uint16 sprIndex, uint16 animID, TextInfo *info, int32 startIndex, int32 length, int32 spacing);
    void (*ProcessAnimation)(Animator *animator);
    int32 (*GetSceneLayerID)(const char *name);
    TileLayer *(*GetSceneLayer)(int32 layerID);
    void (*GetLayerSize)(uint16 layer, Vector2 *size, bool32 pixelSize);
    uint16 (*GetTileInfo)(uint16 layer, int32 x, int32 y);
    void (*SetTileInfo)(uint16 layer, int32 x, int32 y, uint16 tile);
    int32 (*CopyTileLayer)(uint16 dstLayer, int32 startX1, int32 startY1, uint16 srcLayer, int32 startX2, int32 startY2, int32 countX, int32 countY);
    void (*ProcessParallax)(TileLayer *TileLayer);
    ScanlineInfo *(*GetScanlines)(void);
    bool32 (*CheckObjectCollisionTouchBox)(void *thisEntity, Hitbox *thisHitbox, void *otherEntity, Hitbox *otherHitbox);
    bool32 (*CheckObjectCollisionTouchCircle)(void *thisEntity, int32 thisOffset, void *otherEntity, int32 otherOffset);
    uint8 (*CheckObjectCollisionBox)(void *thisEntity, Hitbox *thisHitbox, void *otherEntity, Hitbox *otherHitbox, bool32 setPos);
    bool32 (*CheckObjectCollisionPlatform)(void *thisEntity, Hitbox *thisHitbox, void *otherEntity, Hitbox *otherHitbox, bool32 setPos);
    bool32 (*ObjectTileCollision)(void *entity, uint16 collisionLayers, uint8 collisionMode, uint8 collisionPlane, int32 xOffset, int32 yOffset,
                                  bool32 setPos);
    bool32 (*ObjectTileGrip)(void *entity, uint16 collisionLayers, uint8 collisionMode, uint8 collisionPlane, int32 xOffset, int32 yOffset,
                             int32 tolerance);
    void (*ProcessTileCollisions)(void *entity, Hitbox *outer, Hitbox *inner);
    int32 (*GetTileAngle)(uint16 tileID, uint8 cPlane, uint8 cMode);
    void (*SetTileAngle)(uint16 tileID, uint8 cPlane, uint8 cMode, uint8 value);
    uint8 (*GetTileBehaviour)(uint16 tileID, uint8 cPlane);
    void (*SetTileBehaviour)(uint16 tileID, uint8 cPlane, uint8 value);
    int32 (*GetSFX)(const char *path);
    int32 (*PlaySfx)(uint16 sfx, int32 loop, int32 pan);
    void (*StopSFX)(uint16 sfx);
    int32 (*PlayStream)(const char *filename, uint32 slot, uint32 startPos, uint32 loopPoint, bool32 loadASync);
    int32 (*SetChannelAttributes)(uint8 slot, float volume, float pan, float speed);
    void (*StopChannel)(uint8 slot);
    void (*PauseChannel)(uint8 slot);
    void (*ResumeChannel)(uint8 slot);
    bool32 (*IsSFXPlaying)(uint8 slot);
    bool32 (*ChannelActive)(uint8 slot);
    uint32 (*GetChannelPos)(uint8 slot);
    void (*LoadVideo)(const char *filename, double a2, bool32 (*skipCallback)(void));
    bool32 (*LoadImage)(const char *filename, double displayLength, double speed, bool32 (*skipCallback)(void));
#if RETRO_USE_PLUS
    int32 (*ControllerIDForInputID)(uint8 inputID);
    int32 (*MostRecentActiveControllerID)(int32 deviceID, int32 a2, uint32 a3);
    int32 (*GetControllerType)(int32 inputID);
    int32 (*GetAssignedControllerID)(int32 inputID);
    int32 (*GetAssignedUnknown)(int32 inputID);
    int32 (*DoInputUnknown2)(int32 inputID, int32 a2, int32 a3);
    int32 (*DoInputUnknown3)(int32 inputID, int32 a2, int32 a3);
    int32 (*Missing24)(void);
    int32 (*DoInputUnknown2_Active)(uint8 inputID, int32 a2, int32 a3);
    int32 (*DoInputUnknown3_Active)(uint8 inputID, int32 a2, int32 a3);
    void (*AssignControllerID)(uint8 inputID, int32 deviceID);
    bool32 (*InputIDIsDisconnected)(uint8 inputID);
    void (*ResetControllerAssignments)(void);
#endif
#if !RETRO_USE_PLUS
    void (*Unknown92)(int32 a1, int32 a2, int32 *a3);
#endif
    int32 (*LoadUserFile)(const char *filename, void *buffer, uint32 size); // load user file from exe dir
    int32 (*SaveUserFile)(const char *fileName, void *buffer, uint32 size); // save use file to exe dir
#if RETRO_USE_PLUS
    void (*PrintLog)(SeverityModes severity, const char *message, ...);
    void (*PrintString)(SeverityModes severity, const char *message);
    void (*PrintText)(SeverityModes severity, TextInfo *text);
    void (*PrintIntegerUnsigned)(SeverityModes severity, const char *message, uint32 integer);
    void (*PrintInteger)(SeverityModes severity, const char *message, int32 integer);
    void (*PrintFloat)(SeverityModes severity, const char *message, float f);
    void (*PrintVector2)(SeverityModes severity, const char *message, int32 x, int32 y);
    void (*PrintHitbox)(SeverityModes severity, const char *message, Hitbox *hitbox);
#endif
    void (*SetActiveVariable)(int32 objectID, const char *name);
    void (*AddVarEnumValue)(const char *name);
#if RETRO_USE_PLUS
    void (*ClearDebugValues)(void);
    void (*SetDebugValue)(const char *name, void *valPtr, DebugVarTypes type, int32 min, int32 max);
#endif
#if !RETRO_USE_PLUS
    void (*PrintMessage)(void *message, uint8 type);
#endif
} RSDKFunctionTable;


#if RETRO_USE_MOD_LOADER
extern ModFunctionTable Mod;
#endif
#if RETRO_USE_PLUS
extern APIFunctionTable API;
#endif
extern RSDKFunctionTable RSDK;

#include "All.h"

#define RSDK_EDITABLE_VAR(object, type, var) RSDK.SetEditableVar(type, #var, (uint8)object->objectID, offsetof(Entity##object, var))
#define RSDK_ACTIVE_VAR(object, var)         RSDK.SetActiveVariable(object->objectID, #var)
#define RSDK_ENUM_VAR(var)                   RSDK.AddVarEnumValue(#var)

#if RETRO_INCLUDE_EDITOR
#define RSDK_ADD_OBJECT(object)                                                                                                                      \
    RSDK.RegisterObject((Object **)&object, #object, sizeof(Entity##object), sizeof(Object##object), object##_Update, object##_LateUpdate,           \
                        object##_StaticUpdate, object##_Draw, object##_Create, object##_StageLoad, object##_EditorDraw, object##_EditorLoad,         \
                        object##_Serialize)
#else
#define RSDK_ADD_OBJECT(object)                                                                                                                      \
    RSDK.RegisterObject((Object **)&object, #object, sizeof(Entity##object), sizeof(Object##object), object##_Update, object##_LateUpdate,           \
                        object##_StaticUpdate, object##_Draw, object##_Create, object##_StageLoad, NULL, NULL, object##_Serialize)
#endif

#if RETRO_USE_PLUS
#define RSDK_ADD_OBJECT_CONTAINER(object) RSDK.RegisterObjectContainer((void **)&object, #object, sizeof(Object##object))
#endif

#define RSDK_THIS(type)             Entity##type *entity = (Entity##type *)RSDK_sceneInfo->entity
#define RSDK_GET_ENTITY(slot, type) ((Entity##type *)RSDK.GetEntityByID(slot))
#define SPAWN_CHILD(obj, data)      Entity##obj *child = (Entity##obj *)RSDK.CreateEntity(obj->objectID, (void*)(data), entity->position.x, entity->position.y)
#define CREATE_ENTITY(obj, data, x, y) ((Entity##obj *)RSDK.CreateEntity(obj->objectID, data, x, y))

#define INIT_TEXTINFO(info)                                                                                                                          \
    info.text       = NULL;                                                                                                                          \
    info.textLength = 0;                                                                                                                             \
    info.length     = 0

//Initializes entity values to the defaults
#define INIT_ENTITY(entity)                                                                                                                          \
    (entity)->active        = ACTIVE_BOUNDS;                                                                                                         \
    (entity)->visible       = false;                                                                                                                 \
    (entity)->updateRange.x = 0x800000;                                                                                                              \
    (entity)->updateRange.y = 0x800000;

#define foreach_active(type, entityOut)                                                                                                              \
    Entity##type *entityOut = NULL;                                                                                                                  \
    while (RSDK.GetActiveEntities(type->objectID, (Entity **)&entityOut))
#define foreach_all(type, entityOut)                                                                                                                 \
    Entity##type *entityOut = NULL;                                                                                                                  \
    while (RSDK.GetEntities(type->objectID, (Entity **)&entityOut))
#define foreach_break                                                                                                                                \
    RSDK.BreakForeachLoop();                                                                                                                         \
    break
#define foreach_return                                                                                                                               \
    RSDK.BreakForeachLoop();                                                                                                                         \
    return

// used mainly for cutscenes
#define RSDK_GET_PLAYER(p1, p2, cam)                                                                                                                 \
    EntityPlayer *p1  = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);                                                                                       \
    EntityPlayer *p2  = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);                                                                                       \
    EntityCamera *cam = RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);

#define destroyEntity(entity) RSDK.ResetEntityPtr(entity, TYPE_BLANK, NULL)

#if RETRO_USE_PLUS
#define isMainGameMode() (globals->gameMode == MODE_MANIA || globals->gameMode == MODE_ENCORE)
#else
#define isMainGameMode() (globals->gameMode == MODE_NOSAVE || globals->gameMode == MODE_MANIA)
#endif

DLLExport void LinkGameLogicDLL(GameInfo *gameInfo);
#if RETRO_USE_MOD_LOADER
DLLExport bool32 LinkModLogic(GameInfo *info, const char *id);
#endif

#endif //! GAMEOBJECTS_H
