#ifndef OBJ_FIREFLIES_H
#define OBJ_FIREFLIES_H

#include "SonicMania.h"

// Object Class
struct ObjectFireflies {
    RSDK_OBJECT
    uint16 aniFrames;
    int32 field_8;
};

// Entity Class
struct EntityFireflies {
    RSDK_ENTITY
    StateMachine(state);
    int32 field_5C;
    int32 timer;
    int32 field_64;
    int32 field_68;
    Animator animator;
    int32 field_84;
    int32 screenCount;
    int32 screenID;
    Vector2 pos1;
    Vector2 pos2;
    Vector2 pos3;
    Vector2 pos4;
};

// Object Struct
extern ObjectFireflies *Fireflies;

// Standard Entity Events
void Fireflies_Update(void);
void Fireflies_LateUpdate(void);
void Fireflies_StaticUpdate(void);
void Fireflies_Draw(void);
void Fireflies_Create(void* data);
void Fireflies_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void Fireflies_EditorDraw(void);
void Fireflies_EditorLoad(void);
#endif
void Fireflies_Serialize(void);

// Extra Entity Functions
void Fireflies_State_Unknown1(void);
void Fireflies_State_Unknown2(void);

#endif //!OBJ_FIREFLIES_H
