#include "SonicMania.h"

ObjectFBZStorm *FBZStorm;

void FBZStorm_Update(void)
{
    RSDK_THIS(FBZStorm);
    StateMachine_Run(self->state);

    self->velocity.x = -0x40000;
    foreach_active(Current, current)
    {
        int strength = -0x10000 * current->strength;
        if (strength < self->velocity.x)
            self->velocity.x = strength;
    }

    for (int32 p = 0; p < Player->playerCount; ++p) {
        if (!RSDK_GET_ENTITY(p, Player)->sidekick) {
            self->screenPosX[p] = ScreenInfo[p].position.x;
            int32 x                 = ((self->screenPosX[p] << 14) - (ScreenInfo[p].position.x << 14)) + self->velocity.x;
            self->stormAngle[p] = RSDK.ATan2(self->velocity.y, -x) << 1;
            Vector2 *pos          = &FBZStorm->positions[0x40 * p];

            for (int32 i = 0; i < 0x40; ++i) {
                pos->x += x;
                pos->y += self->velocity.y;

                if (pos->x < -0x1000000)
                    pos->x += 0x2000000;
                if (pos->y < -0xA00000)
                    pos->y += 0x1400000;
                if (pos->x > 0x1000000)
                    pos->x -= 0x2000000;
                if (pos->y > 0xA00000)
                    pos->y -= 0x1400000;
                ++pos;
            }
        }
    }

    if (RSDK.GetSceneLayer(3)->drawLayer[0] < DRAWLAYER_COUNT) {
        self->flag = true;
        if (self->blendAmount < 0x100)
            self->blendAmount += 8;
    }
    else {
        self->flag = false;
        if (self->blendAmount > 0)
            self->blendAmount -= 8;
    }

    RSDK.SetLimitedFade(0, FBZStorm->srcPal, 2, self->blendAmount, 0x80, 0x100);
}

void FBZStorm_LateUpdate(void) {}

void FBZStorm_StaticUpdate(void)
{
    bool32 flag = false;
    foreach_all(FBZStorm, storm)
    {
        if (storm->flag)
            flag = true;
    }

    if (!flag || RSDK_GET_ENTITY(SLOT_PAUSEMENU, PauseMenu)->objectID == PauseMenu->objectID) {
        if (FBZStorm->playingRainSFX) {
            RSDK.StopSFX(FBZStorm->sfxRain);
            FBZStorm->playingRainSFX = false;
        }
    }
}

void FBZStorm_Draw(void)
{
    RSDK_THIS(FBZStorm);
    RSDKScreenInfo *screen = &ScreenInfo[SceneInfo->currentScreenID];

    int32 centerX      = screen->centerX << 16;
    int32 centerY      = screen->centerY << 16;
    self->rotation = self->stormAngle[SceneInfo->currentScreenID];

    Vector2 *pos = &FBZStorm->positions[0x40 * SceneInfo->currentScreenID];
    for (int32 i = 0; i < 0x40; ++i) {
        Vector2 drawPos;
        drawPos.x = centerX + pos->x;
        drawPos.y = centerY + pos->y;
        RSDK.DrawSprite(&self->animator, &drawPos, true);
        ++pos;
    }
}

void FBZStorm_Create(void *data)
{
    RSDK_THIS(FBZStorm);
    if (!SceneInfo->inEditor) {
        self->active     = ACTIVE_NORMAL;
        self->drawOrder  = Zone->drawOrderHigh;
        self->drawFX     = FX_ROTATE;
        self->inkEffect  = INK_ALPHA;
        self->velocity.x = -0x40000;
        self->velocity.y = 0xC0000;
        self->state      = FBZStorm_Unknown1;

        for (int32 p = 0; p < Player->playerCount; ++p) {
            for (int32 i = 0; i < 0x40; ++i) {
                FBZStorm->positions[i].x = RSDK.Rand(-0x1000000, 0x1000000);
                FBZStorm->positions[i].y = RSDK.Rand(-0x800000, 0x800000);
            }
        }

        RSDK.SetSpriteAnimation(FBZStorm->aniFrames, 0, &self->animator, true, 0);
    }
}

void FBZStorm_StageLoad(void)
{
    FBZStorm->active     = ACTIVE_ALWAYS;
    FBZStorm->aniFrames  = RSDK.LoadSpriteAnimation("FBZ/Storm.bin", SCOPE_STAGE);
    FBZStorm->sfxRain    = RSDK.GetSFX("FBZ/Rain.wav");
    FBZStorm->sfxThunder = RSDK.GetSFX("FBZ/Thunder.wav");
}

void FBZStorm_Unknown1(void)
{
    RSDK_THIS(FBZStorm);
    if (self->flag) {
        if (RSDK.GetEntityCount(Current->objectID, true) > 0)
            self->state = FBZStorm_Unknown2;
    }
}

void FBZStorm_Unknown2(void)
{
    RSDK_THIS(FBZStorm);

    self->visible = true;
    if (self->alpha >= 128) {
        FBZStorm->srcPal = 4;
        self->inkEffect   = INK_BLEND;
        self->state  = FBZStorm_Unknown4;
    }
    else {
        self->alpha += 4;
    }

    bool32 flag = false;
    foreach_all(FBZStorm, storm)
    {
        if (storm->flag)
            flag = true;
    }

    if (FBZStorm->playingRainSFX) {
        if (!flag || RSDK_GET_ENTITY(SLOT_PAUSEMENU, PauseMenu)->objectID == PauseMenu->objectID) {
            RSDK.StopSFX(FBZStorm->sfxRain);
            FBZStorm->playingRainSFX = false;
        }
    }
    else {
        RSDK.PlaySfx(FBZStorm->sfxRain, true, 0xFF);
        FBZStorm->playingRainSFX = true;
    }
}

void FBZStorm_Unknown3(void)
{
    RSDK_THIS(FBZStorm);

    if (self->alpha <= 0) {
        self->visible = false;
        if (FBZStorm->playingRainSFX) {
            RSDK.StopSFX(FBZStorm->sfxRain);
        }
        self->state = FBZStorm_Unknown1;
    }
    else {
        self->alpha -= 4;
    }
}

void FBZStorm_Unknown4(void)
{
    RSDK_THIS(FBZStorm);
    if (self->timer <= 0) {
        self->state = FBZStorm_Unknown5;
    }
    else {
        self->timer--;
        if (!self->flag) {
            self->inkEffect = INK_ALPHA;
            self->state          = FBZStorm_Unknown3;
        }
    }
}

void FBZStorm_Unknown5(void)
{
    RSDK_THIS(FBZStorm);
    RSDK.SetLimitedFade(0, 2, 3, FBZStorm->array1[self->timer], 0x80, 0x100);
    if (!self->timer)
        RSDK.PlaySfx(FBZStorm->sfxThunder, 0, 255);
    if (++self->timer == 20) {
        self->timer = RSDK.Rand(180, 320);
        self->state = FBZStorm_Unknown4;
    }
}

#if RETRO_INCLUDE_EDITOR
void FBZStorm_EditorDraw(void) {}

void FBZStorm_EditorLoad(void) {}
#endif

void FBZStorm_Serialize(void) {}
