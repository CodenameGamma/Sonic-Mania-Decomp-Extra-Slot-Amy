#include "SonicMania.h"

ObjectAIZSetup *AIZSetup;

void AIZSetup_Update(void) {}

void AIZSetup_LateUpdate(void) {}

void AIZSetup_StaticUpdate(void)
{
#if RETRO_USE_PLUS
    if (Zone->actID != 3 && !AIZSetup->cutsceneInit) {
#else
    if (!AIZSetup->cutsceneInit) {
#endif
        AIZSetup_SetupObjects();
        AIZSetup_GetCutsceneSetupPtr();
        AIZSetup->cutsceneInit = true;
    }

#if RETRO_USE_PLUS
    if (AIZSetup->bg4Info) {
#endif
        if (ScreenInfo->position.x <= 4096)
            AIZSetup->bg2Info->scrollPos = 0;
        else
            AIZSetup->bg2Info->scrollPos = 0x1000000;

        if (ScreenInfo->position.x <= 8704) {
            AIZSetup->bg2Info->drawLayer[0] = 0;
            AIZSetup->bg3Info->drawLayer[0] = DRAWLAYER_COUNT;
        }
        else {
            AIZSetup->bg2Info->drawLayer[0] = DRAWLAYER_COUNT;
            AIZSetup->bg3Info->drawLayer[0] = 0;
        }
#if RETRO_USE_PLUS
    }
    else {
        if (!(Zone->timer & 3)) {
            ++RSDK.GetSceneLayer(0)->deformationOffsetW;
        }

        if (!(Zone->timer & 1)) {
            for (int32 i = Zone->fgLow; i <= Zone->fgHigh; ++i) {
                RSDK.GetSceneLayer(i)->deformationOffsetW++;
            }
        }
    }
#endif

    if (!(Zone->timer % 5)) {
        RSDK.RotatePalette(0, 171, 174, true);
#if RETRO_USE_PLUS
        RSDK.RotatePalette(1, 171, 174, true);
        CutsceneHBH->paletteColours[43] = RSDK.GetPaletteEntry(0, 171);
        CutsceneHBH->paletteColours[44] = RSDK.GetPaletteEntry(0, 172);
        CutsceneHBH->paletteColours[45] = RSDK.GetPaletteEntry(0, 173);
        CutsceneHBH->paletteColours[46] = RSDK.GetPaletteEntry(0, 174);
#endif
    }

    if (!AIZSetup->playDrillSfxFlag || RSDK_GET_ENTITY(SLOT_PAUSEMENU, PauseMenu)->objectID == PauseMenu->objectID) {
        if (AIZSetup->playingDrillSFX) {
            RSDK.StopSfx(AIZSetup->sfxDrill);
            AIZSetup->playingDrillSFX = false;
        }
    }
    else if (!AIZSetup->playingDrillSFX) {
        RSDK.PlaySfx(AIZSetup->sfxDrill, 1, 255);
        AIZSetup->playingDrillSFX = true;
    }

#if RETRO_USE_PLUS
    --AIZSetup->aniTilesDelay1;
    if (AIZSetup->aniTilesDelay1 < 0) {
        ++AIZSetup->aniTileFrameA;
        if (AIZSetup->aniTiles1[AIZSetup->aniTileFrameA] == -1) {
            AIZSetup->aniTileFrameA = 0;
        }
        AIZSetup->aniTilesDelay1 = AIZSetup->aniTiles2[AIZSetup->aniTileFrameA];
        RSDK.DrawAniTiles(AIZSetup->aniTiles, 196, 16 * AIZSetup->aniTiles1[AIZSetup->aniTileFrameA], 0, 16, 32);
    }

    --AIZSetup->aniTilesDelay2;
    if (AIZSetup->aniTilesDelay2 < 0) {
        ++AIZSetup->aniTileFrameB;
        if (AIZSetup->aniTiles3[AIZSetup->aniTileFrameB] == -1) {
            AIZSetup->aniTileFrameB = 0;
        }
        AIZSetup->aniTilesDelay2 = AIZSetup->aniTiles4[AIZSetup->aniTileFrameB];
        RSDK.DrawAniTiles(AIZSetup->aniTiles, 198, 16 * AIZSetup->aniTiles3[AIZSetup->aniTileFrameB], 32, 16, 48);
    }

    --AIZSetup->aniTilesDelay3;
    if (AIZSetup->aniTilesDelay3 < 0) {
        ++AIZSetup->aniTileFrameC;
        if (AIZSetup->aniTiles5[AIZSetup->aniTileFrameC] == -1) {
            AIZSetup->aniTileFrameC = 0;
        }
        AIZSetup->aniTilesDelay3 = AIZSetup->aniTiles6[AIZSetup->aniTileFrameC];
        RSDK.DrawAniTiles(AIZSetup->aniTiles, 201, 16 * AIZSetup->aniTiles5[AIZSetup->aniTileFrameC], 32, 16, 48);
    }
#endif
}

void AIZSetup_Draw(void) {}

void AIZSetup_Create(void *data) {}

void AIZSetup_StageLoad(void)
{
#if RETRO_USE_PLUS
    if (Zone->actID != 3)
#endif
        Zone->cameraBoundsB[0] = SCREEN_YSIZE;

    AIZSetup->cutsceneInit    = false;
    AIZSetup->aniTiles        = RSDK.LoadSpriteSheet("AIZ/AniTiles.gif", SCOPE_STAGE);
    AIZSetup->knuxSpriteIndex = RSDK.LoadSpriteAnimation("Players/KnuxCutsceneAIZ.bin", SCOPE_STAGE);

#if RETRO_USE_PLUS
    if (RSDK.GetSceneLayerID("Background 4") >= DRAWLAYER_COUNT) {
        for (int32 i = Zone->fgLow; i <= Zone->fgHigh; ++i) {
            int32 *deformData = RSDK.GetSceneLayer(i)->deformationDataW;

            int32 id = 0;
            for (int32 d = 0; d < 0x200; ++d) {
                deformData[d]         = (2 * RSDK.Sin1024(id)) >> 10;
                deformData[d + 0x200] = (2 * RSDK.Sin1024(id)) >> 10;
                id += 16;
            }
        }

        int32 *deformData = RSDK.GetSceneLayer(0)->deformationDataW;

        int32 id = 0;
        for (int32 d = 0; d < 0x200; d += 16) {
            int32 pos = 0;
            if (id >= 0)
                pos = id;
            else
                pos = 0;

            int32 angle = 0;
            int32 rand  = RSDK.Rand(0, 4);
            for (int32 i = 0; i < 16; ++i) {
                deformData[pos + i]           = (rand * RSDK.Sin1024(angle)) >> 10;
                deformData[(pos + 0x200) + i] = (rand * RSDK.Sin1024(angle)) >> 10;
                angle += 64;
            }
            id += 16;
        }
    }
    else {
#endif
        AIZSetup->bg1Info = RSDK.GetSceneLayer(RSDK.GetSceneLayerID("Background 1"));
        AIZSetup->bg2Info = RSDK.GetSceneLayer(RSDK.GetSceneLayerID("Background 2"));
        AIZSetup->bg3Info = RSDK.GetSceneLayer(RSDK.GetSceneLayerID("Background 3"));
        AIZSetup->bg4Info = RSDK.GetSceneLayer(RSDK.GetSceneLayerID("Background 4"));

        for (int32 i = 0; i < AIZSetup->bg2Info->scrollInfoCount; ++i) {
            int32 pFac                                 = AIZSetup->bg2Info->scrollInfo[i].parallaxFactor;
            AIZSetup->bg2Info->scrollInfo[i].scrollPos = -0x7000000 - (0x220000 * pFac);
        }

        for (int32 i = 0; i < AIZSetup->bg3Info->scrollInfoCount; ++i) {
            int32 pFac                                 = AIZSetup->bg3Info->scrollInfo[i].parallaxFactor;
            AIZSetup->bg3Info->scrollInfo[i].scrollPos = -0x7000000 - (0x220000 * pFac);
        }
#if RETRO_USE_PLUS
    }
#endif

    Animals->animalTypes[0] = ANIMAL_FLICKY;
    Animals->animalTypes[1] = ANIMAL_RICKY;
    Music_SetMusicTrack("RubyPresence.ogg", TRACK_RUBYPRESENCE, 198457);
    AIZSetup->sfxDrill     = RSDK.GetSfx("LRZ/Drill.wav");
    AIZSetup->sfxBreak     = RSDK.GetSfx("Stage/LedgeBreak3.wav");
    AIZSetup->sfxImpact    = RSDK.GetSfx("Stage/Impact4.wav");
    AIZSetup->sfxHeliWoosh = RSDK.GetSfx("SPZ1/HeliWooshIn.wav");
    if ((globals->playerID & 0xFF) == ID_KNUCKLES) {
        foreach_all(AIZTornado, tornado) { destroyEntity(tornado); }
        foreach_all(AIZTornadoPath, node) { destroyEntity(node); }
    }

#if RETRO_USE_PLUS
    BGSwitch->switchCallback[AIZ_BG_JUNGLE]  = AIZSetup_BGSwitchCB_Jungle;
    BGSwitch->switchCallback[AIZ_BG_SKY] = AIZSetup_BGSwitchCB_Sky;
    BGSwitch->layerIDs[0]                   = AIZ_BG_JUNGLE;
    BGSwitch->layerIDs[1]                   = AIZ_BG_JUNGLE;
    BGSwitch->layerIDs[2]                   = AIZ_BG_JUNGLE;
    BGSwitch->layerIDs[3]                   = AIZ_BG_JUNGLE;
    RSDK.SetDrawLayerProperties(0, false, Water_SetWaterLevel);
    RSDK.SetDrawLayerProperties(Zone->hudDrawOrder, false, Water_RemoveWaterEffect);
    Water->waterPalette = 1;
#endif
}

#if RETRO_USE_PLUS
void AIZSetup_BGSwitchCB_Jungle(void)
{
    RSDK.GetSceneLayer(0)->drawLayer[BGSwitch->screenID] = 0;
    RSDK.GetSceneLayer(1)->drawLayer[BGSwitch->screenID] = DRAWLAYER_COUNT;
}

void AIZSetup_BGSwitchCB_Sky(void)
{
    RSDK.GetSceneLayer(0)->drawLayer[BGSwitch->screenID] = DRAWLAYER_COUNT;
    RSDK.GetSceneLayer(1)->drawLayer[BGSwitch->screenID] = 0;
}
#endif

void AIZSetup_Unknown4(void)
{
    RSDK_THIS(Player);
    self->active     = ACTIVE_NORMAL;
    self->visible    = false;
    self->position.x = 0;
    self->position.y = 0;
    self->stateInput = StateMachine_None;
}

void AIZSetup_Unknown5(void)
{
    RSDK_THIS(Player);
    self->active      = ACTIVE_NORMAL;
    self->visible     = true;
    self->stateInput  = Player_ProcessP2Input_AI;
    Player->jumpInDelay = 240;
    Player_P2JumpBackIn();
}

void AIZSetup_Unknown24(void)
{
    EntityAIZKingClaw *claw = (EntityAIZKingClaw *)AIZSetup->claw;
    int32 x                 = -0x10000;
    foreach_all(AIZEggRobo, robo)
    {
        if (robo->forKnux && x >= robo->position.x)
            x = robo->position.x;
    }

    if (x < (ScreenInfo->position.x + ScreenInfo->width + 32) << 16) {
        foreach_all(AIZEggRobo, robo)
        {
            if (robo->forKnux)
                robo->position.x += 0x40000;
        }
        claw->position.x += 0x40000;
        claw->position.y -= 0x4000;
    }
}

void AIZSetup_SetupObjects(void)
{
    foreach_all(AIZTornado, tornado)
    {
        AIZSetup->tornado = (Entity *)tornado;
        foreach_break;
    }

    foreach_all(Platform, platform)
    {
        if (!platform->frameID) {
            platform->drawOrder = Zone->drawOrderHigh - 1;
            AIZSetup->platform  = (Entity *)platform;
            foreach_break;
        }
    }

    foreach_all(AIZKingClaw, claw)
    {
        AIZSetup->claw = (Entity *)claw;
        foreach_break;
    }

    foreach_all(PhantomRuby, ruby)
    {
        AIZSetup->phantomRuby = (Entity *)ruby;
        foreach_break;
    }

    int32 id = 0;
    foreach_all(Decoration, decoration) { AIZSetup->decorations[id++] = (Entity *)decoration; }
}

void AIZSetup_GetCutsceneSetupPtr(void)
{
    switch (globals->playerID & 0xFF) {
        default:
        case ID_SONIC:
        case ID_TAILS:
#if RETRO_USE_PLUS
        case ID_MIGHTY:
        case ID_RAY:
#endif
            AIZSetup_CutsceneST_Setup();
            break;
        case ID_KNUCKLES: AIZSetup_CutsceneK_Setup(); break;
    }
}

void AIZSetup_SkipCB(void)
{
#if RETRO_USE_PLUS
    if (globals->gameMode == MODE_ENCORE)
        RSDK.SetScene("Encore Mode", "Green Hill Zone+ 1");
    else
#endif
        RSDK.SetScene("Mania Mode", "Green Hill Zone 1");
}

void AIZSetup_CutsceneST_Setup(void)
{
    void *states[] = { AIZSetup_Cutscene1_Unknown1,
                       AIZSetup_Cutscene1_Unknown2,
                       AIZSetup_Cutscene1_Unknown3,
                       AIZSetup_Cutscene1_Unknown4,
                       AIZSetup_Cutscene1_Unknown5,
                       AIZSetup_Cutscene1_Unknown6,
                       AIZSetup_Cutscene1_Unknown7,
                       AIZSetup_Cutscene1_Unknown8,
                       AIZSetup_Cutscene1_Unknown9,
                       AIZSetup_Cutscene_LoadGHZ,
                       NULL };
    RSDK_THIS(AIZSetup);
    CutsceneSeq_StartSequence((Entity *)self, states);
    EntityCutsceneSeq *seq = RSDK_GET_ENTITY(SLOT_CUTSCENESEQ, CutsceneSeq);
    if (seq->objectID) {
        seq->skipType     = SKIPTYPE_CALLBACK;
        seq->skipCallback = AIZSetup_SkipCB;
    }
}

bool32 AIZSetup_Cutscene1_Unknown1(Entity *h)
{
    EntityCutsceneSeq *host   = (EntityCutsceneSeq *)h;
    EntityCamera *camera      = RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);
    EntityPlayer *player1     = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityPlayer *player2     = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
    EntityAIZTornado *tornado = (EntityAIZTornado *)AIZSetup->tornado;
    if (!host->timer) {
        CutsceneSeq_LockAllPlayerControl();
        player1->stateInput = StateMachine_None;
    }
    Zone->playerBoundActiveL[0] = false;
    if (player2->objectID == Player->objectID)
        player2->state = AIZSetup_Unknown4;
    if (tornado->position.x < ScreenInfo->width << 16)
        camera->position.x = ScreenInfo->width << 16;
    return tornado->disableInteractions;
}
bool32 AIZSetup_Cutscene1_Unknown2(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    if (player1->position.x >= 0x27100000) {
        Music_TransitionTrack(TRACK_HBHMISCHIEF, 0.01);
        player1->right = false;
        player1->left  = true;
        host->storedValue1 = player1->groundVel;
        return true;
    }
    else {
        player1->right = true;
    }
    return false;
}
bool32 AIZSetup_Cutscene1_Unknown3(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityPlayer *player2 = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
    if (player1->position.x >= 0x2A300000) {
        if (player1->groundVel <= 0) {
            if (!host->storedValue2) {
                player1->right = false;
                player1->left  = false;
                host->storedValue2 = host->timer;
            }
        }
        else {
            if (player2->objectID == Player->objectID) {
                player2->stateInput = StateMachine_None;
            }
            Player->curFlyCarryPos.x = player1->position.x - 0x200000;
            Player->curFlyCarryPos.y = player1->position.y;
            player1->state           = Player_State_Ground;
            player1->right           = false;
            player1->left            = true;
        }
    }
    else {
        player1->state = Player_State_None;
        RSDK.SetSpriteAnimation(player1->aniFrames, ANI_SKID, &player1->animator, false, 0);
        player1->groundVel  = host->storedValue1;
        player1->velocity.x = host->storedValue1;
        player1->right      = false;
        player1->left       = false;
        if (player2 && player2->state == AIZSetup_Unknown4)
            player2->state = AIZSetup_Unknown5;
    }

    if (host->storedValue2 > 0 && host->timer > host->storedValue2 + 15) {
        player1->direction = FLIP_NONE;
        return true;
    }

    return false;
}
bool32 AIZSetup_Cutscene1_Unknown4(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityPlayer *player2 = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
    if (player2->objectID == Player->objectID) {
        if (player2->onGround) {
            player2->stateInput = StateMachine_None;
            player2->direction  = FLIP_NONE;
            RSDK.SetSpriteAnimation(player2->aniFrames, 0, &player2->animator, true, 0);
            player2->state = Player_State_None;
            return true;
        }
        else {
            Player->curFlyCarryPos.x = player1->position.x;
            Player->curFlyCarryPos.y = player1->position.y;
            Player->curFlyCarryPos.x -= 0x200000;
            Player->curFlyCarryPos.y += 0x80000;
            player2->position.x = Player->curFlyCarryPos.x;
            player2->direction  = FLIP_NONE;
        }
    }
    else {
        return host->timer > 10;
    }

    return false;
}
bool32 AIZSetup_Cutscene1_Unknown5(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    RSDK_GET_PLAYER(player1, player2, camera);
    if (!host->timer) {
        player1->camera = NULL;
        Camera_SetupLerp(0, 0, AIZSetup->platform->position.x - 0x400000, camera->position.y, 3);
        player1->stateInput = StateMachine_None;
        player1->state      = Player_State_Ground;
        player1->up         = true;
    }
    else {
        if (camera->position.x >= camera->endLerpPos.x) {
            if (host->storedValue2) {
                if (host->timer >= host->storedValue2 + 60)
                    return true;
            }
            else {
                host->storedValue2 = host->timer;
            }
        }
        else {
            player1->up = true;
            if (player2->objectID == Player->objectID) {
                player2->state = Player_State_None;
            }
        }
    }
    return false;
}
bool32 AIZSetup_Cutscene1_Unknown6(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    EntityAIZKingClaw *claw = (EntityAIZKingClaw *)AIZSetup->claw;

    if (claw->position.y < -0x1C0000) {
        claw->position.y += 0x10000;
    }
    else {
        if (host->storedValue2) {
            if (host->timer >= host->storedValue2 + 60)
                return true;
        }
        else {
            host->storedValue2 = host->timer;
            claw->state    = AIZKingClaw_State_Grab;
        }
    }

    return false;
}
bool32 AIZSetup_Cutscene1_Unknown7(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    EntityAIZKingClaw *claw  = (EntityAIZKingClaw *)AIZSetup->claw;
    EntityPlatform *platform = (EntityPlatform *)AIZSetup->platform;
    EntityPlayer *player2    = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
    EntityPhantomRuby *ruby  = (EntityPhantomRuby *)AIZSetup->phantomRuby;

    if (!host->timer) {
        claw->grabbedEntities[0] = (Entity *)platform;
        claw->grabbedEntities[1] = (Entity *)ruby;
        claw->grabCount          = 2;
    }

    if (claw->position.y > -0x520000) {
        AIZSetup->playDrillSfxFlag = true;
        if (!(host->timer % 5))
            Camera_ShakeScreen(0, 0, 2);
        claw->position.y -= 0x4000;
        for (int32 i = 0; i < 3; ++i) {
            EntityDecoration *decor = (EntityDecoration *)AIZSetup->decorations[i];
            decor->drawFX |= FX_ROTATE;
            decor->rotation = ((2 * (!(i & 1)) - 1) * RSDK.Sin256(16 * host->timer)) >> 7;
        }
    }
    else {
        if (host->storedValue2) {
            if (host->values[0]) {
                if (host->values[1] || host->timer < host->storedValue2 || host->timer != host->storedValue2) {
                }
                else {
                    return true;
                }
            }
            else if (host->timer < host->storedValue2 + 30) {
                Vector2 result = MathHelpers_Unknown5(((host->timer - host->storedValue2) << 16) / 30, claw->position.x, host->storedValue1, claw->position.x,
                                                      host->storedValue1 - 0x500000, claw->position.x, host->storedValue1 - 0x300000, claw->position.x,
                                                      host->storedValue1 - 0x300000);
                claw->position.y = result.y;
            }
            else if (host->timer != host->storedValue2 + 30) {
                if (host->values[1] || host->timer < host->storedValue2 || host->timer != host->storedValue2) {
                }
                else {
                    return true;
                }
            }
            else {
                host->values[0] = 1;
                host->storedValue2    = host->timer + 60;
            }
        }
        else {
            AIZSetup->playDrillSfxFlag                               = false;
            host->storedValue2                                           = host->timer;
            host->storedValue1                                           = claw->position.y;
            ((EntityDecoration *)AIZSetup->decorations[0])->rotSpeed = 0;
            ((EntityDecoration *)AIZSetup->decorations[1])->rotSpeed = 0;
            if ((globals->playerID & 0xFFFFFF00) == 512)
                RSDK.SetSpriteAnimation(player2->aniFrames, ANI_SKID, &player2->animator, true, 0);
            RSDK.PlaySfx(AIZSetup->sfxBreak, false, 0);
            Music_TransitionTrack(TRACK_EGGMAN1, 0.05);
        }
    }

    return false;
}
bool32 AIZSetup_Cutscene1_Unknown8(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    EntityPhantomRuby *ruby = (EntityPhantomRuby *)AIZSetup->phantomRuby;
    if (!host->timer)
        PhantomRuby_SetupFlash(ruby);
    return ruby->flashFinished;
}
bool32 AIZSetup_Cutscene1_Unknown9(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    EntityPhantomRuby *ruby = (EntityPhantomRuby *)AIZSetup->phantomRuby;
    EntityPlayer *player1   = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityPlayer *player2   = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);

    EntityFXRuby *fxRuby = NULL;
    if (host->timer) {
        fxRuby = (EntityFXRuby *)AIZSetup->fxRuby;
    }
    else {
        fxRuby            = CREATE_ENTITY(FXRuby, NULL, ruby->position.x, ruby->position.y);
        fxRuby->drawOrder = Zone->playerDrawHigh;
        AIZSetup->fxRuby  = (Entity *)fxRuby;
        Camera_ShakeScreen(0, 4, 4);
        player1->drawOrder = Zone->playerDrawHigh + 1;
        if (player2->objectID == Player->objectID)
            player2->drawOrder = Zone->playerDrawHigh + 1;
    }

    if (!host->values[0]) {
        if (fxRuby->fullyExpanded) {
            if (host->storedValue2) {
                if (host->timer == host->storedValue2 + 30) {
                    fxRuby->delay = 64;
                    fxRuby->state    = FXRuby_State_IncreaseStageDeform;
                    PhantomRuby_PlaySFX(RUBYSFX_ATTACK4);
                    Camera_ShakeScreen(0, 4, 4);
                }
                else if (host->timer == host->storedValue2 + 210) {
                    fxRuby->delay = 32;
                    fxRuby->state    = FXRuby_State_IncreaseStageDeform;
                    PhantomRuby_PlaySFX(RUBYSFX_ATTACK1);
                    Camera_ShakeScreen(0, 4, 4);
                    Music_FadeOut(0.025);
                    host->storedValue2    = host->timer;
                    host->values[0] = true;
                }
            }
            else {
                host->storedValue2 = host->timer;
            }

            if (host->timer >= host->storedValue2 + 32) {
                int32 id = 0;
                for (int32 angle = 0; angle < 0x80; angle += 0x40) {
                    EntityPlayer *player = RSDK_GET_ENTITY(id++, Player);
                    if (!player || player->objectID == TYPE_BLANK)
                        break;
                    RSDK.SetSpriteAnimation(player->aniFrames, ANI_FAN, &player->animator, false, 0);

                    int32 valX = (player->position.x - player->position.x) >> 3;
                    int32 valY = (0xA00 * RSDK.Sin256(2 * (host->timer + angle - host->storedValue2)) + ruby->position.y - player->position.y) >> 3;

                    player->position.x += valX;
                    player->position.y += valY;
                    player->state          = Player_State_None;
                    player->tileCollisions = false;
                    player->onGround       = false;
                }
            }
        }
    }
    else {
        if (fxRuby->fadeWhite >= 512) {
            if (fxRuby->fadeBlack >= 512) {
                if (host->timer == host->storedValue2 + 150)
                    return true;
            }
            else {
                fxRuby->fadeBlack += 16;
            }
        }
        else {
            fxRuby->fadeWhite += 16;
        }
    }
    return false;
}

