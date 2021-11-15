#include "SonicMania.h"

ObjectSpikes *Spikes;

void Spikes_Update(void)
{
    RSDK_THIS(Spikes);
    switch (entity->stateMove) {
        case 1:
            if (entity->stagger << 6 == (Zone->timer & 0x40)) {
                if ((Zone->timer & 0x3F) == entity->timer) {
                    entity->stateMove++;
                    if (entity->activeScreens == 1)
                        RSDK.PlaySfx(Spikes->sfx_Move, 0, 255);
                }
            }
            break;
        case 2:
            if (entity->moveOffset >= 0x200000) {
                entity->stateMove++;
            }
            else {
                entity->moveOffset += 0x80000;
                entity->position.x += entity->velocity.x;
                entity->position.y += entity->velocity.y;
            }
            break;
        case 3:
            if ((Zone->timer & 0x3F) == entity->timer) {
                entity->stateMove++;
                if (entity->activeScreens == 1)
                    RSDK.PlaySfx(Spikes->sfx_Move, 0, 255);
            }
            break;
        case 4:
            if (entity->moveOffset <= 0) {
                entity->stateMove = 1;
            }
            else {
                entity->moveOffset -= 0x80000;
                entity->position.x -= entity->velocity.x;
                entity->position.y -= entity->velocity.y;
            }
            break;
        case 5:
            if (entity->moveOffset >= 0x280000) {
                entity->stateMove = 6;
            }
            else {
                entity->moveOffset += 0x80000;
                entity->position.x -= entity->velocity.x;
                entity->position.y -= entity->velocity.y;
            }
            break;
        default: break;
    }

    entity->position.x -= entity->offset.x;
    entity->position.y -= entity->offset.y;
    if (entity->stateMove != 1) {
        foreach_active(Player, player)
        {
            if (entity->planeFilter <= 0 || player->collisionPlane == (((uint8)entity->planeFilter - 1) & 1)) {
                EntityShield *shield = (EntityShield *)RSDK.GetEntityByID(Player->playerCount + RSDK.GetEntityID(player));
                if (!Ice || (player->state != Ice_State_FrozenPlayer && shield->animator.animationID != 2) || Press) {
                    int32 playerVelX = player->velocity.x;
                    int32 playerVelY = player->velocity.y;

                    uint8 side = Player_CheckCollisionBox(player, entity, &entity->hitbox);
                    if (side) {
                        if (Ice && player->shield == SHIELD_FIRE && player->invincibleTimer <= 0 && !Press && !entity->shatterTimer) {
                            Ice_ShatterGenerator(16, 16, 8, 0, 0, 0);
                            entity->shatterTimer = 15;
                        }

                        bool32 flag = false;
                        if (side == 4) {
                            player->collisionFlagV |= 2;
                            flag = false;
                        }
                        else {
                            if (side == 1) {
                                player->collisionFlagV |= 1;
                                if (entity->moveOffset == 0x80000)
                                    player->onGround = false;
                            }
                            flag = false;
#if RETRO_USE_PLUS
                            if (side == 1 && player->state == Player_State_MightyHammerDrop) {
                                if (Ice) {
                                    if (!Press) {
                                        flag = true;
                                        if (entity->type != 1) {
                                            player->onGround   = false;
                                            player->velocity.y = playerVelY;
                                        }
                                    }
                                }
                            }
#endif
                        }

                        switch (side) {
                            case 1:
                                player->collisionFlagV |= 1;
                                if (player->velocity.y >= 0 || entity->stateMove == 2) {
                                    player->position.x += entity->offset.x;
                                    player->position.y += entity->offset.y;
                                    if (side == entity->type)
                                        Spikes_CheckHit(player, playerVelX, playerVelY);
                                }
                                break;
                            case 2:
                                player->collisionFlagH |= 1;
                                if (player->velocity.x >= 0 || entity->stateMove == 2) {
                                    if (side == entity->type)
                                        Spikes_CheckHit(player, playerVelX, playerVelY);
                                }
                                break;
                            case 3:
                                player->collisionFlagH |= 2;
                                if (player->velocity.x <= 0 || entity->stateMove == 2) {
                                    if (side == entity->type)
                                        Spikes_CheckHit(player, playerVelX, playerVelY);
                                }
                                break;
                            case 4:
                                player->collisionFlagV |= 2;
                                if (player->velocity.y <= 0 || entity->stateMove == 2) {
                                    if (side == entity->type)
                                        Spikes_CheckHit(player, playerVelX, playerVelY);
                                }
                                break;
                            default: break;
                        }

                        if (flag) {
                            RSDK.PlaySfx(Ice->sfxWindowShatter, 0, 255);
                            Ice_ShatterGenerator(16,  16,  16,  0,  0,  0);
                            destroyEntity(entity);
                        }
                    }
                }
                else {
                    int32 posStoreX = player->position.x;
                    int32 posStoreY = player->position.y;

                    uint8 side = 0;
                    if (player->state == Ice_State_FrozenPlayer) {
                        side = RSDK.CheckObjectCollisionBox(entity, &entity->hitbox, player, &Ice->hitbox2, false);
                    }
                    else {
                        side = RSDK.CheckObjectCollisionBox(entity, &entity->hitbox, player, Player_GetHitbox(player), false);
                    }

                    switch (side) {
                        case 1:
                            if (player->velocity.y < 0x40000) {
                                if (Player_CheckCollisionBox(player, entity, &entity->hitbox)) {
                                    player->position.x += entity->offset.x;
                                    player->position.y += entity->offset.y;
                                }
                            }
                            else {
                                Spikes_Unknown1(0, 0);
                                player->position.x = posStoreX;
                                player->position.y = posStoreY;
                                foreach_return;
                            }
                            break;
                        case 2:
                            if (player->velocity.x >= 0x20000) {
                                Spikes_Unknown1(player->velocity.x, 0);
                                player->position.x = posStoreX;
                                player->position.y = posStoreY;
                                foreach_return;
                            }
                            else {
                                if (Player_CheckCollisionBox(player, entity, &entity->hitbox)) {
                                    player->position.x += entity->offset.x;
                                    player->position.y += entity->offset.y;
                                }
                            }
                            break;
                        case 3:
                            if (player->velocity.x > -0x20000) {
                                if (Player_CheckCollisionBox(player, entity, &entity->hitbox)) {
                                    player->position.x += entity->offset.x;
                                    player->position.y += entity->offset.y;
                                }
                            }
                            else {
                                Spikes_Unknown1(player->velocity.x, 0);
                                player->position.x = posStoreX;
                                player->position.y = posStoreY;
                                foreach_return;
                            }
                            break;
                        case 4:
                            if (player->velocity.y > -0x40000) {
                                if (Player_CheckCollisionBox(player, entity, &entity->hitbox)) {
                                    player->position.x += entity->offset.x;
                                    player->position.y += entity->offset.y;
                                }
                            }
                            else {
                                Spikes_Unknown1(0, player->velocity.y);
                                player->position.x = posStoreX;
                                player->position.y = posStoreY;
                                foreach_return;
                            }
                        default:
                            if (Player_CheckCollisionBox(player, entity, &entity->hitbox)) {
                                player->position.x += entity->offset.x;
                                player->position.y += entity->offset.y;
                            }
                            break;
                    }
                }
            }
        }
    }

    entity->position.x += entity->offset.x;
    entity->position.y += entity->offset.y;
    if (Ice && !Press) {
        RSDK.ProcessAnimation(&entity->animator);
        if (--entity->timer2 <= 0) {
            switch (entity->type) {
                case 1:
                case 4: RSDK.SetSpriteAnimation(Spikes->spriteIndex, 2, &entity->animator, true, 0); break;
                case 2:
                case 3: RSDK.SetSpriteAnimation(Spikes->spriteIndex, 3, &entity->animator, true, 0); break;
                default: break;
            }
            entity->timer2 = RSDK.Rand(0, 240) + 30;
        }
        if (entity->shatterTimer > 0) {
            entity->shatterTimer--;
            if (!entity->shatterTimer)
                Spikes_Unknown1(0, 0);
        }
    }
}

