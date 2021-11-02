#include "SonicMania.h"

ObjectDNARiser *DNARiser;

void DNARiser_Update(void)
{
    RSDK_THIS(DNARiser);

    if (entity->field_A8)
        entity->field_A8--;
    if (entity->timer2 > 0)
        entity->timer2--;
    StateMachine_Run(entity->state);
    if (entity->popped) {
        if (entity->animator.frameID == entity->animator.loopIndex)
            entity->popped = false;
        else
            RSDK.ProcessAnimation(&entity->animator);
    }
}

void DNARiser_LateUpdate(void) {}

void DNARiser_StaticUpdate(void)
{
    foreach_active(DNARiser, entity) { RSDK.AddDrawListRef(Zone->drawOrderHigh, RSDK.GetEntityID(entity)); }
}

void DNARiser_Draw(void)
{
    RSDK_THIS(DNARiser);
    StateMachine_Run(entity->stateDraw);
}

void DNARiser_Create(void *data)
{
    RSDK_THIS(DNARiser);
    entity->visible   = true;
    entity->active    = ACTIVE_BOUNDS;
    entity->drawOrder = Zone->drawOrderLow + 1;
    entity->field_CC  = 0x2E0000;
    if (RSDK_sceneInfo->inEditor) {
        entity->speed.x = 0;
        if (!entity->speed.y)
            entity->speed.y = 0x60000;
        if (!entity->height)
            entity->height = 128;
    }
    entity->updateRange.x = 0x800000;
    entity->updateRange.y = 0x800000;
    entity->state         = DNARiser_State_Setup;
}

void DNARiser_StageLoad(void)
{
    DNARiser->aniFrames     = RSDK.LoadSpriteAnimation("CPZ/DNARiser.bin", SCOPE_STAGE);
    DNARiser->hitbox.top    = -16;
    DNARiser->hitbox.bottom = 16;
    DNARiser->hitbox.left   = -16;
    DNARiser->hitbox.right  = 16;
    DNARiser->active        = ACTIVE_ALWAYS;
    DNARiser->sfxBurst      = RSDK.GetSFX("CPZ/DNABurst.wav");
    DNARiser->sfxGrab       = RSDK.GetSFX("CPZ/DNAGrab.wav");
    DNARiser->sfxScan       = RSDK.GetSFX("CPZ/DNAScan.wav");
    DNARiser->sfxTiny[0]    = RSDK.GetSFX("CPZ/DNATiny0.wav");
    DNARiser->sfxTiny[1]    = RSDK.GetSFX("CPZ/DNATiny1.wav");
    DNARiser->sfxTiny[2]    = RSDK.GetSFX("CPZ/DNATiny2.wav");
    DNARiser->sfxTiny[3]    = RSDK.GetSFX("CPZ/DNATiny3.wav");
    DNARiser->sfxTiny[4]    = RSDK.GetSFX("CPZ/DNATiny4.wav");
    DNARiser->sfxTiny[5]    = RSDK.GetSFX("CPZ/DNATiny5.wav");
}

void DNARiser_State_BubbleBurst(void)
{
    RSDK_THIS(DNARiser);
    RSDK.PlaySfx(DNARiser->sfxBurst, 0, 255);
    entity->popPos = entity->position;
    entity->popped = true;
    RSDK.SetSpriteAnimation(DNARiser->aniFrames, 5, &entity->animator, true, 0);
}

void DNARiser_State_Burst_Helix(void)
{
    RSDK_THIS(DNARiser);
    RSDK.PlaySfx(DNARiser->sfxBurst, 0, 255);
    entity->popPos = entity->position;
    entity->popped = true;
    RSDK.SetSpriteAnimation(DNARiser->aniFrames, 5, &entity->animator, true, 0);
    entity->vector_D8  = entity->speed;
    entity->field_60   = 1;
    entity->velocity.x = 0;
    entity->velocity.y = 0;
    entity->state      = DNARiser_State_Unknown4;
    entity->timer      = 0;
}

