#include "SonicMania.h"

ObjectLaundroMobile *LaundroMobile;

void LaundroMobile_Update(void)
{
    RSDK_THIS(LaundroMobile);
    StateMachine_Run(self->state);
}

void LaundroMobile_LateUpdate(void) {}

void LaundroMobile_StaticUpdate(void)
{
    if (LaundroMobile->shouldPlayFanSfx) {
        if (!LaundroMobile->playingFanSfx) {
            if (LaundroMobile->health <= 8)
                RSDK.PlaySfx(LaundroMobile->sfxWash, true, 255);
            else
                RSDK.PlaySfx(LaundroMobile->sfxFan, 47208, 255);
            LaundroMobile->playingFanSfx = true;
        }
    }
    else {
        if (LaundroMobile->playingFanSfx) {
            RSDK.StopSFX(LaundroMobile->sfxFan);
            RSDK.StopSFX(LaundroMobile->sfxWash);
            LaundroMobile->playingFanSfx = false;
        }
    }

    LaundroMobile->shouldPlayFanSfx = false;
    if (SceneInfo->state == ENGINESTATE_REGULAR && LaundroMobile->useStageWrap)
        LaundroMobile_HandleStageWrap();
}

void LaundroMobile_Draw(void)
{
    RSDK_THIS(LaundroMobile);
    StateMachine_Run(self->stateDraw);
}

void LaundroMobile_Create(void *data)
{
    RSDK_THIS(LaundroMobile);

    if (globals->gameMode == MODE_TIMEATTACK) {
        destroyEntity(self);
    }
    else {
        self->drawFX = FX_FLIP;
        if (!SceneInfo->inEditor) {
            if (!self->type)
                self->type = voidToInt(data);

            switch (self->type) {
                case LAUNDROMOBILE_BOSS:
                    self->active    = ACTIVE_BOUNDS;
                    self->visible   = false;
                    self->drawOrder = Zone->drawOrderLow + 1;
                    RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 0, &self->animator1, true, 0);
                    RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 3, &self->animator2, true, 0);
                    RSDK.SetSpriteAnimation(LaundroMobile->eggmanFrames, 0, &self->animator3, true, 0);
                    self->startY                    = self->position.y;
                    self->updateRange.x             = 0x800000;
                    self->updateRange.y             = 0x800000;
                    LaundroMobile->health             = 5 + 8;
                    LaundroMobile->invincibilityTimer = 0;
                    LaundroMobile->loopPointCount     = 0;
                    LaundroMobile->attackDir          = FLIP_NONE;
                    LaundroMobile->currentVelocity    = 0x38000;
                    LaundroMobile->rocketActive       = false;
                    LaundroMobile->animSpeed          = 0;
                    LaundroMobile->travelledPaths     = 0;
                    LaundroMobile->useStageWrap       = false;
                    LaundroMobile->shouldPlayFanSfx   = 0;
                    LaundroMobile->playingFanSfx      = false;
                    RSDK.StopSFX(LaundroMobile->sfxFan);
                    RSDK.StopSFX(LaundroMobile->sfxWash);
                    LaundroMobile->rocketAngles[0] = 0;
                    LaundroMobile->rocketAngles[1] = 0x8000;
                    LaundroMobile->rocketAngles[2] = 0;
                    LaundroMobile->rocketAngles[3] = 0x8000;
                    LaundroMobile->rocketSpeeds[0] = 0;
                    LaundroMobile->rocketSpeeds[1] = 0;
                    LaundroMobile->underwaterFlag  = false;
                    self->tileCollisions         = true;
                    self->collisionLayers        = Zone->fgLayers;
                    self->collisionPlane         = 0;
                    self->state                  = LaundroMobile_State_SetupArena;
                    self->stateDraw              = LaundroMobile_StateDraw_Unknown1;
                    break;
                case LAUNDROMOBILE_BOMB:
                    self->active     = ACTIVE_BOUNDS;
                    self->visible    = true;
                    self->drawOrder  = Zone->drawOrderLow;
                    self->startPos.x = self->position.x;
                    self->startPos.y = self->position.y;
                    RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 7, &self->animator1, true, 0);
                    self->updateRange.x = 0x1800000;
                    self->updateRange.y = 0x1800000;
                    self->state         = LaundroMobile_State1_Unknown1;
                    self->stateDraw     = LaundroMobile_StateDraw4_Unknown1;
                    break;
                case LAUNDROMOBILE_PHASE2:
                    self->active    = ACTIVE_XBOUNDS;
                    self->visible   = true;
                    self->drawOrder = Zone->drawOrderLow + 1;
                    RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 0, &self->animator1, true, 0);
                    RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 1, &self->animator2, true, 0);
                    RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 2, &self->animator3, true, 0);
                    self->updateRange.x = 0x2000000;
                    self->updateRange.y = 0x800000;
                    self->state         = LaundroMobile_State2_Unknown1;
                    self->stateDraw     = LaundroMobile_StateDraw2_Unknown1;
                    break;
                case LAUNDROMOBILE_LOOPPOINT: LaundroMobile->loopPoints[LaundroMobile->loopPointCount++] = (Entity *)self; break;
                case LAUNDROMOBILE_BLOCK:
                    self->active     = ACTIVE_BOUNDS;
                    self->visible    = true;
                    self->drawOrder  = Zone->drawOrderLow;
                    self->startPos.x = self->position.x;
                    self->startPos.y = self->position.y;
                    RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 9, &self->animator1, true, RSDK.Rand(0, 3));
                    self->updateRange.x = 0x1800000;
                    self->updateRange.y = 0x1800000;
                    self->velocity.x    = LaundroMobile->currentVelocity - 0x18000;
                    self->state         = LaundroMobile_State3_Unknown1;
                    self->stateDraw     = LaundroMobile_StateDraw4_Unknown1;
                    break;
                case LAUNDROMOBILE_SPIKES:
                    self->active     = ACTIVE_BOUNDS;
                    self->visible    = true;
                    self->drawOrder  = Zone->drawOrderLow;
                    self->startPos.x = self->position.x;
                    self->startPos.y = self->position.y;
                    RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 9, &self->animator1, true, RSDK.Rand(0, 3) + 3);
                    self->updateRange.x = 0x1800000;
                    self->updateRange.y = 0x1800000;
                    self->velocity.x    = LaundroMobile->currentVelocity - 0x18000;
                    self->state         = LaundroMobile_State3_Unknown1;
                    self->stateDraw     = LaundroMobile_StateDraw4_Unknown1;
                    break;
                case LAUNDROMOBILE_6:
                    self->active = ACTIVE_NORMAL;
                    self->timer  = 8;
                    self->state  = LaundroMobile_State6_Unknown1;
                    break;
                default: break;
            }
        }
    }
}

void LaundroMobile_StageLoad(void)
{
    LaundroMobile->aniFrames    = RSDK.LoadSpriteAnimation("HCZ/LaundroMobile.bin", SCOPE_STAGE);
    LaundroMobile->eggmanFrames = RSDK.LoadSpriteAnimation("Eggman/EggmanHCZ2.bin", SCOPE_STAGE);

    LaundroMobile->hitboxBoss.left   = -32;
    LaundroMobile->hitboxBoss.top    = -32;
    LaundroMobile->hitboxBoss.right  = 32;
    LaundroMobile->hitboxBoss.bottom = 32;

    LaundroMobile->hitboxMissile.left   = -24;
    LaundroMobile->hitboxMissile.top    = -24;
    LaundroMobile->hitboxMissile.right  = 24;
    LaundroMobile->hitboxMissile.bottom = 24;

    LaundroMobile->hitboxBox.left   = -16;
    LaundroMobile->hitboxBox.top    = -16;
    LaundroMobile->hitboxBox.right  = 16;
    LaundroMobile->hitboxBox.bottom = 16;

    LaundroMobile->hitboxBomb.left   = -32;
    LaundroMobile->hitboxBomb.top    = -16;
    LaundroMobile->hitboxBomb.right  = 32;
    LaundroMobile->hitboxBomb.bottom = 16;

    LaundroMobile->innerBox.left   = -39;
    LaundroMobile->innerBox.top    = -40;
    LaundroMobile->innerBox.right  = 39;
    LaundroMobile->innerBox.bottom = 40;

    LaundroMobile->outerBox.left   = -40;
    LaundroMobile->outerBox.top    = -40;
    LaundroMobile->outerBox.right  = 40;
    LaundroMobile->outerBox.bottom = 40;

    LaundroMobile->active           = ACTIVE_ALWAYS;
    LaundroMobile->playingFanSfx    = false;
    LaundroMobile->shouldPlayFanSfx = 0;
    LaundroMobile->sfxHit           = RSDK.GetSFX("Stage/BossHit.wav");
    LaundroMobile->sfxExplosion     = RSDK.GetSFX("Stage/Explosion2.wav");
    LaundroMobile->sfxButton2       = RSDK.GetSFX("Stage/Button2.wav");
    LaundroMobile->sfxFan           = RSDK.GetSFX("HCZ/BigFan.wav");
    LaundroMobile->sfxRoll          = RSDK.GetSFX("Global/Roll.wav");
    LaundroMobile->sfxWash          = RSDK.GetSFX("HCZ/Wash.wav");
    LaundroMobile->sfxHullClose     = RSDK.GetSFX("Stage/HullClose.wav");
    LaundroMobile->sfxPush          = RSDK.GetSFX("Stage/Push.wav");
    LaundroMobile->sfxFireball      = RSDK.GetSFX("Stage/Fireball.wav");
    LaundroMobile->sfxButton        = RSDK.GetSFX("Stage/Button.wav");
    LaundroMobile->sfxLedgeBreak    = RSDK.GetSFX("Stage/LedgeBreak3.wav");
    LaundroMobile->sfxPimPom        = RSDK.GetSFX("Stage/PimPom.wav");
#if RETRO_USE_PLUS
    LaundroMobile->sfxRumble = RSDK.GetSFX("Stage/Rumble.wav");
    LaundroMobile->sfxImpact = RSDK.GetSFX("Stage/Impact6.wav");
#endif
}

