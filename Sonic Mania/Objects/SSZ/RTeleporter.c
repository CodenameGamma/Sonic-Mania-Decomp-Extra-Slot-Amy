#include "SonicMania.h"

ObjectRTeleporter *RTeleporter;

void RTeleporter_Update(void)
{
    RSDK_THIS(RTeleporter);
    StateMachine_Run(self->state);
}

void RTeleporter_LateUpdate(void) {}

void RTeleporter_StaticUpdate(void) {}

void RTeleporter_Draw(void)
{
    RSDK_THIS(RTeleporter);
    StateMachine_Run(self->stateDraw);
}

void RTeleporter_Create(void *data)
{
    RSDK_THIS(RTeleporter);

    self->drawFX = FX_FLIP;
    if (!SceneInfo->inEditor) {
        self->active        = ACTIVE_BOUNDS;
        self->visible       = true;
        self->drawOrder     = Zone->drawOrderHigh;
        self->updateRange.x = 0x800000;
        self->updateRange.y = 0x800000;
        RSDK.SetSpriteAnimation(RTeleporter->aniFrames, 0, &self->animator1, true, 0);
        RSDK.SetSpriteAnimation(RTeleporter->aniFrames, 1, &self->animator2, true, 0);
        self->startY    = self->position.y;
        self->state     = RTeleporter_State_CheckPlayerCollisions;
        self->stateDraw = RTeleporter_StateDraw_Active;
    }
}

void RTeleporter_StageLoad(void)
{
    if (RSDK.CheckStageFolder("SSZ1") || RSDK.CheckStageFolder("SSZ2"))
        RTeleporter->aniFrames = RSDK.LoadSpriteAnimation("SSZ/RTeleporter.bin", SCOPE_STAGE);
    else if (RSDK.CheckStageFolder("MMZ1") || RSDK.CheckStageFolder("MMZ2"))
        RTeleporter->aniFrames = RSDK.LoadSpriteAnimation("MMZ/RTeleporter.bin", SCOPE_STAGE);

    RTeleporter->hitbox.top    = -32;
    RTeleporter->hitbox.left   = -34;
    RTeleporter->hitbox.right  = 34;
    RTeleporter->hitbox.bottom = 32;
    RTeleporter->sfxExplosion  = RSDK.GetSfx("Stage/Explosion2.wav");
    RTeleporter->sfxGoodFuture = RSDK.GetSfx("Stage/GoodFuture.wav");
}

void RTeleporter_State_CheckPlayerCollisions(void)
{
    RSDK_THIS(RTeleporter);

    RSDK.ProcessAnimation(&self->animator1);
    RSDK.ProcessAnimation(&self->animator2);

    self->position.y = (RSDK.Sin256(self->angle) << 9) + self->startY;
    self->angle      = (self->angle + 4) & 0xFF;

    foreach_active(Player, player)
    {
        if (!player->onGround) {
            if (Player_CheckCollisionTouch(player, self, &RTeleporter->hitbox)) {
                Player_CheckHit2(player, self, false);
                self->state     = RTeleporter_State_Destroyed;
                self->stateDraw = RTeleporter_StateDraw_Exploding;
            }
        }
    }
}

void RTeleporter_State_Destroyed(void)
{
    RSDK_THIS(RTeleporter);

    if (!(Zone->timer % 3)) {
        RSDK.PlaySfx(RTeleporter->sfxExplosion, false, 255);
        if (Zone->timer & 4) {
            int x = RSDK.Rand(-32, 32) << 16;
            int y = RSDK.Rand(-32, 32) << 16;
            EntityExplosion *explosion =
                CREATE_ENTITY(Explosion, intToVoid(((RSDK.Rand(0, 256) > 192) + EXPLOSION_BOSS)), x + self->position.x, y + self->position.y);
            explosion->drawOrder = Zone->drawOrderHigh;
        }
    }

    if (++self->timer == 80) {
        RSDK.PlaySfx(RTeleporter->sfxGoodFuture, false, 255);
        RSDK.SetSpriteAnimation(RTeleporter->aniFrames, 2, &self->animator1, true, 0);
        self->position.y += 0x180000;
        RSDK.ObjectTileGrip(self, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x80000, 8);
        self->state     = StateMachine_None;
        self->stateDraw = RTeleporter_StateDraw_Destroyed;
    }
}

void RTeleporter_StateDraw_Active(void)
{
    RSDK_THIS(RTeleporter);
    Vector2 drawPos;

    drawPos.x = self->position.x;
    drawPos.y = self->position.y;

    RSDK.DrawSprite(&self->animator1, NULL, false);

    self->direction = FLIP_Y;
    RSDK.DrawSprite(&self->animator1, NULL, false);

    self->direction = FLIP_NONE;
    RSDK.DrawSprite(&self->animator2, NULL, false);

    drawPos.x -= 0x100000;
    RSDK.DrawSprite(&self->animator2, &drawPos, false);

    drawPos.x += 0x200000;
    RSDK.DrawSprite(&self->animator2, &drawPos, false);
}

void RTeleporter_StateDraw_Exploding(void)
{
    RSDK_THIS(RTeleporter);

    RSDK.DrawSprite(&self->animator1, NULL, false);
    self->direction = FLIP_Y;

    RSDK.DrawSprite(&self->animator1, NULL, false);
    self->direction = FLIP_NONE;
}

void RTeleporter_StateDraw_Destroyed(void)
{
    RSDK_THIS(RTeleporter);
    RSDK.DrawSprite(&self->animator1, NULL, false);
}

#if RETRO_INCLUDE_EDITOR
void RTeleporter_EditorDraw(void)
{
    RSDK_THIS(RTeleporter);

    RSDK.SetSpriteAnimation(RTeleporter->aniFrames, 0, &self->animator1, true, 0);
    RSDK.SetSpriteAnimation(RTeleporter->aniFrames, 1, &self->animator2, true, 0);

    RTeleporter_StateDraw_Active();
}

void RTeleporter_EditorLoad(void)
{
    if (RSDK.CheckStageFolder("SSZ1") || RSDK.CheckStageFolder("SSZ2"))
        RTeleporter->aniFrames = RSDK.LoadSpriteAnimation("SSZ/RTeleporter.bin", SCOPE_STAGE);
    else if (RSDK.CheckStageFolder("MMZ1") || RSDK.CheckStageFolder("MMZ2"))
        RTeleporter->aniFrames = RSDK.LoadSpriteAnimation("MMZ/RTeleporter.bin", SCOPE_STAGE);
}
#endif

void RTeleporter_Serialize(void) {}
