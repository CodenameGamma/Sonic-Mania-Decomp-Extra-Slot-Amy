#include "SonicMania.h"

ObjectSpring *Spring;

void Spring_Update(void)
{
    RSDK_THIS(Spring);

    if (entity->timer > 0)
        entity->timer--;
    StateMachine_Run(entity->state);
    RSDK.ProcessAnimation(&entity->animator);
    if (entity->animator.frameID == 8)
        entity->animator.animationSpeed = 0;
}

void Spring_LateUpdate(void) {}

void Spring_StaticUpdate(void) {}

void Spring_Draw(void)
{
    RSDK_THIS(Spring);
    RSDK.DrawSprite(&entity->animator, NULL, false);
}

void Spring_Create(void *data)
{
    RSDK_THIS(Spring);
    entity->drawFX = FX_FLIP;
    if (!RSDK_sceneInfo->inEditor) {
        entity->type %= 6;
        if (data) {
            int32 propertyVal  = voidToInt(data);
            entity->type     = (propertyVal >> 0) & 0xFF;
            entity->flipFlag = (propertyVal >> 8) & 0xFF;
        }
        RSDK.SetSpriteAnimation(Spring->spriteIndex, entity->type, &entity->animator, true, 0);
        entity->active                  = ACTIVE_BOUNDS;
        entity->animator.animationSpeed = 0;
        entity->updateRange.x           = 0x600000;
        entity->updateRange.y           = 0x600000;
        entity->visible                 = true;
        if (entity->planeFilter && ((uint8)entity->planeFilter - 1) & 2)
            entity->drawOrder = Zone->drawOrderHigh;
        else
            entity->drawOrder = Zone->drawOrderLow;

        switch (entity->type >> 1) {
            case 0:
                entity->direction = entity->flipFlag;
                if (entity->type & 1)
                    entity->velocity.y = 0x100000;
                else
                    entity->velocity.y = 0xA0000;
                if (!entity->flipFlag)
                    entity->velocity.y = -entity->velocity.y;
                entity->hitbox.left   = -16;
                entity->hitbox.top    = -8;
                entity->hitbox.right  = 16;
                entity->hitbox.bottom = 8;
                entity->state         = Spring_State_Vertical;
                break;
            case 1:
                entity->direction = entity->flipFlag;
                if (entity->type & 1)
                    entity->velocity.x = 0x100000;
                else
                    entity->velocity.x = 0xA0000;
                if (entity->flipFlag)
                    entity->velocity.x = -entity->velocity.x;
                entity->hitbox.left   = -8;
                entity->hitbox.top    = -16;
                entity->hitbox.right  = 8;
                entity->hitbox.bottom = 16;
                entity->state         = Spring_State_Horizontal;
                break;
            case 2:
                entity->direction = entity->flipFlag;
                if (entity->type & 1) {
                    entity->velocity.x = 0xB4000;
                    entity->velocity.y = 0xB4000;
                }
                else {
                    entity->velocity.x = 0x74000;
                    entity->velocity.y = 0x74000;
                }
                if (entity->flipFlag < FLIP_Y)
                    entity->velocity.y = -entity->velocity.y;
                if (entity->flipFlag & FLIP_X)
                    entity->velocity.x = -entity->velocity.x;
                entity->hitbox.left   = -12;
                entity->hitbox.top    = -12;
                entity->hitbox.right  = 12;
                entity->hitbox.bottom = 12;
                entity->state         = Spring_State_Diagonal;
                break;
        }
    }
}

void Spring_StageLoad(void)
{
    Spring->spriteIndex = RSDK.LoadSpriteAnimation("Global/Springs.bin", SCOPE_STAGE);
    Spring->sfx_Spring  = RSDK.GetSFX("Global/Spring.wav");
}

