#include "SonicMania.h"

ObjectHeavyGunner *HeavyGunner;

void HeavyGunner_Update(void)
{
    RSDK_THIS(HeavyGunner);
    StateMachine_Run(entity->state);
}

void HeavyGunner_LateUpdate(void) {}

void HeavyGunner_StaticUpdate(void) {}

void HeavyGunner_Draw(void)
{
    RSDK_THIS(HeavyGunner);
    StateMachine_Run(entity->stateDraw);
}

void HeavyGunner_Create(void *data)
{
    RSDK_THIS(HeavyGunner);

    if (!RSDK_sceneInfo->inEditor) {
        if (globals->gameMode < MODE_TIMEATTACK) {
            if (data) {
                entity->type      = voidToInt(data);
                entity->active    = ACTIVE_NORMAL;
                entity->visible   = true;
                entity->drawOrder = Zone->drawOrderLow - 1;
                switch (entity->type) {
                    case HEAVYGUNNER_HELI:
                        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 6, &entity->animator1, true, 0);
                        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 14, &entity->animator2, true, 0);
                        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 7, &entity->animator3, true, 0);
                        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 8, &entity->animator4, true, 0);
                        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 9, &entity->animator5, true, 0);
                        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 10, &entity->animator6, true, 0);
                        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 11, &entity->animator7, true, 0);
                        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 11, &entity->animator8, true, 2);
                        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 0, &entity->animator9, true, 0);
                        entity->drawFX    = FX_ROTATE;
                        entity->rotation  = 64;
                        entity->state     = HeavyGunner_State2_Unknown1;
                        entity->stateDraw = HeavyGunner_StateDraw_Unknown2;
                        break;
                    case HEAVYGUNNER_EGGROBO:
                        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 17, &entity->animator7, true, 0);
                        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 16, &entity->animator1, true, RSDK.Rand(0, 4));
                        entity->drawFX    = FX_SCALE | FX_ROTATE;
                        entity->state     = HeavyGunner_State3_Unknown1;
                        entity->stateDraw = HeavyGunner_StateDraw3_Unknown;
                        entity->scale.x   = 0x200;
                        entity->scale.y   = 0x200;
                        entity->field_70  = 512;
                        entity->field_6C  = RSDK.Rand(-0x200000, 0x600000);
                        entity->timer     = 240;
                        break;
                    case HEAVYGUNNER_GUN:
                        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 20, &entity->animator1, true, 0);
                        entity->drawFX    = FX_ROTATE;
                        entity->rotation  = 64;
                        entity->state     = HeavyGunner_State4_Unknown1;
                        entity->stateDraw = HeavyGunner_StateDraw4_Unknown1;
                        break;
                    case HEAVYGUNNER_MISSILE_F:
                    case HEAVYGUNNER_MISSILE:
                        if (entity->type == HEAVYGUNNER_MISSILE_F)
                            RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 29, &entity->animator1, true, 0);
                        else
                            RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 30, &entity->animator1, true, 0);
                        entity->drawFX     = FX_SCALE | FX_ROTATE | FX_FLIP;
                        entity->velocity.x = 0x40000;
                        entity->velocity.y = -0x38000;
                        entity->scale.x    = 0x100;
                        entity->scale.y    = 0x100;
                        entity->rotation   = -24;
                        entity->state      = HeavyGunner_State6_Unknown1;
                        entity->stateDraw  = HeavyGunner_StateDraw6_Unknown;
                        entity->drawOrder  = Zone->drawOrderLow;
                        break;
                    case HEAVYGUNNER_ESCAPE_HBH:
                        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 5, &entity->animator1, true, 0);
                        entity->velocity.x = -0x20000;
                        entity->velocity.y = -0x20000;
                        entity->state      = HeavyGunner_State7_Unknown1;
                        entity->stateDraw  = HeavyGunner_StateDraw4_Unknown1;
                        entity->drawOrder  = Zone->drawOrderLow;
                        break;
                    default: break;
                }
            }
            else {
                Zone->autoScrollSpeed = 0;
                entity->active        = ACTIVE_XBOUNDS;
                entity->updateRange.x = 0x800000;
                entity->updateRange.y = 0x800000;
                entity->state         = HeavyGunner_State_SetupArena;
            }
        }
        else {
            destroyEntity(entity);
        }
    }
}

void HeavyGunner_StageLoad(void)
{
    HeavyGunner->aniFrames     = RSDK.LoadSpriteAnimation("SPZ1/Boss.bin", SCOPE_STAGE);
    HeavyGunner->sfxFire       = RSDK.GetSFX("Stage/CannonFire.wav");
    HeavyGunner->sfxJet        = RSDK.GetSFX("Stage/RocketJet.wav");
    HeavyGunner->sfxHit        = RSDK.GetSFX("Stage/BossHit.wav");
    HeavyGunner->sfxDestroy    = RSDK.GetSFX("Global/Destroy.wav");
    HeavyGunner->sfxExplosion2 = RSDK.GetSFX("Stage/Explosion2.wav");
    HeavyGunner->sfxExplosion3 = RSDK.GetSFX("Stage/Explosion3.wav");
    HeavyGunner->sfxCharge     = RSDK.GetSFX("SPZ1/BazookaCharge.wav");
    HeavyGunner->sfxThrow      = RSDK.GetSFX("SPZ1/BazookaThrow.wav");
    HeavyGunner->sfxFlyIn      = RSDK.GetSFX("SPZ1/EggRoboFlyIn.wav");
    HeavyGunner->sfxWooshIn    = RSDK.GetSFX("SPZ1/HeliWooshIn.wav");
    HeavyGunner->sfxWooshOut   = RSDK.GetSFX("SPZ1/HeliWooshOut.wav");
    HeavyGunner->sfxFlip       = RSDK.GetSFX("SPZ1/RocketFlip.wav");
    HeavyGunner->sfxRumble     = RSDK.GetSFX("SPZ1/Rumble.wav");

    Soundboard_LoadSFX("SPZ1/HBHSurprise.wav", true, HeavyGunner_SurpriseCheckCB, NULL);
    Soundboard_LoadSFX("SPZ1/HeliProp.wav", true, HeavyGunner_HelicopterCheckCB, NULL);
    Soundboard_LoadSFX("SPZ1/RocketBurn.wav", true, HeavyGunner_BurnCheckCB, NULL);

    HeavyGunner->field_44 = 0x470000;
    HeavyGunner->field_48 = 0x2D0000;
    HeavyGunner->field_4C = 0x100000;
    HeavyGunner->field_50 = 0x80000;

    HeavyGunner->hitbox1.left   = -32;
    HeavyGunner->hitbox1.top    = -8;
    HeavyGunner->hitbox1.right  = 32;
    HeavyGunner->hitbox1.bottom = 8;

    HeavyGunner->hitbox2.left   = -16;
    HeavyGunner->hitbox2.top    = -8;
    HeavyGunner->hitbox2.right  = 16;
    HeavyGunner->hitbox2.bottom = 8;

    HeavyGunner->active = ACTIVE_NEVER;
}

bool32 HeavyGunner_HelicopterCheckCB(void)
{
    Vector2 range;

    range.x     = 0x800000;
    range.y     = 0x800000;
    bool32 flag = false;

    foreach_active(HeavyGunner, gunner)
    {
        if (RSDK.CheckOnScreen(gunner, &range) && gunner->animator4.animationID == 8)
            flag = true;
    }
    return flag;
}

bool32 HeavyGunner_SurpriseCheckCB(void)
{
    Vector2 range;

    range.x     = 0x800000;
    range.y     = 0x800000;
    bool32 flag = false;

    foreach_active(HeavyGunner, gunner)
    {
        if (RSDK.CheckOnScreen(gunner, &range) && gunner->animator9.animationID == 3 && gunner->animator1.animationID != 12)
            flag = true;
    }

    return flag;
}

bool32 HeavyGunner_BurnCheckCB(void)
{
    Vector2 range;

    range.x     = 0x800000;
    range.y     = 0x800000;
    bool32 flag = false;

    foreach_active(HeavyGunner, gunner)
    {
        if (RSDK.CheckOnScreen(gunner, &range) && gunner->animator11.animationID == 25)
            flag = true;
    }
    return flag;
}

void HeavyGunner_HandleBGWrap(int multiplier)
{
    RSDK_THIS(HeavyGunner);

    TileLayer *bg1 = RSDK.GetSceneLayer(0);
    for (int s = 0; s < bg1->scrollInfoCount; ++s) {
        bg1->scrollInfo[s].scrollPos += multiplier * bg1->scrollInfo[s].parallaxFactor;
    }

    TileLayer *bg2 = RSDK.GetSceneLayer(1);
    for (int s = 0; s < bg2->scrollInfoCount; ++s) {
        bg2->scrollInfo[s].scrollPos += multiplier * bg2->scrollInfo[s].parallaxFactor;
    }

    TileLayer *bg3 = RSDK.GetSceneLayer(2);
    for (int s = 0; s < bg3->scrollInfoCount; ++s) {
        bg3->scrollInfo[s].scrollPos += multiplier * bg3->scrollInfo[s].parallaxFactor;
    }
}