void LaundroMobile_CheckPlayerCollisions(void)
{
    RSDK_THIS(LaundroMobile);

    if (LaundroMobile->invincibilityTimer) {
        LaundroMobile->invincibilityTimer--;
    }
    else {
        int storeX = self->position.x;
        int storeY = self->position.y;
        foreach_active(Player, player)
        {
            for (int i = 0; i < 4 && LaundroMobile->rocketActive; ++i) {
                self->position.x = LaundroMobile->rocketPositions[i].x;
                self->position.y = LaundroMobile->rocketPositions[i].y;
                if (Player_CheckCollisionTouch(player, self, &LaundroMobile->hitboxMissile)) {
#if RETRO_USE_PLUS
                    if (!Player_CheckMightyUnspin(0x400, player, 2, &player->uncurlTimer))
#endif
                        Player_CheckHit(player, self);
                    break;
                }
            }
            self->position.x = storeX;
            self->position.y = storeY;

            int velX = player->velocity.x;
            int velY = player->velocity.y;
            if (Player_CheckBadnikTouch(player, self, &LaundroMobile->hitboxBoss) && Player_CheckBossHit(player, self)) {
                if (LaundroMobile->health) {
                    LaundroMobile->health--;
                }

                if (LaundroMobile->health) {
                    if (LaundroMobile->health == 8) {
                        RSDK.StopSFX(LaundroMobile->sfxFan);
                        RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 4, &self->animator2, true, 0);
                        self->timer = 60;
                        self->state = LaundroMobile_State_Destroyed_Phase1;
                    }
                    else {
                        if (LaundroMobile->health < 8) {
                            RSDK.SetSpriteAnimation(LaundroMobile->eggmanFrames, 3, &self->animator3, true, 0);
                        }
                        else {
                            RSDK.SetSpriteAnimation(LaundroMobile->eggmanFrames, 2, &self->animator3, true, 0);
                            player->groundVel  = velX;
                            player->velocity.x = velX;
                            player->velocity.y = velY;
                        }
                        RSDK.PlaySfx(LaundroMobile->sfxHit, false, 255);
                        LaundroMobile->invincibilityTimer = 30;
                    }
                }
                else {
                    SceneInfo->timeEnabled = false;
                    Player_GiveScore(RSDK_GET_ENTITY(SLOT_PLAYER1, Player), 1000);
                    RSDK.PlaySfx(LaundroMobile->sfxExplosion, false, 255);
                    LaundroMobile->invincibilityTimer = 60;

                    EntityWhirlpool *whirlpool = (EntityWhirlpool *)self->whirlpool;
                    self->state              = LaundroMobile_State_Destroyed_Phase2;
                    if (whirlpool) {
                        if (whirlpool->objectID == Whirlpool->objectID)
                            whirlpool->activePlayers = -3;
                        self->whirlpool = NULL;
                    }
                }
                foreach_break;
            }
        }
    }
}

void LaundroMobile_Explode(void)
{
    RSDK_THIS(LaundroMobile);

    int interval = LaundroMobile->health > 8 ? 7 : 3;

    if (!(Zone->timer % interval)) {
        RSDK.PlaySfx(LaundroMobile->sfxExplosion, false, 255);
        if (Zone->timer & 4) {
            int x                      = self->position.x + (RSDK.Rand(-19, 20) << 16);
            int y                      = self->position.y + (RSDK.Rand(-24, 25) << 16);
            EntityExplosion *explosion = CREATE_ENTITY(Explosion, intToVoid((RSDK.Rand(0, 256) > 192) + EXPLOSION_BOSS), x, y);
            explosion->drawOrder       = Zone->drawOrderHigh + 2;
            if (LaundroMobile->health > 8)
                explosion->velocity.x = 0x24000;
        }
    }
}

void LaundroMobile_HandleStageWrap(void)
{
    EntityLaundroMobile *entity = (EntityLaundroMobile *)LaundroMobile->bossPtr;

    if (!(Zone->timer & 3)) {
        EntityCurrent *current = CREATE_ENTITY(Current, intToVoid(2), ScreenInfo->position.x << 16,
                                               (8 * RSDK.Rand(0, ScreenInfo->height >> 3) + ScreenInfo->position.y) << 16);
        current->drawOrder     = Zone->playerDrawLow;
        current->strength      = 6;
        current->type          = 1;
        current->alpha         = 240;
        current->size.x        = (ScreenInfo->position.x + ScreenInfo->width + 0x1000) << 16;
    }

    foreach_active(Player, player)
    {
        if (player->state != Player_State_None) {
            if (player->position.x < entity->position.x)
                player->position.x = entity->position.x;
            if (Player_CheckValidState(player)) {
                if ((abs(player->velocity.y) <= 0x18000 || player->onGround) && player->position.x < 0x6D800000) {
                    player->collisionMode   = 0;
                    player->onGround        = false;
                    player->nextGroundState = StateMachine_None;
                    player->nextAirState    = StateMachine_None;
                    player->state           = Current_Player_State_Type1;
                    if (player->animator.animationID != ANI_CLING && player->animator.animationID != ANI_SHAFTSWING) {
                        if (player->position.x >= entity->position.x + 0xC00000) {
                            player->velocity.x = LaundroMobile->currentVelocity;
                            player->groundVel  = player->velocity.x;
                        }
                        else {
                            player->velocity.x = LaundroMobile->currentVelocity + ((entity->position.x - player->position.x + 0xC00000) >> 6);
                            player->groundVel  = player->velocity.x;
                        }
                        RSDK.SetSpriteAnimation(player->aniFrames, ANI_FAN, &player->animator, false, 0);
                    }
                    if (player->up)
                        player->velocity.y = -0x18000;
                    else
                        player->velocity.y = player->down ? 0x18000 : 0;
                }
            }
        }
    }

    if (LaundroMobile->currentVelocity < 0x80000)
        LaundroMobile->currentVelocity += 0x400;
    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);

    if (Player_CheckValidState(player1)) {
        if (LaundroMobile->loopPointCount == 5 && entity->timer <= 0) {
            if (LaundroMobile->currentVelocity < 0xC0000) {
                LaundroMobile->currentVelocity += 0x1000;
            }
        }
        else {
            Entity *loopPoint = LaundroMobile->loopPoints[LaundroMobile->loopPointCount];
            if (player1->position.x >= loopPoint->position.x + 0xE000000) {
                int startOffsetX = player1->position.x - loopPoint->position.x - 0xE000000;
                int startOffsetY = player1->position.y - loopPoint->position.y;
                if (LaundroMobile->health <= 8) {
                    LaundroMobile->loopPointCount = 5;
                    entity->state                 = LaundroMobile_State_Unknown5;
                }
                else {
                    if (LaundroMobile->travelledPaths == 0x1E) {
                        LaundroMobile->travelledPaths = 0;
                    }

                    int originalCount = LaundroMobile->loopPointCount;
                    while (LaundroMobile->loopPointCount == originalCount) {
                        do {
                            LaundroMobile->loopPointCount = RSDK.Rand(1, 5);
                        } while ((1 << LaundroMobile->loopPointCount) & LaundroMobile->travelledPaths);
                    }
                    LaundroMobile->travelledPaths |= 1 << LaundroMobile->loopPointCount;
                }
                loopPoint = LaundroMobile->loopPoints[LaundroMobile->loopPointCount];

                int offsetX = player1->position.x - startOffsetX - loopPoint->position.x;
                int offsetY = player1->position.y - loopPoint->position.y - startOffsetY;
                player1->position.x -= offsetX;
                player1->position.y -= offsetY;
                entity->position.y -= offsetY;
                entity->startPos.y -= offsetY;
                entity->unknownPos.y -= offsetY;
                entity->position.x -= offsetX;
                entity->startPos.x -= offsetX;
                entity->unknownPos.x -= offsetX;

                ScreenInfo->position.x -= offsetX >> 16;
                ScreenInfo->position.y -= offsetY >> 16;

                EntityCamera *camera = RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);
                camera->position.x -= offsetX;
                camera->position.y -= offsetY;
                camera->center.x -= offsetX >> 16;
                camera->center.y -= offsetY >> 16;
                if (Player->playerCount >= 2) {
                    EntityPlayer *player2 = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
                    player2->position.x -= offsetX;
                    player2->position.y -= offsetY;
                }

                for (int i = 0; i < 0x1000; ++i) {
                    Entity *entPtr = RSDK.GetEntityByID(i);

                    if (entPtr->objectID == LaundroMobile->objectID) {
                        EntityLaundroMobile *laundroMobile = (EntityLaundroMobile *)entPtr;
                        if (laundroMobile->type == LAUNDROMOBILE_BOMB && laundroMobile->state != LaundroMobile_State1_Unknown1
                            || laundroMobile->state == LaundroMobile_State3_Unknown2) {
                            laundroMobile->position.x -= offsetX;
                            laundroMobile->position.y -= offsetY;
                        }
                    }
                    else if (entPtr->objectID == Ring->objectID) {
                        EntityRing *ring = (EntityRing *)entPtr;
                        if (ring->state != Ring_State_Normal) {
                            entPtr->position.x -= offsetX;
                            entPtr->position.y -= offsetY;
                        }
                    }
                    else if (entPtr->objectID == Debris->objectID) {
                        entPtr->position.x -= offsetX;
                        entPtr->position.y -= offsetY;
                    }
                    else if (entPtr->objectID == Water->objectID) {
                        EntityWater *water = (EntityWater *)entPtr;
                        if (water->type == WATER_BUBBLE) {
                            water->position.x -= offsetX;
                            water->position.y -= offsetY;
                            water->bubbleX = water->position.x;
                        }
                    }
                    else if (entPtr->objectID == ImageTrail->objectID) {
                        EntityImageTrail *trail = (EntityImageTrail *)entPtr;
                        trail->position.x -= offsetX;
                        trail->position.y -= offsetY;
                        trail->currentPos.x -= offsetX;
                        trail->currentPos.y -= offsetY;
                        for (int i = 0; i < ImageTrail_TrackCount; ++i) {
                            trail->statePos[i].x -= offsetX;
                            trail->statePos[i].y -= offsetY;
                        }
                    }
                    else if (entPtr->objectID == Current->objectID) {
                        EntityCurrent *current = (EntityCurrent *)entPtr;
                        if (current->state == Current_State_LaundoMobile) {
                            current->position.x -= offsetX;
                            current->position.y -= offsetY;
                            current->size.x -= offsetX;
                        }
                    }
                }
            }
        }
    }
}