void Spikes_LateUpdate(void) {}

void Spikes_StaticUpdate(void) {}

void Spikes_Draw(void) { StateMachine_Run(Spikes->stateDraw); }

void Spikes_Create(void *data)
{
    RSDK_THIS(Spikes);
    entity->drawFX = FX_FLIP;
    if (entity->count < 2)
        entity->count = 2;
    if (!RSDK_sceneInfo->inEditor) {
        if (data)
            entity->type = voidToInt(data);

        entity->active  = ACTIVE_BOUNDS;
        int32 dir         = entity->type & 1;
        entity->visible = true;
        entity->type    = (entity->type >> 1) & 1;
        if (entity->planeFilter > 0 && ((uint8)entity->planeFilter - 1) & 2)
            entity->drawOrder = Zone->drawOrderHigh;
        else
            entity->drawOrder = Zone->drawOrderLow;
        entity->alpha = 128;

        switch (entity->type) {
            case 0:
                entity->updateRange.x = (entity->count + 6) << 20;
                entity->updateRange.y = 0x600000;
                entity->direction     = FLIP_Y * dir;
                if (entity->direction) {
                    entity->velocity.y = 0x80000;
                    entity->type       = 4;
                }
                else {
                    entity->velocity.y = -0x80000;
                    entity->type       = 1;
                }
                entity->hitbox.left   = -8 * entity->count;
                entity->hitbox.top    = -16;
                entity->hitbox.right  = 8 * entity->count;
                entity->hitbox.bottom = 16;
                break;
            case 1:
                entity->updateRange.x = 0x600000;
                entity->updateRange.y = (entity->count + 6) << 20;
                entity->direction     = dir;
                if (entity->direction) {
                    entity->velocity.x = -0x80000;
                    entity->type       = 2;
                }
                else {
                    entity->velocity.x = 0x80000;
                    entity->type       = 3;
                }
                entity->hitbox.left   = -16;
                entity->hitbox.top    = -8 * entity->count;
                entity->hitbox.right  = 16;
                entity->hitbox.bottom = 8 * entity->count;
                break;
        }

        if (entity->moving) {
            entity->position.x -= 4 * entity->velocity.x;
            entity->position.y -= 4 * entity->velocity.y;
            entity->stateMove = 1;
        }
    }
}

