// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: EggTower Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectEggTower *EggTower;

void EggTower_Update(void) {}

void EggTower_LateUpdate(void) {}

void EggTower_StaticUpdate(void) {}

void EggTower_Draw(void)
{
    RSDK_THIS(EggTower);
    int32 y = self->position.y - 160 * ((self->position.y - ((ScreenInfo->position.y + ScreenInfo->centerY) << 16)) >> 8);
    if (y > (ScreenInfo->position.y + 288) << 16)
        y -= (y - (ScreenInfo->position.y << 16) - 0x1200000) >> 1;
    RSDK.MatrixScaleXYZ(&self->matrix2, self->scale.x, -self->scale.x, self->scale.x);
    RSDK.MatrixTranslateXYZ(&self->matrix2, (ScreenInfo->position.x + ScreenInfo->centerX) << 16, y, 0, false);
    RSDK.MatrixRotateY(&self->matrix3, (self->rotationX + ScreenInfo->position.x) / -6);
    RSDK.MatrixMultiply(&self->matrix2, &self->matrix3, &self->matrix2);
    RSDK.Prepare3DScene(EggTower->sceneIndex);
    RSDK.AddModelTo3DScene(EggTower->modelIndex, EggTower->sceneIndex, S3D_FLATCLR_SHADED_BLENDED, &self->matrix2, NULL, 0x000000);
    RSDK.Draw3DScene(EggTower->sceneIndex);
}

void EggTower_Create(void *data)
{
    RSDK_THIS(EggTower);
    if (!SceneInfo->inEditor) {
        self->visible       = true;
        self->drawOrder     = 1;
        self->scale.x       = 0x37C;
        self->active        = ACTIVE_NORMAL;
        self->updateRange.x = 0x800000;
        self->updateRange.y = 0x800000;
    }
}

void EggTower_StageLoad(void)
{
    EggTower->modelIndex = RSDK.LoadMesh("SSZ/EggTower.bin", SCOPE_STAGE);
    EggTower->sceneIndex = RSDK.Create3DScene("View:EggTower", 0x2000, SCOPE_STAGE);
    RSDK.SetDiffuseColour(EggTower->sceneIndex, 192, 192, 192);
    RSDK.SetDiffuseIntensity(EggTower->sceneIndex, 8, 8, 8);
    RSDK.SetSpecularIntensity(EggTower->sceneIndex, 16, 16, 16);
}

#if RETRO_INCLUDE_EDITOR
void EggTower_EditorDraw(void) {}

void EggTower_EditorLoad(void) {}
#endif

void EggTower_Serialize(void) {}
