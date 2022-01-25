#ifndef OBJ_REAGENT_H
#define OBJ_REAGENT_H

#include "SonicMania.h"

// Object Class
struct ObjectReagent {
    RSDK_OBJECT
    Hitbox hitbox;
    uint16 aniFrames;
    uint16 sfxLand;
};

// Entity Class
struct EntityReagent {
    RSDK_ENTITY
    StateMachine(state);
    int32 type;
    int32 timer;
    int32 startX;
    int32 unused;
    int32 r;
    int32 g;
    int32 b;
    Animator animator;
};

// Object Struct
extern ObjectReagent *Reagent;

// Standard Entity Events
void Reagent_Update(void);
void Reagent_LateUpdate(void);
void Reagent_StaticUpdate(void);
void Reagent_Draw(void);
void Reagent_Create(void* data);
void Reagent_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void Reagent_EditorDraw(void);
void Reagent_EditorLoad(void);
#endif
void Reagent_Serialize(void);

// Extra Entity Functions
void Reagent_State_CheckPoolCollisions(void);
void Reagent_State_ChangingPoolType(void);

#endif //!OBJ_REAGENT_H
