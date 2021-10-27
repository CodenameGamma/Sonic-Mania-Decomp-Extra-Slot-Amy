#ifndef OBJ_ZIPLINE_H
#define OBJ_ZIPLINE_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    uint16 aniFrames;
    Hitbox hitbox;
} ObjectZipLine;

// Entity Class
typedef struct {
    RSDK_ENTITY
    StateMachine(state);
    uint8 activePlayers;
    Animator animator;
    int32 length;
    Vector2 startPos;
    Vector2 endPos;
    Vector2 handlePos;
    uint8 grabDelay[PLAYER_MAX];
    Vector2 field_98;
    int32 field_A0;
} EntityZipLine;

// Object Struct
extern ObjectZipLine *ZipLine;

// Standard Entity Events
void ZipLine_Update(void);
void ZipLine_LateUpdate(void);
void ZipLine_StaticUpdate(void);
void ZipLine_Draw(void);
void ZipLine_Create(void* data);
void ZipLine_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void ZipLine_EditorDraw(void);
void ZipLine_EditorLoad(void);
#endif
void ZipLine_Serialize(void);

// Extra Entity Functions
void ZipLine_Unknown1(void);
void ZipLine_Unknown2(void);
Vector2 ZipLine_Unknown3(void);
void ZipLine_Unknown4(void);

#endif //!OBJ_ZIPLINE_H
