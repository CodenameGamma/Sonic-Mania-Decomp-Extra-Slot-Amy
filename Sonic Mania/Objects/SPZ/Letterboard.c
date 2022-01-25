// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: Letterboard Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectLetterboard *Letterboard;

void Letterboard_Update(void)
{
    RSDK_THIS(Letterboard);
    StateMachine_Run(self->state);
}

void Letterboard_LateUpdate(void) {}

void Letterboard_StaticUpdate(void) {}

void Letterboard_Draw(void)
{
    RSDK_THIS(Letterboard);

    self->scale.x = abs(RSDK.Cos512(self->angle));
    if (RSDK.Cos512(self->angle) >= 0)
        RSDK.DrawSprite(&self->animatorBack, NULL, false);
    else
        RSDK.DrawSprite(&self->animatorFront, NULL, false);
}

void Letterboard_Create(void *data)
{
    RSDK_THIS(Letterboard);

    if (!SceneInfo->inEditor) {
        RSDK.SetSpriteAnimation(Letterboard->aniFrames, 0, &self->animatorBack, true, 0);
        RSDK.SetSpriteAnimation(Letterboard->aniFrames, 1, &self->animatorFront, true, 0);

        if (self->ControllerInfo || !self->letterID) {
            if (self->ControllerInfo)
                self->state = Letterboard_State_Controller;
        }
        else {
            self->state                 = Letterboard_State_CheckPlayerSpin;
            self->animatorFront.frameID = self->letterID - 1;
        }

        self->scale.x       = 0x200;
        self->scale.y       = 0x200;
        self->active        = ACTIVE_BOUNDS;
        self->updateRange.x = 0x400000;
        self->updateRange.y = 0x400000;
        self->visible       = true;
        self->drawOrder     = Zone->drawOrderLow;
    }
}

void Letterboard_StageLoad(void)
{
    Letterboard->aniFrames     = RSDK.LoadSpriteAnimation("SPZ2/Letterboard.bin", SCOPE_STAGE);
    Letterboard->hitbox.left   = -12;
    Letterboard->hitbox.top    = -12;
    Letterboard->hitbox.right  = 12;
    Letterboard->hitbox.bottom = 12;
    Letterboard->sfxLetterTurn = RSDK.GetSfx("Stage/LetterTurn.wav");
    Letterboard->sfxWin        = RSDK.GetSfx("Stage/Win.wav");
}

void Letterboard_State_Controller(void)
{
    RSDK_THIS(Letterboard);

    self->active = ACTIVE_BOUNDS;

    bool32 flag = true;
    int count   = 0;
    int slot    = SceneInfo->entitySlot + 1;
    for (int i = 0; i < self->letterID; ++i) {
        EntityLetterboard *letterboard = RSDK_GET_ENTITY(slot + i, Letterboard);
        if (letterboard->state)
            flag = false;

        if (letterboard->state == Letterboard_State_Spun)
            self->active = ACTIVE_NORMAL;
    }
    flag = count == self->letterID;

    if (flag) {
        RSDK.PlaySfx(Letterboard->sfxWin, false, 255);
        self->active = ACTIVE_BOUNDS;
        self->state  = StateMachine_None;
        if (globals->gameMode != MODE_COMPETITION)
            Player_GiveScore(RSDK_GET_ENTITY(SLOT_PLAYER1, Player), 100 * self->letterID);
    }
}

void Letterboard_State_CheckPlayerSpin(void)
{
    RSDK_THIS(Letterboard);

    foreach_active(Player, player)
    {
        self->spinSpeed = (abs(player->velocity.x) + abs(player->velocity.y)) >> 14;
        if (self->spinSpeed > 0) {
            if (Player_CheckCollisionTouch(player, self, &Letterboard->hitbox)) {
                self->drawFX = FX_SCALE;
                self->state  = Letterboard_State_Spun;
                if (self->spinSpeed > 16)
                    self->spinSpeed = 16;
                if (self->spinSpeed < 8)
                    self->spinSpeed = 8;

                self->timer = 2;
                int slot      = SceneInfo->entitySlot;

                EntityLetterboard *letterboard = self;
                while (slot >= 0) {
                    letterboard = RSDK_GET_ENTITY(slot--, Letterboard);
                    if (letterboard->state == Letterboard_State_Controller)
                        break;
                }

                letterboard->active = ACTIVE_NORMAL;
                self->active      = ACTIVE_NORMAL;
                RSDK.PlaySfx(Letterboard->sfxLetterTurn, false, 255);
                foreach_break;
            }
        }
    }
}

void Letterboard_State_Spun(void)
{
    RSDK_THIS(Letterboard);

    int prevAngle = self->angle;
    self->angle = (self->angle + self->spinSpeed) & 0x1FF;
    if (self->angle > 255 && prevAngle < 256 && --self->timer <= 0) {
        self->active = ACTIVE_BOUNDS;
        self->angle  = 256;
        self->state  = 0;
    }
}

#if RETRO_INCLUDE_EDITOR
void Letterboard_EditorDraw(void)
{
    RSDK_THIS(Letterboard);
    RSDK.SetSpriteAnimation(Letterboard->aniFrames, 0, &self->animatorBack, true, 0);
    RSDK.SetSpriteAnimation(Letterboard->aniFrames, 1, &self->animatorFront, true, 0);

    if (!self->ControllerInfo && self->letterID) {
        self->animatorFront.frameID = self->letterID - 1;
        RSDK.DrawSprite(&self->animatorFront, NULL, false);
    }
    else {
        RSDK.DrawSprite(&self->animatorBack, NULL, false);
    }
}

void Letterboard_EditorLoad(void) { Letterboard->aniFrames = RSDK.LoadSpriteAnimation("SPZ2/Letterboard.bin", SCOPE_STAGE); }
#endif

void Letterboard_Serialize(void)
{
    RSDK_EDITABLE_VAR(Letterboard, VAR_BOOL, ControllerInfo);
    RSDK_EDITABLE_VAR(Letterboard, VAR_UINT8, letterID);
}
