#ifndef OBJ_DRAGONFLY_H
#define OBJ_DRAGONFLY_H

#include "SonicMania.h"

#define Dragonfly_SpineCount (6)

// Object Class
struct ObjectDragonfly {
    RSDK_OBJECT
    Hitbox hitboxBadnik;
    Hitbox hitboxSpine;
    uint16 aniFrames;
};

// Entity Class
struct EntityDragonfly {
    RSDK_ENTITY
    StateMachine(state);
    uint8 dir;
    uint8 dist;
    uint8 speed;
    Vector2 positions[Dragonfly_SpineCount];
    uint8 directions[Dragonfly_SpineCount];
    Vector2 spawnPos;
    Animator animator;
    Animator wingAnimator;
    Animator bodyAnimator;
};

// Object Struct
extern ObjectDragonfly *Dragonfly;

// Standard Entity Events
void Dragonfly_Update(void);
void Dragonfly_LateUpdate(void);
void Dragonfly_StaticUpdate(void);
void Dragonfly_Draw(void);
void Dragonfly_Create(void* data);
void Dragonfly_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void Dragonfly_EditorDraw(void);
void Dragonfly_EditorLoad(void);
#endif
void Dragonfly_Serialize(void);

void Dragonfly_DebugDraw(void);
void Dragonfly_DebugSpawn(void);

// Extra Entity Functions
void Dragonfly_CheckPlayerCollisions(void);

void Dragonfly_State_Setup(void);
void Dragonfly_State_Move(void);
void Dragonfly_State_Debris(void);

#endif //!OBJ_DRAGONFLY_H
