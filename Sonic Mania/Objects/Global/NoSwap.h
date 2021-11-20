#ifndef OBJ_NOSWAP_H
#define OBJ_NOSWAP_H

#include "SonicMania.h"

#if RETRO_USE_PLUS
// Object Class
typedef struct {
    RSDK_OBJECT
    int32 counter;
    uint16 aniFrames; // unused in-game (likely used in-editor)
} ObjectNoSwap;

// Entity Class
typedef struct {
    RSDK_ENTITY
    Vector2 size;
    bool32 always;
    bool32 noDeathSwap;
    Hitbox hitbox;
    Vector2 playerPos;
    Animator animator1; // unused in-game (likely used in-editor)
    Animator animator2; // unused in-game (likely used in-editor)
} EntityNoSwap;

// Object Struct
extern ObjectNoSwap *NoSwap;

// Standard Entity Events
void NoSwap_Update(void);
void NoSwap_LateUpdate(void);
void NoSwap_StaticUpdate(void);
void NoSwap_Draw(void);
void NoSwap_Create(void* data);
void NoSwap_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void NoSwap_EditorDraw(void);
void NoSwap_EditorLoad(void);
#endif
void NoSwap_Serialize(void);

// Extra Entity Functions
#endif

#endif //!OBJ_NOSWAP_H