void LaundroMobile_HandleMissileMovement(void)
{
    RSDK_THIS(LaundroMobile);

    for (int i = 0; i < 2; ++i) {
        LaundroMobile->rocketPositions[i].x = self->position.x + 0x1400 * RSDK.Cos256(LaundroMobile->rocketAngles[i] >> 8);
        LaundroMobile->rocketPositions[i].y = self->position.y + 0x1400 * RSDK.Cos256(LaundroMobile->rocketAngles[i] >> 8);
        LaundroMobile->rocketAngles[i] += LaundroMobile->rocketSpeeds[0];
    }

    for (int i = 2; i < 4; ++i) {
        LaundroMobile->rocketPositions[i].x = self->position.x - 0x1400 * RSDK.Cos256(LaundroMobile->rocketAngles[i] >> 8);
        LaundroMobile->rocketPositions[i].y = self->position.y + 0x1400 * RSDK.Cos256(LaundroMobile->rocketAngles[i] >> 8);
        LaundroMobile->rocketAngles[i] += LaundroMobile->rocketSpeeds[1];
    }
}

void LaundroMobile_HandleAnimations(void)
{
    RSDK_THIS(LaundroMobile);

    RSDK.ProcessAnimation(&self->animator3);
    bool32 flag = false;

    switch (self->animator3.animationID) {
        case 0: {
            foreach_active(Player, player)
            {
                if (player->state == Player_State_Hit || player->state == Player_State_Die || player->state == Player_State_Drown)
                    flag = true;
            }
            if (flag)
                RSDK.SetSpriteAnimation(LaundroMobile->eggmanFrames, 4, &self->animator3, true, 0);
            break;
        }
        case 1: {
            foreach_active(Player, player)
            {
                if (player->state == Player_State_Hit || player->state == Player_State_Die || player->state == Player_State_Drown)
                    flag = true;
            }
            if (flag)
                RSDK.SetSpriteAnimation(LaundroMobile->eggmanFrames, 5, &self->animator3, true, 0);
            break;
        }
        case 2:
            if (!LaundroMobile->invincibilityTimer)
                RSDK.SetSpriteAnimation(LaundroMobile->eggmanFrames, 0, &self->animator3, true, 0);
            break;
        case 3:
            if (!LaundroMobile->invincibilityTimer)
                RSDK.SetSpriteAnimation(LaundroMobile->eggmanFrames, 1, &self->animator3, true, 0);
            break;
        case 4: {
            foreach_active(Player, player)
            {
                if (player->state == Player_State_Hit || player->state == Player_State_Die || player->state == Player_State_Drown)
                    flag = true;
            }

            if (self->animator3.frameID >= self->animator3.frameCount - 1) {
                if (flag)
                    RSDK.SetSpriteAnimation(LaundroMobile->eggmanFrames, 4, &self->animator3, true, 6);
                else
                    RSDK.SetSpriteAnimation(LaundroMobile->eggmanFrames, 0, &self->animator3, true, 0);
            }
            break;
        }
        case 5: {
            foreach_active(Player, player)
            {
                if (player->state == Player_State_Hit || player->state == Player_State_Die || player->state == Player_State_Drown)
                    flag = true;
            }

            if (self->animator3.frameID >= self->animator3.frameCount - 1) {
                if (flag)
                    RSDK.SetSpriteAnimation(LaundroMobile->eggmanFrames, 5, &self->animator3, true, 6);
                else
                    RSDK.SetSpriteAnimation(LaundroMobile->eggmanFrames, 1, &self->animator3, true, 0);
            }
            break;
        }
        default: break;
    }
}

void LaundroMobile_HandleTileCollisions(void)
{
    RSDK_THIS(LaundroMobile);

    uint8 flag = 0xFF;
    while (RSDK.ObjectTileCollision(self, Zone->fgLayers, CMODE_FLOOR, 0, 0x200000, 0x200000, false) && flag >= 0) {
        self->position.y -= 0x10000;
        flag = 0;
    }

    while (RSDK.ObjectTileCollision(self, Zone->fgLayers, CMODE_ROOF, 0, 0x200000, -0x200000, false) && flag >= 1) {
        self->position.y += 0x10000;
        flag = 1;
    }

    while (RSDK.ObjectTileCollision(self, Zone->fgLayers, CMODE_FLOOR, 0, -0x200000, 0x200000, false) && flag >= 2) {
        self->position.y -= 0x10000;
        flag = 2;
    }

    while (RSDK.ObjectTileCollision(self, Zone->fgLayers, CMODE_ROOF, 0, -0x200000, -0x200000, false) && flag >= 3) {
        self->position.y += 0x10000;
        flag = 3;
    }
}

void LaundroMobile_State_SetupArena(void)
{
    RSDK_THIS(LaundroMobile);

    if (++self->timer >= 2) {
        self->position.y += 0x500000;
        self->startPos.y = self->position.y;
        self->position.x -= 0x100000;
        self->timer  = 0;
        self->active = ACTIVE_NORMAL;
        RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
        if (self->position.x >= 0x60800000) {
            // did this use to only have 12 health at some point during dev?
            if (LaundroMobile->health == RETRO_USE_PLUS ? (5 + 8) : (4 + 8)) {
                LaundroMobile->health = 8;
                Music_TransitionTrack(TRACK_EGGMAN1, 0.0125);
                self->visible = true;
                self->state   = LaundroMobile_State_Unknown6;
            }
            else {
                destroyEntity(self);
            }
        }
        else {
            LaundroMobile->bossPtr                                                  = (Entity *)self;
            self->state                                                           = LaundroMobile_State_SetupArena2;
            RSDK_GET_ENTITY(SceneInfo->entitySlot + 1, BreakBar)->releaseTimer = 0;
            EntityPlayer *player1                                                   = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
            player1->jumpPress                                                      = false;
        }
    }
}

void LaundroMobile_State_SetupArena2(void)
{
    RSDK_THIS(LaundroMobile);
    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);

    RSDK_GET_ENTITY(SceneInfo->entitySlot + 1, BreakBar)->releaseTimer = 0;
    player1->jumpPress                                                      = false;
    if (player1->position.x > self->position.x + 0x1700000) {
        Music_TransitionTrack(TRACK_EGGMAN1, 0.0125);
        RSDK.GetSceneLayer(4)->drawLayer[0] = DRAWLAYER_COUNT;
        LaundroMobile->loopPointCount       = 0;
        Water->waterLevel                   = 0;
        Water->targetWaterLevel             = 0;
        self->timer                       = 120;
        self->animator2.animationSpeed    = 0;
        self->visible                     = true;
        self->state                       = LaundroMobile_State_Unknown1;
    }
}

void LaundroMobile_State_Unknown1(void)
{
    RSDK_THIS(LaundroMobile);

    RSDK.ProcessAnimation(&self->animator2);

    self->position.y = BadnikHelpers_Oscillate(self->startPos.y, 2, 10);
    self->position.x += 0x18000;
    RSDK_GET_ENTITY(SceneInfo->entitySlot + 1, BreakBar)->releaseTimer = 0;
    RSDK_GET_ENTITY(SLOT_PLAYER1, Player)->jumpPress                        = false;
    if (!--self->timer) {
        RSDK.PlaySfx(LaundroMobile->sfxButton2, false, 255);
        self->state               = LaundroMobile_State_Unknown2;
        LaundroMobile->useStageWrap = true;
    }
    LaundroMobile_CheckPlayerCollisions();
}

