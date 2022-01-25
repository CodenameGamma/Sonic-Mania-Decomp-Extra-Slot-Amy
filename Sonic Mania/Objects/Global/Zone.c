// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: Zone Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"
#include <time.h>

ObjectZone *Zone;

void Zone_Update(void) {}

void Zone_LateUpdate(void)
{
    RSDK_THIS(Zone);
    if (SceneInfo->entitySlot != SLOT_ZONE) {
        StateMachine_Run(self->state);
    }
    else {
        foreach_active(Player, player)
        {
            int32 playerID = 0;
            if (!player->sidekick) {
                playerID = RSDK.GetEntityID(player);
            }

            Hitbox *playerHitbox = Player_GetHitbox(player);
            if (Zone->playerBoundActiveL[playerID]) {
                int32 offset = -0x10000 * playerHitbox->left;
                if (player->position.x - offset <= Zone->playerBoundsL[playerID]) {
                    player->position.x = offset + Zone->playerBoundsL[playerID];
                    if (player->onGround) {
                        if (player->groundVel < Zone->autoScrollSpeed) {
                            player->velocity.x = Zone->autoScrollSpeed;
                            player->groundVel  = Zone->autoScrollSpeed;
                            player->pushing    = false;
                        }
                    }
                    else if (player->velocity.x < Zone->autoScrollSpeed) {
                        player->velocity.x = Zone->autoScrollSpeed;
                        player->groundVel  = 0;
                    }
                }
            }

            if (Zone->playerBoundActiveR[playerID]) {
                int32 offset = playerHitbox->right << 16;
                if (offset + player->position.x >= Zone->playerBoundsR[playerID]) {
                    player->position.x = Zone->playerBoundsR[playerID] - offset;
                    if (player->onGround) {
                        if (player->groundVel > Zone->autoScrollSpeed) {
                            player->velocity.x = Zone->autoScrollSpeed;
                            player->groundVel  = Zone->autoScrollSpeed;
                            player->pushing    = false;
                        }
                    }
                    else {
                        if (player->velocity.x > Zone->autoScrollSpeed) {
                            player->velocity.x = Zone->autoScrollSpeed;
                            player->groundVel  = 0;
                        }
                    }
                }
            }

            if (Zone->playerBoundActiveT[playerID]) {
                if (player->position.y - 0x140000 < Zone->playerBoundsT[playerID]) {
                    player->position.y = Zone->playerBoundsT[playerID] + 0x140000;
                    player->velocity.y = 0;
                }
            }

            if (player->state != Player_State_Die && !player->deathType) {
                if (Zone->playerBoundsB[playerID] <= Zone->deathBoundary[playerID]) {
                    if (player->position.y > Zone->deathBoundary[playerID]) {
                        player->deathType                   = PLAYER_DEATH_DIE_NOSFX;
                        Zone->playerBoundActiveB[playerID] = false;
                    }
                }
                else {
                    if (player->position.y > Zone->playerBoundsB[playerID]) {
                        player->deathType                   = PLAYER_DEATH_DIE_NOSFX;
                        Zone->playerBoundActiveB[playerID] = false;
                    }
                }
            }

            if (Zone->playerBoundActiveB[playerID]) {
                if (player->position.y + 0x140000 > Zone->playerBoundsB[playerID]) {
                    player->position.y = Zone->playerBoundsB[playerID] - 0x140000;
                    player->velocity.y = 0;
                    player->onGround   = true;
                }
            }
        }

        StateMachine_Run(self->state);

        if (SceneInfo->minutes == 10
#if RETRO_USE_PLUS
            && !(globals->medalMods & MEDAL_NOTIMEOVER)
#endif
        ) {
            SceneInfo->minutes      = 9;
            SceneInfo->seconds      = 59;
            SceneInfo->milliseconds = 99;
            SceneInfo->timeEnabled  = false;
            RSDK.PlaySfx(Player->sfxHurt, false, 0xFF);
            EntityCompetitionSession *session = (EntityCompetitionSession *)globals->competitionSession;
            foreach_active(Player, playerLoop)
            {
                bool32 flag = true;
#if RETRO_USE_PLUS
                if (globals->gameMode == MODE_COMPETITION && (session->finishFlags[playerLoop->playerID]) == FINISHFLAG_FINISHED)
                    flag = false;
#endif
                if (!playerLoop->sidekick && flag)
                    playerLoop->deathType = PLAYER_DEATH_DIE_USESFX;
            }
            Zone->gotTimeOver = true;
            StateMachine_Run(Zone->timeOverCallback);
        }

#if RETRO_USE_PLUS
        if (SceneInfo->minutes == 59 && SceneInfo->seconds == 59)
            ActClear->disableTimeBonus = true;
#endif

        if (Player->playerCount > 0) {
            EntityPlayer *sidekick = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
            if ((sidekick->state != Player_State_FlyIn && sidekick->state != Player_State_JumpIn) || sidekick->characterID == ID_TAILS
                || sidekick->scale.x == 0x200) {
                player = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
                RSDK.SwapDrawListEntries(player->drawOrder, SLOT_PLAYER1, SLOT_PLAYER2, Player->playerCount);
            }
        }
    }
}

void Zone_StaticUpdate(void)
{
    ++Zone->timer;
    Zone->timer &= 0x7FFF;
    ++Zone->timer2;
    Zone->timer2 &= 0x7FFF;
    if (!(Zone->timer & 1)) {
        ++Zone->ringFrame;
        Zone->ringFrame &= 0xF;
    }

#if RETRO_USE_PLUS
    int32 zone = Zone_GetZoneID();
    if (zone >= ZONE_AIZ) {
        zone = ZONE_AIZ;
    }
    else {
        if (zone == ZONE_INVALID)
            return;
    }

    int32 act = Zone->actID;
    if (act >= 3)
        act = 0;
    int32 pos = act + 2 * zone;
    if (pos >= 0 && SceneInfo->timeEnabled && globals->gameMode < MODE_TIMEATTACK)
        ++SaveGame->saveRAM->zoneTimes[pos];
#endif
}

void Zone_Draw(void)
{
    RSDK_THIS(Zone);
    if (self->screenID >= PLAYER_MAX || self->screenID == SceneInfo->currentScreenID) {
        StateMachine_Run(self->stateDraw);
    }
}

void Zone_Create(void *data)
{
    RSDK_THIS(Zone);
    self->active = ACTIVE_ALWAYS;
    if (!self->stateDraw) {
        self->visible   = false;
        self->drawOrder = -1;
    }
}

