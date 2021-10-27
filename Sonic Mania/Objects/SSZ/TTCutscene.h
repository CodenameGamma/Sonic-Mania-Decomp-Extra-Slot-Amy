#ifndef OBJ_TTCUTSCENE_H
#define OBJ_TTCUTSCENE_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    int32 field_4;
    EntityFXFade *fxFade;
} ObjectTTCutscene;

// Entity Class
typedef struct {
    MANIA_CUTSCENE_BASE
} EntityTTCutscene;

// Object Struct
extern ObjectTTCutscene *TTCutscene;

// Standard Entity Events
void TTCutscene_Update(void);
void TTCutscene_LateUpdate(void);
void TTCutscene_StaticUpdate(void);
void TTCutscene_Draw(void);
void TTCutscene_Create(void* data);
void TTCutscene_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void TTCutscene_EditorDraw(void);
void TTCutscene_EditorLoad(void);
#endif
void TTCutscene_Serialize(void);

// Extra Entity Functions
void TTCutscene_StartCutscene(void);
void TTCutscene_SkipCB(void);

bool32 TTCutscene_CutsceneState_Setup(EntityCutsceneSeq *host);
bool32 TTCutscene_CutsceneState_FlyIn(EntityCutsceneSeq *host);
bool32 TTCutscene_CutsceneState_Wait(EntityCutsceneSeq *host);
bool32 TTCutscene_CutsceneState_FlyOut(EntityCutsceneSeq *host);
bool32 TTCutscene_CutsceneState_NextScene(EntityCutsceneSeq *host);

#endif //!OBJ_TTCUTSCENE_H
