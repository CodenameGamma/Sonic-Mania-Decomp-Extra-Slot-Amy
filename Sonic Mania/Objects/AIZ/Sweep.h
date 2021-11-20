#ifndef OBJ_SWEEP_H
#define OBJ_SWEEP_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    Hitbox hitboxBadnik;
    Hitbox hitboxRange;
    Hitbox hitboxProjectile;
    uint16 aniFrames;
    uint16 sfxPon;
} ObjectSweep;

// Entity Class
typedef struct {
    RSDK_ENTITY
    StateMachine(state);
    StateMachine(stateStore);
    int32 timer;
    uint8 hasShot;
    Vector2 startPos;
    uint8 startDir;
    Entity *waterPtr;
    Animator animator;
} EntitySweep;

// Object Struct
extern ObjectSweep *Sweep;

// Standard Entity Events
void Sweep_Update(void);
void Sweep_LateUpdate(void);
void Sweep_StaticUpdate(void);
void Sweep_Draw(void);
void Sweep_Create(void* data);
void Sweep_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void Sweep_EditorDraw(void);
void Sweep_EditorLoad(void);
#endif
void Sweep_Serialize(void);

// Extra Entity Functions
void Sweep_DebugSpawn(void);
void Sweep_DebugDraw(void);

void Sweep_CheckOnScreen(void);
void Sweep_HandleInteractions(void);
void Sweep_CheckShoot(void);

void Sweep_State_Setup(void);
void Sweep_State_Idle(void);
void Sweep_State_Dash(void);
void Sweep_State_Stop(void);
void Sweep_State_FiredShot(void);
void Sweep_State_Turn(void);
void Sweep_State_Projectile(void);

#endif //!OBJ_SWEEP_H