void Zone_StageLoad(void)
{
#if RETRO_USE_PLUS
    EntitySaveGame *saveRAM = SaveGame->saveRAM;
    Zone->randSeed           = (uint32)time(NULL);
    if (globals->gameMode == MODE_ENCORE) {
        if (globals->characterFlags == 0) {
            globals->characterFlags = 0;
            saveRAM->characterFlags = 0;
            uint8 id                = globals->playerID & 0xFF;
            if (globals->playerID & 0xFF) {
                int32 charID = -1;
                if (id) {
                    do {
                        id >>= 1;
                        ++charID;
                    } while (id > 0);
                }
                globals->characterFlags |= (1 << charID);
                saveRAM->characterFlags = globals->characterFlags;
            }

            if (globals->playerID & 0xFF00) {
                uint8 id     = globals->playerID >> 8;
                int32 charID = -1;
                if (globals->playerID & 0xFF) {
                    do {
                        id >>= 1;
                        ++charID;
                    } while (id > 0);
                }

                globals->characterFlags |= (1 << charID);
            }
            else {
                if (!globals->stock) {
                    if (globals->stock & 0xFF) {
                        int32 id     = globals->stock & 0xFF;
                        int32 charID = -1;
                        if (globals->stock & 0xFF) {
                            do {
                                id >>= 1;
                                ++charID;
                            } while (id > 0);
                        }

                        globals->characterFlags |= (1 << charID);
                        saveRAM->characterFlags = globals->characterFlags;
                    }

                    if (globals->stock & 0xFF00) {
                        uint8 id     = globals->playerID >> 8;
                        int32 charID = -1;
                        if (charID) {
                            do {
                                id >>= 1;
                                ++charID;
                            } while (id > 0);
                        }
                        globals->characterFlags |= (1 << charID);
                        saveRAM->characterFlags = globals->characterFlags;
                    }

                    if (globals->stock & 0xFF0000) {
                        int32 charID = -1;
                        uint8 id     = globals->playerID >> 16;
                        if (id) {
                            do {
                                id >>= 1;
                                ++charID;
                            } while (id > 0);
                        }
                        globals->characterFlags |= (1 << charID);
                        saveRAM->characterFlags = globals->characterFlags;
                    }
                    saveRAM->playerID = globals->playerID;
                }
                else {
                    globals->playerID |= (globals->stock & 0xFF);
                    globals->stock >>= 8;
                    saveRAM->stock = globals->stock;
                    uint8 id       = globals->playerID >> 8;
                    int32 charID   = -1;
                    if (id) {
                        do {
                            id >>= 1;
                            ++charID;
                        } while (id > 0);
                    }
                    globals->characterFlags |= (1 << charID);
                    saveRAM->characterFlags = globals->characterFlags;

                    if (globals->stock & 0xFF) {
                        int32 charID = -1;
                        id           = globals->stock & 0xFF;
                        if (globals->stock & 0xFF) {
                            do {
                                id >>= 1;
                                ++charID;
                            } while (id > 0);
                        }

                        globals->characterFlags |= (1 << charID);
                        saveRAM->characterFlags = globals->characterFlags;
                    }

                    if (globals->stock & 0xFF00) {
                        uint8 id     = globals->playerID >> 8;
                        int32 charID = -1;
                        if (charID) {
                            do {
                                id >>= 1;
                                ++charID;
                            } while (id > 0);
                        }
                        globals->characterFlags |= (1 << charID);
                        saveRAM->characterFlags = globals->characterFlags;
                    }

                    if (globals->stock & 0xFF0000) {
                        int32 charID = -1;
                        uint8 id     = globals->playerID >> 16;
                        if (id) {
                            do {
                                id >>= 1;
                                ++charID;
                            } while (id > 0);
                        }
                        globals->characterFlags |= (1 << charID);
                        saveRAM->characterFlags = globals->characterFlags;
                    }
                    saveRAM->playerID = globals->playerID;
                }
            }
        }

        if (!TitleCard || TitleCard->suppressCB != Zone_TitleCard_SupressCB) {
            globals->characterFlags = saveRAM->characterFlags;
            globals->stock          = saveRAM->stock;
            globals->playerID       = saveRAM->playerID;
        }
    }
#endif

    Zone->timer           = 0;
    Zone->autoScrollSpeed = 0;
    Zone->ringFrame       = 0;
    Zone->gotTimeOver     = false;
    Zone->vsSwapCBCount   = 0;
    Zone->fgLayerLow      = 0;
    Zone->drawOrderLow    = 2;
    Zone->playerDrawLow   = 4;
    Zone->fgLayerHigh     = 6;
    Zone->drawOrderHigh   = 8;
    Zone->playerDrawHigh  = 12;
    Zone->hudDrawOrder    = 14;

    Zone->fgLow     = RSDK.GetSceneLayerID("FG Low");
    Zone->fgHigh    = RSDK.GetSceneLayerID("FG High");
    Zone->moveLayer = RSDK.GetSceneLayerID("Move");
#if RETRO_USE_PLUS
    Zone->scratchLayer = RSDK.GetSceneLayerID("Scratch");
#endif

    if (Zone->fgLowID)
        Zone->fgLowID = 1 << Zone->fgLowID;

    if (Zone->fgHigh)
        Zone->fgHighID = 1 << Zone->fgHigh;

    if (Zone->moveLayer)
        Zone->moveID = 1 << Zone->moveLayer;

    Zone->fgLayers = 1 << Zone->fgLow;
    Zone->fgLayers |= 1 << Zone->fgHigh;
    Vector2 layerSize;
    RSDK.GetLayerSize(Zone->fgLow, &layerSize, true);

#if RETRO_USE_PLUS
    if (!Zone->swapGameMode) {
#endif
        for (int32 s = 0; s < PLAYER_MAX; ++s) {
            Zone->cameraBoundsL[s] = 0;
            Zone->cameraBoundsR[s] = layerSize.x;
            Zone->cameraBoundsT[s] = 0;
            Zone->cameraBoundsB[s] = layerSize.y;

            Zone->playerBoundsL[s] = Zone->cameraBoundsL[s] << 0x10;
            Zone->playerBoundsR[s] = Zone->cameraBoundsR[s] << 0x10;
            Zone->playerBoundsT[s] = Zone->cameraBoundsT[s] << 0x10;
            Zone->playerBoundsB[s] = Zone->cameraBoundsB[s] << 0x10;

            Zone->deathBoundary[s]      = Zone->cameraBoundsB[s] << 0x10;
            Zone->playerBoundActiveL[s] = true;
            Zone->playerBoundActiveB[s] = false;
        }
#if RETRO_USE_PLUS
    }
#endif

    if (!globals->initCoolBonus) {
        globals->coolBonus[0]  = 10000;
        globals->coolBonus[1]  = 10000;
        globals->coolBonus[2]  = 10000;
        globals->coolBonus[3]  = 10000;
        globals->initCoolBonus = true;
    }

    foreach_all(Zone, entity) { destroyEntity(entity); }

    RSDK.ResetEntitySlot(SLOT_ZONE, Zone->objectID, NULL);
    EntityCompetitionSession *session = (EntityCompetitionSession *)globals->competitionSession;
    if (globals->gameMode == MODE_COMPETITION) {
        if (RSDK.CheckStageFolder("Puyo")) {
            if (globals->gameMode == MODE_COMPETITION) {
                RSDK.SetSettingsValue(SETTINGS_SCREENCOUNT, 1);
            }
            else {
#if RETRO_USE_PLUS
                Competition_ResetOptions();
#else
                CompetitionSession_ResetOptions();
#endif
                RSDK.SetSettingsValue(SETTINGS_SCREENCOUNT, 1);
            }
        }
        else {
            session->playerCount = clampVal(session->playerCount, 2, PLAYER_MAX);
            RSDK.SetSettingsValue(SETTINGS_SCREENCOUNT, session->playerCount);
        }
    }
    else {
#if RETRO_USE_PLUS
        Competition_ResetOptions();
#else
        CompetitionSession_ResetOptions();
#endif
        RSDK.SetSettingsValue(SETTINGS_SCREENCOUNT, 1);
    }

    TextInfo textInfo;
    switch (globals->gameMode) {
#if !RETRO_USE_PLUS
        case MODE_NOSAVE:
#endif
        case MODE_MANIA:
            Localization_GetString(&textInfo, STR_RPC_MANIA);
            API_SetRichPresence(PRESENCE_MANIA, &textInfo);
            break;
#if RETRO_USE_PLUS
        case MODE_ENCORE:
            Localization_GetString(&textInfo, STR_RPC_ENCORE);
            API_SetRichPresence(PRESENCE_ENCORE, &textInfo);
            break;
#endif
        case MODE_TIMEATTACK:
            Localization_GetString(&textInfo, STR_RPC_TA);
            API_SetRichPresence(PRESENCE_TA, &textInfo);
            break;
        case MODE_COMPETITION:
            Localization_GetString(&textInfo, STR_RPC_COMP);
            API_SetRichPresence(PRESENCE_COMP, &textInfo);
            break;
        default: break;
    }
    Zone->sfxFail = RSDK.GetSfx("Stage/Fail.wav");
}

