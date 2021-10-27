#ifndef OBJ_DOORTRIGGER_H
#define OBJ_DOORTRIGGER_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    Hitbox hitboxes[2];
    uint16 aniFrames;
    uint16 sfxShatter;
} ObjectDoorTrigger;

// Entity Class
typedef struct {
    RSDK_ENTITY
    uint8 orientation;
    uint8 id;
    Animator animator1;
    Animator animator2;
} EntityDoorTrigger;

// Object Struct
extern ObjectDoorTrigger *DoorTrigger;

// Standard Entity Events
void DoorTrigger_Update(void);
void DoorTrigger_LateUpdate(void);
void DoorTrigger_StaticUpdate(void);
void DoorTrigger_Draw(void);
void DoorTrigger_Create(void* data);
void DoorTrigger_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void DoorTrigger_EditorDraw(void);
void DoorTrigger_EditorLoad(void);
#endif
void DoorTrigger_Serialize(void);

// Extra Entity Functions


#endif //!OBJ_DOORTRIGGER_H
