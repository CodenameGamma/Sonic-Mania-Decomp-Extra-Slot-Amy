#ifndef OBJ_STARPOST_H
#define OBJ_STARPOST_H

#include "SonicMania.h"

// Object Class
struct ObjectStarPost {
    RSDK_OBJECT
    Hitbox hitbox;
    bool32 hasAchievement;
    Vector2 playerPositions[PLAYER_MAX];
    uint8 playerDirections[PLAYER_MAX];
    uint16 postIDs[PLAYER_MAX];
    uint8 storedMinutes;
    uint8 storedSeconds;
    uint8 storedMS;
    uint8 activePlayers;
    uint16 aniFrames;
    uint16 sfxStarPost;
    uint16 sfxWarp;
};

// Entity Class
struct EntityStarPost {
    RSDK_ENTITY
    StateMachine(state);
    int32 id;
    bool32 vsRemove;
    int32 ballSpeed;
    int32 timer;
    int32 starTimer;
    int32 bonusStageID;
    int32 starAngleX;
    int32 starAngleY;
    int32 starRadius;
    Vector2 ballPos;
    Animator poleAnimator;
    Animator ballAnimator;
    Animator starAnimator;
    Hitbox starHitbox;
    uint8 activated;
};

// Object Struct
extern ObjectStarPost *StarPost;

// Standard Entity Events
void StarPost_Update(void);
void StarPost_LateUpdate(void);
void StarPost_StaticUpdate(void);
void StarPost_Draw(void);
void StarPost_Create(void* data);
void StarPost_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void StarPost_EditorDraw(void);
void StarPost_EditorLoad(void);
#endif
void StarPost_Serialize(void);

// Extra Entity Functions
void StarPost_DebugDraw(void);
void StarPost_DebugSpawn(void);
void StarPost_ResetStarPosts(void);
void StarPost_CheckBonusStageEntry(void);
void StarPost_CheckCollisions(void);

// States
void StarPost_State_Idle(void);
void StarPost_State_BallSpin(void);

#endif //!OBJ_STARPOST_H