Vector2 DNARiser_CalculateScale(Vector2 *vec)
{
    RSDK_THIS(DNARiser);

    Vector2 resultVec;
    int32 x = 512;
    int32 y = 512;
    if (vec) {
        x = vec->x;
        y = vec->y;
    }

    int32 val     = (30 - entity->field_A8);
    int32 sine    = RSDK.Sin1024(((val * (0x40000000 / ((10983 * val + 286520) >> 7))) >> 11) & 0x3FF);
    resultVec.x = (((val - 30) * (sine << 6) / 100 + 0x10000) * x) >> 16;
    resultVec.y = (((30 - val) * (sine << 6) / 100 + 0x10000) * y) >> 16;
    return resultVec;
}

void DNARiser_State_Setup(void)
{
    RSDK_THIS(DNARiser);
    entity->vector_D0     = entity->position;
    entity->active        = ACTIVE_BOUNDS;
    entity->speed.x       = 0;
    entity->velocity.x    = 0;
    entity->speed.y       = -abs(entity->speed.y);
    entity->timer         = 0;
    entity->curHeight     = 0;
    entity->field_AC      = 0;
    entity->field_E4      = 0;
    entity->child         = 0;
    entity->field_E0      = 0;
    entity->activePlayers = 0;
    entity->field_60      = 0;
    entity->field_B6      = 0;
    entity->timer2        = 0;
    entity->field_B0      = 0;
    entity->velocity.y    = 0;
    entity->stateDraw     = DNARiser_StateDraw_Main;
    entity->state         = DNARiser_HandleInteractions;
    entity->field_70      = 512;
    RSDK.SetSpriteAnimation(DNARiser->aniFrames, 0, &entity->animator2, true, 0);
    DNARiser_HandleInteractions();
}

void DNARiser_HandleInteractions(void)
{
    RSDK_THIS(DNARiser);

    foreach_active(Player, player)
    {
        int32 playerID = RSDK.GetEntityID(player);

        if (!((1 << playerID) & entity->activePlayers)) {
            if (Player_CheckCollisionTouch(player, entity, &DNARiser->hitbox)) {
                RSDK.PlaySfx(DNARiser->sfxGrab, 0, 255);
                entity->field_A8 = 30;
                entity->timer2   = 30;
                RSDK.SetSpriteAnimation(player->spriteIndex, ANI_JUMP, &player->playerAnimator, false, 0);
                player->groundVel        = 0;
                player->drawOrder        = 1;
                player->jumpAbilityTimer = 1;
                player->onGround         = false;
                player->tileCollisions   = false;
                player->state            = Player_State_None;
                player->nextAirState     = 0;
                player->nextGroundState  = 0;
                entity->activePlayers |= 1 << playerID;
            }
        }

        if (((1 << playerID) & entity->activePlayers)) {
            if (player->state == Player_State_None) {
                player->position.x = entity->position.x;
                player->position.y = entity->position.y;
                player->velocity.x = 0;
                player->velocity.y = 0;
                if (!player->sidekick) {
                    entity->curHeight = 0;
                    entity->active    = 2;
                    entity->state     = DNARiser_State_Unknown3;
                }
            }
            else {
                entity->activePlayers &= ~(1 << playerID);
            }
        }
    }
}

