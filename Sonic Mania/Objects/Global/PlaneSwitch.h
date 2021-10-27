#ifndef OBJ_PLANESWITCH_H
#define OBJ_PLANESWITCH_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    uint16 spriteIndex;
} ObjectPlaneSwitch;

// Entity Class
typedef struct {
    RSDK_ENTITY
    int32 flags;
    int32 size;
    bool32 onPath;
    int32 negAngle;
    Animator animator;
} EntityPlaneSwitch;

// Object Struct
extern ObjectPlaneSwitch *PlaneSwitch;

// Standard Entity Events
void PlaneSwitch_Update(void);
void PlaneSwitch_LateUpdate(void);
void PlaneSwitch_StaticUpdate(void);
void PlaneSwitch_Draw(void);
void PlaneSwitch_Create(void* data);
void PlaneSwitch_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void PlaneSwitch_EditorDraw(void);
void PlaneSwitch_EditorLoad(void);
#endif
void PlaneSwitch_Serialize(void);

// Extra Entity Functions


#endif //!OBJ_PLANESWITCH_H