void HeavyGunner_Unknown5(void)
{
    RSDK_THIS(HeavyGunner);

    foreach_active(HeavyGunner, gunner)
    {
        if (gunner != entity) {
            if (gunner->type == HEAVYGUNNER_MISSILE_F || gunner->type == HEAVYGUNNER_MISSILE) {
                RSDK.PlaySfx(HeavyGunner->sfxFlip, false, 255);
                gunner->velocity.x = -0x10000;
                RSDK.SetSpriteAnimation(0xFFFF, 0, &gunner->animator11, true, 0);
                gunner->direction = FLIP_NONE;
                gunner->rotation  = 256;
                gunner->state     = HeavyGunner_State6_Unknown5;
            }
        }
    }
}

void HeavyGunner_StateDraw_Unknown2(void)
{
    RSDK_THIS(HeavyGunner);
    Vector2 drawPos;

    drawPos.x = entity->position.x + 0x1B0000;
    drawPos.y = entity->position.y;
    RSDK.DrawSprite(&entity->animator7, &drawPos, false);
    RSDK.DrawSprite(&entity->animator1, NULL, false);
    RSDK.DrawSprite(&entity->animator2, NULL, false);
    RSDK.DrawSprite(&entity->animator3, NULL, false);
    RSDK.DrawSprite(&entity->animator4, NULL, false);
    RSDK.DrawSprite(&entity->animator5, NULL, false);
    RSDK.DrawSprite(&entity->animator6, NULL, false);
    RSDK.DrawSprite(&entity->animator8, NULL, false);
    RSDK.DrawSprite(&entity->animator9, NULL, false);
}

void HeavyGunner_StateDraw3_Unknown(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.DrawSprite(&entity->animator7, NULL, false);
    RSDK.DrawSprite(&entity->animator1, NULL, false);
    RSDK.DrawSprite(&entity->animator10, NULL, false);
}

void HeavyGunner_StateDraw6_Unknown(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.DrawSprite(&entity->animator1, NULL, false);
    RSDK.DrawSprite(&entity->animator11, NULL, false);
}

void HeavyGunner_StateDraw4_Unknown1(void)
{
    RSDK_THIS(HeavyGunner);
    RSDK.DrawSprite(&entity->animator1, NULL, false);
}

void HeavyGunner_StateDraw4_Unknown2(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.FillScreen(0xF0F0F0, entity->timer, entity->timer - 0x80, entity->timer - 0x100);
}

void HeavyGunner_State_SetupArena(void)
{
    RSDK_THIS(HeavyGunner);

    if (++entity->timer >= 8) {
        EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);

        if (player1->position.x >= entity->position.x - 0x1000000) {
            HeavyGunner->active = ACTIVE_ALWAYS;
            entity->timer       = 0;
            entity->active      = ACTIVE_NORMAL;

            EntityCamera *camera = RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);
            entity->position.x   = camera->position.x - 0x100000;
            entity->position.y   = camera->position.y;

            Zone->playerBoundActiveL[0] = true;
            Zone->playerBoundActiveR[0] = true;
            HeavyGunner->boundsL        = Zone->screenBoundsL1[0];
            HeavyGunner->boundsR        = Zone->screenBoundsR1[0];
            HeavyGunner->boundsT        = Zone->screenBoundsT1[0];
            HeavyGunner->boundsB        = 0xC00;
            HeavyGunner->field_14       = true;

            int mult = -0x1E00 * RSDK_screens->position.y;

            TileLayer *bg1 = RSDK.GetSceneLayer(0);
            bg1->scrollPos += mult;
            bg1->parallaxFactor = 88;

            TileLayer *bg2 = RSDK.GetSceneLayer(1);
            bg2->scrollPos += mult;
            bg2->parallaxFactor = 88;

            TileLayer *bg3 = RSDK.GetSceneLayer(2);
            bg3->scrollPos += mult;
            bg3->parallaxFactor = 88;

            foreach_active(ParallaxSprite, sprite)
            {
                sprite->parallaxFactor.y = 0x5800;
                sprite->position.y -= mult;
            }

            Zone->autoScrollSpeed = player1->velocity.x;

            EntityHeavyGunner *robo1 = RSDK_GET_ENTITY(RSDK_sceneInfo->entitySlot + 1, HeavyGunner);
            int x                    = robo1->position.x;
            int y                    = robo1->position.y;
            RSDK.ResetEntitySlot(RSDK_sceneInfo->entitySlot + 1, HeavyGunner->objectID, intToVoid(HEAVYGUNNER_EGGROBO));
            robo1->position.x = x;
            robo1->position.y = y;
            robo1->parent     = (Entity *)RSDK_GET_ENTITY(RSDK_sceneInfo->entitySlot + 4, HeavyGunner);
            robo1->angle      = 0;
            RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 22, &robo1->animator10, true, 0);

            EntityHeavyGunner *robo2 = RSDK_GET_ENTITY(RSDK_sceneInfo->entitySlot + 2, HeavyGunner);
            x                        = robo2->position.x;
            y                        = robo2->position.y;
            RSDK.ResetEntitySlot(RSDK_sceneInfo->entitySlot + 2, HeavyGunner->objectID, intToVoid(HEAVYGUNNER_EGGROBO));
            robo2->position.x = x;
            robo2->position.y = y;
            robo2->parent     = (Entity *)RSDK_GET_ENTITY(RSDK_sceneInfo->entitySlot + 4, HeavyGunner);
            robo2->angle      = 80;
            RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 23, &robo2->animator10, true, 0);

            EntityHeavyGunner *robo3 = RSDK_GET_ENTITY(RSDK_sceneInfo->entitySlot + 3, HeavyGunner);
            x                        = robo3->position.x;
            y                        = robo3->position.y;
            RSDK.ResetEntitySlot(RSDK_sceneInfo->entitySlot + 3, HeavyGunner->objectID, intToVoid(HEAVYGUNNER_EGGROBO));
            robo3->position.x = x;
            robo3->position.y = y;
            robo3->parent     = (Entity *)RSDK_GET_ENTITY(RSDK_sceneInfo->entitySlot + 4, HeavyGunner);
            robo3->angle      = 160;
            robo3->timer      = 0;
            robo3->state      = HeavyGunner_State3_Unknown2;
            RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 19, &robo3->animator10, true, 0);

            EntityHeavyGunner *heli = RSDK_GET_ENTITY(RSDK_sceneInfo->entitySlot + 4, HeavyGunner);
            x                       = heli->position.x;
            y                       = heli->position.y;
            RSDK.ResetEntitySlot(RSDK_sceneInfo->entitySlot + 4, HeavyGunner->objectID, intToVoid(HEAVYGUNNER_HELI));
            heli->position.x = x;
            heli->position.y = y;

            entity->state = HeavyGunner_State_Unknown3;
            Music_TransitionTrack(TRACK_HBHBOSS, 0.0125);
        }
    }
}