void LaundroMobile_State_Unknown2(void)
{
    RSDK_THIS(LaundroMobile);

    RSDK_GET_ENTITY(SceneInfo->entitySlot + 1, BreakBar)->releaseTimer = 0;
    RSDK_GET_ENTITY(SLOT_PLAYER1, Player)->jumpPress                        = false;
    RSDK.ProcessAnimation(&self->animator2);

    if (self->animator2.animationSpeed >= 0x200) {
        ++LaundroMobile->shouldPlayFanSfx;
        RSDK_GET_ENTITY(SceneInfo->entitySlot + 1, BreakBar)->releaseTimer = 240;
        self->state                                                           = LaundroMobile_State_Unknown3;
    }
    else {
        self->animator2.animationSpeed += 4;
        if (self->animator2.animationSpeed >= 0x20)
            ++LaundroMobile->shouldPlayFanSfx;
    }
    self->position.y = BadnikHelpers_Oscillate(self->startPos.y, 2, 10);
    LaundroMobile_CheckPlayerCollisions();
}

void LaundroMobile_State_Unknown3(void)
{
    RSDK_THIS(LaundroMobile);

    LaundroMobile_HandleAnimations();
    RSDK.ProcessAnimation(&self->animator2);
    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);

    self->position.y = self->startPos.y;

    if (abs(self->startPos.y - player1->position.y) >= 0x100000) {
        if (player1->position.y >= self->startPos.y) {
            if (self->velocity.y < 0x18000)
                self->velocity.y += 0x1000;
        }
        else {
            if (self->velocity.y > -0x18000)
                self->velocity.y -= 0x1000;
        }
    }
    else if (self->velocity.y) {
        if (self->velocity.y <= 0) {
            self->velocity.y += 0x1000;
            if (self->velocity.y > 0)
                self->velocity.y = 0;
        }
        else {
            self->velocity.y -= 0x1000;
            if (self->velocity.y < 0)
                self->velocity.y = 0;
        }
    }

    self->position.y += self->velocity.y;
    LaundroMobile_HandleTileCollisions();

    if (self->position.y == self->startPos.y) {
        self->position.y = BadnikHelpers_Oscillate(self->startPos.y, 2, 10);
    }
    else {
        self->startPos.y = self->position.y;
    }
    self->position.x += LaundroMobile->currentVelocity;
    ++LaundroMobile->shouldPlayFanSfx;
    LaundroMobile_CheckPlayerCollisions();
}

void LaundroMobile_State_Destroyed_Phase1(void)
{
    RSDK_THIS(LaundroMobile);

    LaundroMobile_Explode();
    RSDK.ProcessAnimation(&self->animator2);
    self->position.y = self->startPos.y;
    LaundroMobile_HandleTileCollisions();

    if (self->position.y == self->startPos.y) {
        self->position.y = BadnikHelpers_Oscillate(self->startPos.y, 2, 10);
    }
    else {
        self->startPos.y = self->position.y;
    }

    self->position.x += LaundroMobile->currentVelocity - 0x8000;

    LaundroMobile_CheckPlayerCollisions();
    if (--self->timer <= 0) {
        EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_FallAndFlicker, self->position.x, self->position.y);
        RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 4, &debris->animator, true, 0);
        debris->velocity.x    = 0x50000;
        debris->velocity.y    = -0x28000;
        debris->gravity       = 0x3800;
        debris->drawOrder     = Zone->drawOrderHigh;
        debris->updateRange.x = 0x400000;
        debris->updateRange.y = 0x400000;
        RSDK.SetSpriteAnimation(0xFFFF, 0, &self->animator2, true, 0);
        self->state = LaundroMobile_State_Unknown4;
    }
}

void LaundroMobile_State_Unknown4(void)
{
    RSDK_THIS(LaundroMobile);

    LaundroMobile_Explode();
    RSDK.ProcessAnimation(&self->animator2);
    self->position.y = self->startPos.y;
    LaundroMobile_HandleTileCollisions();
    if (self->position.y == self->startPos.y) {
        self->position.y = BadnikHelpers_Oscillate(self->startPos.y, 2, 10);
    }
    else {
        self->startPos.y = self->position.y;
    }
    self->position.x += LaundroMobile->currentVelocity - 0x10000;
    LaundroMobile_CheckPlayerCollisions();
}

void LaundroMobile_State_Unknown5(void)
{
    RSDK_THIS(LaundroMobile);

    if (RSDK_GET_ENTITY(SLOT_PLAYER1, Player)->position.x > 0x6D800000) {
        LaundroMobile->useStageWrap = false;
        foreach_active(Player, player) { player->state = Player_State_Air; }
        self->state = LaundroMobile_State_Unknown6;
    }
    LaundroMobile_CheckPlayerCollisions();
}

void LaundroMobile_State_Unknown6(void)
{
    RSDK_THIS(LaundroMobile);

    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    if (player1->onGround && !player1->angle) {
        self->timer = 60;
        RSDK.SetSpriteAnimation(LaundroMobile->eggmanFrames, 1, &self->animator3, true, 0);
        RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 5, &self->animator2, true, 0);
        self->state = LaundroMobile_State_Unknown7;
    }
    LaundroMobile_CheckPlayerCollisions();
}

void LaundroMobile_State_Unknown7(void)
{
    RSDK_THIS(LaundroMobile);

    if (--self->timer <= 0) {
        self->position.x = 0x6E000000;
        self->timer      = 104;
        self->position.y = (ScreenInfo->position.y - 64) << 16;
        LaundroMobile_HandleMissileMovement();
        self->stateDraw = LaundroMobile_StateDraw_Unknown2;
        self->state     = LaundroMobile_State_Unknown8;
    }
    LaundroMobile_CheckPlayerCollisions();
}

void LaundroMobile_State_Unknown8(void)
{
    RSDK_THIS(LaundroMobile);

    self->position.y += 0x10000;
    LaundroMobile_HandleMissileMovement();
    if (--self->timer <= 0) {
        self->timer = 32;
        self->state = LaundroMobile_State_Unknown9;
    }
    LaundroMobile_CheckPlayerCollisions();
    LaundroMobile_HandleAnimations();
}

void LaundroMobile_State_Unknown9(void)
{
    RSDK_THIS(LaundroMobile);

    LaundroMobile_HandleMissileMovement();

    if (LaundroMobile->rocketSpeeds[0] < 0x200)
        LaundroMobile->rocketSpeeds[0] += 8;

    if (self->timer <= 0) {
        if (LaundroMobile->rocketSpeeds[1] < 0x200)
            LaundroMobile->rocketSpeeds[1] += 8;
    }
    else {
        self->timer--;
    }

    if (LaundroMobile->rocketSpeeds[0] + LaundroMobile->rocketSpeeds[1] == 0x400) {
        self->timer = 96;
        RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 6, &self->animator4, true, 0);
        LaundroMobile->rocketActive = true;
        RSDK.PlaySfx(LaundroMobile->sfxPush, false, 255);
        self->state = LaundroMobile_State_Unknown11;
    }
    LaundroMobile_CheckPlayerCollisions();
    LaundroMobile_HandleAnimations();
}

void LaundroMobile_State_Unknown11(void)
{
    RSDK_THIS(LaundroMobile);

    LaundroMobile_HandleMissileMovement();

    if (LaundroMobile->rocketSpeeds[0] < 0x400) {
        LaundroMobile->rocketSpeeds[1] += 8;
        LaundroMobile->rocketSpeeds[0] = LaundroMobile->rocketSpeeds[1];
    }
    if (--self->timer <= 0) {
        self->timer = 60;
        RSDK.PlaySfx(LaundroMobile->sfxFireball, false, 255);
        self->state = LaundroMobile_State_Unknown12;
    }
    LaundroMobile_CheckPlayerCollisions();
    LaundroMobile_HandleAnimations();
}

void LaundroMobile_State_Unknown12(void)
{
    RSDK_THIS(LaundroMobile);

    self->position.y += self->velocity.y;
    self->velocity.y -= 0x3800;
    LaundroMobile_HandleMissileMovement();
    if (--self->timer <= 0) {
        if (LaundroMobile->attackDir)
            self->position.x += 0x580000;
        else
            self->position.x -= 0x580000;
        self->velocity.y = 0x50000;
        RSDK.PlaySfx(LaundroMobile->sfxRoll, false, 255);
        LaundroMobile->attackDelay = LaundroMobile->attackDelays[RSDK.Rand(0, 8)];
        self->state              = LaundroMobile_State_Unknown13;
    }
    LaundroMobile_CheckPlayerCollisions();
    LaundroMobile_HandleAnimations();
}

void LaundroMobile_State_Unknown13(void)
{
    RSDK_THIS(LaundroMobile);

    RSDK.ProcessTileCollisions(self, &LaundroMobile->outerBox, &LaundroMobile->innerBox);

    if (self->velocity.y < 0 && self->position.y < (ScreenInfo->position.y - 256) << 16) {
        self->onGround   = false;
        self->velocity.y = 0x50000;
        LaundroMobile->attackDir ^= FLIP_X;
        RSDK.PlaySfx(LaundroMobile->sfxRoll, false, 255);
    }

    if (self->position.y <= Water->waterLevel) {
        if (LaundroMobile->underwaterFlag) {
            LaundroMobile->underwaterFlag = false;
            CREATE_ENTITY(Water, intToVoid(WATER_SPLASH), self->position.x, Water->waterLevel);
            CREATE_ENTITY(LaundroMobile, intToVoid(LAUNDROMOBILE_6), self->position.x - 0x100000, Water->waterLevel);
            CREATE_ENTITY(LaundroMobile, intToVoid(LAUNDROMOBILE_6), self->position.x + 0x100000, Water->waterLevel);
            RSDK.PlaySfx(Water->sfxSplash, false, 255);
            if (--LaundroMobile->attackDelay <= 0) {
                self->onGround = false;
                self->state    = LaundroMobile_State_Unknown14;
                if (LaundroMobile->attackDir == FLIP_NONE)
                    self->velocity.x = -0x18000;
                else
                    self->velocity.x = 0x18000;
            }
        }
    }
    else if (!LaundroMobile->underwaterFlag) {
        LaundroMobile->underwaterFlag = true;
        CREATE_ENTITY(Water, intToVoid(WATER_SPLASH), self->position.x, Water->waterLevel);
        CREATE_ENTITY(LaundroMobile, intToVoid(LAUNDROMOBILE_6), self->position.x - 0x100000, Water->waterLevel);
        CREATE_ENTITY(LaundroMobile, intToVoid(LAUNDROMOBILE_6), self->position.x + 0x100000, Water->waterLevel);
        RSDK.PlaySfx(Water->sfxSplash, false, 255);
    }
    LaundroMobile_HandleMissileMovement();
    LaundroMobile_CheckPlayerCollisions();
    LaundroMobile_HandleAnimations();
}

