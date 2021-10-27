#ifndef OBJ_TWISTEDTUBES_H
#define OBJ_TWISTEDTUBES_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    Hitbox hitbox1;
    Hitbox hitbox2;
    Hitbox hitbox3;
    int32 playerFlags[4];
    uint16 aniFrames;
    uint16 sfxTravel;
} ObjectTwistedTubes;

// Entity Class
typedef struct {
    RSDK_ENTITY
    StateMachine(state);
    uint8 height;
    int32 timer;
    EntityPlayer *playerPtr;
    Animator animator1;
    Animator animator2;
    Animator animator3;
    Animator animator4;
    Animator animator5;
} EntityTwistedTubes;

// Object Struct
extern ObjectTwistedTubes *TwistedTubes;

// Standard Entity Events
void TwistedTubes_Update(void);
void TwistedTubes_LateUpdate(void);
void TwistedTubes_StaticUpdate(void);
void TwistedTubes_Draw(void);
void TwistedTubes_Create(void* data);
void TwistedTubes_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void TwistedTubes_EditorDraw(void);
void TwistedTubes_EditorLoad(void);
#endif
void TwistedTubes_Serialize(void);

// Extra Entity Functions
void TwistedTubes_HandleInteractions(void);

void TwistedTubes_Unknown2(void);
void TwistedTubes_Unknown3(void);
void TwistedTubes_Unknown4(void);
void TwistedTubes_Unknown5(void);
void TwistedTubes_Unknown6(void);
void TwistedTubes_Unknown7(void);
void TwistedTubes_Unknown8(void);

#endif //!OBJ_TWISTEDTUBES_H
