#ifndef OBJ_SYRINGE_H
#define OBJ_SYRINGE_H

#include "SonicMania.h"

// Object Class
struct ObjectSyringe {
    RSDK_OBJECT
    Hitbox hitboxBody;
    Hitbox hitboxHandle;
    uint16 aniFrames;
    uint16 sfxBloop;
};

// Entity Class
struct EntitySyringe {
    RSDK_ENTITY
    StateMachine(state); // unused
    int32 type;
    int32 offsetY;
    bool32 activated;
    colour colour;
    uint8 tag;
    Animator animator1;
    Animator animator2;
};

// Object Struct
extern ObjectSyringe *Syringe;

// Standard Entity Events
void Syringe_Update(void);
void Syringe_LateUpdate(void);
void Syringe_StaticUpdate(void);
void Syringe_Draw(void);
void Syringe_Create(void* data);
void Syringe_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void Syringe_EditorDraw(void);
void Syringe_EditorLoad(void);
#endif
void Syringe_Serialize(void);

// Extra Entity Functions


#endif //!OBJ_SYRINGE_H
