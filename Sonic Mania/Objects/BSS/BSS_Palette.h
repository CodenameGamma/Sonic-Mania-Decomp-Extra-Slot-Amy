#ifndef OBJ_BSS_PALETTE_H
#define OBJ_BSS_PALETTE_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    int32 startColourID;
    int32 alpha1;
    int32 alpha2;
    int32 field_10;
} ObjectBSS_Palette;

// Entity Class
typedef struct {
    RSDK_ENTITY
    bool32 useStageConfig;
    int32 paletteID;
    uint8 skyAlpha;
    uint8 globeAlpha;
    colour playfieldA;
    colour playfieldB;
    colour bgColor1;
    colour bgColor2;
    colour bgColor3;
} EntityBSS_Palette;

// Object Struct
extern ObjectBSS_Palette *BSS_Palette;

// Standard Entity Events
void BSS_Palette_Update(void);
void BSS_Palette_LateUpdate(void);
void BSS_Palette_StaticUpdate(void);
void BSS_Palette_Draw(void);
void BSS_Palette_Create(void* data);
void BSS_Palette_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void BSS_Palette_EditorDraw(void);
void BSS_Palette_EditorLoad(void);
#endif
void BSS_Palette_Serialize(void);

// Extra Entity Functions


#endif //!OBJ_BSS_PALETTE_H
