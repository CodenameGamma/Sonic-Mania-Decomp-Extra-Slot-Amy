#ifndef OBJ_SWITCHDOOR_H
#define OBJ_SWITCHDOOR_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    uint16 aniFrames;
    Hitbox hitbox;
} ObjectSwitchDoor;

// Entity Class
typedef struct {
    RSDK_ENTITY
    int32 go;
    bool32 reversible;
    bool32 field_60;
    Animator animator;
} EntitySwitchDoor;

// Object Struct
extern ObjectSwitchDoor *SwitchDoor;

// Standard Entity Events
void SwitchDoor_Update(void);
void SwitchDoor_LateUpdate(void);
void SwitchDoor_StaticUpdate(void);
void SwitchDoor_Draw(void);
void SwitchDoor_Create(void* data);
void SwitchDoor_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void SwitchDoor_EditorDraw(void);
void SwitchDoor_EditorLoad(void);
#endif
void SwitchDoor_Serialize(void);

// Extra Entity Functions
void SwitchDoor_DrawSprites(void);


#endif //!OBJ_SWITCHDOOR_H
