#ifndef OBJ_ROCKEMSOCKEM_H
#define OBJ_ROCKEMSOCKEM_H

#include "SonicMania.h"

//Extras
#define RockemSockem_coilCount 6

// Object Class
struct ObjectRockemSockem {
    RSDK_OBJECT
    Hitbox hitbox;
    uint16 aniFrames;
    uint16 sfxRockemSockem;
};

// Entity Class
struct EntityRockemSockem {
    RSDK_ENTITY
    int32 timer;
    int32 timer2;
    Vector2 drawPos;
    Vector2 startPos;
    Vector2 jointPositions[RockemSockem_coilCount];
    Vector2 unknownPositions[RockemSockem_coilCount];
    int32 jointRotations[RockemSockem_coilCount];
    int32 field_E8;
    int32 field_EC;
    Animator baseAnimator;
    Animator jointAnimator;
    Animator ballAnimator;
};

// Object Struct
extern ObjectRockemSockem *RockemSockem;

// Standard Entity Events
void RockemSockem_Update(void);
void RockemSockem_LateUpdate(void);
void RockemSockem_StaticUpdate(void);
void RockemSockem_Draw(void);
void RockemSockem_Create(void* data);
void RockemSockem_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void RockemSockem_EditorDraw(void);
void RockemSockem_EditorLoad(void);
#endif
void RockemSockem_Serialize(void);

// Extra Entity Functions
void RockemSockem_Unknown1(void);

#endif //!OBJ_ROCKEMSOCKEM_H
