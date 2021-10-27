#ifndef OBJ_GRABBER_H
#define OBJ_GRABBER_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    Hitbox hitbox1;
    Hitbox hitbox2;
    Hitbox hitboxRange;
    uint16 aniFrames;
    uint16 sfxGrab;
    uint16 sfxDestroy;
} ObjectGrabber;

// Entity Class
typedef struct {
    RSDK_ENTITY
    StateMachine(state);
    Animator animator1;
    Animator animator2;
    Animator animator3;
    Entity *grabbedPlayer;
    uint16 field_A8;
    uint16 struggleTimer;
    uint16 struggleFlags;
    uint16 timer;
    Vector2 startPos;
    uint8 startDir;
    int32 field_BC;
    int32 explodeTimer;
} EntityGrabber;

// Object Struct
extern ObjectGrabber *Grabber;

// Standard Entity Events
void Grabber_Update(void);
void Grabber_LateUpdate(void);
void Grabber_StaticUpdate(void);
void Grabber_Draw(void);
void Grabber_Create(void* data);
void Grabber_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void Grabber_EditorDraw(void);
void Grabber_EditorLoad(void);
#endif
void Grabber_Serialize(void);

// Extra Entity Functions
void Grabber_DebugDraw(void);
void Grabber_DebugSpawn(void);

void Grabber_CheckPlayerCollisions(void);
void Grabber_CheckOnScreen(void);
void Grabber_HandleExplode(void);

void Grabber_State_Setup(void);
void Grabber_State_Unknown5(void);
void Grabber_State_Unknown2(void);
void Grabber_State_Unknown6(void);
void Grabber_State_Unknown3(void);
void Grabber_State_Unknown4(void);
void Grabber_State_Unknown7(void);
void Grabber_State_Struggle(void);
void Grabber_State_Unknown9(void);

#endif //!OBJ_GRABBER_H