void HeavyGunner_State_Unknown3(void)
{
    RSDK_THIS(HeavyGunner);

    entity->position.x += Zone->autoScrollSpeed;

    if (HeavyGunner->field_14 == true) {
        if (entity->position.x >= 0x49800000) {
            HeavyGunner_HandleBGWrap(0x80000);
            foreach_active(HeavyGunner, gunner) { gunner->position.x -= 0x8000000; }
            for (int p = 0; p < Player->playerCount; ++p) {
                EntityPlayer *player = RSDK_GET_ENTITY(p, Player);
                player->position.x -= 0x8000000;
            }
            foreach_active(Ring, ring) { ring->position.x -= 0x8000000; }
            foreach_active(ImageTrail, imageTrail)
            {
                imageTrail->position.x -= 0x8000000;
                imageTrail->currentPos.x -= 0x8000000;
                for (int i = 0; i < ImageTrail_TrackCount; ++i) imageTrail->statePos[i].x -= 0x8000000;
            }
        }

        Zone->screenBoundsL1[0] = (entity->position.x >> 16) - RSDK_screens->centerX;
        Zone->screenBoundsR1[0] = (entity->position.x >> 16) + RSDK_screens->centerX;
        Zone->screenBoundsL2[0] = Zone->screenBoundsL1[0] << 16;
        Zone->screenBoundsR2[0] = Zone->screenBoundsR1[0] << 16;
        EntityCamera *camera    = RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);
        camera->boundsL         = Zone->screenBoundsL1[0];
        camera->boundsR         = Zone->screenBoundsR1[0];

        if (entity->field_68 == 120) {
            if (!RSDK.ObjectTileGrip(entity, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x800000, 0x40))
                entity->position.y += 0x80000;
            Zone->screenBoundsB1[0] = (entity->position.y >> 16) + 168;
            Zone->screenBoundsT1[0] = Zone->screenBoundsB1[0] - RSDK_screens->height;
        }
        else {
            entity->field_68++;
        }
    }
    else if (entity->position.x < 0x49800000) {
        Zone->screenBoundsL1[0] = (entity->position.x >> 16) - RSDK_screens->centerX;
        Zone->screenBoundsR1[0] = (entity->position.x >> 16) + RSDK_screens->centerX;
        Zone->screenBoundsL2[0] = Zone->screenBoundsL1[0] << 16;
        Zone->screenBoundsR2[0] = Zone->screenBoundsR1[0] << 16;
        EntityCamera *camera    = RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);
        camera->boundsL         = Zone->screenBoundsL1[0];
        camera->boundsR         = Zone->screenBoundsR1[0];

        if (entity->field_68 == 120) {
            if (!RSDK.ObjectTileGrip(entity, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x800000, 0x40))
                entity->position.y += 0x80000;
            Zone->screenBoundsB1[0] = (entity->position.y >> 16) + 168;
            Zone->screenBoundsT1[0] = Zone->screenBoundsB1[0] - RSDK_screens->height;
        }
        else {
            entity->field_68++;
        }
    }
    else {
        Zone->screenBoundsL1[0]     = HeavyGunner->boundsL;
        Zone->screenBoundsR1[0]     = HeavyGunner->boundsR;
        Zone->screenBoundsT1[0]     = HeavyGunner->boundsT;
        Zone->screenBoundsB1[0]     = HeavyGunner->boundsB;
        Zone->playerBoundActiveR[0] = false;
        for (int p = 0; p < Player->playerCount; ++p) {
            EntityPlayer *player = RSDK_GET_ENTITY(p, Player);
            if (player->stateInput == Player_ProcessP1Input) {
                player->stateInput = HeavyGunner_InputState_LockedP1;
            }
            else if (player->stateInput == Player_ProcessP2Input_Player) {
                player->stateInput = HeavyGunner_InputState_LockedP2;
            }
            else if (player->stateInput == Player_ProcessP2Input_AI) {
                player->stateInput = HeavyGunner_InputState_LockedP2_AI;
            }
        }
        entity->state = HeavyGunner_State_Unknown2;
    }

    if (Zone->autoScrollSpeed > 0x70000) {
        Zone->autoScrollSpeed -= 0x2000;
        if (Zone->autoScrollSpeed < 0x70000)
            Zone->autoScrollSpeed = 0x70000;
    }
    else if (Zone->autoScrollSpeed < 0x70000) {
        Zone->autoScrollSpeed += 0x2000;
        if (Zone->autoScrollSpeed > 0x70000)
            Zone->autoScrollSpeed = 0x70000;
    }

    for (int p = 0; p < Player->playerCount; ++p) {
        EntityPlayer *player = RSDK_GET_ENTITY(p, Player);

        if (player->groundVel < Zone->autoScrollSpeed - 0x20000)
            player->groundVel = Zone->autoScrollSpeed - 0x20000;
        player->direction = FLIP_NONE;
        player->topSpeed  = Zone->autoScrollSpeed + 0x20000;

        if (player->state == Player_State_KnuxGlideLeft) {
            player->timer                  = 0;
            player->playerAnimator.frameID = 6;
            player->state                  = Player_State_KnuxGlideRight;
            player->abilitySpeed           = Zone->autoScrollSpeed - 0x18000;
        }
        else if (player->state == Player_State_KnuxGlideRight) {
            if (player->abilitySpeed < Zone->autoScrollSpeed - 0x18000) {
                player->abilitySpeed = Zone->autoScrollSpeed - 0x18000;
            }
        }

        if (player->groundVel > player->topSpeed)
            player->groundVel = player->topSpeed;
    }
}

void HeavyGunner_State_Unknown2(void)
{
    RSDK_THIS(HeavyGunner);

    EntityCamera *camera = RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);
    if (camera->position.x <= 0x51800000) {
        for (int p = 0; p < Player->playerCount; ++p) {
            EntityPlayer *player = RSDK_GET_ENTITY(p, Player);

            if (player->onGround) {
                if (player->groundVel < 0x80000)
                    player->groundVel = 0x80000;
            }
            else if (player->velocity.x < 0x80000) {
                player->velocity.x = 0x80000;
            }
            player->direction = FLIP_NONE;

            if (player->state == Player_State_KnuxGlideLeft) {
                player->timer                  = 0;
                player->playerAnimator.frameID = 6;
                player->state                  = Player_State_KnuxGlideRight;
                player->abilitySpeed           = Zone->autoScrollSpeed;
            }
            else if (player->state == Player_State_KnuxGlideRight) {
                if (player->abilitySpeed < Zone->autoScrollSpeed) {
                    player->abilitySpeed = Zone->autoScrollSpeed;
                }
            }
        }
    }
    else {
        camera->position.x -= 0x10000000;
        entity->position.x = camera->position.x - 0x100000;
        entity->position.y = camera->position.y - 0x800000;
        HeavyGunner_HandleBGWrap(0x100000);
        for (int p = 0; p < Player->playerCount; ++p) {
            EntityPlayer *player = RSDK_GET_ENTITY(p, Player);
            player->position.x -= 0x10000000;
            if (player->velocity.x > Zone->autoScrollSpeed)
                Zone->autoScrollSpeed = player->velocity.x;
        }

        foreach_active(Ring, ring) { ring->position.x -= 0x10000000; }
        foreach_active(ImageTrail, imageTrail)
        {
            imageTrail->position.x -= 0x10000000;
            imageTrail->currentPos.x -= 0x10000000;
            for (int i = 0; i < ImageTrail_TrackCount; ++i) imageTrail->statePos[i].x -= 0x10000000;
        }

        for (int p = 0; p < Player->playerCount; ++p) {
            EntityPlayer *player = RSDK_GET_ENTITY(p, Player);

            if (player->stateInput == HeavyGunner_InputState_LockedP1) {
                player->stateInput = Player_ProcessP1Input;
            }
            else if (player->stateInput == HeavyGunner_InputState_LockedP2) {
                player->stateInput = Player_ProcessP2Input_Player;
            }
            else if (player->stateInput == HeavyGunner_InputState_LockedP2_AI) {
                player->stateInput = Player_ProcessP2Input_AI;
            }
        }
        Zone->playerBoundActiveR[0] = true;
        HeavyGunner->field_14       = true;
        entity->state               = HeavyGunner_State_Unknown3;

        EntityHeavyGunner *heli = RSDK_GET_ENTITY(RSDK_sceneInfo->entitySlot + 4, HeavyGunner);
        heli->position.x        = camera->position.x - 0x1200000;
        heli->position.y        = camera->position.y - 0xC00000;
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 1, &heli->animator9, true, 0);
        heli->state = HeavyGunner_State2_Unknown1;
        for (int c = 3 - heli->field_74; c > 0; --c) {
            EntityHeavyGunner *robo = RSDK_GET_ENTITY(RSDK_sceneInfo->entitySlot + c, HeavyGunner);
            robo->position.x        = camera->position.x - 0x200000;
            robo->position.y        = camera->position.y - 0xC00000;
        }
    }
}

void HeavyGunner_InputState_LockedP1(void)
{
    RSDK_THIS(Player);
    Player_ProcessP1Input();

    if (entity->state != Player_State_None) {
        entity->up        = false;
        entity->down      = false;
        entity->jumpPress = false;
        entity->jumpHold  = false;
    }
}

void HeavyGunner_InputState_LockedP2(void)
{
    RSDK_THIS(Player);
    Player_ProcessP2Input_Player();

    entity->up        = false;
    entity->down      = false;
    entity->jumpPress = false;
    entity->jumpHold  = false;
    if (entity->stateInput == Player_ProcessP2Input_AI)
        entity->stateInput = HeavyGunner_InputState_LockedP2_AI;
}

void HeavyGunner_InputState_LockedP2_AI(void)
{
    RSDK_THIS(Player);
    Player_ProcessP2Input_AI();

    entity->up        = false;
    entity->down      = false;
    entity->jumpPress = false;
    entity->jumpHold  = false;
    if (entity->stateInput == Player_ProcessP2Input_Player)
        entity->stateInput = HeavyGunner_InputState_LockedP2;
}

