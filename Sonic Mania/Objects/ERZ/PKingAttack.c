#include "SonicMania.h"

ObjectPKingAttack *PKingAttack;

void PKingAttack_Update(void)
{
    RSDK_THIS(PKingAttack);
    StateMachine_Run(self->state);
}

void PKingAttack_LateUpdate(void) {}

void PKingAttack_StaticUpdate(void) {}

void PKingAttack_Draw(void)
{
    RSDK_THIS(PKingAttack);

    if (self->type == 2)
        RSDK.DrawCircle(self->position.x, self->position.y, 32, 0, 255, INK_LOOKUP, false);
    RSDK.DrawSprite(&self->animator, NULL, false);
}

void PKingAttack_Create(void *data)
{
    RSDK_THIS(PKingAttack);

    if (!SceneInfo->inEditor) {
        self->drawOrder     = Zone->drawOrderHigh;
        self->type          = voidToInt(data);
        self->active        = ACTIVE_NORMAL;
        self->updateRange.x = 0x800000;
        self->updateRange.y = 0x800000;

        switch (self->type) {
            default: break;
            case 1:
                self->drawFX  = FX_SCALE;
                self->visible = true;
                RSDK.SetSpriteAnimation(PKingAttack->aniFrames, 10, &self->animator, true, RSDK.Rand(0, 6));
                self->state         = PKingAttack_Unknown2;
                self->hitbox.left   = -10;
                self->hitbox.top    = -10;
                self->hitbox.right  = 10;
                self->hitbox.bottom = 10;
                break;
            case 3:
                self->drawOrder = Zone->drawOrderLow;
                self->visible   = true;
                self->inkEffect = INK_ADD;
                self->alpha     = 0xC0;
                RSDK.SetSpriteAnimation(PKingAttack->aniFrames, 12, &self->animator, true, 0);
                self->state = PKingAttack_Unknown5;
                break;
            case 6:
                self->drawOrder = Zone->drawOrderLow;
                self->drawFX    = FX_ROTATE;
                self->visible   = true;
                RSDK.SetSpriteAnimation(PKingAttack->aniFrames, 11, &self->animator, true, 0);
                self->state         = PKingAttack_Unknown6;
                self->hitbox.left   = -4;
                self->hitbox.top    = -4;
                self->hitbox.right  = 4;
                self->hitbox.bottom = 4;
                break;
        }
    }
}

void PKingAttack_StageLoad(void)
{
    PKingAttack->aniFrames = RSDK.LoadSpriteAnimation("Phantom/PhantomKing.bin", SCOPE_STAGE);
    PKingAttack->sfxPulse  = RSDK.GetSFX("Stage/ElecPulse.wav");
}

void PKingAttack_CheckPlayerCollisions(void)
{
    RSDK_THIS(PKingAttack);

    foreach_active(Player, player)
    {
        if (Player_CheckCollisionTouch(player, self, &self->hitbox)) {
            if (player->superState == SUPERSTATE_SUPER) {
                if (!player->blinkTimer) {
                    int angle = RSDK.ATan2(player->position.x - self->position.x, player->position.y - self->position.y);
                    if (self->state == PKingAttack_Unknown4) {
                        player->blinkTimer = 120;
                        Ring_LoseRings(player, minVal(player->rings, 8), player->collisionPlane);
                        player->rings -= minVal(player->rings, 8);
                        RSDK.PlaySfx(Player->sfxLoseRings, false, 255);
                    }
                    else {
                        RSDK.PlaySfx(PKingAttack->sfxPulse, false, 255);
                        destroyEntity(self);
                    }
                    player->rotation   = 0;
                    player->velocity.x = RSDK.Cos256(angle) << 10;
                    player->velocity.y = RSDK.Sin256(angle) << 10;
                    player->groundVel  = player->velocity.x;
                    player->state      = ERZStart_State_PlayerRebound;
                }
            }
            else {
                Player_CheckHit(player, self);
            }
        }
    }
}

