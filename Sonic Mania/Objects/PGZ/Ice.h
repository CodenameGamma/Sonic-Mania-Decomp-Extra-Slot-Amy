#ifndef OBJ_ICE_H
#define OBJ_ICE_H

#include "SonicMania.h"

#if RETRO_USE_PLUS
typedef enum {
    ICEANI_ICEBLOCK,
    ICEANI_PLAYERBLOCK,
    ICEANI_PLAYERGLINT,
    ICEANI_PILLARBLOCK,
    ICEANI_PILLARGLINT,
    ICEANI_SHARD,
    ICEANI_LARGEGLINT,
    ICEANI_SMALLGLINT,
    ICEANI_PIECE,
    ICEANI_RINGS,
    ICEANI_SONICIDLE,
    ICEANI_SONICLEFT,
    ICEANI_SONICRIGHT,
    ICEANI_SONICSHAKE,
    ICEANI_SUPERSONICIDLE,
    ICEANI_SUPERSONICLEFT,
    ICEANI_SUPERSONICRIGHT,
    ICEANI_SUPERSONICSHAKE,
    ICEANI_TAILSIDLE,
    ICEANI_TAILSLEFT,
    ICEANI_TAILSRIGHT,
    ICEANI_TAILSSHAKE,
    ICEANI_KNUXIDLE,
    ICEANI_KNUXLEFT,
    ICEANI_KNUXRIGHT,
    ICEANI_KNUXSHAKE,
    ICEANI_MIGHTYIDLE,
    ICEANI_MIGHTYLEFT,
    ICEANI_MIGHTYRIGHT,
    ICEANI_MIGHTYSHAKE,
    ICEANI_RAYIDLE,
    ICEANI_RAYLEFT,
    ICEANI_RAYRIGHT,
    ICEANI_RAYSHAKE
}IceAni;
#else
typedef enum {
    ICEANI_ICEBLOCK,
    ICEANI_PLAYERBLOCK,
    ICEANI_RINGS,
    ICEANI_SONIC,
    ICEANI_TAILS,
    ICEANI_KNUX,
    ICEANI_PILLARBLOCK,
    ICEANI_PLAYERGLINT,
    ICEANI_PILLARGLINT,
    ICEANI_SHARD,
    ICEANI_LARGEGLINT,
    ICEANI_SMALLGLINT,
    ICEANI_PIECE
} IceAni;
#endif

// Object Class
typedef struct {
    RSDK_OBJECT
    uint16 aniFrames;
    Hitbox hitbox1;
    Hitbox hitbox2;
    int32 playerTimers[PLAYER_MAX];
    uint16 sfxFreeze;
    uint16 sfxLedgeBreak;
    uint16 sfxWindowShatter;
    uint16 sfxStruggle;
} ObjectIce;

// Entity Class
typedef struct {
    RSDK_ENTITY
    StateMachine(state);
    StateMachine(stateDraw);
    uint8 size;
    uint8 type;
    uint8 subType;
    uint8 subFlip;
    bool32 bottomSmash;
    bool32 knuxSmash;
    uint8 timer;
    uint8 animationID;
    Animator animator1;
    Animator animator2;
    Animator animator3;
    Animator animator4;
    Entity *playerPtr;
    Vector2 playerPos;
    Vector2 dwordDC;
    int32 dwordE4;
    Hitbox hitbox1;
    Hitbox hitbox2;
    Hitbox hitbox3;
} EntityIce;

// Object Struct
extern ObjectIce *Ice;

// Standard Entity Events
void Ice_Update(void);
void Ice_LateUpdate(void);
void Ice_StaticUpdate(void);
void Ice_Draw(void);
void Ice_Create(void *data);
void Ice_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void Ice_EditorDraw(void);
void Ice_EditorLoad(void);
#endif
void Ice_Serialize(void);

// Extra Entity Functions
void Ice_ZoneCB(void);

void Ice_FreezePlayer(void *p);
bool32 Ice_Unknown3(void);
bool32 Ice_Unknown4(void);
void Ice_State_FrozenPlayer(void);
void Ice_ShatterGenerator(int32 xr, int32 xy, int32 count, int32 velX, int32 velY, int32 a6);
void Ice_Unknown7(int32 velX, Entity *p, int32 velY);
void Ice_Unknown8(Entity *p);
Entity *Ice_Shatter(EntityIce *ice, int32 velX, int32 velY);
void Ice_TimeOverCB(void);

void Ice_Unknown11(void);
void Ice_State_Pillar(void);
void Ice_Unknown13(void);
void Ice_Unknown14(void);
void Ice_Unknown15(void);
void Ice_State_PlayerBlock(void);
void Ice_State_Shard(void);

void Ice_StateDraw_Unknown1(void);
void Ice_StateDraw_PlayerBlock(void);
void Ice_StateDraw_Pillar(void);
void Ice_StateDraw_Shard(void);

#endif //! OBJ_ICE_H