int32 Zone_GetZoneID(void)
{
    if (RSDK.CheckStageFolder("GHZ"))
        return ZONE_GHZ;
    if (RSDK.CheckStageFolder("CPZ"))
        return ZONE_CPZ;
    if (RSDK.CheckStageFolder("SPZ1") || RSDK.CheckStageFolder("SPZ2"))
        return ZONE_SPZ;
    if (RSDK.CheckStageFolder("FBZ"))
        return ZONE_FBZ;
    if (RSDK.CheckStageFolder("PSZ1") || RSDK.CheckStageFolder("PSZ2"))
        return ZONE_PGZ;
    if (RSDK.CheckStageFolder("SSZ1") || RSDK.CheckStageFolder("SSZ2"))
        return ZONE_SSZ;
    if (RSDK.CheckStageFolder("HCZ"))
        return ZONE_HCZ;
    if (RSDK.CheckStageFolder("MSZ"))
        return ZONE_MSZ;
    if (RSDK.CheckStageFolder("OOZ1") || RSDK.CheckStageFolder("OOZ2"))
        return ZONE_OOZ;
    if (RSDK.CheckStageFolder("LRZ1") || RSDK.CheckStageFolder("LRZ2") || RSDK.CheckStageFolder("LRZ3"))
        return ZONE_LRZ;
    if (RSDK.CheckStageFolder("MMZ"))
        return ZONE_MMZ;
    if (RSDK.CheckStageFolder("TMZ1") || RSDK.CheckStageFolder("TMZ2") || RSDK.CheckStageFolder("TMZ3"))
        return ZONE_TMZ;
    if (RSDK.CheckStageFolder("ERZ"))
        return ZONE_ERZ;
#if RETRO_USE_PLUS
    if (RSDK.CheckStageFolder("AIZ") && globals->gameMode == MODE_ENCORE)
        return ZONE_AIZ;
#endif
    return ZONE_INVALID;
}

void Zone_StoreEntities(int32 xOffset, int32 yOffset)
{
    //"Normalize" the positions of players, signposts & itemboxes when we store them
    // (this is important for later)

    int32 count = 0;
    int32 pos   = 0;
    foreach_active(Player, player)
    {
        player->position.x -= xOffset;
        player->position.y -= yOffset;
        globals->atlEntitySlot[count] = RSDK.GetEntityID(player);
        RSDK.CopyEntity(&globals->atlEntityData[pos], player, false);
        count++;
        pos += 0x200;
    }

    foreach_active(SignPost, signPost)
    {
        signPost->position.x -= xOffset;
        signPost->position.y -= yOffset;
        globals->atlEntitySlot[count] = RSDK.GetEntityID(signPost);
        RSDK.CopyEntity(&globals->atlEntityData[pos], signPost, false);
        count++;
        pos += 0x200;
    }

    foreach_active(ItemBox, itemBox)
    {
        itemBox->position.x -= xOffset;
        itemBox->position.y -= yOffset;
        globals->atlEntitySlot[count] = RSDK.GetEntityID(itemBox);
        RSDK.CopyEntity(&globals->atlEntityData[pos], itemBox, false);
        count++;
        pos += 0x200;
    }

    // store any relevant info about the player
    EntityPlayer *player1    = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    globals->restartLives[0] = player1->lives;
    globals->restartScore    = player1->score;
    globals->restartPowerups = player1->shield;
    globals->atlEntityCount  = count;
    globals->atlEnabled      = true;
}

void Zone_ReloadStoredEntities(int32 xOffset, int32 yOffset, bool32 setATLBounds)
{
    //reload any stored entities we have
    for (int32 e = 0; e < globals->atlEntityCount; ++e) {
        Entity *entityData = (Entity *)&globals->atlEntityData[e << 9];
        Entity *entity = NULL;

        //only players & powerups get to be overridden, everything else is just added to the temp area
        if (globals->atlEntitySlot[e] >= SLOT_ZONE)
            entity = RSDK.CreateEntity(TYPE_BLANK, NULL, 0, 0);
        else
            entity = RSDK_GET_ENTITY(globals->atlEntitySlot[e], );
        if (entityData->objectID == Player->objectID) {
            EntityPlayer *playerData = (EntityPlayer *)entityData;
            EntityPlayer *player     = (EntityPlayer *)entity;
            player->shield           = playerData->shield;
            if (player->shield && player->superState != SUPERSTATE_SUPER && player->invincibleTimer <= 0) {
                EntityShield *shield = RSDK_GET_ENTITY(Player->playerCount + RSDK.GetEntityID(player), Shield);
                RSDK.ResetEntityPtr(shield, Shield->objectID, player);
            }
        }
        else {
            RSDK.CopyEntity(entity, entityData, false);
        }
        entity->position.x = xOffset + entityData->position.x;
        entity->position.y = yOffset + entityData->position.y;
    }

    //clear ATL data, we dont wanna do it again
    memset(globals->atlEntityData, 0, globals->atlEntityCount << 9);

    //if we've set the flag, update our camera to use ATL bounds instead of the default ones
    Zone->setATLBounds = setATLBounds;
    if (setATLBounds) {
        EntityPlayer *player   = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
        player->camera         = NULL;
        EntityCamera *camera   = RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);
        camera->position.x     = xOffset;
        camera->position.y     = yOffset;
        camera->state          = 0;
        camera->targetPtr      = NULL;
        camera->boundsL        = (xOffset >> 16) - ScreenInfo->centerX;
        camera->boundsR        = (xOffset >> 16) + ScreenInfo->centerX;
        camera->boundsT        = (yOffset >> 16) - ScreenInfo->height;
        camera->boundsB        = yOffset >> 16;
        Camera->centerBounds.x = 0x80000;
        Camera->centerBounds.y = 0x40000;
    }

    Player->savedLives      = globals->restartLives[0];
    Player->savedScore      = globals->restartScore;
    Player->powerups        = globals->restartPowerups;
    globals->atlEntityCount = 0;
}