void Spikes_StageLoad(void)
{
    Spikes->stateDraw = Spikes_StateDraw_Stage;
    if (RSDK.CheckStageFolder("FBZ")) {
        Spikes->spriteIndex = RSDK.LoadSpriteAnimation("FBZ/Spikes.bin", SCOPE_STAGE);
    }
    if (RSDK.CheckStageFolder("PSZ2")) {
        Spikes->spriteIndex = RSDK.LoadSpriteAnimation("PSZ2/Spikes.bin", SCOPE_STAGE);
    }
    else {
        Spikes->spriteIndex = RSDK.LoadSpriteAnimation("Global/Spikes.bin", SCOPE_STAGE);
        Spikes->stateDraw   = Spikes_StateDraw_Global;
    }
    RSDK.SetSpriteAnimation(Spikes->spriteIndex, 0, &Spikes->vData, true, 0);
    RSDK.SetSpriteAnimation(Spikes->spriteIndex, 1, &Spikes->hData, true, 0);
    Spikes->dword3C   = 0x100000;
    Spikes->dword40   = 0x100000;
    Spikes->sfx_Move  = RSDK.GetSFX("Global/SpikesMove.wav");
    Spikes->sfx_Spike = RSDK.GetSFX("Global/Spike.wav");
}

void Spikes_StateDraw_Global(void)
{
    Vector2 drawPos;

    RSDK_THIS(Spikes);
    drawPos.x = entity->position.x;
    drawPos.y = entity->position.y;
    int32 cnt   = entity->count >> 1;
    switch (entity->type) {
        case 1:
        case 4:
            drawPos.x = 0x100000 - (entity->count << 19) + entity->position.x;
            for (; cnt; --cnt) {
                RSDK.DrawSprite(&Spikes->vData, &drawPos, false);
                drawPos.x += 0x200000;
            }

            if (entity->count & 1) {
                drawPos.x -= 0x100000;
                RSDK.DrawSprite(&Spikes->vData, &drawPos, false);
            }
            break;
        case 2:
        case 3:
            drawPos.y = 0x100000 - (entity->count << 19) + entity->position.y;
            for (; cnt; --cnt) {
                RSDK.DrawSprite(&Spikes->hData, &drawPos, false);
                drawPos.y += 0x200000;
            }

            if (entity->count & 1) {
                drawPos.y -= 0x100000;
                RSDK.DrawSprite(&Spikes->hData, &drawPos, false);
            }
            break;
        default: break;
    }
    entity->inkEffect = INK_ADD;
    RSDK.DrawSprite(&entity->animator, NULL, false);
    entity->inkEffect = INK_NONE;
}