void HeavyGunner_State3_Unknown1(void)
{
    RSDK_THIS(HeavyGunner);
    EntityHeavyGunner *parent = (EntityHeavyGunner *)entity->parent;

    RSDK.ProcessAnimation(&entity->animator1);
    RSDK.ProcessAnimation(&entity->animator7);
    RSDK.ProcessAnimation(&entity->animator10);

    int angle = 0;
    if (entity->position.x <= parent->position.x)
        angle = RSDK.Sin256(entity->angle);
    else
        angle = RSDK.Sin256(entity->angle) - 64;

    entity->position.x += (angle << 9) + Zone->autoScrollSpeed;
    entity->rotation = (RSDK.Sin256(entity->angle) >> 4) - 32;
    entity->angle    = (entity->angle + 1) & 0xFF;

    if (entity->scale.x <= entity->field_70) {
        if (entity->scale.x < entity->field_70)
            entity->scale.x += 2;
    }
    else {
        entity->scale.x -= 2;
    }

    entity->scale.y = entity->scale.x;
    if (entity->scale.x <= 0x200)
        entity->drawOrder = Zone->drawOrderLow - 1;
    else
        entity->drawOrder = Zone->drawOrderHigh;
    if (parent->state == HeavyGunner_State2_Unknown6)
        entity->velocity.y = -0x8000;
    else
        entity->velocity.y += ((parent->position.y + entity->field_6C - entity->position.y) >> 8) - (entity->velocity.y >> 4);

    entity->position.y += entity->velocity.y;
    if (--entity->timer <= 0) {
        entity->timer    = RSDK.Rand(120, 240);
        entity->field_6C = RSDK.Rand(-0x200000, 0x600000);
        switch (RSDK.Rand(0, 8)) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4: entity->field_70 = 0x200; break;
            case 5:
            case 6: entity->field_70 = 0x180; break;
            case 7: entity->field_70 = 0x300; break;
            default: break;
        }
    }
}

void HeavyGunner_State3_Unknown2(void)
{
    RSDK_THIS(HeavyGunner);

    if (++entity->timer == 120) {
        entity->position.x = entity->parent->position.x - 0x1800000;
        entity->position.y = entity->parent->position.y + 0x800000;
        entity->scale.x    = 0x400;
        entity->scale.y    = 0x400;
        entity->velocity.x = 0x6A000;
        entity->drawOrder  = Zone->drawOrderHigh;
        RSDK.PlaySfx(HeavyGunner->sfxFlyIn, false, 255);
        entity->state = HeavyGunner_State3_Unknown3;
    }
}

void HeavyGunner_State3_Unknown3(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.ProcessAnimation(&entity->animator1);
    RSDK.ProcessAnimation(&entity->animator7);
    RSDK.ProcessAnimation(&entity->animator10);
    entity->velocity.x -= 0x1000;
    entity->position.x += entity->velocity.x + Zone->autoScrollSpeed;
    entity->rotation = (entity->velocity.x - 0x40000) >> 13;
    entity->position.y += entity->velocity.y + ((entity->parent->position.y - entity->position.y + 0x400000) >> 6);
    if (entity->velocity.x < 0 && entity->position.x < entity->parent->position.x + 0x600000) {
        entity->drawOrder = Zone->drawOrderLow - 1;
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 21, &entity->animator10, true, 0);
        entity->state            = HeavyGunner_State3_Unknown4;
        EntityHeavyGunner *child = CREATE_ENTITY(HeavyGunner, intToVoid(HEAVYGUNNER_GUN), entity->position.x + 0x80000, entity->position.y - 0x80000);
        child->velocity.x        = -0x34000;
        child->velocity.y        = -0x80000;
        child->parent            = (Entity *)entity->parent;
        RSDK.PlaySfx(HeavyGunner->sfxThrow, false, 255);
    }

    if (entity->scale.x > 512) {
        entity->scale.x -= 4;
        entity->scale.y = entity->scale.x;
    }
}

void HeavyGunner_State3_Unknown4(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.ProcessAnimation(&entity->animator1);
    RSDK.ProcessAnimation(&entity->animator7);
    RSDK.ProcessAnimation(&entity->animator10);
    entity->position.x += Zone->autoScrollSpeed;
    entity->velocity.y += ((entity->parent->position.y - entity->position.y + 0x400000) >> 8) - (entity->velocity.y >> 4);
    entity->position.y += entity->velocity.y;

    if (entity->animator10.frameID >= entity->animator10.frameCount - 1) {
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 18, &entity->animator10, true, 0);
        entity->timer = 0;
        entity->state = HeavyGunner_State3_Unknown1;
    }
}

void HeavyGunner_State4_Unknown1(void)
{
    RSDK_THIS(HeavyGunner);
    EntityHeavyGunner *parent = (EntityHeavyGunner *)entity->parent;

    entity->velocity.y += 0x4000;
    entity->rotation = (entity->rotation - 16) & 0x1FF;
    entity->position.x += entity->velocity.x + Zone->autoScrollSpeed;
    entity->position.y += entity->velocity.y;

    if (entity->position.x < parent->position.x - 0x180000)
        entity->velocity.x = 0;
    if (entity->velocity.y > 0) {
        if (entity->position.y > parent->position.y - 0x200000) {
            RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 1, &parent->animator9, true, 0);
            RSDK.PlaySfx(Player->sfx_Grab, false, 255);
            entity->drawOrder = Zone->hudDrawOrder;
            entity->state     = HeavyGunner_State4_Unknown2;
            entity->timer     = 0x200;
            entity->stateDraw = HeavyGunner_StateDraw4_Unknown2;
        }
    }
}

void HeavyGunner_State4_Unknown2(void)
{
    RSDK_THIS(HeavyGunner);

    entity->timer -= 64;
    if (!entity->timer)
        destroyEntity(entity);
}

void HeavyGunner_State6_Unknown1(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.ProcessAnimation(&entity->animator1);
    entity->velocity.x -= 0x1800;
    entity->velocity.y += 0x3800;
    entity->position.x += entity->velocity.x + Zone->autoScrollSpeed;
    entity->position.y += entity->velocity.y;
    entity->scale.x += ((0x200 - entity->scale.x) >> 3);
    entity->scale.y = entity->scale.x;

    if (entity->rotation < 0)
        entity->rotation++;

    if (++entity->timer == 32) {
        entity->scale.x = 0x200;
        entity->timer   = entity->field_7C + 48;
        entity->scale.y = 0x200;
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 26, &entity->animator11, true, 0);
        entity->state = HeavyGunner_State6_Unknown2;
        RSDK.PlaySfx(HeavyGunner->sfxJet, false, 255);
    }
}

void HeavyGunner_State6_Unknown2(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.ProcessAnimation(&entity->animator1);
    RSDK.ProcessAnimation(&entity->animator11);

    if (entity->velocity.x < 0x80000)
        entity->velocity.x += 0x3800;
    entity->position.x += entity->velocity.x + Zone->autoScrollSpeed;
    if (entity->animator11.animationID == 26 && entity->animator11.frameID == entity->animator11.frameCount - 1)
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 25, &entity->animator11, true, 0);

    if (!(Zone->timer & 3)) {
        EntityExplosion *explosion = CREATE_ENTITY(Explosion, intToVoid(EXPLOSION_BOSSPUFF), entity->position.x - 0x180000, entity->position.y);
        explosion->drawOrder       = Zone->drawOrderLow - 1;
        explosion->drawFX          = FX_ROTATE;
        explosion->rotation        = 128;
    }
    if (--entity->timer <= 0) {
        entity->timer = 0;
        entity->state = HeavyGunner_State6_Unknown3;
    }
}

void HeavyGunner_State6_Unknown3(void)
{
    RSDK_THIS(HeavyGunner);

    entity->position.x += Zone->autoScrollSpeed;
    entity->position.y += 0x20000;

    if (RSDK.ObjectTileCollision(entity, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x100000, true)) {
        entity->direction = FLIP_X;
        if (entity->type == HEAVYGUNNER_MISSILE)
            entity->velocity.x = -0x10000;
        else
            entity->velocity.x = -0x20000;
        entity->state = HeavyGunner_State6_Unknown4;
    }
}