void Zone_StartFadeOut(int32 fadeSpeed, int32 fadeColour)
{
    EntityZone *zone = RSDK_GET_ENTITY(SLOT_ZONE, Zone);
    zone->fadeColour = fadeColour;
    zone->fadeSpeed  = fadeSpeed;
    zone->screenID   = PLAYER_MAX;
    zone->timer      = 0;
    zone->state      = Zone_State_Fadeout;
    zone->stateDraw  = Zone_Draw_Fade;
    zone->visible    = true;
    zone->drawOrder  = DRAWLAYER_COUNT - 1;
}

void Zone_StartFadeIn(int32 fadeSpeed, int32 fadeColour)
{
    EntityZone *zone = CREATE_ENTITY(Zone, NULL, 0, 0);
    zone->fadeColour = fadeColour;
    zone->fadeSpeed  = fadeSpeed;
    zone->screenID   = PLAYER_MAX;
    zone->timer      = 640;
    zone->state      = Zone_State_FadeIn;
    zone->stateDraw  = Zone_Draw_Fade;
    zone->visible    = true;
    zone->drawOrder  = DRAWLAYER_COUNT - 1;
}

void Zone_StartFadeOut_MusicFade(int32 fadeSpeed, int32 fadeColour)
{
    EntityZone *zone = RSDK_GET_ENTITY(SLOT_ZONE, Zone);
    zone->fadeColour = fadeColour;
    zone->fadeSpeed  = fadeSpeed;
    zone->screenID   = PLAYER_MAX;
    zone->timer      = 0;
    zone->state      = Zone_State_Fadeout;
    zone->stateDraw  = Zone_Draw_Fade;
    zone->visible    = true;
    zone->drawOrder  = DRAWLAYER_COUNT - 1;
    Music_FadeOut(0.025);
}

void Zone_StartFadeOut_Competition(int32 fadeSpeed, int32 fadeColour)
{
    EntityZone *zone = RSDK_GET_ENTITY(SLOT_ZONE, Zone);
    zone->fadeColour = fadeColour;
    zone->fadeSpeed  = fadeSpeed;
    zone->screenID   = PLAYER_MAX;
    zone->timer      = 0;
    zone->state      = Zone_State_Fadeout_Competition;
    zone->stateDraw  = Zone_Draw_Fade;
    zone->visible    = true;
    zone->drawOrder  = DRAWLAYER_COUNT - 1;
    Music_FadeOut(0.025);
}

void Zone_RotateOnPivot(Vector2 *position, Vector2 *pivotPos, int32 angle)
{
    int32 x = (position->x - pivotPos->x) >> 8;
    int32 y = (position->y - pivotPos->y) >> 8;
    position->x  = (y * RSDK.Sin256(angle)) + x * RSDK.Cos256(angle) + pivotPos->x;
    position->y  = (y * RSDK.Cos256(angle)) - x * RSDK.Sin256(angle) + pivotPos->y;
}

void Zone_ReloadScene(int32 screen)
{
    EntityZone *entity = CREATE_ENTITY(Zone, NULL, 0, 0);
    entity->screenID   = screen;
    entity->timer      = 640;
    entity->fadeSpeed  = 16;
    entity->fadeColour = 0xF0F0F0;
#if RETRO_USE_PLUS
    if (globals->gameMode != MODE_ENCORE || EncoreIntro) {
#endif
        entity->state     = Zone_State_Fadeout_Destroy;
        entity->stateDraw = Zone_Draw_Fade;
        entity->visible   = true;
        entity->drawOrder = DRAWLAYER_COUNT - 1;
#if RETRO_USE_PLUS
    }
    else {
        entity->state     = Zone_State_ReloadScene;
        entity->stateDraw = Zone_Draw_Fade;
        entity->visible   = true;
        entity->drawOrder = DRAWLAYER_COUNT - 1;
    }
#endif
}

void Zone_StartTeleportAction(void)
{
    EntityZone *entity = CREATE_ENTITY(Zone, NULL, 0, 0);
    entity->fadeColour = 0xF0F0F0;
    entity->timer      = 640;
    entity->screenID   = PLAYER_MAX;
    entity->fadeSpeed  = 16;
    entity->state      = Zone_State_SwapPlayers;
    entity->stateDraw  = Zone_Draw_Fade;
    entity->visible    = true;
    entity->drawOrder  = DRAWLAYER_COUNT - 1;
#if RETRO_USE_PLUS
    Zone->teleportActionActive = true;
#endif
}

void Zone_ApplyWorldBounds(void)
{
    if (Zone->setATLBounds) {
        EntityCamera *camera = RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);
        foreach_active(Player, player)
        {
            int32 camWorldL = camera->boundsL << 16;
            if (player->position.x - 0xA0000 <= camWorldL) {
                player->position.x = camWorldL + 0xA0000;
                if (player->onGround) {
                    if (player->groundVel < 0) {
                        player->velocity.x = 0;
                        player->groundVel  = 0;
                        player->pushing    = false;
                    }
                }
                else if (player->velocity.x < 0) {
                    player->velocity.x = 0;
                    player->groundVel  = 0;
                }
            }

            int32 camWorldR = camera->boundsR << 16;
            if (player->position.x + 0xA0000 >= camWorldR) {
                player->position.x = camWorldR - 0xA0000;
                if (player->onGround) {
                    if (player->groundVel > 0) {
                        player->velocity.x = 0;
                        player->groundVel  = 0;
                        player->pushing    = false;
                    }
                }
                else if (player->velocity.x > 0) {
                    player->velocity.x = 0;
                    player->groundVel  = 0;
                }
            }
        }
    }
}

//Generally, this is just "isAct2", however stuff like LRZ3, SSZ boss, TMZ3 & ERZ's cases prove thats not always the case
bool32 Zone_IsZoneLastAct(void)
{
    if ((RSDK.CheckStageFolder("GHZ") && Zone->actID == 1) || (RSDK.CheckStageFolder("CPZ") && Zone->actID == 1) || RSDK.CheckStageFolder("SPZ2")
        || (RSDK.CheckStageFolder("FBZ") && Zone->actID == 1) || RSDK.CheckStageFolder("PSZ2")) {
        return true;
    }
    if (RSDK.CheckStageFolder("SSZ2")) {
        if (RSDK.GetSceneLayerID("Tower") < LAYER_COUNT)
            return true;
    }
    else if ((RSDK.CheckStageFolder("HCZ") && Zone->actID == 1) || (RSDK.CheckStageFolder("MSZ") && Zone->actID == 1) || RSDK.CheckStageFolder("OOZ2")
             || RSDK.CheckStageFolder("LRZ3") || (RSDK.CheckStageFolder("MMZ") && Zone->actID == 1) || RSDK.CheckStageFolder("TMZ3")
             || RSDK.CheckStageFolder("ERZ")) {
        return true;
    }
    return false;
}