void Spring_State_Vertical(void)
{
    RSDK_THIS(Spring);
    if (entity->direction == FLIP_NONE) {
        foreach_active(Player, player)
        {
            if (!entity->planeFilter || player->collisionPlane == ((uint8)(entity->planeFilter - 1) & 1)) {
                int32 col = (entity->type == 0xFF && player->velocity.y < -0x50000) ? Player_CheckCollisionPlatform(player, entity, &entity->hitbox)
                                                                                  : Player_CheckCollisionBox(player, entity, &entity->hitbox);
                if (col == 1) {
                    int32 anim = player->playerAnimator.animationID;
                    if (anim == ANI_WALK || (anim > ANI_AIRWALK && anim <= ANI_DASH))
                        player->storedAnim = player->playerAnimator.animationID;
                    else
                        player->storedAnim = ANI_WALK;

                    if (player->state != Ice_State_FrozenPlayer) {
                        if (player->state == Player_State_ForceRoll_Air || player->state == Player_State_ForceRoll_Ground) {
                            player->state = Player_State_ForceRoll_Air;
                        }
                        else {
                            RSDK.SetSpriteAnimation(player->spriteIndex, ANI_SPRINGTWIRL, &player->playerAnimator, true, 0);
                            player->state = Player_State_Air;
                        }
                    }
                    player->onGround                = false;
                    player->velocity.y              = entity->velocity.y;
                    player->tileCollisions          = true;
                    entity->animator.animationSpeed = 0x80;
                    entity->animator.animationTimer = 0;
                    entity->animator.frameID        = 1;
                    if (entity->timer == 0) {
                        RSDK.PlaySfx(Spring->sfx_Spring, 0, 255);
                        entity->timer = 8;
                    }
                }
            }
        }
    }
    else {
        foreach_active(Player, player)
        {
            if ((!entity->planeFilter || player->collisionPlane == ((uint8)(entity->planeFilter - 1) & 1))
                && Player_CheckCollisionBox(player, entity, &entity->hitbox) == 4) {
                if (player->state != Ice_State_FrozenPlayer) {
                    if (player->state == Player_State_ForceRoll_Air || player->state == Player_State_ForceRoll_Ground)
                        player->state = Player_State_ForceRoll_Air;
                    else
                        player->state = Player_State_Air;
                }
                player->onGround                = false;
                player->velocity.y              = entity->velocity.y;
                player->tileCollisions          = true;
                entity->animator.animationSpeed = 0x80;
                entity->animator.animationTimer = 0;
                entity->animator.frameID        = 1;
                if (!entity->timer) {
                    RSDK.PlaySfx(Spring->sfx_Spring, 0, 255);
                    entity->timer = 8;
                }
            }
        }
    }
}
void Spring_State_Horizontal(void)
{
    RSDK_THIS(Spring);
    if (entity->direction == FLIP_NONE) {
        foreach_active(Player, player)
        {
            if ((!entity->planeFilter || player->collisionPlane == ((uint8)(entity->planeFilter - 1) & 1))
                && Player_CheckCollisionBox(player, entity, &entity->hitbox) == 3 && (!entity->onGround || player->onGround)) {
                if (player->collisionMode == CMODE_ROOF) {
                    player->velocity.x = -entity->velocity.x;
                    player->groundVel  = -entity->velocity.x;
                }
                else {
                    player->velocity.x = entity->velocity.x;
                    player->groundVel  = player->velocity.x;
                }

                if (player->state != Ice_State_FrozenPlayer) {
                    if (player->state != Player_State_Roll && player->state != Player_State_ForceRoll_Air && player->state != Player_State_ForceRoll_Ground) {
                        if (player->onGround)
                            player->state = Player_State_Ground;
                        else
                            player->state = Player_State_Air;
                    }
                    int32 anim = player->playerAnimator.animationID;
                    if (anim != ANI_JUMP && anim != ANI_JOG && anim != ANI_RUN && anim != ANI_DASH)
                        player->playerAnimator.animationID = ANI_WALK;
                }
                player->controlLock             = 16;
                player->skidding                = 0;
                player->pushing                 = false;
                player->direction               = FLIP_NONE;
                player->tileCollisions          = true;
                entity->animator.animationSpeed = 0x80;
                entity->animator.animationTimer = 0;
                entity->animator.frameID        = 1;
                if (entity->timer == 0) {
                    RSDK.PlaySfx(Spring->sfx_Spring, 0, 255);
                    entity->timer = 8;
                }
            }
        }
    }
    else {
        foreach_active(Player, player)
        {
            if ((!entity->planeFilter || player->collisionPlane == ((uint8)(entity->planeFilter - 1) & 1))
                && Player_CheckCollisionBox(player, entity, &entity->hitbox) == 2 && (!entity->onGround || player->onGround)) {
                if (player->collisionMode == CMODE_ROOF) {
                    player->velocity.x = -entity->velocity.x;
                    player->groundVel  = -entity->velocity.x;
                }
                else {
                    player->velocity.x = entity->velocity.x;
                    player->groundVel  = player->velocity.x;
                }

                if (player->state != Ice_State_FrozenPlayer) {
                    if (player->state != Player_State_Roll && player->state != Player_State_ForceRoll_Air && player->state != Player_State_ForceRoll_Ground) {
                        if (player->onGround)
                            player->state = Player_State_Ground;
                        else
                            player->state = Player_State_Air;
                    }
                    int32 anim = player->playerAnimator.animationID;
                    if (anim != ANI_JUMP && anim != ANI_JOG && anim != ANI_RUN && anim != ANI_DASH)
                        player->playerAnimator.animationID = ANI_WALK;
                }
                player->controlLock             = 16;
                player->skidding                = 0;
                player->pushing                 = false;
                player->direction               = FLIP_X;
                player->tileCollisions          = true;
                entity->animator.animationSpeed = 0x80;
                entity->animator.animationTimer = 0;
                entity->animator.frameID        = 1;
                if (entity->timer == 0) {
                    RSDK.PlaySfx(Spring->sfx_Spring, 0, 255);
                    entity->timer = 8;
                }
            }
        }
    }
}
void Spring_State_Diagonal(void)
{
    RSDK_THIS(Spring);
    foreach_active(Player, player)
    {
        if ((!entity->planeFilter || player->collisionPlane == ((uint8)(entity->planeFilter - 1) & 1))) {
            if (Player_CheckCollisionTouch(player, entity, &entity->hitbox)) {
                bool32 flag = false;
                if (player->onGround) {
                    flag = true;
                }
                else {
                    int32 y = player->velocity.y;
                    if (y >= 0) {
                        flag = true;
                    }
                    else {
                        int32 x = player->velocity.x;
                        if (x < 0)
                            x = -x;
                        flag = x > -y;
                    }
                }
                if (flag) {
                    if (player->state != Ice_State_FrozenPlayer) {
                        if (player->state == Player_State_ForceRoll_Air || player->state == Player_State_ForceRoll_Ground) {
                            player->state = Player_State_ForceRoll_Air;
                        }
                        else {
                            player->state = Player_State_Air;
                            int32 anim      = player->playerAnimator.animationID;
                            if (anim != ANI_JUMP && anim != ANI_JOG && anim != ANI_RUN && anim != ANI_DASH)
                                player->playerAnimator.animationID = ANI_WALK;
                        }
                    }
                    if (entity->direction < FLIP_Y) {
                        if (player->state != Player_State_ForceRoll_Air && player->state != Player_State_ForceRoll_Ground) {
                            int32 anim = player->playerAnimator.animationID;
                            if (anim == ANI_WALK || (anim > ANI_AIRWALK && anim <= ANI_DASH))
                                player->storedAnim = player->playerAnimator.animationID;
                            else
                                player->storedAnim = ANI_WALK;
                            RSDK.SetSpriteAnimation(player->spriteIndex, ANI_SPRINGDIAGONAL, &player->playerAnimator, true, 0);
                        }
                    }
                    player->direction               = entity->direction & 1;
                    player->onGround                = false;
                    player->velocity.x              = entity->velocity.x;
                    player->velocity.y              = entity->velocity.y;
                    player->tileCollisions          = true;
                    entity->animator.animationSpeed = 0x80;
                    entity->animator.animationTimer = 0;
                    entity->animator.frameID        = 1;
                    if (entity->timer == 0) {
                        RSDK.PlaySfx(Spring->sfx_Spring, 0, 255);
                        entity->timer = 8;
                    }
                }
            }
        }
    }
}