void HeavyGunner_State6_Unknown4(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.ProcessAnimation(&entity->animator1);
    RSDK.ProcessAnimation(&entity->animator11);
    entity->position.x += entity->velocity.x + Zone->autoScrollSpeed;
    RSDK.ObjectTileGrip(entity, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x100000, 64);

    Hitbox *hitbox = NULL;
    if (entity->type == HEAVYGUNNER_MISSILE)
        hitbox = &HeavyGunner->hitbox2;
    else
        hitbox = &HeavyGunner->hitbox1;

    foreach_active(Player, player)
    {
        if (Player_CheckBadnikTouch(player, entity, hitbox)) {
            if (Player_CheckAttacking(player, entity) && entity->type == HEAVYGUNNER_MISSILE) {
                if (player->onGround) {
                    player->groundVel = Zone->autoScrollSpeed - 0x20000;
                }
#if RETRO_USE_PLUS
                else if (player->state != Player_State_MightyHammerDrop) {
#endif
                    player->velocity.y = -player->velocity.y;
                    if (player->velocity.y > -0x40000)
                        player->velocity.y = -0x40000;
#if RETRO_USE_PLUS
                }
#endif
                entity->velocity.x = -0x10000;
                RSDK.SetSpriteAnimation(0xFFFF, 0, &entity->animator11, true, 0);
                entity->direction = FLIP_NONE;
                entity->rotation  = 256;
                entity->state     = HeavyGunner_State6_Unknown5;
                RSDK.PlaySfx(HeavyGunner->sfxHit, false, 255);
                RSDK.PlaySfx(HeavyGunner->sfxFlip, false, 255);
            }
            else {
                Player_CheckHit(player, entity);
                RSDK.PlaySfx(HeavyGunner->sfxExplosion3, false, 255);
                int y                = RSDK.Sin512(entity->rotation) << 11;
                int x                = RSDK.Cos512(entity->rotation) << 11;
                EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_LightningSpark, x + entity->position.x, y + entity->position.y);
                RSDK.SetSpriteAnimation(Explosion->aniFrames, EXPLOSION_BOSS, &debris->animator, true, 0);
                debris->velocity.x = entity->velocity.x + Zone->autoScrollSpeed;
                debris->drawOrder  = Zone->drawOrderLow;
                debris->timer      = 41;
                destroyEntity(entity);
            }
        }
    }
}

void HeavyGunner_State6_Unknown5(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.ProcessAnimation(&entity->animator1);
    entity->position.x += entity->velocity.x + Zone->autoScrollSpeed;
    entity->position.x += RSDK.Cos512(entity->rotation) << 8;
    entity->position.y += (RSDK.Sin512(entity->rotation) - 0x200) << 8;
    entity->rotation += 32;

    if (!(Zone->timer & 3)) {
        int x = RSDK.Cos512(entity->rotation) << 11;
        int y = RSDK.Sin512(entity->rotation) << 11;
        CREATE_ENTITY(Explosion, intToVoid(EXPLOSION_BOSSPUFF), entity->position.x - x, entity->position.y - y);
    }

    if (entity->rotation == 0x400) {
        EntityHeavyGunner *parent = (EntityHeavyGunner *)entity->parent;
        if (parent->animator9.animationID == 3) {
            EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_LightningSpark, entity->position.x, entity->position.y);
            RSDK.SetSpriteAnimation(Explosion->aniFrames, 2, &debris->animator, true, 0);
            debris->velocity.x = Zone->autoScrollSpeed;
            debris->drawOrder  = Zone->drawOrderHigh;
            debris->drawFX     = FX_SCALE;
            debris->scale.x    = 0x300;
            debris->scale.y    = 0x300;
            debris->timer      = 52;
            --parent->field_7C;
            destroyEntity(entity);
            RSDK.PlaySfx(HeavyGunner->sfxExplosion3, false, 255);
        }
        else {
            entity->rotation   = 0;
            entity->state      = HeavyGunner_State6_Unknown6;
            entity->velocity.x = 0;
            RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 3, &parent->animator9, true, 0);
            RSDK.PlaySfx(HeavyGunner->sfxJet, false, 255);
        }
    }
}

void HeavyGunner_State6_Unknown6(void)
{
    RSDK_THIS(HeavyGunner);
    EntityHeavyGunner *parent = (EntityHeavyGunner *)entity->parent;

    RSDK.ProcessAnimation(&entity->animator1);

    entity->position.y += entity->velocity.y;
    entity->position.x += entity->velocity.x + Zone->autoScrollSpeed;
    entity->velocity.y += ((parent->position.y - entity->position.y) >> 8) - (entity->velocity.y >> 4);
    if (entity->velocity.x >= -0x20000) {
        if (entity->velocity.x > 0x70000)
            entity->velocity.x = 0x70000;
    }
    else {
        entity->velocity.x = -0x20000;
    }

    if (entity->rotation <= -0x200 || entity->direction) {
        entity->rotation  = 0;
        entity->direction = FLIP_X;
        entity->velocity.x -= RSDK.Cos512(0) << 6;
        if (Zone->timer & 3) {
            if ((Zone->timer & 3) == 2) {
                int x                = 0x600 * RSDK.Cos512(entity->rotation);
                int y                = 0x600 * RSDK.Sin512(entity->rotation);
                EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_LightningSpark, entity->position.x - x, entity->position.y - y);
                RSDK.SetSpriteAnimation(Explosion->aniFrames, 2, &debris->animator, true, 0);
                debris->velocity.x = Zone->autoScrollSpeed;
                debris->drawOrder  = Zone->drawOrderHigh;
                debris->drawFX     = FX_SCALE;
                debris->scale.x    = RSDK.Rand(0x80, 0x180);
                debris->scale.y    = debris->scale.x;
                debris->timer      = 52;
            }
        }
        else {
            int x                = RSDK.Cos512(entity->rotation) << 11;
            int y                = RSDK.Sin512(entity->rotation) << 11;
            EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_LightningSpark, x + entity->position.x, y + entity->position.y);
            RSDK.SetSpriteAnimation(Explosion->aniFrames, EXPLOSION_BOSSPUFF, &debris->animator, true, 0);
            debris->velocity.x = (RSDK.Cos512(entity->rotation) << 8) + Zone->autoScrollSpeed;
            debris->velocity.y = 16 * RSDK.Cos512(entity->rotation);
            debris->drawOrder  = Zone->drawOrderLow;
            debris->timer      = 41;
        }

        if (entity->position.x - parent->position.x < 0xA00000) {
            if (parent->field_74 >= 3) {
                parent->velocity.y = 0;
                parent->timer      = 0;
                parent->state      = HeavyGunner_State2_Unknown7;
                entity->state      = HeavyGunner_State6_Unknown8;
            }
            else {
                parent->field_74++;
                entity->parent = (Entity *)RSDK_GET_ENTITY(RSDK.GetEntityID(parent) - parent->field_74, HeavyGunner);
                parent->state  = HeavyGunner_State2_Unknown6;
                entity->state  = HeavyGunner_State6_Unknown7;
                RSDK.PlaySfx(HeavyGunner->sfxWooshOut, false, 255);
            }
            entity->groundVel = 256;
        }
    }
    else {
        entity->rotation -= 4;
        entity->velocity.x += RSDK.Cos512(entity->rotation) << 6;

        if (Zone->timer & 3) {
            if ((Zone->timer & 3) == 2) {
                int x                = 0x600 * RSDK.Cos512(entity->rotation);
                int y                = 0x600 * RSDK.Sin512(entity->rotation);
                EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_LightningSpark, entity->position.x - x, entity->position.y - y);
                RSDK.SetSpriteAnimation(Explosion->aniFrames, EXPLOSION_BOSS, &debris->animator, true, 0);
                debris->velocity.x = Zone->autoScrollSpeed;
                debris->drawOrder  = Zone->drawOrderHigh;
                debris->drawFX     = FX_SCALE;
                debris->scale.x    = RSDK.Rand(0x80, 0x180);
                debris->scale.y    = debris->scale.x;
                debris->timer      = 52;
            }
        }
        else {
            int x                = RSDK.Cos512(entity->rotation) << 11;
            int y                = RSDK.Sin512(entity->rotation) << 11;
            EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_LightningSpark, entity->position.x - (x << 11), entity->position.y - y);
            RSDK.SetSpriteAnimation(Explosion->aniFrames, EXPLOSION_BOSSPUFF, &debris->animator, true, 0);
            debris->velocity.x = Zone->autoScrollSpeed - (RSDK.Cos512(entity->rotation) << 8);
            debris->velocity.y = -16 * RSDK.Cos512(entity->rotation);
            debris->drawOrder  = Zone->drawOrderLow;
            debris->timer      = 41;
        }
    }
}

