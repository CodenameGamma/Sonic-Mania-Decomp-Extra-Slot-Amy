#ifndef OBJ_SP500MKII_H
#define OBJ_SP500MKII_H

#include "SonicMania.h"

// Object Class
struct ObjectSP500MkII {
    RSDK_OBJECT
    uint16 aniFrames;
    Hitbox hitboxButton;
    Hitbox hitboxBody;
    Hitbox hitboxSolid;
};

// Entity Class
struct EntitySP500MkII {
    RSDK_ENTITY
    StateMachine(state);
    uint8 start;
    uint8 yDir;
    int32 printRows;
    int32 length;
    int32 printRowID;
    int32 xDir;
    bool32 showGreenLight;
    bool32 showRedLight;
    bool32 buttonDown;
    bool32 isActive;
    uint8 activePlayers;
    int32 printDistance;
    Vector2 pos2;
    Vector2 pos;
    Vector2 printPos;
    Vector2 printMoveOffset;
    int32 timer;
    Animator animator;
};

// Object Struct
extern ObjectSP500MkII *SP500MkII;

// Standard Entity Events
void SP500MkII_Update(void);
void SP500MkII_LateUpdate(void);
void SP500MkII_StaticUpdate(void);
void SP500MkII_Draw(void);
void SP500MkII_Create(void *data);
void SP500MkII_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void SP500MkII_EditorDraw(void);
void SP500MkII_EditorLoad(void);
#endif
void SP500MkII_Serialize(void);

// Extra Entity Functions
void SP500MkII_DrawDebugOverlay(void);
void SP500MkII_DrawPrinter(void);
void SP500MkII_DrawRails(void);

void SP500MkII_CheckPlayerCollisions(void);
void SP500MkII_State_Setup(void);
void SP500MkII_State_CheckActivated(void);
void SP500MkII_State_PrintX(void);
void SP500MkII_State_PrintY(void);

#endif //! OBJ_SP500MKII_H