int32 Zone_GetEncoreStageID(void)
{
    int32 pos = SceneInfo->listPos;
    RSDK.SetScene("Mania Mode", "");
    int32 offset = pos - SceneInfo->listPos;
    RSDK.SetScene("Encore Mode", "");
    int32 eOff = SceneInfo->listPos;

    int32 listPos = SceneInfo->listPos;

    int32 pos2 = 0;
    if (offset >= 15) {
        if (offset == 15 || offset == 16) {
            pos2 = listPos + 15;
        }
        else {
            --listPos;
            pos2 = offset + listPos;
        }
    }
    else {
        pos2 = offset + listPos;
    }
    SceneInfo->listPos = pos;
    LogHelpers_Print("Mania Mode offset %d, pos %d -> Encore Mode offset %d, pos %d", offset, pos, pos2 - eOff, pos2);
    return pos2;
}
int32 Zone_GetManiaStageID(void)
{
    int32 pos = SceneInfo->listPos;
    RSDK.SetScene("Encore Mode", "");
    int32 offset = pos - SceneInfo->listPos;
    RSDK.SetScene("Mania Mode", "");
    int32 mOff = SceneInfo->listPos;

    int32 pos2 = 0;
    if (offset >= 15) {
        if (offset == 15) {
            if (checkPlayerID(ID_KNUCKLES, 1))
                pos2 = mOff + 16;
            else
                pos2 = mOff + 15;
        }
        else {
            pos2 = offset + mOff + 1;
        }
    }
    else {
        pos2 = offset + mOff;
    }
    SceneInfo->listPos = pos;
    LogHelpers_Print("Encore Mode offset %d, pos %d -> Mania Mode offset %d, pos %d", offset, pos, pos2 - mOff, pos2);
    return pos2;
}

void Zone_Draw_Fade(void)
{
    RSDK_THIS(Zone);
    RSDK.FillScreen(self->fadeColour, self->timer, self->timer - 0x80, self->timer - 0x100);
}

void Zone_State_Fadeout(void)
{
    RSDK_THIS(Zone);
    self->timer += self->fadeSpeed;
    if (self->timer > 0x400) {
#if RETRO_USE_PLUS
        if (Zone->swapGameMode) {
            if (SceneInfo->filter == (FILTER_BOTH | FILTER_MANIA)) {
                if (RSDK.CheckValidScene())
                    SceneInfo->listPos = Zone_GetEncoreStageID();
                globals->gameMode = MODE_ENCORE;
            }
            else if (SceneInfo->filter == (FILTER_BOTH | FILTER_ENCORE)) {
                if (RSDK.CheckValidScene())
                    SceneInfo->listPos = Zone_GetManiaStageID();
                globals->gameMode = MODE_MANIA;
            }
            SceneInfo->filter ^= 6;
            globals->enableIntro         = true;
            globals->suppressAutoMusic   = true;
            globals->suppressTitlecard   = true;
            globals->restartMilliseconds = SceneInfo->milliseconds;
            globals->restartSeconds      = SceneInfo->seconds;
            globals->restartMinutes      = SceneInfo->minutes;
            EntityPlayer *player         = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
            RSDK.CopyEntity(Zone->entityData, player, false);
            if (player->camera)
                RSDK.CopyEntity(Zone->entityData[8], player->camera, false);
        }
#endif
        RSDK.LoadScene();
    }
}

void Zone_State_FadeIn(void)
{
    RSDK_THIS(Zone);
    SceneInfo->timeEnabled = true;
    if (self->timer <= 0) {
        globals->suppressAutoMusic = false;
        globals->suppressTitlecard = false;
        destroyEntity(self);
    }
    else {
        self->timer -= self->fadeSpeed;
    }
}

void Zone_State_Fadeout_Competition(void)
{
    RSDK_THIS(Zone);
    self->timer += self->fadeSpeed;
    EntityCompetitionSession *session = (EntityCompetitionSession *)globals->competitionSession;
    if (self->timer > 0x400) {
        session->zoneFlags[session->levelIndex] = 1;
#if RETRO_USE_PLUS
        session->matchID = session->prevMatchID + 1;
#else
        session->matchID++;
#endif
        RSDK.SetScene("Presentation", "Menu");
        RSDK.SetSettingsValue(SETTINGS_SCREENCOUNT, 1);
        RSDK.LoadScene();
    }
}

void Zone_TitleCard_SupressCB(void)
{
    RSDK_THIS(Zone);
    SceneInfo->timeEnabled = true;
    SaveGame_LoadPlayerState();
    if (Music->activeTrack != Music->restartTrackID)
        Music_TransitionTrack(Music->restartTrackID, 0.04);
    EntityZone *zone           = CREATE_ENTITY(Zone, NULL, 0, 0);
    zone->screenID             = 0;
    zone->timer                = 640;
    zone->fadeSpeed            = 16;
    zone->fadeColour           = 0xF0F0F0;
    zone->state                = Zone_State_Fadeout_Destroy;
    zone->stateDraw            = Zone_Draw_Fade;
    zone->visible              = true;
    zone->drawOrder            = 15;
    globals->suppressTitlecard = false;
    TitleCard->suppressCB      = StateMachine_None;
    Player->rings              = 0;
    destroyEntity(self);
}

void Zone_State_ReloadScene(void)
{
    EntityPlayer *player1       = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    StarPost->storedMinutes    = SceneInfo->minutes;
    StarPost->storedSeconds    = SceneInfo->seconds;
    StarPost->storedMS         = SceneInfo->milliseconds;
    globals->suppressAutoMusic = true;
    globals->suppressTitlecard = true;
    TitleCard->suppressCB      = Zone_TitleCard_SupressCB;
    SaveGame_SavePlayerState();
    Player->rings = player1->rings;
    RSDK.LoadScene();
}

void Zone_State_Fadeout_Destroy(void)
{
    RSDK_THIS(Zone);
    if (self->timer <= 0)
        destroyEntity(self);
    else
        self->timer -= self->fadeSpeed;
}

