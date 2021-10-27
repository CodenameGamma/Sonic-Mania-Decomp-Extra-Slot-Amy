#ifndef OBJ_MSORB_H
#define OBJ_MSORB_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    uint16 aniFrames;
    Hitbox hitbox;
} ObjectMSOrb;

// Entity Class
typedef struct {
    RSDK_ENTITY
    StateMachine(state);
    StateMachine(stateDraw);
    Animator animator;
    int field_78;
    int field_7C;
} EntityMSOrb;

// Object Struct
extern ObjectMSOrb *MSOrb;

// Standard Entity Events
void MSOrb_Update(void);
void MSOrb_LateUpdate(void);
void MSOrb_StaticUpdate(void);
void MSOrb_Draw(void);
void MSOrb_Create(void* data);
void MSOrb_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void MSOrb_EditorDraw(void);
void MSOrb_EditorLoad(void);
#endif
void MSOrb_Serialize(void);

// Extra Entity Functions
void MSOrb_CheckPlayerCollisions(void);
void MSOrb_State_Orb(void);
void MSOrb_StateDraw_Orb(void);

#endif //!OBJ_MSORB_H