void PKingAttack_Unknown2(void)
{
    RSDK_THIS(PKingAttack);
    RSDK.ProcessAnimation(&self->animator);

    self->angle = (self->angle + 12) & 0x3FF;
    if (self->scale.x < 512) {
        self->scale.x += 0x20;
        self->scale.y = self->scale.x;
    }

    self->velocity.x = self->position.x;
    self->velocity.y = self->position.y;
    self->position.x = (RSDK.Cos1024(self->angle) << 12) + self->target->position.x;
    self->position.y = (RSDK.Sin1024(self->angle) << 12) + self->target->position.y;
    self->velocity.x = self->position.x - self->velocity.x;
    self->velocity.y = self->position.y - self->velocity.y;

    PKingAttack_CheckPlayerCollisions();
    if (++self->timer == 120) {
        self->timer       = 0;
        self->targetPos.x = self->target->position.x;
        self->targetPos.y = self->target->position.y;
        self->state       = PKingAttack_Unknown3;
    }
}

void PKingAttack_Unknown3(void)
{
    RSDK_THIS(PKingAttack);

    RSDK.ProcessAnimation(&self->animator);

    self->position.x += self->target->position.x - self->targetPos.x;
    self->position.y += self->target->position.y - self->targetPos.y;
    if (self->position.x <= self->target->position.x) {
        if (self->position.x < self->target->position.x)
            self->velocity.x += 0x4000;
    }
    else {
        self->velocity.x -= 0x4000;
    }

    if (self->position.y <= self->target->position.y) {
        if (self->position.y < self->target->position.y)
            self->velocity.y += 0x3800;
    }
    else {
        self->velocity.y -= 0x3800;
    }

    self->velocity.x = clampVal(self->velocity.x, -0x50000, 0x50000);
    self->velocity.y = clampVal(self->velocity.y, -0x50000, 0x50000);

    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;
    self->targetPos.x = self->target->position.x;
    self->targetPos.y = self->target->position.y;

    self->angle = (self->angle + 12) & 0x3FF;
    int inc       = (384 - self->scale.x - (RSDK.Sin1024(self->angle) >> 3)) >> 3;
    self->scale.x += inc;
    self->scale.y = self->scale.x;
    PKingAttack_CheckPlayerCollisions();

    if (self->angle < 0x200)
        self->drawOrder = Zone->drawOrderLow - 1;
    else
        self->drawOrder = Zone->drawOrderLow;
}

void PKingAttack_Unknown4(void)
{
    RSDK_THIS(PKingAttack);

    RSDK.ProcessAnimation(&self->animator);
    ++self->timer;
    if (!(self->timer & 3))
        CREATE_ENTITY(PKingAttack, intToVoid(3), self->position.x, self->position.y);

    if (self->scale.x < 512) {
        self->scale.x += 0x20;
        self->scale.y = self->scale.x;
    }

    self->velocity.x = self->velocity.x + ((self->field_70.x - self->velocity.x) >> 3);
    self->velocity.y = self->velocity.y + ((self->field_70.y - self->velocity.y) >> 3);
    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;

    PKingAttack_CheckPlayerCollisions();

    if (!RSDK.CheckOnScreen(self, NULL))
        destroyEntity(self);
}

void PKingAttack_Unknown5(void)
{
    RSDK_THIS(PKingAttack);

    if (self->timer >= 8) {
        self->alpha -= 8;
        if (self->alpha <= 0)
            destroyEntity(self);
    }
    else {
        self->timer++;
    }
}

void PKingAttack_Unknown6(void)
{
    RSDK_THIS(PKingAttack);

    RSDK.ProcessAnimation(&self->animator);
    self->rotation = (self->rotation + 16) & 0x1FF;
    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;
    PKingAttack_CheckPlayerCollisions();
    if (RSDK.ObjectTileCollision(self, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x40000, true)) {
        self->position.y += 0x40000;
        destroyEntity(self);
    }

    if (!RSDK.CheckOnScreen(self, NULL))
        destroyEntity(self);
}

#if RETRO_INCLUDE_EDITOR
void PKingAttack_EditorDraw(void)
{
    RSDK_THIS(PKingAttack);

    self->type      = 1;
    self->inkEffect = INK_ADD;
    self->alpha     = 0xC0;
    RSDK.SetSpriteAnimation(PKingAttack->aniFrames, 12, &self->animator, true, 0);

    PKingAttack_Draw();
}

void PKingAttack_EditorLoad(void) { PKingAttack->aniFrames = RSDK.LoadSpriteAnimation("Phantom/PhantomKing.bin", SCOPE_STAGE); }
#endif

void PKingAttack_Serialize(void) {}