void Zone_HandlePlayerSwap(void)
{
    int32 playerBoundActiveB[4];
    int32 playerBoundActiveT[4];
    int32 playerBoundActiveR[4];
    int32 playerBoundActiveL[4];
    int32 deathBounds[4];
    int32 playerBoundsB[4];
    int32 playerBoundsT[4];
    int32 playerBoundsR[4];
    int32 playerBoundsL[4];
    int32 cameraBoundsB[4];
    int32 cameraBoundsT[4];
    int32 cameraBoundsR[4];
    int32 cameraBoundsL[4];
    int32 layerIDs[LAYER_COUNT];

#if RETRO_USE_PLUS
    for (int32 p = 0; p < Player->playerCount; ++p) {
        EntityPlayer *player = RSDK_GET_ENTITY(Zone->preSwapPlayerIDs[p], Player);
        RSDK.CopyEntity(&Zone->entityData[p], player, false);

        cameraBoundsL[p]     = Zone->cameraBoundsL[p];
        cameraBoundsR[p]     = Zone->cameraBoundsR[p];
        cameraBoundsT[p]     = Zone->cameraBoundsT[p];
        cameraBoundsB[p]     = Zone->cameraBoundsB[p];
        playerBoundsL[p]     = Zone->playerBoundsL[p];
        playerBoundsR[p]     = Zone->playerBoundsR[p];
        playerBoundsT[p]     = Zone->playerBoundsT[p];
        playerBoundsB[p]     = Zone->playerBoundsB[p];
        deathBounds[p]        = Zone->deathBoundary[p];
        playerBoundActiveL[p] = Zone->playerBoundActiveL[p];
        playerBoundActiveR[p] = Zone->playerBoundActiveR[p];
        playerBoundActiveT[p] = Zone->playerBoundActiveT[p];
        playerBoundActiveB[p] = Zone->playerBoundActiveB[p];

        uint8 *layerPlanes = (uint8 *)&layerIDs[2 * p];
        for (int32 l = 0; l < LAYER_COUNT; ++l) {
            TileLayer *layer = RSDK.GetSceneLayer(l);
            if (layer)
                layerPlanes[l] = layer->drawLayer[Zone->preSwapPlayerIDs[p]];
            else
                layerPlanes[l] = DRAWLAYER_COUNT;
        }

        EntityCamera *camera = player->camera;
        RSDK.CopyEntity(&Zone->entityData[8 + p], camera, false);
        Zone->screenPosX[p] = ScreenInfo[camera->screenID].position.x;
        Zone->screenPosY[p] = ScreenInfo[camera->screenID].position.y;

        RSDK.CopyEntity(&Zone->entityData[4 + p], RSDK_GET_ENTITY(Player->playerCount + Zone->preSwapPlayerIDs[p], ), false);
        RSDK.CopyEntity(&Zone->entityData[12 + p], RSDK_GET_ENTITY((2 * Player->playerCount) + Zone->preSwapPlayerIDs[p], ), false);
    }

    for (int32 p = 0; p < Player->playerCount; ++p) {
        EntityPlayer *playerPtr = (EntityPlayer *)RSDK.GetEntityByID(Zone->swappedPlayerIDs[p]);
        EntityPlayer *player    = (EntityPlayer *)Zone->entityData[p];
        void *state             = player->state;

        if (state == Player_State_Ground || state == Player_State_Air || state == Player_State_Roll || state == Player_State_ForceRoll_Ground
            || state == Player_State_ForceRoll_Air) {
            playerPtr->state           = state;
            playerPtr->nextAirState    = player->nextAirState;
            playerPtr->nextGroundState = player->nextGroundState;
            playerPtr->onGround        = player->onGround;
            playerPtr->groundedStore   = player->groundedStore;
            for (int i = 0; i < 8; ++i) {
                playerPtr->abilityValues[i] = player->abilityValues[i];
                playerPtr->abilityPtrs[i]   = player->abilityPtrs[i];
            }
            playerPtr->angle          = player->angle;
            playerPtr->rotation       = player->rotation;
            playerPtr->direction      = player->direction;
            playerPtr->tileCollisions = player->tileCollisions;
            playerPtr->interaction    = player->interaction;
            RSDK.SetSpriteAnimation(playerPtr->aniFrames, player->animator.animationID, &playerPtr->animator, false, 0);
        }
        else {
            playerPtr->state = Player_State_Air;
            RSDK.SetSpriteAnimation(playerPtr->aniFrames, ANI_JUMP, &playerPtr->animator, false, 0);
            playerPtr->tileCollisions = true;
            playerPtr->interaction    = true;
        }

        playerPtr->position.x      = player->position.x;
        playerPtr->position.y      = player->position.y;
        playerPtr->velocity.x      = player->velocity.x;
        playerPtr->velocity.y      = player->velocity.y;
        playerPtr->groundVel       = player->groundVel;
        playerPtr->shield          = player->shield;
        playerPtr->collisionLayers = player->collisionLayers;
        playerPtr->collisionPlane  = player->collisionPlane;
        playerPtr->collisionMode   = player->collisionMode;
        playerPtr->invincibleTimer = player->invincibleTimer;
        playerPtr->speedShoesTimer = player->speedShoesTimer;
        playerPtr->blinkTimer      = player->blinkTimer;
        playerPtr->visible         = player->visible;
        Player_ChangePhysicsState(playerPtr);
        Zone->cameraBoundsL[Zone->swappedPlayerIDs[p]]     = cameraBoundsL[p];
        Zone->cameraBoundsR[Zone->swappedPlayerIDs[p]]     = cameraBoundsR[p];
        Zone->cameraBoundsT[Zone->swappedPlayerIDs[p]]     = cameraBoundsT[p];
        Zone->cameraBoundsB[Zone->swappedPlayerIDs[p]]     = cameraBoundsB[p];
        Zone->playerBoundsL[Zone->swappedPlayerIDs[p]]     = playerBoundsL[p];
        Zone->playerBoundsR[Zone->swappedPlayerIDs[p]]     = playerBoundsR[p];
        Zone->playerBoundsT[Zone->swappedPlayerIDs[p]]     = playerBoundsT[p];
        Zone->playerBoundsB[Zone->swappedPlayerIDs[p]]     = playerBoundsB[p];
        Zone->deathBoundary[Zone->swappedPlayerIDs[p]]      = deathBounds[p];
        Zone->playerBoundActiveL[Zone->swappedPlayerIDs[p]] = playerBoundActiveL[p];
        Zone->playerBoundActiveR[Zone->swappedPlayerIDs[p]] = playerBoundActiveR[p];
        Zone->playerBoundActiveT[Zone->swappedPlayerIDs[p]] = playerBoundActiveT[p];
        Zone->playerBoundActiveB[Zone->swappedPlayerIDs[p]] = playerBoundActiveB[p];

        uint8 *layerPlanes = (uint8 *)&layerIDs[2 * p];
        for (int32 l = 0; l < LAYER_COUNT; ++l) {
            TileLayer *layer                     = RSDK.GetSceneLayer(l);
            layer->drawLayer[Zone->swappedPlayerIDs[p]] = layerPlanes[l];
        }

        EntityCamera *camera = playerPtr->camera;
        void *camTarget      = camera->targetPtr;
        void *camState       = camera->state;
        int32 camScreen      = camera->screenID;
        RSDK.CopyEntity(camera, &Zone->entityData[8 + p], false);
        camera->targetPtr                  = camTarget;
        camera->screenID                   = camScreen;
        camera->state                      = camState;
        ScreenInfo[camera->screenID].position.x = Zone->screenPosX[p];
        ScreenInfo[camera->screenID].position.y = Zone->screenPosY[p];

        EntityShield *shield = RSDK_GET_ENTITY(Player->playerCount + Zone->swappedPlayerIDs[p], Shield);
        RSDK.CopyEntity(shield, &Zone->entityData[4 + p], false);
        shield->player = player;

        EntityImageTrail *trail = RSDK_GET_ENTITY(Player->playerCount + Zone->swappedPlayerIDs[p], ImageTrail);
        RSDK.CopyEntity(trail, &Zone->entityData[12 + p], false);
        trail->player = (Entity *)player;

        EntityCamera *cam = playerPtr->camera;
        if (cam) {
            cam->position.x = playerPtr->position.x;
            cam->position.y = playerPtr->position.y;
        }
        memset(&Zone->entityData[0 + p], 0, ENTITY_SIZE);
        memset(&Zone->entityData[4 + p], 0, ENTITY_SIZE);
        memset(&Zone->entityData[8 + p], 0, ENTITY_SIZE);
        memset(&Zone->entityData[12 + p], 0, ENTITY_SIZE);
    }
#else

    int preSwapPlayerIDs[] = { 0, 1, 1, 1 };
    int swappedPlayerIDs[] = { 1, 0, 0 ,0 };

    EntityPlayer *playerPtrs[] = { NULL, NULL, NULL, NULL };
    EntityShield *shieldPtrs[] = { NULL, NULL, NULL, NULL };
    EntityCamera *cameraPtrs[] = { NULL, NULL, NULL, NULL };
    Entity *powerupPtrs[] = { NULL, NULL, NULL, NULL };
    Vector2 screenPos[4];

    for (int32 p = 0; p < Player->playerCount; ++p) {
        EntityPlayer *player = RSDK_GET_ENTITY(preSwapPlayerIDs[p], Player);

        playerPtrs[p] = (EntityPlayer*)RSDK.CreateEntity(TYPE_BLANK, NULL, 0, 0);
        RSDK.CopyEntity(playerPtrs[p], player, false);

        cameraBoundsL[p] = Zone->cameraBoundsL[p];
        cameraBoundsR[p] = Zone->cameraBoundsR[p];
        cameraBoundsT[p] = Zone->cameraBoundsT[p];
        cameraBoundsB[p] = Zone->cameraBoundsB[p];
        playerBoundsL[p] = Zone->playerBoundsL[p];
        playerBoundsR[p] = Zone->playerBoundsR[p];
        playerBoundsT[p] = Zone->playerBoundsT[p];
        playerBoundsB[p] = Zone->playerBoundsB[p];
        deathBounds[p] = Zone->deathBoundary[p];
        playerBoundActiveL[p] = Zone->playerBoundActiveL[p];
        playerBoundActiveR[p] = Zone->playerBoundActiveR[p];
        playerBoundActiveT[p] = Zone->playerBoundActiveT[p];
        playerBoundActiveB[p] = Zone->playerBoundActiveB[p];

        uint8 *layerPlanes = (uint8 *)&layerIDs[2 * p];
        for (int32 l = 0; l < LAYER_COUNT; ++l) {
            TileLayer *layer = RSDK.GetSceneLayer(l);
            if (layer)
                layerPlanes[l] = layer->drawLayer[preSwapPlayerIDs[p]];
            else
                layerPlanes[l] = DRAWLAYER_COUNT;
        }

        EntityCamera *camera = player->camera;
        cameraPtrs[p] = (EntityCamera *)RSDK.CreateEntity(TYPE_BLANK, NULL, 0, 0);
        RSDK.CopyEntity(cameraPtrs[p], camera, false);
        screenPos[p].x = ScreenInfo[camera->screenID].position.x;
        screenPos[p].y = ScreenInfo[camera->screenID].position.y;

        shieldPtrs[p] = (EntityShield *)RSDK.CreateEntity(TYPE_BLANK, NULL, 0, 0);
        powerupPtrs[p] = RSDK.CreateEntity(TYPE_BLANK, NULL, 0, 0);
        RSDK.CopyEntity(shieldPtrs[p], RSDK_GET_ENTITY(Player->playerCount + preSwapPlayerIDs[p], ), false);
        RSDK.CopyEntity(powerupPtrs[p], RSDK_GET_ENTITY((2 * Player->playerCount) + preSwapPlayerIDs[p], ), false);
    }

    for (int32 p = 0; p < Player->playerCount; ++p) {
        EntityPlayer *playerPtr = (EntityPlayer *)RSDK_GET_ENTITY(swappedPlayerIDs[p], Player);
        EntityPlayer *player = (EntityPlayer *)playerPtrs[p];
        void *state = player->state;

        if (state == Player_State_Ground || state == Player_State_Air || state == Player_State_Roll || state == Player_State_ForceRoll_Ground
            || state == Player_State_ForceRoll_Air) {
            playerPtr->state = state;
            playerPtr->nextAirState = player->nextAirState;
            playerPtr->nextGroundState = player->nextGroundState;
            playerPtr->onGround = player->onGround;
            playerPtr->groundedStore = player->groundedStore;
            for (int i = 0; i < 8; ++i) {
                playerPtr->abilityValues[i] = player->abilityValues[i];
                playerPtr->abilityPtrs[i] = player->abilityPtrs[i];
            }
            playerPtr->angle = player->angle;
            playerPtr->rotation = player->rotation;
            playerPtr->direction = player->direction;
            playerPtr->tileCollisions = player->tileCollisions;
            playerPtr->interaction = player->interaction;
            RSDK.SetSpriteAnimation(playerPtr->aniFrames, player->animator.animationID, &playerPtr->animator, false, 0);
        }
        else {
            playerPtr->state = Player_State_Air;
            RSDK.SetSpriteAnimation(playerPtr->aniFrames, ANI_JUMP, &playerPtr->animator, false, 0);
            playerPtr->tileCollisions = true;
            playerPtr->interaction = true;
        }

        playerPtr->position.x = player->position.x;
        playerPtr->position.y = player->position.y;
        playerPtr->velocity.x = player->velocity.x;
        playerPtr->velocity.y = player->velocity.y;
        playerPtr->groundVel = player->groundVel;
        playerPtr->shield = player->shield;
        playerPtr->collisionLayers = player->collisionLayers;
        playerPtr->collisionPlane = player->collisionPlane;
        playerPtr->collisionMode = player->collisionMode;
        playerPtr->invincibleTimer = player->invincibleTimer;
        playerPtr->speedShoesTimer = player->speedShoesTimer;
        playerPtr->blinkTimer = player->blinkTimer;
        playerPtr->visible = player->visible;
        Player_ChangePhysicsState(playerPtr);
        Zone->cameraBoundsL[swappedPlayerIDs[p]] = cameraBoundsL[p];
        Zone->cameraBoundsR[swappedPlayerIDs[p]] = cameraBoundsR[p];
        Zone->cameraBoundsT[swappedPlayerIDs[p]] = cameraBoundsT[p];
        Zone->cameraBoundsB[swappedPlayerIDs[p]] = cameraBoundsB[p];
        Zone->playerBoundsL[swappedPlayerIDs[p]] = playerBoundsL[p];
        Zone->playerBoundsR[swappedPlayerIDs[p]] = playerBoundsR[p];
        Zone->playerBoundsT[swappedPlayerIDs[p]] = playerBoundsT[p];
        Zone->playerBoundsB[swappedPlayerIDs[p]] = playerBoundsB[p];
        Zone->deathBoundary[swappedPlayerIDs[p]] = deathBounds[p];
        Zone->playerBoundActiveL[swappedPlayerIDs[p]] = playerBoundActiveL[p];
        Zone->playerBoundActiveR[swappedPlayerIDs[p]] = playerBoundActiveR[p];
        Zone->playerBoundActiveT[swappedPlayerIDs[p]] = playerBoundActiveT[p];
        Zone->playerBoundActiveB[swappedPlayerIDs[p]] = playerBoundActiveB[p];

        uint8 *layerPlanes = (uint8 *)&layerIDs[2 * p];
        for (int32 l = 0; l < LAYER_COUNT; ++l) {
            TileLayer *layer = RSDK.GetSceneLayer(l);
            layer->drawLayer[swappedPlayerIDs[p]] = layerPlanes[l];
        }

        EntityCamera *camera = playerPtr->camera;
        void *camTarget = camera->targetPtr;
        void *camState = camera->state;
        int32 camScreen = camera->screenID;
        RSDK.CopyEntity(camera, cameraPtrs[p], false);
        camera->targetPtr = camTarget;
        camera->screenID = camScreen;
        camera->state = camState;
        ScreenInfo[camScreen].position.x = screenPos[p].x;
        ScreenInfo[camScreen].position.y = screenPos[p].y;

        EntityShield *shield = RSDK_GET_ENTITY(Player->playerCount + swappedPlayerIDs[p], Shield);
        RSDK.CopyEntity(shield, shieldPtrs[p], false);
        shield->player = player;

        EntityImageTrail *trail = RSDK_GET_ENTITY(Player->playerCount + swappedPlayerIDs[p], ImageTrail);
        RSDK.CopyEntity(trail, powerupPtrs[p], false);
        trail->player = (Entity *)player;

        EntityCamera *cam = player->camera;
        if (cam) {
            cam->position.x = playerPtr->position.x;
            cam->position.y = playerPtr->position.y;
        }

        destroyEntity(playerPtrs[p]);
        destroyEntity(shieldPtrs[p]);
        destroyEntity(cameraPtrs[p]);
        destroyEntity(powerupPtrs[p]);
    }
#endif
}

