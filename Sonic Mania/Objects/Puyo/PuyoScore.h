#ifndef OBJ_PUYOSCORE_H
#define OBJ_PUYOSCORE_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    uint16 aniFrames;
} ObjectPuyoScore;

// Entity Class
typedef struct {
    RSDK_ENTITY
    StateMachine(state);
    int32 playerID;
    bool32 counter;
    bool32 flag;
    int32 score;
    Animator animator;
} EntityPuyoScore;

// Object Struct
extern ObjectPuyoScore *PuyoScore;

// Standard Entity Events
void PuyoScore_Update(void);
void PuyoScore_LateUpdate(void);
void PuyoScore_StaticUpdate(void);
void PuyoScore_Draw(void);
void PuyoScore_Create(void* data);
void PuyoScore_StageLoad(void);
void PuyoScore_EditorDraw(void);
void PuyoScore_EditorLoad(void);
void PuyoScore_Serialize(void);

// Extra Entity Functions
void PuyoScore_DrawScore(int score);

#endif //!OBJ_PUYOSCORE_H
