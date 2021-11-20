#ifndef OBJ_TIMEATTACKGATE_H
#define OBJ_TIMEATTACKGATE_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    Hitbox hitbox;
    uint16 aniFrames;
    Entity *activeEntity;
    EntityPlayer *playerPtr;
    bool32 started;
    int32 isFinished;
    int32 unused1;
    bool32 suppressedTitlecard;
    bool32 debugEnabled;
    uint16 sfxSignpost;
    uint16 sfxTeleport;
    int32 teleportChannel;
    StateMachine(startCB);
    StateMachine(endCB);
} ObjectTimeAttackGate;

// Entity Class
typedef struct {
    RSDK_ENTITY
    StateMachine(state);
    StateMachine(stateDraw);
    bool32 finishLine;
    int32 extendTop;
    int32 extendBottom;
    Vector2 boundsSize;
    Vector2 boundsOffset;
    int32 topBoundary;
    int32 timer;
    bool32 hasFinished;
    int32 baseRotation;
    int32 spinSpeed;
    int32 spinTimer;
    int32 radius;
    int32 restartTimer;
    int32 fadeTimer;
    Vector2 playerPos;
    Animator baseAnimator;
    Animator topAnimator;
    Animator finAnimator;
} EntityTimeAttackGate;

// Object Struct
extern ObjectTimeAttackGate *TimeAttackGate;

// Standard Entity Events
void TimeAttackGate_Update(void);
void TimeAttackGate_LateUpdate(void);
void TimeAttackGate_StaticUpdate(void);
void TimeAttackGate_Draw(void);
void TimeAttackGate_Create(void* data);
void TimeAttackGate_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void TimeAttackGate_EditorDraw(void);
void TimeAttackGate_EditorLoad(void);
#endif
void TimeAttackGate_Serialize(void);

// Extra Entity Functions
void TimeAttackGate_HandleSpin(void);
void TimeAttackGate_HandleStart(void);
#if RETRO_USE_PLUS
void TimeAttackGate_Unknown1(void);
#endif
void TimeAttackGate_LeaderboardCB(int32 status);
void TimeAttackGate_CheckTouch(void);

void TimeAttackGate_State_Main(void);
void TimeAttackGate_State_Restarter(void);
void TimeAttackGate_State_Fadeout(void);

void TimeAttackGate_Draw_Main(void);
void TimeAttackGate_Draw_Restarter(void);

#endif //!OBJ_TIMEATTACKGATE_H
