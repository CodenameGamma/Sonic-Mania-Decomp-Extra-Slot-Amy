#include "SonicMania.h"

ObjectBSS_HUD *BSS_HUD;

void BSS_HUD_Update(void) {}

void BSS_HUD_LateUpdate(void) {}

void BSS_HUD_StaticUpdate(void) {}

void BSS_HUD_Draw(void)
{
    Vector2 drawPos;
    RSDK_THIS(BSS_HUD);

    drawPos.y             = 0xD0000;
    drawPos.x             = (ScreenInfo->centerX - 141) << 16;
    RSDK.DrawSprite(&self->sphereAnimator, &drawPos, true);

    drawPos.x += 0x250000;
    drawPos.y = 0x110000;
    BSS_HUD_DrawNumbers(BSS_Setup->sphereCount, &drawPos);

    drawPos.x = (ScreenInfo->centerX + 64) << 16;
    drawPos.y = 0xD0000;
    RSDK.DrawSprite(&self->ringAnimator, &drawPos, true);

    drawPos.x += 0x380000;
    drawPos.y = 0x110000;
    BSS_HUD_DrawNumbers(BSS_Setup->ringCount, &drawPos);

    // DEBUG, TODO: REMOVE
    if (false) {
        uint16 *playFields[3] = { BSS_Setup->playField, BSS_Setup->playField2, BSS_Setup->playField3 };

        uint32 colours[24] = { 0x000000, 0x0000FF, 0xFF0000, 0xFFFFFF, 0xFFFF00, 0x00FF00, 0xFF00FF, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
                             0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000 };

        for (int32 p = 0; p < 3; ++p) {
            for (int32 y = 0; y < BSS_PLAYFIELD_H; ++y) {
                for (int32 x = 0; x < BSS_PLAYFIELD_W; ++x) {
                    int32 playFieldPos = (x * BSS_PLAYFIELD_H) + y;
                    uint16 type      = playFields[p][playFieldPos] & 0x7F;

                    RSDK.DrawRect((ScreenInfo->width - 64) + (x << 1), (ScreenInfo->height - (3 * 64)) + (p * 64) + (y << 1), 2, 2, colours[type],
                                  0xFF, INK_NONE, true);
                }
            }
        }
    }
}

void BSS_HUD_Create(void *data)
{
    RSDK_THIS(BSS_HUD);
    if (!SceneInfo->inEditor) {
        self->active        = ACTIVE_NORMAL;
        self->visible       = true;
        self->drawOrder     = DRAWLAYER_COUNT - 1;
        self->updateRange.x = 0x800000;
        self->updateRange.y = 0x800000;
        RSDK.SetSpriteAnimation(BSS_HUD->aniFrames, 0, &self->sphereAnimator, true, 0);
        RSDK.SetSpriteAnimation(BSS_HUD->aniFrames, 0, &self->ringAnimator, true, 1);
        RSDK.SetSpriteAnimation(BSS_HUD->aniFrames, 1, &self->numbersAnimator, true, 0);
    }
}

void BSS_HUD_StageLoad(void)
{
    BSS_HUD->aniFrames = RSDK.LoadSpriteAnimation("SpecialBS/HUD.bin", SCOPE_STAGE);
    RSDK.ResetEntitySlot(SLOT_BSS_HUD, BSS_HUD->objectID, 0);
}

void BSS_HUD_DrawNumbers(int32 value, Vector2 *drawPos)
{
    RSDK_THIS(BSS_HUD);
    int32 mult              = 1;
    for (int32 i = 0; i < 3; ++i) {
        self->numbersAnimator.frameID = value / mult % 10;
        RSDK.DrawSprite(&self->numbersAnimator, drawPos, true);
        drawPos->x -= 0x100000;
        mult *= 10;
    }
}

#if RETRO_INCLUDE_EDITOR
void BSS_HUD_EditorDraw(void) {}

void BSS_HUD_EditorLoad(void) {}
#endif

void BSS_HUD_Serialize(void) {}
