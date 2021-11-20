#ifndef OBJ_SCHRODINGERSCAPSULE_H
#define OBJ_SCHRODINGERSCAPSULE_H

#include "SonicMania.h"

#if RETRO_USE_PLUS
// Object Class
typedef struct {
    RSDK_OBJECT
    uint16 aniFrames;
    uint16 sfxExplosion2;
    uint16 sfxExplosion3;
} ObjectSchrodingersCapsule;

// Entity Class
typedef struct {
    RSDK_ENTITY
    StateMachine(state);
    int32 timer;
    int32 buttonPos;
    // both of these 2 below go unused, but based on the similarities to Global/EggPrison, its likely to be these
    int32 notSolid;
    int32 checkTileCollisions;
    Hitbox hitboxSolid;
    Hitbox hitboxButton;
    Hitbox hitboxButtonTrigger;
    Animator mainAnimator;
    Animator glassAnimator;
    Animator buttonAnimator;
    Animator mightyAnimator;
    Animator rayAnimator;
} EntitySchrodingersCapsule;

// Object Struct
extern ObjectSchrodingersCapsule *SchrodingersCapsule;

// Standard Entity Events
void SchrodingersCapsule_Update(void);
void SchrodingersCapsule_LateUpdate(void);
void SchrodingersCapsule_StaticUpdate(void);
void SchrodingersCapsule_Draw(void);
void SchrodingersCapsule_Create(void* data);
void SchrodingersCapsule_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void SchrodingersCapsule_EditorDraw(void);
void SchrodingersCapsule_EditorLoad(void);
#endif
void SchrodingersCapsule_Serialize(void);

// Extra Entity Functions
void SchrodingersCapsule_Unknown1(void);
void SchrodingersCapsule_Unknown2(void);
void SchrodingersCapsule_Unknown3(void);
void SchrodingersCapsule_Unknown4(void);
void SchrodingersCapsule_Unknown5(void);

#endif

#endif //!OBJ_SCHRODINGERSCAPSULE_H
