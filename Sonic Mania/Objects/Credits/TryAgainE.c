// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: TryAgainE Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

#if RETRO_USE_PLUS
ObjectTryAgainE *TryAgainE;

void TryAgainE_Update(void)
{
    RSDK_THIS(TryAgainE);
    StateMachine_Run(self->state);
}

void TryAgainE_LateUpdate(void) {}

void TryAgainE_StaticUpdate(void)
{
    foreach_active(TryAgainE, tryAgainE) { RSDK.AddDrawListRef(2, RSDK.GetEntityID(tryAgainE)); }
}

void TryAgainE_Draw(void)
{
    RSDK_THIS(TryAgainE);
    if (SceneInfo->currentDrawGroup == self->drawOrder) {
        Vector2 drawPos;

        RSDK.SetActivePalette(3, 0, ScreenInfo->height);
        self->mainAnimator.frameID = 0;
        RSDK.DrawSprite(&self->mainAnimator, NULL, false);

        self->mainAnimator.frameID = 1;
        RSDK.DrawSprite(&self->mainAnimator, NULL, false);
        RSDK.SetActivePalette(0, 0, ScreenInfo->height);

        drawPos.x = self->position.x;
        drawPos.y = self->position.y + 0x2E0000;
        if (globals->playerID > 0xFF)
            drawPos.x = self->position.x - 0x140000;
        if (self->player1Animator.animationID == 2)
            drawPos.y = self->position.y + 0x2E0000 + 0x40000;
        RSDK.DrawSprite(&self->player1Animator, &drawPos, false);

        int32 drawY = drawPos.y;
        if (self->player1Animator.animationID == 2) {
            drawY = drawPos.y - 0x40000;
            drawPos.y -= 0x40000;
        }

        if (globals->playerID > 255) {
            drawPos.x += 0x280000;
            self->direction = FLIP_X;
            if (self->player2Animator.animationID == 2)
                drawPos.y = drawY + 0x40000;
            RSDK.DrawSprite(&self->player2Animator, &drawPos, false);
            self->direction = FLIP_NONE;
        }
    }
    else {
        self->direction = FLIP_X;
        RSDK.DrawSprite(&self->handUpAnimator, NULL, false);

        self->direction = FLIP_NONE;
        RSDK.DrawSprite(&self->handDownAnimator, NULL, false);
    }
}

void TryAgainE_Create(void *data)
{
    RSDK_THIS(TryAgainE);
    if (!SceneInfo->inEditor) {
        self->visible       = true;
        self->drawOrder     = 1;
        self->active        = ACTIVE_BOUNDS;
        self->updateRange.x = 0x800000;
        self->updateRange.y = 0x800000;
        self->drawFX        = FX_FLIP;
        self->state         = TryAgainE_State_Stinger;
        RSDK.SetSpriteAnimation(TryAgainE->aniFrames, 0, &self->mainAnimator, true, 0);
        RSDK.SetSpriteAnimation(TryAgainE->aniFrames, 1, &self->handUpAnimator, true, 4);
        RSDK.SetSpriteAnimation(TryAgainE->aniFrames, 2, &self->handDownAnimator, true, 3);

        int32 id        = -1;
        uint8 playerID = globals->playerID & 0xFF;
        if (playerID) {
            do {
                playerID >>= 1;
                ++id;
            } while (playerID > 0);
        }
        RSDK.SetSpriteAnimation(TryAgainE->playerFrames, 2 * id, &self->player1Animator, true, 3);

        if (globals->playerID >= 0x100) {
            id       = -1;
            playerID = (globals->playerID >> 8) & 0xFF;
            if (playerID) {
                do {
                    playerID >>= 1;
                    ++id;
                } while (playerID > 0);
            }
            RSDK.SetSpriteAnimation(TryAgainE->playerFrames, 2 * id, &self->player2Animator, true, 3);
        }
    }
}

