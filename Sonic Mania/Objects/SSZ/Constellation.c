// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: Constellation Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectConstellation *Constellation;

void Constellation_Update(void)
{
    RSDK_THIS(Constellation);
    RSDK.ProcessAnimation(&self->animator);
}

void Constellation_LateUpdate(void) {}

void Constellation_StaticUpdate(void) {}

void Constellation_Draw(void)
{
    RSDK_THIS(Constellation);
    RSDKScreenInfo *screen = &ScreenInfo[SceneInfo->currentScreenID];
    Vector2 drawPos;

    drawPos.y = self->startPos.y - ((screen->position.y * Constellation->bg1->parallaxFactor / 256) << 16);
    drawPos.x = ((((self->position.x >> 16) - screen->position.x - (screen->width >> 1)) << 7) / (256 - self->scrollInfo->parallaxFactor)) << 16;
    drawPos.x += screen->width << 15;
    RSDK.DrawSprite(&self->animator, &drawPos, true);
}

void Constellation_Create(void *data)
{
    RSDK_THIS(Constellation);

    self->startPos.x    = self->position.x;
    self->startPos.y    = self->position.y;
    self->active        = ACTIVE_BOUNDS;
    self->drawOrder     = 1;
    self->visible       = true;
    self->drawFX        = FX_FLIP;
    self->updateRange.x = 0x800000;
    self->updateRange.y = 0x3000000;
    if (!SceneInfo->inEditor)
        Constellation_SetupInfo();
    RSDK.SetSpriteAnimation(Constellation->aniFrames, self->shape, &self->animator, true, 0);
}

void Constellation_StageLoad(void)
{
    Constellation->aniFrames = RSDK.LoadSpriteAnimation("SSZ1/Constellation.bin", SCOPE_STAGE);
    Constellation->bg1       = RSDK.GetSceneLayer(RSDK.GetSceneLayerID("Background 1"));
}

void Constellation_SetupInfo(void)
{
    RSDK_THIS(Constellation);
    TileLayer *bgLayer = RSDK.GetSceneLayer(RSDK.GetSceneLayerID("Background 1"));

    int id = minVal(self->startPos.y >> 3, 20);
    self->scrollInfo = &bgLayer->scrollInfo[id];
    if (self->scrollInfo) {
        int factor = self->scrollInfo->parallaxFactor;
        if (factor > 0) {
            self->updateRange.x += ((ScreenInfo->width << 8) / factor) << 16;
        }
    }
}

void Constellation_EditorDraw(void)
{
    RSDK_THIS(Constellation);
    RSDK.SetSpriteAnimation(Constellation->aniFrames, self->shape, &self->animator, true, 0);
    RSDK.DrawSprite(&self->animator, NULL, false);
}

void Constellation_EditorLoad(void) { Constellation->aniFrames = RSDK.LoadSpriteAnimation("SSZ1/Constellation.bin", SCOPE_STAGE); }

void Constellation_Serialize(void)
{
    RSDK_EDITABLE_VAR(Constellation, VAR_UINT8, direction);
    RSDK_EDITABLE_VAR(Constellation, VAR_UINT8, shape);
}