void LaundroMobile_State_Unknown14(void)
{
    RSDK_THIS(LaundroMobile);

    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;
    self->velocity.y += 0x2800;
    if ((LaundroMobile->attackDir && self->position.x >= 0x6E000000) || (!LaundroMobile->attackDir && self->position.x <= 0x6E000000)) {
        self->position.x = 0x6E000000;
        RSDK.SetSpriteAnimation(0xFFFF, 0, &self->animator4, true, 0);
        LaundroMobile->rocketActive = false;
        RSDK.PlaySfx(LaundroMobile->sfxHullClose, false, 255);
        self->state = LaundroMobile_State_Unknown15;
    }

    LaundroMobile_HandleMissileMovement();
    LaundroMobile_CheckPlayerCollisions();
    LaundroMobile_HandleAnimations();
}

void LaundroMobile_State_Unknown15(void)
{
    RSDK_THIS(LaundroMobile);

    if (LaundroMobile->rocketSpeeds[0] <= 0x180) {
        if (!(LaundroMobile->rocketAngles[0] & 0x7E00)) {
            LaundroMobile->rocketSpeeds[0] = 0;
            LaundroMobile->rocketAngles[0] &= 0xFE00;
            LaundroMobile->rocketAngles[1] &= 0xFE00;
        }
    }
    else {
        LaundroMobile->rocketSpeeds[0] -= 8;
    }

    if (LaundroMobile->rocketSpeeds[1] <= 0x180) {
        if (!(LaundroMobile->rocketAngles[2] & 0x7E00)) {
            LaundroMobile->rocketSpeeds[1] = 0;
            LaundroMobile->rocketAngles[2] &= 0xFE00;
            LaundroMobile->rocketAngles[3] &= 0xFE00;
        }
    }
    else {
        LaundroMobile->rocketSpeeds[1] -= 8;
    }

    if (!(LaundroMobile->rocketSpeeds[0] + LaundroMobile->rocketSpeeds[1])) {
        RSDK.PlaySfx(LaundroMobile->sfxFan, false, 255);
        self->state = LaundroMobile_State_Unknown16;
    }
    LaundroMobile_HandleMissileMovement();
    LaundroMobile_CheckPlayerCollisions();
    LaundroMobile_HandleAnimations();
}

void LaundroMobile_State_Unknown16(void)
{
    RSDK_THIS(LaundroMobile);

    LaundroMobile->animSpeed += 4;
    if (LaundroMobile->animSpeed < 256) {
        LaundroMobile_CheckPlayerCollisions();
        LaundroMobile_HandleAnimations();
    }
    else {
        EntityWhirlpool *whirlpool = (EntityWhirlpool *)self->whirlpool;
        if (whirlpool) {
            if (whirlpool->objectID == Whirlpool->objectID) {
                whirlpool->position.x = self->position.x;
            }
            else {
                self->whirlpool = 0;
            }
            self->timer = 480;
            self->state = LaundroMobile_State_Unknown17;
        }
        else {
            Vector2 pos;
            pos.x                    = 0x1000000;
            pos.y                    = 0x1000000;
            whirlpool                = CREATE_ENTITY(Whirlpool, &pos, self->position.x, self->startPos.y - 0x200000);
            whirlpool->activePlayers = -1;
            whirlpool->angVel        = 10;
            whirlpool->alpha         = 0;
            whirlpool->drawOrder     = Zone->drawOrderLow + 1;
            whirlpool->isPermanent   = true;
            self->whirlpool        = (Entity *)whirlpool;
            self->timer            = 480;
            self->state            = LaundroMobile_State_Unknown17;
        }
    }
}

void LaundroMobile_State_Unknown17(void)
{
    RSDK_THIS(LaundroMobile);

    ++LaundroMobile->shouldPlayFanSfx;
    --self->timer;
    if (self->timer > 0) {
        foreach_active(Player, player)
        {
            int playerID = RSDK.GetEntityID(player);
            if (player->position.y > Water->waterLevel) {
                if (player->state == Player_State_None) {
                    player->onGround        = false;
                    player->nextGroundState = StateMachine_None;
                    player->nextAirState    = StateMachine_None;

                    if ((player->position.y & 0xFFFF0000) != (self->position.y & 0xFFFF0000) + 0x580000) {
                        if (player->position.y >= self->position.y + 0x580000)
                            player->position.y -= 0x10000;
                        else
                            player->position.y += 0x10000;
                    }

                    if (LaundroMobile->playerRadius[playerID] != 0x5000) {
                        if (LaundroMobile->playerRadius[playerID] >= 0x5000)
                            LaundroMobile->playerRadius[playerID] -= 0x100;
                        else
                            LaundroMobile->playerRadius[playerID] += 0x100;
                    }
                    if ((LaundroMobile->playerAngles[playerID] & 0xFF) >= 0x80)
                        player->drawOrder = Zone->playerDrawLow;
                    else
                        player->drawOrder = self->drawOrder - 1;
                    LaundroMobile->playerAngles[playerID] += 3;
                    player->position.x =
                        LaundroMobile->playerRadius[playerID] * RSDK.Cos256(LaundroMobile->playerAngles[playerID]) + self->position.x;
                }
                else if (Player_CheckValidState(player)) {
                    player->onGround = false;
                    ;
                    player->velocity.x                    = 0;
                    player->velocity.y                    = 0;
                    player->groundVel                     = 0;
                    player->nextGroundState               = StateMachine_None;
                    player->nextAirState                  = StateMachine_None;
                    LaundroMobile->playerRadius[playerID] = ((player->position.x - self->position.x) & 0xFFFF0000);

                    if (LaundroMobile->playerRadius[playerID] <= 0) {
                        LaundroMobile->playerRadius[playerID] = -LaundroMobile->playerRadius[playerID];
                        LaundroMobile->playerAngles[playerID] = 128;
                    }
                    else {
                        LaundroMobile->playerAngles[playerID] = 0;
                    }
                    LaundroMobile->playerRadius[playerID] >>= 8;
                    RSDK.SetSpriteAnimation(player->aniFrames, ANI_FAN, &player->animator, false, 0);
                    player->state = Player_State_None;
                }
            }
        }
    }
    else {
        foreach_active(Player, player)
        {
            int playerID = RSDK.GetEntityID(player);
            if (player->state == Player_State_None) {
                player->velocity.x = player->position.x
                                     - LaundroMobile->playerRadius[playerID] * RSDK.Cos256(LaundroMobile->playerAngles[playerID] - 3)
                                     - self->position.x;
                player->drawOrder = Zone->playerDrawLow;
                player->state     = Player_State_Air;
            }
        }

        EntityWhirlpool *whirlpool = (EntityWhirlpool *)self->whirlpool;
        if (whirlpool) {
            if (whirlpool->objectID == Whirlpool->objectID)
                whirlpool->activePlayers = -3;
            self->whirlpool = NULL;
        }
        self->state = LaundroMobile_State_Unknown18;
    }
    LaundroMobile_CheckPlayerCollisions();
    LaundroMobile_HandleAnimations();
}

void LaundroMobile_State_Unknown18(void)
{
    RSDK_THIS(LaundroMobile);

    LaundroMobile->animSpeed -= 4;
    if (LaundroMobile->animSpeed <= 0) {
        self->velocity.y = 0;
        self->timer      = 32;
        self->state      = LaundroMobile_State_Unknown9;
    }
    LaundroMobile_CheckPlayerCollisions();
    LaundroMobile_HandleAnimations();
}

