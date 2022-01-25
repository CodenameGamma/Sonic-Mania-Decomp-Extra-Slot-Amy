#ifndef OBJ_SHIVERSAW_H
#define OBJ_SHIVERSAW_H

#include "SonicMania.h"

#define Shiversaw_SawCount (2)

// Object Class
struct ObjectShiversaw {
    RSDK_OBJECT
    uint8 health;
    uint8 invincibilityTimer;
    int32 timer;
    uint16 aniFrames;
    Hitbox hitbox1;
    Hitbox hitbox2;
    Hitbox hitboxR;
    Hitbox hitboxB;
    Hitbox hitboxT;
    Hitbox hitboxL;
#if RETRO_USE_PLUS
    Hitbox hitbox7;
    Hitbox hitbox8;
#endif
    uint16 sfxHit;
    uint16 sfxExplosion2;
    uint16 sfxTargeting;
    uint16 sfxRocketJet;
    uint16 sfxExplosion3;
};

// Entity Class
struct EntityShiversaw {
    RSDK_ENTITY
    StateMachine(state);
    StateMachine(stateSaw[Shiversaw_SawCount]);
    uint8 sawID;
    int32 timer;
    int32 sawTimers[Shiversaw_SawCount];
    int32 sawRadiusVel[Shiversaw_SawCount];
    int32 unused1;
    int32 unused2;
    int32 startY;
    Vector2 origin;
    int32 sawRadius[Shiversaw_SawCount];
    int32 sawAngles[Shiversaw_SawCount];
    int32 sawTargetAngles[Shiversaw_SawCount];
    Vector2 sawPos[Shiversaw_SawCount];
    int32 storedVelX;
    int32 circleRadius;
    Animator tanksAnimator;
    Animator bellowsAnimator;
    Animator connectorAnimator;
    Animator armAnimator;
    Animator spinnerAnimator;
    Animator sawAnimator[Shiversaw_SawCount];
};

// Object Struct
extern ObjectShiversaw *Shiversaw;

// Standard Entity Events
void Shiversaw_Update(void);
void Shiversaw_LateUpdate(void);
void Shiversaw_StaticUpdate(void);
void Shiversaw_Draw(void);
void Shiversaw_Create(void* data);
void Shiversaw_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void Shiversaw_EditorDraw(void);
void Shiversaw_EditorLoad(void);
#endif
void Shiversaw_Serialize(void);

// Extra Entity Functions
bool32 Shiversaw_CheckSawHit(EntityPlayer *player, int32 sawID);
void Shiversaw_CheckPlayerCollisions(void);
void Shiversaw_SetupSawPos(void);
void Shiversaw_Hit(void);
void Shiversaw_Explode(void);
void Shiversaw_CheckBoxCollisions(EntityPlayer *player);

void Shiversaw_State_Entry(void);
void Shiversaw_State_SetupBounds(void);
void Shiversaw_State_EnterShiversaw(void);
void Shiversaw_State_HitRecoil_Tutorial(void);
void Shiversaw_State_Idle(void);
void Shiversaw_State_MoveToPlayer(void);
void Shiversaw_State_HitRecoil(void);
void Shiversaw_State_Destroyed(void);
void Shiversaw_State_StageFinishCB(void);
void Shiversaw_State_Explode(void);
void Shiversaw_State_Explode_NoTransition(void);
void Shiversaw_State_DropSignPost(void);

void Shiversaw_CheckCrateCollisions(void);
void Shiversaw_HandleSawMovement(int32 speed);

void Shiversaw_StateSaw_Active(void);
void Shiversaw_StateSaw_Targeting(void);
void Shiversaw_StateSaw_Extend(void);
void Shiversaw_StateSaw_Retract(void);
void Shiversaw_StateSaw_Destroyed(void);
void Shiversaw_StateSaw_Form(void);
void Shiversaw_StateSaw_Setup(void);

void Shiversaw_ProcessSawMovement(int32 sawID);

void Shiversaw_StateDust_Debris(void);

#endif //!OBJ_SHIVERSAW_H