void TryAgainE_StageLoad(void)
{
    TryAgainE->aniFrames    = RSDK.LoadSpriteAnimation("Credits/TryAgainE.bin", SCOPE_STAGE);
    TryAgainE->playerFrames = RSDK.LoadSpriteAnimation("Players/Continue.bin", SCOPE_STAGE);
    TryAgainE->active       = ACTIVE_ALWAYS;
    RSDK.CopyPalette(0, 0, 1, 0, 128);
}

void TryAgainE_SetupEmeralds(void)
{
    int32 id    = 1;
    int32 timer = 0;
    foreach_all(TAEmerald, emerald)
    {
        emerald->state = TAEmerald_State_MoveCircle;
        emerald->timer = timer;
        if (SaveGame->saveRAM) {
            if (!(id & SaveGame->saveRAM->chaosEmeralds))
                timer += 8;
        }
        id <<= 1;
    }
}

void TryAgainE_State_Stinger(void)
{
    RSDK_THIS(TryAgainE);

    RSDK.ProcessAnimation(&self->handUpAnimator);
    RSDK.ProcessAnimation(&self->handDownAnimator);
    RSDK.ProcessAnimation(&self->player1Animator);
    if (globals->playerID >= 0x100)
        RSDK.ProcessAnimation(&self->player2Animator);

    if ((self->timer & 0x7F) == 1) {
        if ((self->timer & 0x80) != 0 && self->handUpAnimator.animationID == 2)
            RSDK.SetSpriteAnimation(TryAgainE->aniFrames, 1, &self->handUpAnimator, true, 0);
        if ((int8)(self->timer & 0xFF) >= 0 && self->handDownAnimator.animationID == 2)
            RSDK.SetSpriteAnimation(TryAgainE->aniFrames, 1, &self->handDownAnimator, true, 0);
        TryAgainE_SetupEmeralds();
    }
    else if ((self->timer & 0x7F) == 59) {
        if ((self->timer & 0x80) == 0 && self->handUpAnimator.animationID == 1)
            RSDK.SetSpriteAnimation(TryAgainE->aniFrames, 2, &self->handUpAnimator, true, 0);
        if ((int8)(self->timer & 0xFF) < 0 && self->handDownAnimator.animationID == 1)
            RSDK.SetSpriteAnimation(TryAgainE->aniFrames, 2, &self->handDownAnimator, true, 0);
    }
    if (++self->timer == 1) {
        foreach_all(TAEmerald, emerald)
        {
            emerald->startPos.x = self->position.x;
            emerald->startPos.y = self->position.y;
            emerald->position.x = emerald->startPos.x;
            emerald->position.y = emerald->startPos.y;
            emerald->startPos.y += 0x100000;
            emerald->position.y = emerald->startPos.y;
            emerald->position.x += 0x480000;
            emerald->angle  = 0;
            emerald->drawFX = FX_ROTATE;
            emerald->state  = StateMachine_None;
        }
    }

    if (ControllerInfo->keyA.press || ControllerInfo->keyStart.press)
        self->timer = 600;
#if RETRO_USE_TOUCH_CONTROLS
    else if (TouchInfo->count)
        self->timer = 600;
#endif

    if (self->timer == 600) {
        PhantomRuby_PlaySFX(RUBYSFX_ATTACK4);
        EntityFXFade *fxFade = CREATE_ENTITY(FXFade, intToVoid(0xFFFFFF), self->position.x, self->position.y);
        fxFade->speedIn      = 24;
        fxFade->speedOut     = 24;
        fxFade->fadeOutBlack = true;
    }

    if (self->timer >= 740) {
        if (API.CheckDLC(DLC_PLUS))
            RSDK.SetScene("Presentation", "Encore Summary");
        else
            RSDK.SetScene("Presentation", "Menu");
        RSDK.LoadScene();
    }
}

#if RETRO_INCLUDE_EDITOR
void TryAgainE_EditorDraw(void) {}

void TryAgainE_EditorLoad(void) {}
#endif

void TryAgainE_Serialize(void) {}
#endif
