#ifndef OBJ_ICEBOMBA_H
#define OBJ_ICEBOMBA_H

#include "SonicMania.h"

// Object Class
struct ObjectIceBomba {
	RSDK_OBJECT
    Hitbox hitboxBadnik;
    Hitbox hitboxBomb;
    Hitbox hitboxRange;
	uint16 aniFrames;
	uint16 sfxExplosion;
	uint16 sfxFreeze;
};

// Entity Class
struct EntityIceBomba {
	RSDK_ENTITY
    StateMachine(state);
    uint8 dir;
    uint16 dist;
    uint16 dip;
    uint8 spawnDir;
    Vector2 spawnPos;
    uint32 spawnDist;
    Animator animator;
    Animator wingAnimator;
    Animator bombAnimator;
};

// Object Struct
extern ObjectIceBomba *IceBomba;

// Standard Entity Events
void IceBomba_Update(void);
void IceBomba_LateUpdate(void);
void IceBomba_StaticUpdate(void);
void IceBomba_Draw(void);
void IceBomba_Create(void* data);
void IceBomba_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void IceBomba_EditorDraw(void);
void IceBomba_EditorLoad(void);
#endif
void IceBomba_Serialize(void);

void IceBomba_DebugDraw(void);
void IceBomba_DebugSpawn(void);

// Extra Entity Functions
void IceBomba_CheckOnScreen(void);

void IceBomba_Fly_Collide(void);
void IceBomba_State_Setup(void);
void IceBomba_State_Flying(void);
void IceBomba_State_FlyTurn(void);
void IceBomba_State_FlyAway(void);

void IceBomba_State_Bomb(void);

#endif //!OBJ_ICEBOMBA_H