void Spikes_StateDraw_Stage(void)
{
    Vector2 drawPos;

    RSDK_THIS(Spikes);
    drawPos.x = entity->position.x;
    drawPos.y = entity->position.y;
    int32 cnt   = entity->count >> 1;
    switch (entity->type) {
        case 1:
        case 4:
            drawPos.x = (0x100000 - (entity->count << 19)) + entity->position.x;
            for (; cnt; --cnt) {
                RSDK.DrawSprite(&Spikes->vData, &drawPos, 0);
                drawPos.x += 0x200000;
            }
            if (entity->count & 1) {
                drawPos.x -= 0x100000;
                RSDK.DrawSprite(&Spikes->vData, &drawPos, 0);
            }
            break;
        case 2:
        case 3:
            drawPos.y = (0x100000 - (entity->count << 19)) + entity->position.y;
            for (; cnt; --cnt) {
                RSDK.DrawSprite(&Spikes->hData, &drawPos, 0);
                drawPos.y += 0x200000;
            }
            if (entity->count & 1) {
                drawPos.y -= 0x100000;
                RSDK.DrawSprite(&Spikes->hData, &drawPos, 0);
            }
            break;
        default: break;
    }
}

void Spikes_Unknown1(int32 velX, int32 velY)
{
    RSDK_THIS(Spikes);
    RSDK.PlaySfx(Ice->sfxWindowShatter, 0, 255);
    Ice_ShatterGenerator(16,  16,  16,  velX,  velY,  0);
    destroyEntity(entity);
}
void Spikes_CheckHit(EntityPlayer *player, int32 playerVelX, int32 playerVelY)
{
    RSDK_THIS(Spikes);
    if (player->state == Player_State_Hit)
        return;
    if (!Player_CheckValidState(player) || player->invincibleTimer || player->blinkTimer > 0)
        return;

#if RETRO_USE_PLUS
    if (player->characterID == ID_MIGHTY
        && (player->playerAnimator.animationID == ANI_JUMP || player->playerAnimator.animationID == ANI_SPINDASH
            || player->playerAnimator.animationID == ANI_DROPDASH)) {

        if (abs(playerVelX) < 0x20000) {
            switch (entity->type) {
                case 1:
                    if (player->playerAnimator.animationID != ANI_DROPDASH || (Ice && !Press)) {
                        player->velocity.y = -0x48000;
                        if (!(player->direction & FLIP_X))
                            player->velocity.x = 0x48000;
                        else
                            player->velocity.x = -0x48000;
                        player->state = Player_State_Air;
                    }
                    else {
                        player->velocity.y = -0x48000;
                        if (!(player->direction & FLIP_X))
                            player->velocity.x = -0x28000;
                        else
                            player->velocity.x = 0x28000;
                        player->blinkTimer = 60;
                        player->state      = Player_State_Hit;
                        RSDK.StopSFX(Player->sfx_MightyDrill);
                    }

                    player->velocity.x -= player->velocity.x >> 2;
                    break;
                case 2:
                    player->velocity.y = -0x40000;
                    player->velocity.x = -0x28000;
                    player->state      = Player_State_Air;
                    break;
                case 3:
                    player->velocity.y = -0x40000;
                    player->velocity.x = 0x28000;
                    player->state      = Player_State_Air;
                    break;
                case 4:
                    player->velocity.y = 0x20000;
                    player->state      = Player_State_Air;
                    break;
                default: break;
            }

            player->onGround         = false;
            player->jumpAbility      = 0;
            player->jumpAbilityTimer = 0;
            if (player->state == Player_State_Hit) {
                RSDK.SetSpriteAnimation(player->spriteIndex, ANI_HURT, &player->playerAnimator, false, 0);
                RSDK.PlaySfx(Spikes->sfx_Spike, 0, 255);
            }
            else {
                RSDK.SetSpriteAnimation(player->spriteIndex, ANI_FLY, &player->playerAnimator, false, 0);
                RSDK.PlaySfx(Player->sfx_MightyUnspin, 0, 255);
            }
            if (player->playerAnimator.animationID != ANI_FLY)
                RSDK.PlaySfx(Player->sfx_PimPom, 0, 255);
            if (player->underwater) {
                player->velocity.x >>= 1;
                player->velocity.y >>= 1;
            }
        }
        else if (entity->type == 1) {
            if (player->playerAnimator.animationID == ANI_DROPDASH) {
                player->velocity.y = -0x48000;
                if (!(player->direction & FLIP_X))
                    player->velocity.x = 0x48000;
                else
                    player->velocity.x = -0x48000;
                player->state = Player_State_Air;
                player->velocity.x -= player->velocity.x >> 2;
                player->onGround         = false;
                player->jumpAbility      = 0;
                player->jumpAbilityTimer = 0;
                if (player->state == Player_State_Hit) {
                    RSDK.SetSpriteAnimation(player->spriteIndex, ANI_HURT, &player->playerAnimator, false, 0);
                    RSDK.PlaySfx(Spikes->sfx_Spike, 0, 255);
                }
                else {
                    RSDK.SetSpriteAnimation(player->spriteIndex, ANI_FLY, &player->playerAnimator, false, 0);
                    RSDK.PlaySfx(Player->sfx_MightyUnspin, 0, 255);
                }
                if (player->playerAnimator.animationID != ANI_FLY)
                    RSDK.PlaySfx(Player->sfx_PimPom, 0, 255);
                if (player->underwater) {
                    player->velocity.x >>= 1;
                    player->velocity.y >>= 1;
                }
            }
            else {
                if (playerVelY > 0x28000) {
                    player->velocity.y       = -0x20000;
                    player->state            = Player_State_Air;
                    player->onGround         = false;
                    player->jumpAbility      = 0;
                    player->jumpAbilityTimer = 0;
                    RSDK.PlaySfx(Player->sfx_PimPom, 0, 255);
                }

                player->groundedStore   = true;
                player->state           = Player_State_Roll;
                player->nextGroundState = Player_State_Roll;
                player->nextAirState    = Player_State_Air;
            }
        }
        return; // dont do the code below
    }
#endif

    if (player->position.x > entity->position.x)
        player->velocity.x = 0x20000;
    else
        player->velocity.x = -0x20000;
    Player_Hit(player);
    if (player->hurtFlag == 1) {
        player->hurtFlag = 2;
        RSDK.PlaySfx(Spikes->sfx_Spike, 0, 255);
    }
    else if (player->state == Player_State_Hit && (player->shield || player->sidekick)) {
        RSDK.StopSFX(Player->sfx_Hurt);
        RSDK.PlaySfx(Spikes->sfx_Spike, 0, 255);
    }
}

