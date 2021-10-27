#ifndef OBJ_CONVEYORWHEEL_H
#define OBJ_CONVEYORWHEEL_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    uint16 aniFrames;
    Hitbox hitbox;
} ObjectConveyorWheel;

// Entity Class
typedef struct {
    RSDK_ENTITY
    Animator animator;
} EntityConveyorWheel;

// Object Struct
extern ObjectConveyorWheel *ConveyorWheel;

// Standard Entity Events
void ConveyorWheel_Update(void);
void ConveyorWheel_LateUpdate(void);
void ConveyorWheel_StaticUpdate(void);
void ConveyorWheel_Draw(void);
void ConveyorWheel_Create(void* data);
void ConveyorWheel_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void ConveyorWheel_EditorDraw(void);
void ConveyorWheel_EditorLoad(void);
#endif
void ConveyorWheel_Serialize(void);

// Extra Entity Functions


#endif //!OBJ_CONVEYORWHEEL_H