void HeavyGunner_State6_Unknown7(void)
{
    RSDK_THIS(HeavyGunner);
    EntityHeavyGunner *parent = (EntityHeavyGunner *)entity->parent;

    RSDK.ProcessAnimation(&entity->animator1);
    int angle = RSDK.ATan2(entity->position.x - parent->position.x, entity->position.y - parent->position.y);
    entity->groundVel += 16;
    entity->rotation   = 2 * angle;
    entity->velocity.x = entity->groundVel * RSDK.Cos512(entity->rotation);
    entity->velocity.y = entity->groundVel * RSDK.Sin512(entity->rotation);
    entity->position.x += Zone->autoScrollSpeed - entity->velocity.x;
    entity->position.y -= entity->velocity.y;

    if (entity->scale.x >= parent->scale.x) {
        if (entity->scale.x > parent->scale.x)
            entity->scale.x -= 2;
    }
    else {
        entity->scale.x += 2;
    }

    entity->scale.y   = entity->scale.x;
    entity->drawOrder = parent->drawOrder;
    if (Zone->timer & 3) {
        if ((Zone->timer & 3) == 2) {
            int x                = 0x600 * RSDK.Cos512(entity->rotation);
            int y                = 0x600 * RSDK.Sin512(entity->rotation);
            EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_LightningSpark, entity->position.x - x, entity->position.y - y);
            RSDK.SetSpriteAnimation(Explosion->aniFrames, 2, &debris->animator, true, 0);
            debris->velocity.x = Zone->autoScrollSpeed;
            debris->drawOrder  = Zone->drawOrderHigh;
            debris->drawFX     = FX_SCALE;
            debris->scale.x    = RSDK.Rand(0x80, 0x180);
            debris->scale.y    = debris->scale.x;
            debris->timer      = 52;
        }
    }
    else {
        int x                = RSDK.Cos512(entity->rotation) << 11;
        int y                = RSDK.Sin512(entity->rotation) << 11;
        EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_LightningSpark, x + entity->position.x, y + entity->position.y);
        RSDK.SetSpriteAnimation(Explosion->aniFrames, EXPLOSION_BOSSPUFF, &debris->animator, true, 0);
        debris->velocity.x = (RSDK.Cos512(entity->rotation) << 8) + Zone->autoScrollSpeed;
        debris->velocity.y = 16 * RSDK.Cos512(entity->rotation);
        debris->drawOrder  = Zone->drawOrderLow;
        debris->timer      = 41;
    }

    int rx = (entity->position.x - parent->position.x) >> 16;
    int ry = (entity->position.y - parent->position.y) >> 16;
    if (rx * rx + ry * ry < 0x100) {
        for (int i = 0; i < 12; ++i) {
            int x                = entity->position.x + RSDK.Rand(-0xC0000, 0xC0000);
            int y                = entity->position.y + RSDK.Rand(-0xC0000, 0xC0000);
            EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_Fall, x, y);
            RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 28, &debris->animator, true, i >> 1);
            debris->velocity.x = Zone->autoScrollSpeed + RSDK.Rand(-0x20000, 0x20000);
            debris->velocity.y = RSDK.Rand(-0x20000, 0x20000) - 0x40000;
            debris->gravity    = 0x3800;
            debris->scale      = parent->scale;
            debris->rotSpeed   = RSDK.Rand(-24, 24);
            debris->drawOrder  = Zone->drawOrderLow;
            if (i >> 1 < 3)
                debris->drawFX = FX_SCALE | FX_ROTATE;
        }

        EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_LightningSpark, parent->position.x, parent->position.y);
        RSDK.SetSpriteAnimation(Explosion->aniFrames, EXPLOSION_ENEMY, &debris->animator, true, 0);
        debris->velocity.x    = Zone->autoScrollSpeed;
        debris->drawOrder     = Zone->drawOrderHigh;
        debris->timer         = 32;
        HeavyGunner->field_14 = false;

        int plane = RSDK.Rand(0, 2);
        foreach_active(Player, player) { player->collisionPlane = plane; }

        --((EntityHeavyGunner *)parent->parent)->field_7C;
        destroyEntity(entity);
        destroyEntity(parent);
        RSDK.PlaySfx(HeavyGunner->sfxDestroy, false, 255);
        RSDK.PlaySfx(HeavyGunner->sfxExplosion3, false, 255);
        HeavyGunner_Unknown5();
    }
}

void HeavyGunner_State6_Unknown8(void)
{
    RSDK_THIS(HeavyGunner);
    EntityHeavyGunner *parent = (EntityHeavyGunner *)entity->parent;

    RSDK.ProcessAnimation(&entity->animator1);

    int angle = RSDK.ATan2(entity->position.x - parent->position.x, entity->position.y - parent->position.y);
    entity->groundVel += 16;
    entity->rotation   = 2 * angle;
    entity->velocity.x = entity->groundVel * RSDK.Cos512(entity->rotation);
    entity->velocity.y = entity->groundVel * RSDK.Sin512(entity->rotation);
    entity->position.x += Zone->autoScrollSpeed - entity->velocity.x;
    entity->position.y -= entity->velocity.y;
    if (Zone->timer & 3) {
        if ((Zone->timer & 3) == 2) {
            int x                = 0x600 * RSDK.Cos512(entity->rotation);
            int y                = 0x600 * RSDK.Sin512(entity->rotation);
            EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_LightningSpark, entity->position.x - x, entity->position.y - y);
            RSDK.SetSpriteAnimation(Explosion->aniFrames, 2, &debris->animator, true, 0);
            debris->velocity.x = Zone->autoScrollSpeed;
            debris->drawOrder  = Zone->drawOrderHigh;
            debris->drawFX     = FX_SCALE;
            debris->scale.x    = RSDK.Rand(0x80, 0x180);
            debris->scale.y    = debris->scale.x;
            debris->timer      = 52;
        }
    }
    else {
        int x                = RSDK.Cos512(entity->rotation) << 11;
        int y                = RSDK.Sin512(entity->rotation) << 11;
        EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_LightningSpark, x + entity->position.x, y + entity->position.y);
        RSDK.SetSpriteAnimation(Explosion->aniFrames, EXPLOSION_BOSSPUFF, &debris->animator, true, 0);
        debris->velocity.x = (RSDK.Cos512(entity->rotation) << 8) + Zone->autoScrollSpeed;
        debris->velocity.y = 16 * RSDK.Cos512(entity->rotation);
        debris->drawOrder  = Zone->drawOrderLow;
        debris->timer      = 41;
    }

    int rx = (entity->position.x - parent->position.x) >> 16;
    int ry = (entity->position.y - parent->position.y) >> 16;
    if (rx * rx + ry * ry < 0x300) {
        for (int i = 0; i < 12; ++i) {
            int x                = entity->position.x + RSDK.Rand(0x60000, 0x180000);
            int y                = entity->position.y + RSDK.Rand(-0xC0000, 0xC0000);
            EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_Fall, x, y);
            RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 28, &debris->animator, true, i >> 1);
            debris->velocity.x = Zone->autoScrollSpeed + RSDK.Rand(-0x20000, 0x20000);
            debris->velocity.y = RSDK.Rand(-0x20000, 0x20000) - 0x40000;
            debris->gravity    = 0x3800;
            debris->rotSpeed   = RSDK.Rand(-24, 24);
            debris->drawOrder  = Zone->drawOrderHigh;
            if (i >> 1 < 3)
                debris->drawFX = FX_ROTATE;
        }

        EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_Fall, entity->position.x, entity->position.y - 0x200000);
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 13, &debris->animator, true, 0);
        debris->velocity.x = Zone->autoScrollSpeed + RSDK.Rand(-0x20000, 0x20000);
        debris->velocity.y = RSDK.Rand(-0x20000, 0x20000) - 0x40000;
        debris->gravity    = 0x3800;
        debris->drawOrder  = Zone->drawOrderHigh;

        debris = CREATE_ENTITY(Debris, Debris_State_LightningSpark, entity->position.x + 0x60000, entity->position.y - 0x40000);
        RSDK.SetSpriteAnimation(Explosion->aniFrames, EXPLOSION_BOSS, &debris->animator, true, 0);
        debris->velocity.x = Zone->autoScrollSpeed;
        debris->drawOrder  = Zone->drawOrderHigh;
        debris->drawFX     = FX_SCALE;
        debris->scale.x    = 768;
        debris->scale.y    = 768;
        debris->timer      = 52;

        destroyEntity(entity);
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 12, &parent->animator1, true, 0);
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 12, &parent->animator3, true, 1);
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 12, &parent->animator4, true, 2);
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 12, &parent->animator6, true, 3);
        RSDK.SetSpriteAnimation(0xFFFF, 0, &parent->animator2, true, 0);
        --parent->field_7C;
        parent->state               = HeavyGunner_State2_Unknown8;
        RSDK_sceneInfo->timeEnabled = false;
        Player_GiveScore(RSDK_GET_ENTITY(SLOT_PLAYER1, Player), 1000);
        RSDK.PlaySfx(HeavyGunner->sfxExplosion3, false, 255);
        entity->field_80 = 1;
        HeavyGunner_Unknown5();
    }
}

