#ifndef OBJ_VALVE_H
#define OBJ_VALVE_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    Hitbox hitbox;
    uint16 aniFrames;
    uint16 sfxClick;
} ObjectValve;

// Entity Class
typedef struct {
    RSDK_ENTITY
    Animator animator1;
    Animator animator2;
} EntityValve;

// Object Struct
extern ObjectValve *Valve;

// Standard Entity Events
void Valve_Update(void);
void Valve_LateUpdate(void);
void Valve_StaticUpdate(void);
void Valve_Draw(void);
void Valve_Create(void* data);
void Valve_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void Valve_EditorDraw(void);
void Valve_EditorLoad(void);
#endif
void Valve_Serialize(void);

// Extra Entity Functions


#endif //!OBJ_VALVE_H