void Zone_State_SwapPlayers(void)
{
    RSDK_THIS(Zone);

    if (self->timer > 512) {
        self->timer = self->timer - self->fadeSpeed;
#if RETRO_USE_PLUS
        Zone->teleportActionActive = true;
#endif
    }
    else {
#if RETRO_USE_PLUS
        Zone->swapPlayerCount = 0;
        Zone->swapPlayerID    = 0;

        for (Zone->swapPlayerID = 0; Zone->swapPlayerID < Player->playerCount; ++Zone->swapPlayerID) {
            Zone->playerSwapEnabled[Zone->swapPlayerID] = true;
            EntityPlayer *player              = RSDK_GET_ENTITY(Zone->swapPlayerID, Player);

            if (!Player_CheckValidState(player) || !player->interaction || !player->tileCollisions) {
                Zone->playerSwapEnabled[Zone->swapPlayerID] = false;
            }

            EntityCompetition *competition = (EntityCompetition *)Competition->activeEntity;

            if (competition) {
                if (competition->playerFlags[Zone->swapPlayerID])
                    Zone->playerSwapEnabled[Zone->swapPlayerID] = false;
            }

            for (int i = 0; i < Zone->vsSwapCBCount; ++i) {
                StateMachine_Run(Zone->vsSwapCB[i]);
            }

            if (Zone->playerSwapEnabled[Zone->swapPlayerID]) {
                Zone->preSwapPlayerIDs[Zone->swapPlayerCount] = Zone->swapPlayerID;
                ++Zone->swapPlayerCount;
            }
        }

        if (Zone->swapPlayerCount <= 1) {
            RSDK.PlaySfx(Zone->sfxFail, false, 255);
        }
        else {
            EntityCompetitionSession *session = (EntityCompetitionSession *)globals->competitionSession;

            uint8 playerIDs = 0;
            if (session->swapFlag) {
                if (session->swapFlag == 1) {
                    for (Zone->swapPlayerID = 0; Zone->swapPlayerID < Zone->swapPlayerCount; ++Zone->swapPlayerID) {
                        Zone->swappedPlayerIDs[Zone->swapPlayerID] = Zone->preSwapPlayerIDs[Zone->swapPlayerID];
                    }

                    Zone->swapPlayerID = 0;
                    for (Zone->swapPlayerID = 0; Zone->swapPlayerID < Zone->swapPlayerCount; ++Zone->swapPlayerID) {
                        do {
                            Zone->swappedPlayerIDs[Zone->swapPlayerID] = Zone->preSwapPlayerIDs[RSDK.Rand(0, Zone->swapPlayerCount)];
                        } while ((1 << Zone->swappedPlayerIDs[Zone->swapPlayerID]) & playerIDs);

                        if (Zone->swappedPlayerIDs[Zone->swapPlayerID] != Zone->preSwapPlayerIDs[Zone->swapPlayerID]) {
                            playerIDs |= 1 << Zone->swappedPlayerIDs[Zone->swapPlayerID++];
                            if (Zone->swapPlayerID >= Zone->swapPlayerCount)
                                break;
                        }
                        else if (Zone->swapPlayerID >= Zone->swapPlayerCount - 1) {
                            int id                                     = RSDK.Rand(0, Zone->swapPlayerID - 1);
                            int store                                  = Zone->swappedPlayerIDs[id];
                            Zone->swappedPlayerIDs[id]                 = Zone->swappedPlayerIDs[Zone->swapPlayerID];
                            Zone->swappedPlayerIDs[Zone->swapPlayerID] = store;
                            playerIDs |= 1 << Zone->swappedPlayerIDs[id];
                            playerIDs |= 1 << Zone->swappedPlayerIDs[Zone->swapPlayerID++];
                        }
                    }
                }
            }
            else {
                for (Zone->swapPlayerID = 1; Zone->swapPlayerID < Zone->swapPlayerCount; ++Zone->swapPlayerID) {
                    Zone->swappedPlayerIDs[Zone->swapPlayerID] = Zone->preSwapPlayerIDs[Zone->swapPlayerID - 1];
                }
                Zone->swappedPlayerIDs[0] = Zone->preSwapPlayerIDs[Zone->swapPlayerCount - 1];
            }
#else
        Zone->playerSwapEnabled = true;
        for (int p = 0; p < Player->playerCount; ++p) {
            EntityPlayer *player = RSDK_GET_ENTITY(p, Player);
            if (player->state == Player_State_Drown || player->state == Player_State_None || player->state == Player_State_Die || !player->interaction
                || !player->tileCollisions)
                Zone->playerSwapEnabled = false;
        }

        if (Competition->activeEntity)
            Zone->playerSwapEnabled = false;

        for (int i = 0; i < Zone->vsSwapCBCount; ++i) {
            StateMachine_Run(Zone->vsSwapCB[i]);
        }

        if (!Zone->playerSwapEnabled) {
            RSDK.PlaySfx(Zone->sfxFail, false, 255);
        }
        else {
#endif
            Zone_HandlePlayerSwap();
        }
        self->state = Zone_State_HandleSwapFadeIn;
#if RETRO_USE_PLUS
        Zone->teleportActionActive = true;
#endif
    }
}

void Zone_State_HandleSwapFadeIn(void)
{
    RSDK_THIS(Zone);
    if (self->timer <= 0) {
#if RETRO_USE_PLUS
        Zone->teleportActionActive = false;
#endif
        destroyEntity(self);
    }
    else {
        self->timer -= self->fadeSpeed;
#if RETRO_USE_PLUS
        Zone->teleportActionActive = true;
#endif
    }
}

#if RETRO_INCLUDE_EDITOR
void Zone_EditorDraw(void) {}

void Zone_EditorLoad(void)
{

    Zone->fgLayerLow     = 0;
    Zone->drawOrderLow   = 2;
    Zone->playerDrawLow  = 4;
    Zone->fgLayerHigh    = 6;
    Zone->drawOrderHigh  = 8;
    Zone->playerDrawHigh = 12;
    Zone->hudDrawOrder   = 14;
}
#endif

void Zone_Serialize(void) {}