void HeavyGunner_State2_Unknown1(void)
{
    RSDK_THIS(HeavyGunner);

    if (RSDK_GET_ENTITY(SLOT_PLAYER1, Player)->position.x > entity->position.x) {
        entity->velocity.x = 0x40000;
        entity->velocity.y = 0;
        entity->drawFX     = FX_ROTATE;
        entity->state      = HeavyGunner_State2_Unknown2;
        Camera_ShakeScreen(12, 0, 12);
        RSDK.PlaySfx(HeavyGunner->sfxRumble, false, 255);
        RSDK.PlaySfx(HeavyGunner->sfxWooshIn, false, 255);
        Zone->deathBoundary[0] += 0x8000000;
    }
}

void HeavyGunner_State2_Unknown2(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.ProcessAnimation(&entity->animator1);
    RSDK.ProcessAnimation(&entity->animator2);
    RSDK.ProcessAnimation(&entity->animator3);
    RSDK.ProcessAnimation(&entity->animator4);
    RSDK.ProcessAnimation(&entity->animator5);
    RSDK.ProcessAnimation(&entity->animator6);
    RSDK.ProcessAnimation(&entity->animator7);
    RSDK.ProcessAnimation(&entity->animator8);
    RSDK.ProcessAnimation(&entity->animator9);

    entity->position.x += entity->velocity.x;
    entity->position.x += Zone->autoScrollSpeed;
    entity->position.y += 0x10000;
    entity->rotation = (entity->velocity.x + 0x40000) >> 13;
    if (entity->rotation < 0)
        entity->rotation = 0;
    RSDK.ObjectTileCollision(entity, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x800000, true);
    if (entity->position.x > Zone->screenBoundsL2[0] + ((RSDK_screens->centerX - 16) << 16))
        entity->state = HeavyGunner_State2_Unknown3;
}

void HeavyGunner_State2_Unknown3(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.ProcessAnimation(&entity->animator1);
    RSDK.ProcessAnimation(&entity->animator2);
    RSDK.ProcessAnimation(&entity->animator3);
    RSDK.ProcessAnimation(&entity->animator4);
    RSDK.ProcessAnimation(&entity->animator5);
    RSDK.ProcessAnimation(&entity->animator6);
    RSDK.ProcessAnimation(&entity->animator7);
    RSDK.ProcessAnimation(&entity->animator8);
    RSDK.ProcessAnimation(&entity->animator9);
    entity->velocity.x -= 0x1000;
    entity->position.x += entity->velocity.x;
    entity->position.x += Zone->autoScrollSpeed;
    entity->position.y += 0x10000;
    entity->rotation = (entity->velocity.x + 0x40000) >> 13;
    if (entity->rotation < 0)
        entity->rotation = 0;

    bool32 collided = RSDK.ObjectTileCollision(entity, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x800000, true);
    if (entity->velocity.x < 0 && collided) {
        if (entity->position.x < Zone->screenBoundsL2[0] + ((RSDK_screens->centerX - 16) << 16)) {
            entity->velocity.x = 0;
            entity->drawFX     = 0;
            if (entity->field_74 > 0)
                entity->timer = 240;
            entity->state = HeavyGunner_State2_Unknown4;
        }
    }
}

void HeavyGunner_State2_Unknown4(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.ProcessAnimation(&entity->animator1);
    RSDK.ProcessAnimation(&entity->animator2);
    RSDK.ProcessAnimation(&entity->animator3);
    RSDK.ProcessAnimation(&entity->animator4);
    RSDK.ProcessAnimation(&entity->animator5);
    RSDK.ProcessAnimation(&entity->animator6);
    RSDK.ProcessAnimation(&entity->animator7);
    RSDK.ProcessAnimation(&entity->animator8);
    RSDK.ProcessAnimation(&entity->animator9);
    entity->position.x += Zone->autoScrollSpeed;
    RSDK.ObjectTileGrip(entity, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x800000, 64);
    entity->position.y += RSDK.Sin256(entity->angle) << 11;
    entity->angle = (entity->angle + 3) & 0xFF;
    if (entity->animator9.animationID == 3)
        entity->timer = 0;

    if (++entity->timer == 320) {
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 2, &entity->animator9, true, 0);
        RSDK.PlaySfx(HeavyGunner->sfxCharge, false, 255);
    }

    if (entity->timer == 380) {
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 1, &entity->animator9, true, 0);
        entity->timer      = 0;
        entity->state      = HeavyGunner_State2_Unknown5;
        entity->drawFX     = FX_ROTATE;
        entity->velocity.x = -0x30000;

        int dudType              = RSDK.Rand(0, 3);
        EntityHeavyGunner *child = CREATE_ENTITY(HeavyGunner, intToVoid((dudType == 0) + HEAVYGUNNER_MISSILE_F), entity->position.x - 0x200000,
                                                 entity->position.y - 0x100000);
        child->velocity.y -= 0x10000;
        child->parent   = (Entity *)entity;
        child->field_7C = 0;
        ++entity->field_7C;

        child           = CREATE_ENTITY(HeavyGunner, intToVoid((dudType == 1) + HEAVYGUNNER_MISSILE_F), entity->position.x - 0x200000,
                              entity->position.y - 0x100000);
        child->parent   = (Entity *)entity;
        child->field_7C = 15 * ((dudType != 1) + 1);
        ++entity->field_7C;

        child = CREATE_ENTITY(HeavyGunner, intToVoid((dudType == 2) + HEAVYGUNNER_MISSILE_F), entity->position.x - 0x200000,
                              entity->position.y - 0x100000);
        child->velocity.y += 0x10000;
        child->parent   = (Entity *)entity;
        child->field_7C = 30 * ((dudType != 2) + 1);

        ++entity->field_7C;

        EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_LightningSpark, entity->position.x - 0x200000, entity->position.y - 0x140000);
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 27, &debris->animator, true, 0);
        debris->timer      = 22;
        debris->drawOrder  = Zone->drawOrderHigh;
        debris->velocity.x = Zone->autoScrollSpeed;
        RSDK.PlaySfx(HeavyGunner->sfxFire, false, 255);
    }
}

void HeavyGunner_State2_Unknown5(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.ProcessAnimation(&entity->animator1);
    RSDK.ProcessAnimation(&entity->animator2);
    RSDK.ProcessAnimation(&entity->animator3);
    RSDK.ProcessAnimation(&entity->animator4);
    RSDK.ProcessAnimation(&entity->animator5);
    RSDK.ProcessAnimation(&entity->animator6);
    RSDK.ProcessAnimation(&entity->animator7);
    RSDK.ProcessAnimation(&entity->animator8);
    RSDK.ProcessAnimation(&entity->animator9);

    entity->velocity.x += 0x1800;
    entity->position.x += entity->velocity.x + Zone->autoScrollSpeed;
    entity->rotation = entity->velocity.x >> 14;
    RSDK.ObjectTileGrip(entity, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x800000, 64);
    entity->position.y += RSDK.Sin256(entity->angle) << 11;
    entity->angle = (entity->angle + 3) & 0xFF;
    if (entity->rotation > 0)
        entity->rotation = 0;

    if (entity->position.x > Zone->screenBoundsL2[0] + ((RSDK_screens->centerX - 16) << 16)) {
        entity->velocity.x = 0;
        entity->drawFX     = 0;
        entity->state      = HeavyGunner_State2_Unknown4;
    }
}

void HeavyGunner_State2_Unknown6(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.ProcessAnimation(&entity->animator1);
    RSDK.ProcessAnimation(&entity->animator2);
    RSDK.ProcessAnimation(&entity->animator3);
    RSDK.ProcessAnimation(&entity->animator4);
    RSDK.ProcessAnimation(&entity->animator5);
    RSDK.ProcessAnimation(&entity->animator6);
    RSDK.ProcessAnimation(&entity->animator7);
    RSDK.ProcessAnimation(&entity->animator8);
    RSDK.ProcessAnimation(&entity->animator9);
    entity->position.x += entity->velocity.x + Zone->autoScrollSpeed;
    entity->position.y += entity->velocity.y;
    entity->velocity.y -= 0x4000;
}

