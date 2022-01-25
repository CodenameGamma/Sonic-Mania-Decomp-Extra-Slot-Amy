// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: HPZEmerald Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectHPZEmerald *HPZEmerald;

void HPZEmerald_Update(void)
{
    RSDK_THIS(HPZEmerald);
    RSDK.ProcessAnimation(&self->animator2);
    if (self->solid) {
        if (self->type) {
            foreach_active(Player, player) { Player_CheckCollisionPlatform(player, self, self->hitbox); }
        }

        foreach_active(HeavyKing, king)
        {
            if (king->animator1.animationID != 5 && !king->onGround
                && RSDK.CheckObjectCollisionPlatform(self, self->hitbox, king, &HeavyKing->hitbox1, true)) {
                Camera_ShakeScreen(0, 0, 3);
                RSDK.PlaySfx(HeavyKing->sfxImpact2, false, 255);
            }
        }
    }

    if (!self->onGround) {
        self->velocity.y += 0x3800;
        self->position.y += self->velocity.y;
        if (self->position.y >= self->startPos.y && self->velocity.y > 0) {
            self->position.y = self->startPos.y;
            self->velocity.y = -(self->velocity.y >> 1);
            if (!self->velocity.y)
                self->onGround = true;
        }
    }
}

void HPZEmerald_LateUpdate(void) {}

void HPZEmerald_StaticUpdate(void) {}

void HPZEmerald_Draw(void)
{
    RSDK_THIS(HPZEmerald);

    self->inkEffect = INK_NONE;
    if (self->solid) {
        RSDK.DrawSprite(&self->animator2, NULL, false);
    }
    else {
        RSDK.DrawSprite(&self->animator2, NULL, false);
        self->inkEffect = INK_ADD;
        RSDK.DrawSprite(&self->animator, NULL, false);
    }
}

void HPZEmerald_Create(void *data)
{
    RSDK_THIS(HPZEmerald);
    if (!SceneInfo->inEditor) {
        self->visible = true;

        switch (self->type) {
            case HPZEMERALD_MASTER:
            default: self->drawOrder = 1; break;
            case HPZEMERALD_EMERALD_LOW:
                self->solid     = true;
                self->drawOrder = Zone->drawOrderLow;
                break;
            case HPZEMERALD_EMERALD_HIGH:
                self->solid     = true;
                self->drawOrder = Zone->drawOrderHigh;
                break;
        }

        self->startPos.x    = self->position.x;
        self->startPos.y    = self->position.y;
        self->active        = ACTIVE_BOUNDS;
        self->updateRange.x = 0x800000;
        self->updateRange.y = 0x800000;
        self->onGround      = true;
        if (self->type) {
            RSDK.SetSpriteAnimation(HPZEmerald->aniFrames, 1, &self->animator2, true, 0);
        }
        else {
            RSDK.SetSpriteAnimation(HPZEmerald->aniFrames, 0, &self->animator2, true, 0);
            RSDK.SetSpriteAnimation(HPZEmerald->aniFrames, 0, &self->animator, true, 1);
        }
        self->hitbox = RSDK.GetHitbox(&self->animator2, 0);
    }
}

void HPZEmerald_StageLoad(void) { HPZEmerald->aniFrames = RSDK.LoadSpriteAnimation("LRZ3/Emerald.bin", SCOPE_STAGE); }

#if RETRO_INCLUDE_EDITOR
void HPZEmerald_EditorDraw(void)
{
    RSDK_THIS(HPZEmerald);

    self->solid = false;
    switch (self->type) {
        case HPZEMERALD_MASTER:
        default:
        case HPZEMERALD_EMERALD_LOW: self->solid = true; break;
        case HPZEMERALD_EMERALD_HIGH: self->solid = true; break;
    }

    if (self->type) {
        RSDK.SetSpriteAnimation(HPZEmerald->aniFrames, 1, &self->animator2, true, 0);
    }
    else {
        RSDK.SetSpriteAnimation(HPZEmerald->aniFrames, 0, &self->animator2, true, 0);
        RSDK.SetSpriteAnimation(HPZEmerald->aniFrames, 0, &self->animator, true, 1);
    }

    HPZEmerald_Draw();
}

void HPZEmerald_EditorLoad(void)
{
    HPZEmerald->aniFrames = RSDK.LoadSpriteAnimation("LRZ3/Emerald.bin", SCOPE_STAGE);

    RSDK_ACTIVE_VAR(HPZEmerald, type);
    RSDK_ENUM_VAR("Master Emerald", HPZEMERALD_MASTER);
    RSDK_ENUM_VAR("Stone Emerald (Low Layer)", HPZEMERALD_EMERALD_LOW);
    RSDK_ENUM_VAR("Stone Emerald (High Layer)", HPZEMERALD_EMERALD_HIGH);
}
#endif

void HPZEmerald_Serialize(void) { RSDK_EDITABLE_VAR(HPZEmerald, VAR_UINT8, type); }