void AIZSetup_CutsceneK_Setup(void)
{
    void *states[] = { AIZSetup_Cutscene2_Unknown1, AIZSetup_Cutscene2_Unknown2,
                       AIZSetup_Cutscene2_Unknown3, AIZSetup_Cutscene2_Unknown4,
                       AIZSetup_Cutscene2_Unknown5, AIZSetup_Cutscene2_Unknown6,
                       AIZSetup_Cutscene2_Unknown7, AIZSetup_Cutscene2_Unknown8,
                       AIZSetup_Cutscene_LoadGHZ,   NULL };

    RSDK_THIS(AIZSetup);
    CutsceneSeq_StartSequence((Entity *)self, states);
    EntityCutsceneSeq *seq = RSDK_GET_ENTITY(SLOT_CUTSCENESEQ, CutsceneSeq);
    if (seq->objectID) {
        seq->skipType     = SKIPTYPE_CALLBACK;
        seq->skipCallback = AIZSetup_SkipCB;
    }
}

bool32 AIZSetup_Cutscene2_Unknown1(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    if (!host->timer) {
        player1->state      = Player_State_None;
        player1->stateInput = StateMachine_None;
        player1->direction  = FLIP_X;
        player1->drawFX     = FX_FLIP;
        RSDK.SetSpriteAnimation(AIZSetup->knuxSpriteIndex, 1, &player1->animator, true, 0);
    }
    return host->timer == 180;
}
bool32 AIZSetup_Cutscene2_Unknown2(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    switch (host->timer) {
        case 0: RSDK.PlaySfx(AIZKingClaw->sfxClack, 0, 0); break;
        case 10:
            RSDK.PlaySfx(AIZKingClaw->sfxWalkerLegs, 0, 0);
            RSDK.SetSpriteAnimation(AIZSetup->knuxSpriteIndex, 2, &player1->animator, true, 0);
            break;
        case 40: return true;
        default: break;
    }
    return false;
}
bool32 AIZSetup_Cutscene2_Unknown3(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    if (host->timer < 120) {
        AIZSetup->playDrillSfxFlag = true;
        if (!(host->timer % 5)) {
            Camera_ShakeScreen(0, 0, 2);
        }
    }
    else if (host->timer == 120) {
        AIZSetup->playDrillSfxFlag = false;
        RSDK.PlaySfx(AIZSetup->sfxBreak, false, 0);
        Music_TransitionTrack(TRACK_HBHMISCHIEF, 0.2);
    }
    return host->timer == 200;
}
bool32 AIZSetup_Cutscene2_Unknown4(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    if (player1->animator.animationID == 3 && player1->animator.frameID == player1->animator.frameCount - 1) {
        RSDK.SetSpriteAnimation(AIZSetup->knuxSpriteIndex, 0, &player1->animator, true, 0);
    }

    if (!host->timer) {
        RSDK.SetSpriteAnimation(AIZSetup->knuxSpriteIndex, 3, &player1->animator, true, 0);
        foreach_active(Animals, animal)
        {
            animal->active    = ACTIVE_NORMAL;
            animal->behaviour = 0;
        }
    }
    return host->timer == 60;
}
bool32 AIZSetup_Cutscene2_Unknown5(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityCamera *camera  = RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);

    if (!host->timer) {
        player1->camera = 0;
        Camera_SetupLerp(0, 0, player1->position.x - 0x600000, camera->position.y, 3);
    }
    else if (camera->position.x <= camera->endLerpPos.x) {
        if (host->storedValue2) {
            if (host->timer >= host->storedValue2 + 60)
                return true;
        }
        else {
            host->storedValue2 = host->timer;
        }
    }
    else {
        player1->state = Player_State_None;
    }
    return false;
}
bool32 AIZSetup_Cutscene2_Unknown6(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    EntityAIZKingClaw *claw = (EntityAIZKingClaw *)AIZSetup->claw;
    EntityPlayer *player1   = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityPhantomRuby *ruby = (EntityPhantomRuby *)AIZSetup->phantomRuby;

    if (!host->timer) {
        claw->grabbedEntities[0] = AIZSetup->platform;
        claw->grabbedEntities[1] = (Entity *)ruby;
        claw->grabCount          = 2;
        claw->angle              = -16;
        claw->position.y         = -0x300000;
        claw->position.x         = 0x2F9C0000;
        ruby->sfx                = 0;
        PhantomRuby_SetupFlash(ruby);
        player1->drawOrder = Zone->playerDrawHigh + 2;
        RSDK.PlaySfx(AIZSetup->sfxHeliWoosh, false, 0);
    }
    AIZSetup_Unknown24();
    return ruby->position.x >= player1->position.x - 0x100000;
}
bool32 AIZSetup_Cutscene2_Unknown7(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    EntityPlayer *player1   = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityPlayer *player2   = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
    EntityPhantomRuby *ruby = (EntityPhantomRuby *)AIZSetup->phantomRuby;
    if (!host->timer) {
        RSDK.PlaySfx(AIZSetup->sfxImpact, false, 0);
        PhantomRuby_PlaySFX(RUBYSFX_REDCUBE);
        Music_TransitionTrack(TRACK_EGGMAN1, 1.0);

        EntityFXRuby *fxRuby     = CREATE_ENTITY(FXRuby, NULL, ruby->position.x, ruby->position.y);
        fxRuby->drawOrder        = Zone->playerDrawHigh;
        AIZSetup->fxRuby         = (Entity *)fxRuby;
        player1->velocity.x      = 0x20000;
        player1->velocity.y      = -0x40000;
        player1->nextGroundState = StateMachine_None;
        player1->nextAirState    = StateMachine_None;
        player1->state           = Player_State_None;
        player1->onGround        = false;
        player1->drawOrder       = Zone->playerDrawHigh + 1;
        if (player2->objectID == Player->objectID)
            player2->drawOrder = Zone->playerDrawHigh + 1;
        Camera_ShakeScreen(0, 4, 4);
    }

    AIZSetup_Unknown24();
    if (player1->onGround) {
        if (player1->velocity.x > 0)
            player1->velocity.x -= 0x2000;
        if (player1->velocity.x < 0)
            player1->velocity.x = 0;
        player1->state = Player_State_None;
        RSDK.SetSpriteAnimation(AIZSetup->knuxSpriteIndex, 5, &player1->animator, false, 0);
        return true;
    }
    else {
        player1->velocity.y += 0x3800;
        if (player1->velocity.x > 0)
            player1->velocity.x -= 0x1000;
        if (player1->velocity.x < 0)
            player1->velocity.x = 0;
        RSDK.SetSpriteAnimation(AIZSetup->knuxSpriteIndex, 4, &player1->animator, true, 0);
    }
    return false;
}
bool32 AIZSetup_Cutscene2_Unknown8(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityFXRuby *fxRuby  = (EntityFXRuby *)AIZSetup->fxRuby;

    if (player1->velocity.x > 0)
        player1->velocity.x -= 4096;
    if (player1->velocity.x < 0)
        player1->velocity.x = 0;

    if (host->timer == 180) {
        fxRuby->delay = 32;
        fxRuby->state    = FXRuby_State_IncreaseStageDeform;
        PhantomRuby_PlaySFX(RUBYSFX_ATTACK1);
        Camera_ShakeScreen(0, 4, 4);
        Music_FadeOut(0.025);
        host->storedValue2    = host->timer;
        host->values[0] = 1;
    }

    AIZSetup_Unknown24();
    if (host->values[0]) {
        if (fxRuby->fadeWhite < 512) {
            fxRuby->fadeWhite += 16;
        }
        else if (fxRuby->fadeBlack >= 512) {
            if (host->timer == host->storedValue2 + 150)
                return true;
        }
        else {
            fxRuby->fadeBlack += 16;
        }
    }
    return false;
}

bool32 AIZSetup_Cutscene_LoadGHZ(Entity *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;
    unused(host);

    RSDK.SetScene("Cutscenes", "Green Hill Zone");
    RSDK.LoadScene();
    return true;
}

#if RETRO_INCLUDE_EDITOR
void AIZSetup_EditorDraw(void) {}

void AIZSetup_EditorLoad(void)
{
    RSDK_ACTIVE_VAR(BGSwitch, bgID);
    RSDK_ENUM_VAR("Jungle BG", AIZ_BG_JUNGLE);
    RSDK_ENUM_VAR("Sky BG", AIZ_BG_SKY);
}
#endif

void AIZSetup_Serialize(void) {}