void HeavyGunner_State2_Unknown7(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.ProcessAnimation(&entity->animator1);
    RSDK.ProcessAnimation(&entity->animator2);
    RSDK.ProcessAnimation(&entity->animator3);
    RSDK.ProcessAnimation(&entity->animator4);
    RSDK.ProcessAnimation(&entity->animator5);
    RSDK.ProcessAnimation(&entity->animator6);
    RSDK.ProcessAnimation(&entity->animator7);
    RSDK.ProcessAnimation(&entity->animator8);

    entity->position.x += entity->velocity.x + Zone->autoScrollSpeed;
    entity->velocity.x -= 0xE00;
    if (entity->velocity.y < 0x400)
        entity->velocity.y += 16;
    entity->position.y += entity->velocity.y * RSDK.Sin256(entity->angle);
    entity->angle = (entity->angle + 6) & 0xFF;

    if (!entity->timer) {
        entity->timer = 1;
        RSDK.SetSpriteAnimation(0xFFFF, 0, &entity->animator9, true, 0);
        CREATE_ENTITY(HeavyGunner, intToVoid(HEAVYGUNNER_ESCAPE_HBH), entity->position.x - 0x80000, entity->position.y - 0x20000);

        EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_Fall, entity->position.x - 0x300000, entity->position.y - 0x240000);
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 20, &debris->animator, true, 0);
        debris->velocity.y = -0x20000;
        debris->velocity.x = Zone->autoScrollSpeed - 0x20000;
        debris->gravity    = 0x3800;
        debris->drawFX     = FX_ROTATE;
        debris->rotSpeed   = -16;
        debris->drawOrder  = Zone->drawOrderHigh;
    }
}

void HeavyGunner_State2_Unknown8(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.ProcessAnimation(&entity->animator1);
    RSDK.ProcessAnimation(&entity->animator5);
    RSDK.ProcessAnimation(&entity->animator7);
    RSDK.ProcessAnimation(&entity->animator8);

    entity->position.x += entity->velocity.x + Zone->autoScrollSpeed;
    if (entity->velocity.x < 0x8000)
        entity->velocity.x += 0xE00;
    ++entity->timer;
    entity->position.y += entity->velocity.y * RSDK.Sin256(entity->angle);
    entity->angle = (entity->angle + 6) & 0xFF;

    if ((entity->timer & 7) == 2)
        RSDK.PlaySfx(HeavyGunner->sfxExplosion2, false, 255);

    if (!(entity->timer & 0x1F))
        RSDK.PlaySfx(HeavyGunner->sfxHit, false, 255);

    if (!(entity->timer & 3)) {
        int y = entity->position.y + RSDK.Rand(-0x300000, 0x300000);
        int x = entity->position.x + RSDK.Rand(-0x300000, 0x300000);

        EntityExplosion *explosion = CREATE_ENTITY(Explosion, intToVoid(RSDK.Rand(EXPLOSION_BOSS, EXPLOSION_BOSSPUFF + 1)), x, y);
        explosion->drawOrder       = Zone->drawOrderHigh;
        explosion->drawFX          = FX_SCALE;
        explosion->scale.x         = RSDK.Rand(0x100, 0x300);
        explosion->scale.y         = explosion->scale.x;
    }

    if (entity->timer > 60) {
        entity->timer = 0;

        EntityDebris *debris = CREATE_ENTITY(Debris, Debris_State_Fall, entity->position.x, entity->position.y);
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 11, &debris->animator, true, entity->animator7.frameID);
        debris->velocity.y = -0x10000;
        debris->velocity.x = Zone->autoScrollSpeed - 0x10000;
        debris->gravity    = 0x3800;
        debris->drawOrder  = Zone->drawOrderHigh;

        debris = CREATE_ENTITY(Debris, Debris_State_Fall, entity->position.x + 0x1B0000, entity->position.y);
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 11, &debris->animator, true, entity->animator8.frameID);
        debris->velocity.y = -0x10000;
        debris->velocity.x = Zone->autoScrollSpeed + 0x10000;
        debris->gravity    = 0x3800;
        debris->drawOrder  = Zone->drawOrderLow - 1;
        RSDK.SetSpriteAnimation(0xFFFF, 0, &entity->animator7, true, 0);
        RSDK.SetSpriteAnimation(0xFFFF, 0, &entity->animator8, true, 0);
        entity->state = HeavyGunner_State2_Unknown9;
    }
}

void HeavyGunner_State2_Unknown9(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.ProcessAnimation(&entity->animator1);
    RSDK.ProcessAnimation(&entity->animator5);
    entity->position.x += entity->velocity.x + Zone->autoScrollSpeed;
    entity->velocity.y += 0x3800;
    entity->position.y += entity->velocity.y;
    if (RSDK.ObjectTileCollision(entity, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x200000, true))
        entity->velocity.y = -(entity->velocity.y >> 1);

    if ((++entity->timer & 7) == 2)
        RSDK.PlaySfx(HeavyGunner->sfxExplosion2, false, 255);
    if (!(entity->timer & 0x1F))
        RSDK.PlaySfx(HeavyGunner->sfxHit, false, 255);

    if (!(entity->timer & 3)) {
        int x                      = entity->position.x + RSDK.Rand(-0x300000, 0x300000);
        int y                      = entity->position.y + RSDK.Rand(-0x300000, 0x300000);
        EntityExplosion *explosion = CREATE_ENTITY(Explosion, intToVoid(RSDK.Rand(EXPLOSION_BOSS, EXPLOSION_BOSSPUFF + 1)), x, y);
        explosion->drawOrder       = Zone->drawOrderHigh;
        explosion->drawFX          = FX_SCALE;
        explosion->scale.x         = RSDK.Rand(0x100, 0x200);
        explosion->scale.y         = explosion->scale.x;
    }

    if (entity->timer > 200) {
        entity->drawOrder = Zone->hudDrawOrder;
        entity->state     = HeavyGunner_State2_Unknown10;
        entity->timer     = 768;
        entity->stateDraw = HeavyGunner_StateDraw4_Unknown2;
        Music_TransitionTrack(TRACK_STAGE, 0.0125);
        RSDK.PlaySfx(HeavyGunner->sfxExplosion3, false, 255);

        EntityCamera *camera = RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);
        camera->position.x   = 0x57C00000;
        if (camera->position.y <= 0x6600000) {
            camera->position.y = 0x2EC0000;
            if (camera->position.y > 0x3800000)
                camera->position.y = 0x5EC0000;
        }
        else {
            camera->position.y = 0x8EC0000;
        }

        foreach_active(Player, player)
        {
            player->position.x = 0x57C00000;
            player->position.y = camera->position.y;
        }

        foreach_all(SignPost, signPost)
        {
            signPost->position.y    = camera->position.y;
            Zone->screenBoundsL1[0] = (signPost->position.x >> 16) - 512;
            RSDK.ObjectTileGrip(signPost, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x180000, 0x40);
        }

        Zone->autoScrollSpeed       = 0;
        Zone->screenBoundsR1[0]     = HeavyGunner->boundsR;
        Zone->playerBoundActiveR[0] = false;
        destroyEntitySlot(RSDK_sceneInfo->entitySlot - 4);
    }
}

void HeavyGunner_State7_Unknown1(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.ProcessAnimation(&entity->animator1);
    entity->position.x += entity->velocity.x + Zone->autoScrollSpeed;
    entity->velocity.y += 0x3800;
    entity->position.y += entity->velocity.y;

    if (++entity->timer == 30) {
        entity->timer = 0;
        RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 4, &entity->animator1, true, 0);
        entity->state = HeavyGunner_State7_Unknown2;
        RSDK.PlaySfx(HeavyGunner->sfxJet, false, 255);
    }
}

void HeavyGunner_State7_Unknown2(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.ProcessAnimation(&entity->animator1);
    entity->position.x += entity->velocity.x + Zone->autoScrollSpeed;
    entity->velocity.y -= 0x6000;
    entity->position.y += entity->velocity.y;
    if (++entity->timer == 60)
        destroyEntity(entity);
}

void HeavyGunner_State2_Unknown10(void)
{
    RSDK_THIS(HeavyGunner);

    entity->timer -= 8;
    if (!entity->timer)
        destroyEntity(entity);
}

#if RETRO_INCLUDE_EDITOR
void HeavyGunner_EditorDraw(void)
{
    RSDK_THIS(HeavyGunner);

    RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 6, &entity->animator1, true, 0);
    RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 14, &entity->animator2, true, 0);
    RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 7, &entity->animator3, true, 0);
    RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 8, &entity->animator4, true, 0);
    RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 9, &entity->animator5, true, 0);
    RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 10, &entity->animator6, true, 0);
    RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 11, &entity->animator7, true, 0);
    RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 11, &entity->animator8, true, 2);
    RSDK.SetSpriteAnimation(HeavyGunner->aniFrames, 0, &entity->animator9, true, 0);
    entity->drawFX   = FX_ROTATE;
    entity->rotation = 64;

    HeavyGunner_StateDraw_Unknown2();
}

void HeavyGunner_EditorLoad(void) { HeavyGunner->aniFrames = RSDK.LoadSpriteAnimation("SPZ1/Boss.bin", SCOPE_STAGE); }
#endif

void HeavyGunner_Serialize(void) {}