void DNARiser_State_Unknown3(void)
{
    RSDK_THIS(DNARiser);
    if (entity->curHeight >= entity->height << 16) {
        DNARiser_State_Burst_Helix();
    }
    else {
        if (entity->velocity.y >= abs(entity->speed.y)) {
            entity->velocity.y = abs(entity->speed.y);
        }
        else {
            entity->velocity.y += 0x1200;
        }

        entity->curHeight += entity->velocity.y;
        if (entity->height << 16 < entity->curHeight)
            entity->curHeight = entity->height << 16;

        int32 angle = (((0x57262 * (entity->curHeight >> 16)) >> 16) + 250) & 0x3FF;
        if (angle >= 0x200u && !entity->field_B0) {
            entity->field_B0 = 1;
            RSDK.PlaySfx(DNARiser->sfxScan, 0, 255);
        }
        else if (angle <= 0x1FF && entity->field_B0) {
            entity->field_B0 = 0;
            RSDK.PlaySfx(DNARiser->sfxScan, 0, 255);
        }

        entity->position.y = entity->vector_D0.y - entity->curHeight;
        for (; entity->field_E0 <= (entity->curHeight >> 18) + 1; ++entity->field_E0) {
            if (!(entity->field_E0 & 1) || !(entity->field_E0 % 5)) {
                if (!entity->timer2) {
                    int32 sfxID = 0;
                    if (!entity->field_B6) {
                        do
#if RETRO_USE_PLUS
                            sfxID = RSDK.Random(0, 5, &Zone->randKey);
#else
                            sfxID = RSDK.Rand(0, 5);
#endif
                        while (sfxID == entity->field_B6);
                    }
                    RSDK.PlaySfx(DNARiser->sfxTiny[sfxID], 0, 255);
                    entity->field_B6 = sfxID;
#if RETRO_USE_PLUS
                    entity->timer2   = RSDK.Random(2, 8, &Zone->randKey);
#else
                    entity->timer2 = RSDK.Rand(2, 8);
#endif
                }
                EntityDNARiser *child = CREATE_ENTITY(DNARiser, entity, entity->vector_D0.x, entity->vector_D0.y - (entity->field_E0 << 18));
                child->state          = DNARiser_State_SetupChild;
                child->active         = ACTIVE_NORMAL;
                child->field_A4       = entity->field_E0;
                child->parent         = (Entity *)entity;
                child->angle          = (((0x57262 * (4 * entity->field_E0)) >> 16) + 100) & 0x3FF;
                child->field_CC       = 0x2E0000;
                child->drawOrder      = entity->drawOrder - 1;
                if (!entity->field_E4)
                    entity->field_E4 = (Entity *)child;

                if (entity->child) {
                    child->field_F0                             = entity->child;
                    ((EntityDNARiser *)entity->child)->field_EC = (Entity *)child;
                }
                entity->child = (Entity *)child;
            }
        }
    }

    bool32 flag      = false;
    entity->field_70 = (0x10000 - 85 * ((entity->curHeight / entity->height) >> 8)) >> 7;

    foreach_active(Player, player)
    {
        int32 playerID = RSDK.GetEntityID(player);
        if (!((1 << playerID) & entity->activePlayers)) {
            if (Player_CheckCollisionTouch(player, entity, &DNARiser->hitbox)) {
                RSDK.PlaySfx(DNARiser->sfxGrab, 0, 255);
                entity->timer2   = 30;
                entity->field_A8 = 30;
                entity->activePlayers |= (1 << playerID);
            }
        }

        if (((1 << playerID) & entity->activePlayers)) {
            bool32 skipFlag = false;
            if (!Player_CheckValidState(player)) {
                entity->activePlayers &= ~(1 << playerID);
                if (entity->activePlayers)
                    skipFlag = true;
                else
                    DNARiser_State_Burst_Helix();
            }
            if (!skipFlag) {
                if (player->jumpPress) {
                    player->velocity.y = 0;
                    player->velocity.x = 0;
                    Player_StartJump(player);
                    DNARiser_State_Burst_Helix();
                }

                if (entity->state == DNARiser_State_Unknown4) {
                    flag = true;
                }
                else {
                    RSDK.SetSpriteAnimation(player->spriteIndex, ANI_JUMP, &player->playerAnimator, false, 0);
                    player->groundVel        = 0;
                    player->drawOrder        = 1;
                    player->jumpAbilityTimer = 1;
                    player->onGround         = false;
                    player->tileCollisions   = false;
                    player->state            = Player_State_None;
                    player->nextAirState     = StateMachine_None;
                    player->nextGroundState  = StateMachine_None;
                    player->velocity.x       = 0;
                    player->velocity.y       = -entity->velocity.y;
                    player->position.x       = entity->position.x;
                    player->position.y       = entity->position.y;
                }
            }
        }
    }

    if (flag) {
        foreach_active(Player, player)
        {
            int32 playerID = RSDK.GetEntityID(player);
            if (((1 << playerID) & entity->activePlayers)) {
                player->drawOrder      = Zone->playerDrawLow;
                player->tileCollisions = true;
                player->state          = Player_State_Air;
                player->velocity.x     = entity->speed.x;
                player->velocity.y     = entity->speed.y;
                entity->activePlayers &= ~(1 << playerID);
            }
        }
    }
}

