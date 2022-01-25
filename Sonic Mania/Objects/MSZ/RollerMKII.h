#ifndef OBJ_ROLLERMKII_H
#define OBJ_ROLLERMKII_H

#include "SonicMania.h"

// Object Class
struct ObjectRollerMKII {
    RSDK_OBJECT
    Hitbox hitbox1;
    Hitbox hitbox2;
    Hitbox hitbox3;
    Hitbox hitbox4;
    Hitbox hitboxInner;
    Hitbox hitboxOuter;
    uint16 aniFrames;
    uint16 sfxBumper;
    uint16 sfxJump;
    uint16 sfxDropDash;
    uint16 sfxRelease;
    uint16 sfxSkidding;
};

// Entity Class
struct EntityRollerMKII {
    RSDK_ENTITY
    StateMachine(state);
    Vector2 startPos;
    uint8 startDir;
    uint8 timer;
    bool32 field_68;
    EntityPlayer* playerPtr;
    Animator animator;
};

// Object Struct
extern ObjectRollerMKII *RollerMKII;

// Standard Entity Events
void RollerMKII_Update(void);
void RollerMKII_LateUpdate(void);
void RollerMKII_StaticUpdate(void);
void RollerMKII_Draw(void);
void RollerMKII_Create(void* data);
void RollerMKII_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void RollerMKII_EditorDraw(void);
void RollerMKII_EditorLoad(void);
#endif
void RollerMKII_Serialize(void);

// Extra Entity Functions
void RollerMKII_DebugSpawn(void);
void RollerMKII_DebugDraw(void);

void RollerMKII_CheckOnScreen(void);

void RollerMKII_CheckPlayerCollisions(void);
void RollerMKII_CheckPlayerCollisions_Rolling(void);
int RollerMKII_HandleObjectCollisions(Entity *otherEntity, Hitbox *hitbox);
bool32 RollerMKII_HandlePlatformCollisions(EntityPlatform *platform);
void RollerMKII_HandleCollisions(void);

void RollerMKII_State_Setup(void);
void RollerMKII_State_Unknown1(void);
void RollerMKII_State_Unknown2(void);
void RollerMKII_State_Unknown3(void);
void RollerMKII_State_Unknown4(void);
void RollerMKII_State_Unknown5(void);
void RollerMKII_State_Unknown6(void);

#endif //!OBJ_ROLLERMKII_H
