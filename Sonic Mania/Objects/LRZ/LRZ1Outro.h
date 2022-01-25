#ifndef OBJ_LRZ1OUTRO_H
#define OBJ_LRZ1OUTRO_H

#include "SonicMania.h"

// Object Class
struct ObjectLRZ1Outro {
	RSDK_OBJECT
};

// Entity Class
struct EntityLRZ1Outro {
    RSDK_ENTITY
    EntityDashLift *lift;
    int32 timer;
};

// Object Struct
extern ObjectLRZ1Outro *LRZ1Outro;

// Standard Entity Events
void LRZ1Outro_Update(void);
void LRZ1Outro_LateUpdate(void);
void LRZ1Outro_StaticUpdate(void);
void LRZ1Outro_Draw(void);
void LRZ1Outro_Create(void* data);
void LRZ1Outro_StageLoad(void);
void LRZ1Outro_EditorDraw(void);
void LRZ1Outro_EditorLoad(void);
void LRZ1Outro_Serialize(void);

// Extra Entity Functions
void LRZ1Outro_StartCutscene(void);

bool32 LRZ1Outro_CutsceneAct1_Unknown1(EntityCutsceneSeq *host);
bool32 LRZ1Outro_CutsceneAct2_Unknown1(EntityCutsceneSeq *host);
bool32 LRZ1Outro_CutsceneAct1_Unknown2(EntityCutsceneSeq *host);
bool32 LRZ1Outro_CutsceneAct1_Unknown3(EntityCutsceneSeq *host);
bool32 LRZ1Outro_CutsceneAct1_Unknown4(EntityCutsceneSeq *host);

bool32 LRZ1Outro_CutsceneAct2_Unknown2(EntityCutsceneSeq *host);
bool32 LRZ1Outro_CutsceneAct2_Unknown3(EntityCutsceneSeq *host);

#endif //!OBJ_LRZ1OUTRO_H