void DNARiser_State_Unknown4(void)
{
    RSDK_THIS(DNARiser);

    if (entity->timer <= 0) {
        if (!entity->timer) {
            if (!entity->field_AC) {
                EntityDNARiser *child = (EntityDNARiser *)entity->child;
                if (child) {
                    entity->field_AC   = 1;
                    child->timer       = 0;
                    child->velocity.x  = 0;
                    child->state       = DNARiser_State_Unknown8;
                    child->velocity.y  = entity->vector_D8.y >> 1;
                    child->vector_D8.y = entity->vector_D8.y >> 1;
                }
            }
            if (entity->position.y < entity->vector_D0.y) {
                entity->velocity.y += 0x38000;
                entity->position.y += entity->velocity.y;
            }

            int32 posY = entity->vector_D0.y;
            if (entity->position.y < entity->vector_D0.y)
                posY = entity->position.y;

            entity->position.y = posY;
            entity->curHeight  = entity->vector_D0.y - posY;
            if (posY == entity->vector_D0.y) {
                entity->state = DNARiser_State_Unknown5;
                entity->timer = 60;
            }
        }
    }
    else {
        entity->timer--;
    }
}

void DNARiser_State_Unknown5(void)
{
    RSDK_THIS(DNARiser);
    if (!entity->field_E4) {
        if (entity->timer <= 0) {
            if (RSDK.CheckOnScreen(entity, &entity->updateRange)) {
                DNARiser_State_BubbleBurst();
                DNARiser_State_Setup();
                entity->field_A8 = 30;
            }
            else {
                entity->state = DNARiser_State_Setup;
            }
        }
        else {
            entity->timer--;
        }
    }
}

void DNARiser_State_SetupChild(void)
{
    RSDK_THIS(DNARiser);
    entity->active    = ACTIVE_NORMAL;
    entity->stateDraw = DNARiser_StateDraw_Helix;
    entity->state     = DNARiser_State_None;
}

void DNARiser_State_None(void) {}

void DNARiser_State_Unknown8(void)
{
    RSDK_THIS(DNARiser);
    EntityDNARiser *parent = (EntityDNARiser *)entity->parent;

    if (entity->timer <= 0) {
        if (entity->field_AC) {
            if (!RSDK.CheckOnScreen(entity, &entity->updateRange))
                destroyEntity(entity);
        }
        else {
            EntityDNARiser *child = (EntityDNARiser *)entity->field_F0;
            entity->field_AC      = 1;
            if (child) {
                while (true) {
                    if (RSDK.CheckOnScreen(child, &child->updateRange)) {
                        child->velocity.y  = 0;
                        child->timer       = 1;
                        child->state       = DNARiser_State_Unknown8;
                        child->velocity.x  = 0;
                        child->velocity.y  = (entity->vector_D8.y >> 1) + (entity->vector_D8.y >> 2);
                        child->vector_D8.y = (entity->vector_D8.y >> 1) + (entity->vector_D8.y >> 2);
                        break;
                    }
                    else {
                        EntityDNARiser *grandChild = (EntityDNARiser *)child->field_F0;
                        if (child == (EntityDNARiser *)parent->child)
                            parent->child = (Entity *)grandChild;
                        destroyEntity(child);
                        child = grandChild;
                        if (!grandChild)
                            break;
                    }
                }
            }

            if (!child) {
                parent->field_E4 = NULL;
            }

            if (entity->field_AC) {
                if (!RSDK.CheckOnScreen(entity, &entity->updateRange))
                    destroyEntity(entity);
            }
        }
    }
    else {
        entity->timer--;
    }
    entity->velocity.y += 0x3800;
    entity->position.y += entity->velocity.y;
    entity->field_CC += 0x20000;
}

