#ifndef OBJ_SSZ1OUTRO_H
#define OBJ_SSZ1OUTRO_H

#include "SonicMania.h"

// Object Class
struct ObjectSSZ1Outro {
    RSDK_OBJECT
    int32 field_4;
    EntityFXFade *fxFade;
    EntityRTeleporter *teleporter;
};

// Entity Class
struct EntitySSZ1Outro {
    RSDK_ENTITY
    Vector2 size;
    bool32 activated;
    int32 field_64;
    int32 field_68;
    int32 field_6C;
    int32 field_70;
    int32 field_74;
    int32 field_78;
    Hitbox hitbox;
};

// Object Struct
extern ObjectSSZ1Outro *SSZ1Outro;

// Standard Entity Events
void SSZ1Outro_Update(void);
void SSZ1Outro_LateUpdate(void);
void SSZ1Outro_StaticUpdate(void);
void SSZ1Outro_Draw(void);
void SSZ1Outro_Create(void* data);
void SSZ1Outro_StageLoad(void);
void SSZ1Outro_EditorDraw(void);
void SSZ1Outro_EditorLoad(void);
void SSZ1Outro_Serialize(void);

// Extra Entity Functions
bool32 SSZ1Outro_Cutscene_TimeWarpRunway(EntityCutsceneSeq *host);
bool32 SSZ1Outro_Cutscene_TimeWarp(EntityCutsceneSeq *host);

void SSZ1Outro_DestroyHotaru(Entity *entity);
void SSZ1Outro_DestroyLeftoverHotarus(void);

#endif //!OBJ_SSZ1OUTRO_H