void LaundroMobile_State_Destroyed_Phase2(void)
{
    RSDK_THIS(LaundroMobile);

    foreach_active(Player, player)
    {
        int playerID = RSDK.GetEntityID(player);
        if (player->state == Player_State_None) {
            player->velocity.x = player->position.x - LaundroMobile->playerRadius[playerID] * RSDK.Cos256(LaundroMobile->playerAngles[playerID] - 3)
                                 - self->position.x;
            player->drawOrder = Zone->playerDrawLow;
            player->state     = Player_State_Air;
        }
    }

    LaundroMobile_Explode();

    if (LaundroMobile->animSpeed > 0)
        LaundroMobile->animSpeed -= 4;

    if (--LaundroMobile->invincibilityTimer) {
        if (LaundroMobile->invincibilityTimer == 30) {
            for (int i = 0; i < 4; ++i) {
                EntityDebris *debris =
                    CREATE_ENTITY(Debris, Debris_State_FallAndFlicker, LaundroMobile->rocketPositions[i].x, LaundroMobile->rocketPositions[i].y);
                RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 5, &debris->animator, true, (LaundroMobile->rocketAngles[i] >> 12) & 0xF);
                debris->animator.animationSpeed = 0;

                if (debris->position.y >= self->position.y) {
                    if (debris->position.x < self->position.x)
                        debris->velocity.x = -0x20000;
                    else
                        debris->velocity.x = 0x20000;
                    debris->velocity.y = -0x10000;
                }
                else {
                    if (debris->position.x < self->position.x)
                        debris->velocity.x = -0x40000;
                    else
                        debris->velocity.x = 0x40000;
                    debris->velocity.y = -0x20000;
                }
                debris->gravity                     = 0x3800;
                debris->drawOrder                   = Zone->drawOrderHigh;
                debris->updateRange.x               = 0x400000;
                debris->updateRange.y               = 0x400000;
                LaundroMobile->rocketPositions[i].x = 0;
                LaundroMobile->rocketPositions[i].y = 0;
            }
        }
    }
    else {
        self->timer      = 60;
        self->velocity.y = 0x20000;
        self->state      = LaundroMobile_State_Finish;
    }
}

void LaundroMobile_State_Finish(void)
{
    RSDK_THIS(LaundroMobile);

    self->position.y += self->velocity.y;
    self->velocity.y -= 0x3800;
    LaundroMobile_Explode();

    if (--self->timer <= 0) {
        Water->moveWaterLevel = true;
        Water->targetWaterLevel += 0x780000;
        Water->waterMoveSpeed = 0x10000;
        self->timer         = 0;
        Music_TransitionTrack(TRACK_STAGE, 0.0125);
        CREATE_ENTITY(EggPrison, intToVoid(EGGPRISON_FLYING), (ScreenInfo->position.x + ScreenInfo->centerX) << 16,
                      (ScreenInfo->position.y - 48) << 16);
#if RETRO_USE_PLUS
        Zone->stageFinishCallback = LaundroMobile_StageFinishCB_Blank;
        self->state             = LaundroMobile_State_StartOutro;
#else
        destroyEntity(self);
#endif
    }
}

#if RETRO_USE_PLUS
void LaundroMobile_StageFinishCB_Blank(void) {}

void LaundroMobile_State_StartOutro(void)
{
    RSDK_THIS(LaundroMobile);

    EntityActClear *actClear = RSDK_GET_ENTITY(SLOT_ACTCLEAR, ActClear);

    if (self->timer) {
        if (actClear->objectID != ActClear->objectID) {
            self->timer = 0;

            EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
            player1->drawOrder    = Zone->playerDrawLow;
            player1->state        = Player_State_Ground;
            player1->direction    = FLIP_NONE;
            player1->stateInput   = StateMachine_None;
            RSDK.SetSpriteAnimation(player1->aniFrames, ANI_IDLE, &player1->animator, true, 0);

            EntityPlayer *player2 = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
            if (player2->objectID == Player->objectID) {
                player2->drawOrder  = Zone->playerDrawLow;
                player2->state      = Player_State_Ground;
                player2->direction  = FLIP_NONE;
                player2->stateInput = StateMachine_None;
                RSDK.SetSpriteAnimation(player2->aniFrames, ANI_IDLE, &player2->animator, true, 0);
            }
            self->state = LaundroMobile_State_OutroRumble;
        }
    }
    else if (actClear->objectID == ActClear->objectID) {
        self->timer = 1;
    }
}

void LaundroMobile_State_OutroRumble(void)
{
    RSDK_THIS(LaundroMobile);

    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityPlayer *player2 = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);

    player1->timer = 0;
    if (player2->objectID == Player->objectID)
        player2->timer = 0;

    if (!(Zone->timer & 3)) {
        Camera_ShakeScreen(0, 0, 2);
    }

    if (!(Zone->timer & 7))
        RSDK.PlaySfx(LaundroMobile->sfxRumble, false, 255);

    if (++self->timer == 90) {
        self->timer = 0;
        foreach_active(WaterGush, gush)
        {
            if (gush->position.x > self->position.x) {
                gush->flag      = true;
                gush->inkEffect = INK_ALPHA;
                gush->alpha     = 256;
                gush->drawOrder = Zone->playerDrawLow;
            }
        }

        for (int i = 0; i < 0x20; ++i) {
            EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_Fall, 0x6EB00000, 0xAE00000);
            RSDK.SetSpriteAnimation(WaterGush->aniFrames, 4, &debris->animator, true, 0);
            debris->position.x += 0x60000 * RSDK.Rand(-8, 8);
            debris->position.y += 0x60000 * RSDK.Rand(-8, 8);
            debris->velocity.x = RSDK.Rand(-8, 8) << 16;
            debris->velocity.y = RSDK.Rand(-8, 8) << 16;
            debris->velocity.x = RSDK.Rand(-8, 9) << 15;
            debris->velocity.y = RSDK.Rand(-8, 5) << 16;
            debris->direction  = RSDK.Rand(0, 4);
            debris->drawFX     = FX_FLIP;
            debris->drawOrder  = Zone->drawOrderHigh;
            debris->gravity    = 0x3800;
        }

        RSDK.CopyTileLayer(Zone->fgHigh, 1763, 172, Zone->fgHigh, 1919, 172, 11, 4);
        RSDK.PlaySfx(LaundroMobile->sfxImpact, false, 255);
        RSDK.PlaySfx(WaterGush->sfxGush, false, 255);
        RSDK.PlaySfx(Water->sfxSplash, false, 255);
        self->state = LaundroMobile_State_OutroWaterGush;
    }
}

void LaundroMobile_State_OutroWaterGush(void)
{
    RSDK_THIS(LaundroMobile);

    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityPlayer *player2 = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);

    player1->timer = 0;
    if (player2->objectID == Player->objectID)
        player2->timer = 0;

    if (++self->timer < 75) {
        foreach_active(WaterGush, gush)
        {
            gush->position.y += 0x8000;
            gush->alpha -= 2;
            if (gush->position.x > self->position.x) {
                if (gush->position.x - gush->field_78 < player1->position.x) {
                    player1->velocity.x = -0x30000;
                    player1->groundVel  = -0x30000;
                }

                if (player2->objectID == Player->objectID && gush->position.x - gush->field_78 < player2->position.x) {
                    player2->velocity.x = -0x30000;
                    player2->groundVel  = -0x30000;
                }
            }
        }
    }
    else {
        foreach_active(WaterGush, gush)
        {
            gush->position.y += 0x8000;
            gush->alpha -= 2;
            if (gush->position.x > self->position.x && gush->alpha <= 0) {
                destroyEntity(gush);
                for (int p = 0; p < PLAYER_MAX; ++p) Zone->playerBoundActiveR[p] = false;
                self->timer = 0;
                self->state = LaundroMobile_State_ExitHCZ;
            }
        }
    }
}

void LaundroMobile_State_ExitHCZ(void)
{
    RSDK_THIS(LaundroMobile);

    if (++self->timer > 120) {
        RSDK_GET_ENTITY(SLOT_PLAYER2, Player)->right = true;

        EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
        if (player1->objectID == Player->objectID)
            player1->right = true;
        if (player1->position.x > (Zone->screenBoundsR1[0] + 64) << 16) {
            HCZSetup_StageFinishCB_Act2();
            destroyEntity(self);
        }
    }
}
#endif

void LaundroMobile_StateDraw_Unknown1(void)
{
    RSDK_THIS(LaundroMobile);
    Vector2 drawPos;

    if (LaundroMobile->invincibilityTimer & 1) {
        RSDK.SetPaletteEntry(0, 128, 0xE0E0E0);
        RSDK.SetPaletteEntry(1, 128, 0xE0E0E0);
    }
    drawPos.x = self->position.x - 0x20000;
    drawPos.y = self->position.y - 0x1B0000;

    self->animator1.frameID = 4;
    RSDK.DrawSprite(&self->animator1, &drawPos, false);

    drawPos.y += 0x300000;
    RSDK.DrawSprite(&self->animator1, &drawPos, false);

    drawPos = self->position;
    drawPos.x += 0x100000;
    RSDK.DrawSprite(&self->animator3, &drawPos, false);

    self->animator1.frameID = 0;
    RSDK.DrawSprite(&self->animator1, NULL, false);
    RSDK.DrawSprite(&self->animator2, NULL, false);

    drawPos.x                 = self->position.x - 0x120000;
    drawPos.y                 = self->position.y - 0x1B0000;
    self->animator1.frameID = 4;
    RSDK.DrawSprite(&self->animator1, &drawPos, false);

    drawPos.y += 0x300000;
    RSDK.DrawSprite(&self->animator1, &drawPos, false);

    RSDK.SetPaletteEntry(0, 128, 0x000000);
    RSDK.SetPaletteEntry(1, 128, 0x000000);
}