void DNARiser_StateDraw_Main(void)
{
    RSDK_THIS(DNARiser);

    bool32 flag = false;
    int32 angle   = ((0x57262 * (entity->curHeight >> 16)) >> 16) + 100;

    Animator animator;
    memset(&animator, 0, sizeof(animator));
    if ((uint32)(angle & 0x3FF) - 256 <= 0x200)
        flag = true;

    int32 sineOff       = (RSDK.Sin1024(angle & 0x3FF) << 6) * (entity->field_CC >> 16);
    entity->alpha     = 0x100;
    entity->inkEffect = INK_NONE;

    Vector2 drawPos;
    drawPos = entity->position;

    entity->scale.x = 0x200;
    entity->scale.y = 0x200;
    entity->drawFX  = FX_NONE;
    if (RSDK_sceneInfo->currentDrawGroup == Zone->drawOrderHigh) {
        if (flag) {
            drawPos.x = sineOff + entity->position.x;
            drawPos.y = entity->position.y;
            RSDK.SetSpriteAnimation(DNARiser->aniFrames, 4, &animator, true, 0);
        }
        else {
            drawPos.x = entity->position.x - sineOff;
            drawPos.y = entity->position.y;
            RSDK.SetSpriteAnimation(DNARiser->aniFrames, 2, &animator, true, 0);
        }

        RSDK.DrawSprite(&animator, &drawPos, false);
        if (!entity->field_60) {
            drawPos           = entity->position;
            entity->drawFX    = FX_SCALE;
            entity->inkEffect = INK_BLEND;
            entity->alpha     = 128;
            Vector2 vec;
            vec.x         = entity->field_70;
            vec.y         = entity->field_70;
            entity->scale = DNARiser_CalculateScale(&vec);
            RSDK.DrawSprite(&entity->animator2, &drawPos, false);
        }
        if (entity->popped) {
            drawPos           = entity->popPos;
            entity->scale.x   = 0x200;
            entity->scale.y   = 0x200;
            entity->drawFX    = FX_NONE;
            entity->inkEffect = INK_NONE;
            RSDK.DrawSprite(&entity->animator, &drawPos, false);
        }
    }
    else {
        if (flag) {
            drawPos.y = entity->position.y;
            drawPos.x = entity->position.x - sineOff;
            RSDK.SetSpriteAnimation(DNARiser->aniFrames, 2, &animator, true, 0);
        }
        else {
            drawPos.x = sineOff + entity->position.x;
            drawPos.y = entity->position.y;
            RSDK.SetSpriteAnimation(DNARiser->aniFrames, 4, &animator, true, 0);
        }
        RSDK.DrawSprite(&animator, &drawPos, false);
    }
}

