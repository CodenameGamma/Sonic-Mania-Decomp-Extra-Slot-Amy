#ifndef OBJ_SHURIKEN_H
#define OBJ_SHURIKEN_H

#include "SonicMania.h"

// Object Class
struct ObjectShuriken {
    RSDK_OBJECT
    Hitbox hitboxShooter;
    Hitbox hitboxRange;
    Hitbox hitboxArrow;
    Hitbox hitboxPlatform;
    int32 timerGroups[255];
    uint16 aniFrames;
    uint16 sfxArrowLaunch;
    uint16 sfxArrowHit;
};

// Entity Class
struct EntityShuriken {
    RSDK_ENTITY
    StateMachine(state);
    uint16 timer;
    uint16 dropTimer;
    uint8 activePlayers;
    Animator animator;
    uint16 delay;
    bool32 timerMode;
    uint16 interval;
    uint16 intervalOffset;
    uint8 timerGroup;
};

// Object Struct
extern ObjectShuriken *Shuriken;

// Standard Entity Events
void Shuriken_Update(void);
void Shuriken_LateUpdate(void);
void Shuriken_StaticUpdate(void);
void Shuriken_Draw(void);
void Shuriken_Create(void* data);
void Shuriken_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void Shuriken_EditorDraw(void);
void Shuriken_EditorLoad(void);
#endif
void Shuriken_Serialize(void);

// Extra Entity Functions
void Shuriken_CheckPlayerCollisions(void);
void Shuriken_HandleSolidCollisions(void);

// Shooter States
void Shuriken_State_Setup(void);
void Shuriken_State_WaitForActive(void);
void Shuriken_State_CheckPlayerInRange(void);
void Shuriken_State_ShootDelay(void);
void Shuriken_State_FireArrow(void);
void Shuriken_State_ArrowFired(void);
void Shuriken_State_Deactivate(void);

// Arrow States
void Shuriken_State_ArrowSetup(void);
void Shuriken_State_ArrowInAir(void);
void Shuriken_State_ArrowOnWall(void);
void Shuriken_State_ArrowFall(void);
#if RETRO_USE_PLUS
void Shuriken_State_ArrowDebris(void);
#endif

#endif //!OBJ_SHURIKEN_H
