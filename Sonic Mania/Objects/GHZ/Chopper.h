#ifndef OBJ_CHOPPER_H
#define OBJ_CHOPPER_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    Hitbox hitbox;
    Hitbox hitbox2;
    Hitbox hitbox3;
    Hitbox hitbox4;
    uint16 spriteIndex;
} ObjectChopper;

// Entity Class
typedef struct {
    RSDK_ENTITY
    StateMachine(state);
    uint8 type;
    uint16 timer;
    bool32 charge;
    Vector2 startPos;
    uint8 startDir;
    Animator animator;
} EntityChopper;

// Object Struct
extern ObjectChopper *Chopper;

// Standard Entity Events
void Chopper_Update(void);
void Chopper_LateUpdate(void);
void Chopper_StaticUpdate(void);
void Chopper_Draw(void);
void Chopper_Create(void* data);
void Chopper_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void Chopper_EditorDraw(void);
void Chopper_EditorLoad(void);
#endif
void Chopper_Serialize(void);

// Extra Entity Functions
void Chopper_DebugDraw(void);
void Chopper_DebugSpawn(void);

void Chopper_CheckOnScreen(void);
void Chopper_Unknown3(void);
void Chopper_Unknown4(void);
void Chopper_Unknown5(void);
void Chopper_Unknown6(void);
void Chopper_Unknown7(void);
void Chopper_CheckPlayerCollisions(void);
void Chopper_CheckPlayerCollisions2(void);

#endif //!OBJ_CHOPPER_H
