#ifndef OBJ_CPZSETUP_H
#define OBJ_CPZSETUP_H

#include "SonicMania.h"

typedef enum {
    CPZ_BG_CPZ2,
    CPZ_BG_CPZ1,
} BGSwitchIDsCPZ;

typedef enum { CPZ_DECOR_WARNSIGN } DecorTypesCPZ;

// Object Class
struct ObjectCPZSetup {
    RSDK_OBJECT
    uint16 aniTiles;
    TileLayer *cpz1BG;
    TABLE(int32 deformation[64], { 1, 2, 1, 3, 1, 2, 2, 1, 2, 3, 1, 2, 1, 2, 0, 0, 2, 0, 3, 2, 2, 3, 2, 2, 1, 3, 0, 0, 1, 0, 1, 3,
                                 1, 2, 1, 3, 1, 2, 2, 1, 2, 3, 1, 2, 1, 2, 0, 0, 2, 0, 3, 2, 2, 3, 2, 2, 1, 3, 0, 0, 1, 0, 1, 3 });
    int32 palAnimFrameA;
    int32 palAnimFrameB;
    int32 palAnimFrameC;
    int32 palAnimFrameD;
    int32 palAnimFrameE;
    uint16 aniTileFrame;
    Entity* outro;
};

// Entity Class
struct EntityCPZSetup {
	RSDK_ENTITY
};

// Object Struct
extern ObjectCPZSetup *CPZSetup;

// Standard Entity Events
void CPZSetup_Update(void);
void CPZSetup_LateUpdate(void);
void CPZSetup_StaticUpdate(void);
void CPZSetup_Draw(void);
void CPZSetup_Create(void* data);
void CPZSetup_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void CPZSetup_EditorDraw(void);
void CPZSetup_EditorLoad(void);
#endif
void CPZSetup_Serialize(void);

// Extra Entity Functions
void CPZSetup_BGSwitchCB_Act2BG(void);
void CPZSetup_BGSwitchCB_Act1BG(void);
void CPZSetup_StageFinishCBAct1(void);
void CPZSetup_StageFinishCBAct2(void);

#endif //!OBJ_CPZSETUP_H
