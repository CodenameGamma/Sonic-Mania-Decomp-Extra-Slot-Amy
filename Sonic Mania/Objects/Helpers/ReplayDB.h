#ifndef OBJ_REPLAYDB_H
#define OBJ_REPLAYDB_H

#include "SonicMania.h"

#if RETRO_USE_PLUS
// Object Class
typedef struct {
    RSDK_OBJECT
    Entity *loadEntity;
    void (*loadCallback)(bool32);
    Entity *saveEntity;
    void (*saveCallback)(bool32);
    Entity *deleteEntity;
    void (*deleteCallback)(bool32);
} ObjectReplayDB;

// Entity Class
typedef struct {
	RSDK_ENTITY
} EntityReplayDB;

// Object Struct
extern ObjectReplayDB *ReplayDB;

// Standard Entity Events
void ReplayDB_Update(void);
void ReplayDB_LateUpdate(void);
void ReplayDB_StaticUpdate(void);
void ReplayDB_Draw(void);
void ReplayDB_Create(void* data);
void ReplayDB_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void ReplayDB_EditorDraw(void);
void ReplayDB_EditorLoad(void);
#endif
void ReplayDB_Serialize(void);

// Extra Entity Functions
int32 ReplayDB_Buffer_PackEntry(uint8 *compressed, uint8 *uncompressed);
int32 ReplayDB_Buffer_UnpackEntry(uint8 *uncompressed, uint8 *compressed);
#endif

#endif //!OBJ_REPLAYDB_H
