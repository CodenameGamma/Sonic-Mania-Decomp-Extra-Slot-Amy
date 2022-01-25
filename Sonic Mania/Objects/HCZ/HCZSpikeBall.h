#ifndef OBJ_HCZSPIKEBALL_H
#define OBJ_HCZSPIKEBALL_H

#include "SonicMania.h"

// Object Class
struct ObjectHCZSpikeBall {
    RSDK_OBJECT
    uint16 aniFrames;
};

// Entity Class
struct EntityHCZSpikeBall {
    RSDK_ENTITY
    int32 field_58;
    Vector2 field_5C;
    int32 field_64;
    int32 field_68;
    EntityHangConveyor *conveyor;
    Hitbox hitbox;
    Animator animator;
};

// Object Struct
extern ObjectHCZSpikeBall *HCZSpikeBall;

// Standard Entity Events
void HCZSpikeBall_Update(void);
void HCZSpikeBall_LateUpdate(void);
void HCZSpikeBall_StaticUpdate(void);
void HCZSpikeBall_Draw(void);
void HCZSpikeBall_Create(void* data);
void HCZSpikeBall_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void HCZSpikeBall_EditorDraw(void);
void HCZSpikeBall_EditorLoad(void);
#endif
void HCZSpikeBall_Serialize(void);

// Extra Entity Functions
void HCZSpikeBall_HandleConveyorSetup(void);
void HCZSpikeBall_Unknown2(void);
void HCZSpikeBall_HandlePlayerInteractions(void);
void HCZSpikeBall_Unknown4(void);

#endif //!OBJ_HCZSPIKEBALL_H