#if RETRO_INCLUDE_EDITOR
void Spikes_EditorDraw(void)
{
    RSDK_THIS(Spikes);

    int32 dir = entity->type & 1;
    int32 type = 0;
    switch ((entity->type >> 1) & 1) {
        case 0:
            entity->direction = FLIP_Y * dir;
            if (entity->direction)
                type = 4;
            else 
                type = 1;
            break;
        case 1:
            entity->direction = dir;
            if (entity->direction) 
                type = 2;
            else 
                type = 3;
            break;
    }

    Vector2 drawPos;
    drawPos.x = entity->position.x;
    drawPos.y = entity->position.y;
    int32 cnt = entity->count >> 1;
    switch (type) {
        case 1:
        case 4:
            drawPos.x = 0x100000 - (entity->count << 19) + entity->position.x;
            for (; cnt; --cnt) {
                RSDK.DrawSprite(&Spikes->vData, &drawPos, false);
                drawPos.x += 0x200000;
            }

            if (entity->count & 1) {
                drawPos.x -= 0x100000;
                RSDK.DrawSprite(&Spikes->vData, &drawPos, false);
            }
            break;
        case 2:
        case 3:
            drawPos.y = 0x100000 - (entity->count << 19) + entity->position.y;
            for (; cnt; --cnt) {
                RSDK.DrawSprite(&Spikes->hData, &drawPos, false);
                drawPos.y += 0x200000;
            }

            if (entity->count & 1) {
                drawPos.y -= 0x100000;
                RSDK.DrawSprite(&Spikes->hData, &drawPos, false);
            }
            break;
        default: break;
    }

}

