#ifndef OBJ_POHBEE_H
#define OBJ_POHBEE_H

#include "SonicMania.h"

// Object Class
struct ObjectPohBee {
    RSDK_OBJECT
    Hitbox hitbox;
    uint16 aniFrames;
};

// Entity Class
struct EntityPohBee {
    RSDK_ENTITY
    StateMachine(state);
    Vector2 startPos;
    uint8 startDir;
    int32 field_68;
    int32 field_6C;
    int32 field_70;
    int32 field_74;
    int32 field_78;
    int32 field_7C;
    Animator animator1;
    Animator animator2;
    Animator animator3;
    Animator animator4;
    Hitbox hitboxes[2];
    uint8 spikeCount;
    Vector2 amplitude;
    PlaneFilterTypes planeFilter;
};

// Object Struct
extern ObjectPohBee *PohBee;

// Standard Entity Events
void PohBee_Update(void);
void PohBee_LateUpdate(void);
void PohBee_StaticUpdate(void);
void PohBee_Draw(void);
void PohBee_Create(void* data);
void PohBee_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void PohBee_EditorDraw(void);
void PohBee_EditorLoad(void);
#endif
void PohBee_Serialize(void);

// Extra Entity Functions
void PohBee_DebugSpawn(void);
void PohBee_DebugDraw(void);

void PohBee_CheckOnScreen(void);
void PohBee_CheckPlayerCollisions(void);

void PohBee_DrawSprites(void);

Vector2 PohBee_GetSpikePos(uint8 spikeID, uint8 shift);
void PohBee_SetupHitboxes(void);

void PohBee_State_Setup(void);
void PohBee_State_Move(void);

#endif //!OBJ_POHBEE_H