void LaundroMobile_StateDraw_Unknown2(void)
{
    RSDK_THIS(LaundroMobile);

    if (LaundroMobile->invincibilityTimer & 1) {
        RSDK.SetPaletteEntry(0, 128, 0xE0E0E0);
        RSDK.SetPaletteEntry(1, 128, 0xE0E0E0);
    }

    for (int p = 0; p < 4; ++p) {
        self->direction = p >= 2 ? FLIP_X : FLIP_NONE;
        uint8 angle       = LaundroMobile->rocketAngles[p] >> 8;
        if (angle >= 0x80) {
            self->animator2.frameID = angle >> 4;

            self->animator4.frameID = 12;
            if (!(Zone->timer & 1))
                self->animator4.frameID = self->animator2.frameID;

            RSDK.DrawSprite(&self->animator2, &LaundroMobile->rocketPositions[p], false);
            RSDK.DrawSprite(&self->animator4, &LaundroMobile->rocketPositions[p], false);
        }
    }

    self->direction         = FLIP_NONE;
    self->animator1.frameID = 5;
    RSDK.DrawSprite(&self->animator1, NULL, false);
    RSDK.DrawSprite(&self->animator3, NULL, false);

    self->animator1.frameID = 1;
    RSDK.DrawSprite(&self->animator1, NULL, false);

    for (int p = 0; p < 4; ++p) {
        self->direction = p >= 2 ? FLIP_X : FLIP_NONE;
        uint8 angle       = LaundroMobile->rocketAngles[p] >> 8;
        if (angle < 0x80) {
            self->animator2.frameID = angle >> 4;

            self->animator4.frameID = 12;
            if (!(Zone->timer & 1))
                self->animator4.frameID = self->animator2.frameID;

            angle = (angle + 64) & 0xFF;
            if (angle <= 0x80)
                RSDK.DrawSprite(&self->animator4, &LaundroMobile->rocketPositions[p], false);

            RSDK.DrawSprite(&self->animator2, &LaundroMobile->rocketPositions[p], false);

            if (angle > 0x80)
                RSDK.DrawSprite(&self->animator4, &LaundroMobile->rocketPositions[p], false);
        }
    }

    self->direction = FLIP_NONE;
    RSDK.SetPaletteEntry(0, 128, 0x000000);
    RSDK.SetPaletteEntry(1, 128, 0x000000);
}

void LaundroMobile_State1_Unknown1(void)
{
    RSDK_THIS(LaundroMobile);

    if (self->active == ACTIVE_BOUNDS) {
        if (self->position.x + 0x200000 > ScreenInfo->position.x << 16) {
            if (self->position.x - 0x200000 <= (ScreenInfo->position.x + ScreenInfo->width) << 16) {
                self->visible            = false;
                EntityLaundroMobile *child = CREATE_ENTITY(LaundroMobile, intToVoid(LAUNDROMOBILE_BOMB), self->position.x, self->position.y);
                child->velocity.x          = LaundroMobile->currentVelocity - 0x20000;
                child->active              = ACTIVE_NORMAL;
                child->state               = LaundroMobile_State1_Unknown2;
                child->isPermanent         = true;
                self->active             = ACTIVE_NORMAL;
            }
        }
    }
    else {
        if (self->position.x + 0x200000 < ScreenInfo->position.x << 16) {
            self->visible = true;
            self->active  = ACTIVE_BOUNDS;
        }
    }
}

void LaundroMobile_State1_Unknown2(void)
{
    RSDK_THIS(LaundroMobile);

    self->position.x += self->velocity.x;

    foreach_active(Player, player)
    {
        int velX = player->velocity.x;
        if (Player_CheckCollisionBox(player, self, &LaundroMobile->hitboxBox) == C_LEFT) {
            RSDK.PlaySfx(LaundroMobile->sfxButton, false, 255);
            self->velocity.x = LaundroMobile->currentVelocity + 0x18000;
            RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 8, &self->animator1, true, 0);
            self->state = LaundroMobile_State1_Unknown3;
        }
        player->velocity.x = velX;
    }

    if (self->position.x + 0x200000 < ScreenInfo->position.x << 16) {
        destroyEntity(self);
    }
    else {
        foreach_active(LaundroMobile, boss)
        {
            if (boss->type == LAUNDROMOBILE_BOSS
                && RSDK.CheckObjectCollisionTouchBox(boss, &LaundroMobile->hitboxBoss, self, &LaundroMobile->hitboxBox)
                && boss->state == LaundroMobile_State_Unknown4) {
                RSDK.PlaySfx(LaundroMobile->sfxPimPom, false, 255);

                EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_FallAndFlicker, self->position.x, self->position.y);
                RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 7, &debris->animator, true, 0);
                debris->velocity.y    = -0x28000;
                debris->velocity.x    = LaundroMobile->currentVelocity + 0x28000;
                debris->gravity       = 0x3800;
                debris->drawOrder     = Zone->drawOrderHigh;
                debris->updateRange.x = 0x400000;
                debris->updateRange.y = 0x400000;
                destroyEntity(self);
                foreach_break;
            }
        }
    }
}

void LaundroMobile_State1_Unknown3(void)
{
    RSDK_THIS(LaundroMobile);

    self->position.x += self->velocity.x;
    self->velocity.x -= 0x800;
    RSDK.ProcessAnimation(&self->animator1);
    if (self->position.x + 0x200000 < ScreenInfo->position.x << 16) {
        destroyEntity(self);
    }
    else {
        foreach_active(Player, player)
        {
            int velX = player->velocity.x;
            Player_CheckCollisionBox(player, self, &LaundroMobile->hitboxBox);
            player->velocity.x = velX;
        }

        EntityLaundroMobile *boss = (EntityLaundroMobile *)LaundroMobile->bossPtr;
        if (RSDK.CheckObjectCollisionTouchBox(boss, &LaundroMobile->hitboxBoss, self, &LaundroMobile->hitboxBox)) {
            EntityExplosion *explosion = CREATE_ENTITY(Explosion, intToVoid(EXPLOSION_BOSS), self->position.x, self->position.y);
            explosion->drawOrder       = Zone->drawOrderHigh;
            explosion->velocity.x      = LaundroMobile->currentVelocity - 0x10000;
            RSDK.PlaySfx(LaundroMobile->sfxExplosion, false, 255);

            EntityWater *water = CREATE_ENTITY(Water, intToVoid(WATER_BUBBLE), self->position.x, self->position.y);
            water->drawOrder   = Zone->drawOrderLow + 1;
            water->angle       = 2 * RSDK.Rand(0, 256);
            water->speed       = -0x1400;
            water->velocity.x  = (LaundroMobile->currentVelocity + (LaundroMobile->currentVelocity >> 2)) + (LaundroMobile->currentVelocity >> 1);
            water->bubbleX    = water->position.x;
            water->childPtr    = 0;
            RSDK.SetSpriteAnimation(Water->aniFrames, 3, &water->animator, false, 5);
            water->tileCollisions = true;

            if (!LaundroMobile->invincibilityTimer) {
                --LaundroMobile->health;
                RSDK.SetSpriteAnimation(LaundroMobile->eggmanFrames, 2, &boss->animator3, true, 0);
                RSDK.PlaySfx(LaundroMobile->sfxHit, false, 255);
                LaundroMobile->invincibilityTimer = 30;
                if (LaundroMobile->health <= 8) {
                    RSDK.StopSFX(LaundroMobile->sfxFan);
                    RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 4, &boss->animator2, true, 0);
                    boss->timer = 60;
                    boss->state = LaundroMobile_State_Destroyed_Phase1;
                }
            }
            destroyEntity(self);
        }
    }
}

void LaundroMobile_StateDraw4_Unknown1(void)
{
    RSDK_THIS(LaundroMobile);
    RSDK.DrawSprite(&self->animator1, NULL, false);
}

void LaundroMobile_State3_Unknown1(void)
{
    RSDK_THIS(LaundroMobile);

    if (self->active == ACTIVE_BOUNDS) {
        if (self->position.x + 0x200000 > ScreenInfo->position.x << 16) {
            if (self->position.x - 0x200000 <= (ScreenInfo->position.x + ScreenInfo->width) << 16) {
                self->visible            = false;
                EntityLaundroMobile *child = CREATE_ENTITY(LaundroMobile, intToVoid(self->type), self->position.x, self->position.y);
                child->velocity.x          = LaundroMobile->currentVelocity - 0x20000;
                child->active              = ACTIVE_NORMAL;
                child->state               = LaundroMobile_State3_Unknown2;
                child->isPermanent         = true;
                self->active             = ACTIVE_NORMAL;
            }
        }
    }
    else {
        if (self->position.x + 0x200000 < ScreenInfo->position.x << 16) {
            self->visible = true;
            self->active  = ACTIVE_BOUNDS;
        }
    }
}