void Spikes_EditorLoad(void)
{
    Spikes->stateDraw = Spikes_StateDraw_Stage;
    if (RSDK.CheckStageFolder("FBZ")) {
        Spikes->spriteIndex = RSDK.LoadSpriteAnimation("FBZ/Spikes.bin", SCOPE_STAGE);
    }
    if (RSDK.CheckStageFolder("PSZ2")) {
        Spikes->spriteIndex = RSDK.LoadSpriteAnimation("PSZ2/Spikes.bin", SCOPE_STAGE);
    }
    else {
        Spikes->spriteIndex = RSDK.LoadSpriteAnimation("Global/Spikes.bin", SCOPE_STAGE);
        Spikes->stateDraw   = Spikes_StateDraw_Global;
    }
    RSDK.SetSpriteAnimation(Spikes->spriteIndex, 0, &Spikes->vData, true, 0);
    RSDK.SetSpriteAnimation(Spikes->spriteIndex, 1, &Spikes->hData, true, 0);

    RSDK_ACTIVE_VAR(Spikes, type);
    RSDK_ENUM_VAR("Up", SPIKES_UP);
    RSDK_ENUM_VAR("Down", SPIKES_DOWN);
    RSDK_ENUM_VAR("Left", SPIKES_LEFT);
    RSDK_ENUM_VAR("Right", SPIKES_RIGHT);

    RSDK_ACTIVE_VAR(Spikes, planeFilter);
    RSDK_ENUM_VAR("No Filter", PLANEFILTER_NONE);
    RSDK_ENUM_VAR("Plane A", PLANEFILTER_A);
    RSDK_ENUM_VAR("Plane B", PLANEFILTER_B);
}
#endif

void Spikes_Serialize(void)
{
    RSDK_EDITABLE_VAR(Spikes, VAR_ENUM, type);
    RSDK_EDITABLE_VAR(Spikes, VAR_BOOL, moving);
    RSDK_EDITABLE_VAR(Spikes, VAR_UINT8, count);
    RSDK_EDITABLE_VAR(Spikes, VAR_UINT8, stagger);
    RSDK_EDITABLE_VAR(Spikes, VAR_UINT8, timer);
    RSDK_EDITABLE_VAR(Spikes, VAR_ENUM, planeFilter);
}