#if RETRO_INCLUDE_EDITOR
void Spring_EditorDraw(void)
{
    RSDK_THIS(Spring);
    RSDK.SetSpriteAnimation(Spring->spriteIndex, entity->type, &entity->animator, true, 0);
    entity->direction = entity->flipFlag;
    RSDK.DrawSprite(&entity->animator, NULL, false);
}

void Spring_EditorLoad(void)
{
    Spring->spriteIndex = RSDK.LoadSpriteAnimation("Global/Springs.bin", SCOPE_STAGE);

    RSDK_ACTIVE_VAR(Spring, type);
    RSDK_ENUM_VAR("Vertical (Red)", SPRING_VERT_RED);
    RSDK_ENUM_VAR("Vertical (Yellow)", SPRING_VERT_YELLOW);
    RSDK_ENUM_VAR("Horizontal (Red)", SPRING_HORIZ_RED);
    RSDK_ENUM_VAR("Horizontal (Yellow)", SPRING_HORIZ_YELLOW);
    RSDK_ENUM_VAR("Diagonal (Red)", SPRING_DIAG_RED);
    RSDK_ENUM_VAR("Diagonal (Yellow)", SPRING_DIAG_YELLOW);

    RSDK_ACTIVE_VAR(Spring, flipFlag);
    RSDK_ENUM_VAR("No Flip", FLIP_NONE);
    RSDK_ENUM_VAR("Flip X", FLIP_X);
    RSDK_ENUM_VAR("Flip Y", FLIP_Y);
    RSDK_ENUM_VAR("Flip XY", FLIP_XY);

    RSDK_ACTIVE_VAR(Spring, planeFilter);
    RSDK_ENUM_VAR("No Filter", PLANEFILTER_NONE);
    RSDK_ENUM_VAR("Plane A", PLANEFILTER_A);
    RSDK_ENUM_VAR("Plane B", PLANEFILTER_B);
}
#endif

void Spring_Serialize(void)
{
    RSDK_EDITABLE_VAR(Spring, VAR_ENUM, type);
    RSDK_EDITABLE_VAR(Spring, VAR_ENUM, flipFlag);
    RSDK_EDITABLE_VAR(Spring, VAR_BOOL, onGround);
    RSDK_EDITABLE_VAR(Spring, VAR_UINT8, planeFilter);
}