void LaundroMobile_State3_Unknown2(void)
{
    RSDK_THIS(LaundroMobile);

    self->position.x += self->velocity.x;

    foreach_active(Player, player)
    {
        if (self->type == LAUNDROMOBILE_SPIKES) {
            if (Player_CheckCollisionBox(player, self, &LaundroMobile->hitboxBomb) == C_LEFT) {
                Player_CheckHit(player, self);
            }
        }
        else {
            Player_CheckCollisionBox(player, self, &LaundroMobile->hitboxBox);
        }
    }

    if (self->position.x + 0x200000 >= ScreenInfo->position.x << 16) {
        foreach_active(LaundroMobile, laundroMobile)
        {
            if (laundroMobile->type == LAUNDROMOBILE_BOSS) {
                Hitbox *hitbox = &LaundroMobile->hitboxBomb;
                if (self->type != LAUNDROMOBILE_SPIKES)
                    hitbox = &LaundroMobile->hitboxBox;
                if (RSDK.CheckObjectCollisionTouchBox(laundroMobile, &LaundroMobile->hitboxBoss, self, hitbox)
                    || laundroMobile->state == LaundroMobile_State_Unknown4) {
                    RSDK.PlaySfx(LaundroMobile->sfxLedgeBreak, false, 255);

                    EntityDebris *debris = NULL;
                    if (self->type == LAUNDROMOBILE_SPIKES) {
                        debris = CREATE_ENTITY(Debris, Debris_State_FallAndFlicker, self->position.x, self->position.y - 0x80000);
                        RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 9, &debris->animator, true, 18);
                        debris->drawFX |= FX_ROTATE;
                        debris->velocity.x    = LaundroMobile->currentVelocity + 0x18000;
                        debris->velocity.y    = -0x28000;
                        debris->gravity       = 0x3800;
                        debris->rotSpeed      = RSDK.Rand(-8, 8);
                        debris->drawOrder     = Zone->drawOrderHigh;
                        debris->updateRange.x = 0x400000;
                        debris->updateRange.y = 0x400000;

                        debris = CREATE_ENTITY(Debris, Debris_State_FallAndFlicker, self->position.x, self->position.y + 0x80000);
                        RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 9, &debris->animator, true, 18);
                        debris->drawFX |= FX_ROTATE;
                        debris->velocity.x    = LaundroMobile->currentVelocity + 0x18000;
                        debris->velocity.y    = -0x20000;
                        debris->gravity       = 0x3800;
                        debris->rotSpeed      = RSDK.Rand(-8, 8);
                        debris->drawOrder     = Zone->drawOrderHigh;
                        debris->updateRange.x = 0x400000;
                        debris->updateRange.y = 0x400000;
                        self->position.x += 0x100000;
                    }

                    debris = CREATE_ENTITY(Debris, Debris_State_FallAndFlicker, self->position.x, self->position.y);
                    RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 9, &debris->animator, true, 4 * (self->animator1.frameID % 3) + 6);
                    debris->velocity.y    = -0x28000;
                    debris->velocity.x    = LaundroMobile->currentVelocity + 0x20000;
                    debris->gravity       = 0x3800;
                    debris->drawOrder     = Zone->drawOrderHigh;
                    debris->updateRange.x = 0x400000;
                    debris->updateRange.y = 0x400000;

                    debris = CREATE_ENTITY(Debris, Debris_State_FallAndFlicker, self->position.x, self->position.y);
                    RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 9, &debris->animator, true, 4 * (self->animator1.frameID % 3) + 7);
                    debris->velocity.y    = -0x28000;
                    debris->velocity.x    = LaundroMobile->currentVelocity + 0x28000;
                    debris->gravity       = 0x3800;
                    debris->drawOrder     = Zone->drawOrderHigh;
                    debris->updateRange.x = 0x400000;
                    debris->updateRange.y = 0x400000;

                    debris = CREATE_ENTITY(Debris, Debris_State_FallAndFlicker, self->position.x, self->position.y);
                    RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 9, &debris->animator, true, 4 * (self->animator1.frameID % 3) + 8);
                    debris->velocity.y    = -0x20000;
                    debris->velocity.x    = LaundroMobile->currentVelocity + 0x20000;
                    debris->gravity       = 0x3800;
                    debris->drawOrder     = Zone->drawOrderHigh;
                    debris->updateRange.x = 0x400000;
                    debris->updateRange.y = 0x400000;

                    debris = CREATE_ENTITY(Debris, Debris_State_FallAndFlicker, self->position.x, self->position.y);
                    RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 9, &debris->animator, true, 4 * (self->animator1.frameID % 3) + 9);
                    debris->velocity.y    = -0x20000;
                    debris->velocity.x    = LaundroMobile->currentVelocity + 0x28000;
                    debris->gravity       = 0x3800;
                    debris->drawOrder     = Zone->drawOrderHigh;
                    debris->updateRange.x = 0x400000;
                    debris->updateRange.y = 0x400000;

                    destroyEntity(self);
                    foreach_break;
                }
            }
        }
    }
    else {
        destroyEntity(self);
    }
}

void LaundroMobile_State2_Unknown1(void)
{
    RSDK_THIS(LaundroMobile);

    self->animator2.animationSpeed = LaundroMobile->animSpeed;
    self->animator3.animationSpeed = LaundroMobile->animSpeed;
    RSDK.ProcessAnimation(&self->animator2);
    RSDK.ProcessAnimation(&self->animator3);
    Zone->playerBoundActiveL[0] = true;
    Zone->playerBoundActiveR[0] = true;
    Zone->screenBoundsL1[0]     = ScreenInfo->position.x;
    Zone->screenBoundsR1[0]     = ScreenInfo->centerX + (self->position.x >> 16);
    if (RSDK_GET_ENTITY(SLOT_PLAYER1, Player)->position.x > self->position.x - 0xC00000)
        Zone->screenBoundsT1[0] = ScreenInfo->position.y;

    if (!LaundroMobile->health && !LaundroMobile->invincibilityTimer) {
        Debris_FallFlickerSetup(LaundroMobile->aniFrames, LaundroMobile->debrisInfo);
        destroyEntity(self);
    }
}

void LaundroMobile_StateDraw2_Unknown1(void)
{
    RSDK_THIS(LaundroMobile);
    Vector2 drawPos;

    drawPos.x                 = self->position.x;
    drawPos.y                 = self->position.y;
    self->animator1.frameID = 2;
    RSDK.DrawSprite(&self->animator1, 0, false);
    RSDK.DrawSprite(&self->animator2, 0, false);

    drawPos.y += 0x200000;
    RSDK.DrawSprite(&self->animator2, &drawPos, false);

    drawPos.y += 0x200000;
    RSDK.DrawSprite(&self->animator2, &drawPos, false);

    self->animator1.frameID = 3;
    RSDK.DrawSprite(&self->animator1, &drawPos, false);
    RSDK.DrawSprite(&self->animator3, &drawPos, false);
}

void LaundroMobile_State6_Unknown1(void)
{
    RSDK_THIS(LaundroMobile);

    if (--self->timer <= 0) {
        CREATE_ENTITY(Water, intToVoid(WATER_SPLASH), self->position.x, Water->waterLevel);
        destroyEntity(self);
    }
}

#if RETRO_INCLUDE_EDITOR
void LaundroMobile_EditorDraw(void)
{
    RSDK_THIS(LaundroMobile);

    switch (self->type) {
        case LAUNDROMOBILE_BOSS:
            RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 0, &self->animator1, true, 0);
            RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 3, &self->animator2, true, 0);
            RSDK.SetSpriteAnimation(LaundroMobile->eggmanFrames, 0, &self->animator3, true, 0);
            self->startY        = self->position.y;
            self->updateRange.x = 0x800000;
            self->updateRange.y = 0x800000;
            self->stateDraw     = LaundroMobile_StateDraw_Unknown1;
            break;
        case LAUNDROMOBILE_BOMB:
            self->startPos.x = self->position.x;
            self->startPos.y = self->position.y;
            RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 7, &self->animator1, true, 0);
            self->updateRange.x = 0x1800000;
            self->updateRange.y = 0x1800000;
            self->stateDraw     = LaundroMobile_StateDraw4_Unknown1;
            break;
        case LAUNDROMOBILE_PHASE2:
            RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 0, &self->animator1, true, 0);
            RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 1, &self->animator2, true, 0);
            RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 2, &self->animator3, true, 0);
            self->updateRange.x = 0x2000000;
            self->updateRange.y = 0x800000;
            self->stateDraw     = LaundroMobile_StateDraw2_Unknown1;
            break;
        case LAUNDROMOBILE_LOOPPOINT:
            RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 0, &self->animator1, true, 0);
            RSDK.DrawSprite(&self->animator1, NULL, false);
            self->stateDraw = StateMachine_None;
            break;
        case LAUNDROMOBILE_BLOCK:
            self->startPos.x = self->position.x;
            self->startPos.y = self->position.y;
            RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 9, &self->animator1, false, 0);
            self->updateRange.x = 0x1800000;
            self->updateRange.y = 0x1800000;
            self->stateDraw     = LaundroMobile_StateDraw4_Unknown1;
            break;
        case LAUNDROMOBILE_SPIKES:
            self->startPos.x = self->position.x;
            self->startPos.y = self->position.y;
            RSDK.SetSpriteAnimation(LaundroMobile->aniFrames, 9, &self->animator1, false, 3);
            self->updateRange.x = 0x1800000;
            self->updateRange.y = 0x1800000;
            self->stateDraw     = LaundroMobile_StateDraw4_Unknown1;
            break;
        default: break;
    }

    StateMachine_Run(self->stateDraw);
}

void LaundroMobile_EditorLoad(void)
{
    LaundroMobile->aniFrames    = RSDK.LoadSpriteAnimation("HCZ/LaundroMobile.bin", SCOPE_STAGE);
    LaundroMobile->eggmanFrames = RSDK.LoadSpriteAnimation("Eggman/EggmanHCZ2.bin", SCOPE_STAGE);

    RSDK_ACTIVE_VAR(LaundroMobile, type);
    RSDK_ENUM_VAR("Boss", LAUNDROMOBILE_BOSS);
    RSDK_ENUM_VAR("Bomb", LAUNDROMOBILE_BOMB);
    RSDK_ENUM_VAR("Phase 2", LAUNDROMOBILE_PHASE2);
    RSDK_ENUM_VAR("Loop Point", LAUNDROMOBILE_LOOPPOINT);
    RSDK_ENUM_VAR("Block", LAUNDROMOBILE_BLOCK);
    RSDK_ENUM_VAR("Spikes", LAUNDROMOBILE_SPIKES);
}
#endif

void LaundroMobile_Serialize(void) { RSDK_EDITABLE_VAR(LaundroMobile, VAR_UINT8, type); }