void DNARiser_StateDraw_Helix(void)
{
    RSDK_THIS(DNARiser);

    bool32 flag            = false;
    EntityDNARiser *parent = (EntityDNARiser *)entity->parent;

    Animator animator;
    memset(&animator, 0, sizeof(animator));
    if ((entity->angle - 256) <= 0x200)
        flag = true;

    int32 sineOff       = (RSDK.Sin1024(entity->angle) << 6) * (entity->field_CC >> 16);
    entity->alpha     = 0x100;
    entity->inkEffect = INK_NONE;

    Vector2 drawPos;
    drawPos = entity->position;

    entity->scale.x = 0x200;
    entity->scale.y = 0x200;
    entity->drawFX  = FX_NONE;
    entity->drawFX  = INK_NONE;
    if (RSDK_sceneInfo->currentDrawGroup != Zone->drawOrderHigh && !(entity->field_A4 & 1)) {
        if (flag) {
            drawPos.x = entity->position.x - sineOff;
            drawPos.y = entity->position.y;
            RSDK.SetSpriteAnimation(DNARiser->aniFrames, 2, &animator, true, 0);
        }
        else {
            drawPos.x = sineOff + entity->position.x;
            drawPos.y = entity->position.y;
            RSDK.SetSpriteAnimation(DNARiser->aniFrames, 4, &animator, true, 0);
        }
        RSDK.DrawSprite(&animator, &drawPos, false);
    }

    if (!(entity->field_A4 % 5)) {
        int32 distance = (parent->field_E0 - entity->field_A4 >= 8) ? 4 : ((parent->field_E0 - entity->field_A4) / 2);
        int32 sine     = (RSDK.Sin1024(entity->angle) << 6) * ((entity->field_CC >> 16) - 12);
        drawPos.y    = entity->position.y;
        drawPos.x    = sine + entity->position.x;
        if (flag)
            drawPos.x = entity->position.x - sine;
        int32 startX = drawPos.x;
        sine       = 2 * sine / 7;

        bool32 flagArray[8];
        flagArray[0] = distance >= 1;
        flagArray[1] = distance >= 2;
        flagArray[2] = distance >= 3;
        flagArray[3] = distance >= 4;
        flagArray[4] = flagArray[3];
        flagArray[5] = flagArray[2];
        flagArray[6] = flagArray[1];
        flagArray[7] = flagArray[0];

        for (int32 i = 0; i < 8; ++i) {
            bool32 flagA = (flag && i <= 3) || (!flag && i >= 4);
            bool32 flagB = RSDK_sceneInfo->currentDrawGroup == Zone->drawOrderHigh ? ((flag && !flagA) || (!flag && flagA))
                                                                                   : ((flag && flagA) || (!flag && !flagA));

            if (flagArray[i] && flagB) {
                if (!flagA)
                    RSDK.SetSpriteAnimation(DNARiser->aniFrames, 3, &animator, true, 0);
                else
                    RSDK.SetSpriteAnimation(DNARiser->aniFrames, 1, &animator, true, 0);
                drawPos.x = startX + sine * i * (2 * flag - 1);
                RSDK.DrawSprite(&animator, &drawPos, false);
            }
        }
    }

    if (RSDK_sceneInfo->currentDrawGroup == Zone->drawOrderHigh && !(entity->field_A4 & 1)) {
        if (flag) {
            drawPos.x = sineOff + entity->position.x;
            drawPos.y = entity->position.y;
            RSDK.SetSpriteAnimation(DNARiser->aniFrames, 4, &animator, true, 0);
        }
        else {
            drawPos.x = entity->position.x - sineOff;
            drawPos.y = entity->position.y;
            RSDK.SetSpriteAnimation(DNARiser->aniFrames, 2, &animator, true, 0);
        }
        RSDK.DrawSprite(&animator, &drawPos, false);
    }
}

void DNARiser_EditorDraw(void)
{
    RSDK_THIS(DNARiser);

    entity->vector_D0     = entity->position;
    entity->active        = ACTIVE_BOUNDS;
    entity->speed.x       = 0;
    entity->velocity.x    = 0;
    entity->speed.y       = -abs(entity->speed.y);
    entity->timer         = 0;
    entity->curHeight     = 0;
    entity->field_AC      = 0;
    entity->field_E4      = 0;
    entity->child         = 0;
    entity->field_E0      = 0;
    entity->activePlayers = 0;
    entity->field_60      = 0;
    entity->field_B6      = 0;
    entity->timer2        = 0;
    entity->field_B0      = 0;
    entity->velocity.y    = 0;
    entity->field_70      = 0x200;
    RSDK.SetSpriteAnimation(DNARiser->aniFrames, 0, &entity->animator2, true, 0);

    DNARiser_StateDraw_Main();
}

void DNARiser_EditorLoad(void) { DNARiser->aniFrames = RSDK.LoadSpriteAnimation("CPZ/DNARiser.bin", SCOPE_STAGE); }

void DNARiser_Serialize(void)
{
    RSDK_EDITABLE_VAR(DNARiser, VAR_UINT32, height);
    RSDK_EDITABLE_VAR(DNARiser, VAR_VECTOR2, speed);
}
