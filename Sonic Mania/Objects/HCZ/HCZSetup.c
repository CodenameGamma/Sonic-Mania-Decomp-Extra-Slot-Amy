#include "SonicMania.h"

ObjectHCZSetup *HCZSetup;

void HCZSetup_Update(void) {}

void HCZSetup_LateUpdate(void) {}

void HCZSetup_StaticUpdate(void)
{
    if (!(Zone->timer & 3)) {
        ++HCZSetup->bg->deformationOffsetW;
        if (!(Zone->timer & 3)) {
            HCZSetup->timerA += 16;
            HCZSetup->timerA &= 0x3F;
            RSDK.DrawAniTiles(HCZSetup->aniTiles1, 386, 0, HCZSetup->timerA, 64, 16);

            HCZSetup->timerB += 32;
            HCZSetup->timerB &= 0x1FF;
            RSDK.DrawAniTiles(HCZSetup->aniTiles2, 820, HCZSetup->timerB, 0, 32, 96);
            RSDK.DrawAniTiles(HCZSetup->aniTiles2, 857, HCZSetup->timerB, 96, 32, 128);
            RSDK.RotatePalette(0, 176, 182, true);
        }
    }

    int32 *sizes = HCZSetup->aniTilesDelay;
    int32 sizeX  = 0;
    int32 sizeY  = 0;

    for (int32 i = 0; i < 7; ++i) {
        sizeX += sizes[0];
        sizeY += sizes[1];
        sizes += 2;
    }

    int32 val   = Zone->timer % (sizeY + sizeX);
    int32 valID = 0;

    while (val >= HCZSetup->aniTilesDelay[valID]) {
        val -= HCZSetup->aniTilesDelay[valID];
        ++valID;
    }

    if (!val) {
        RSDK.DrawAniTiles(HCZSetup->aniTiles3, 874, 0, 16 * valID, 32, 16);
    }

    if (!(Zone->timer & 1)) {
        for (int32 id = Zone->fgLow; id <= Zone->fgHigh; ++id) {
            ++RSDK.GetSceneLayer(id)->deformationOffsetW;
        }
    }

#if RETRO_USE_PLUS
    if (HCZSetup->activePlayerCount) {
        if (HCZSetup->playingLoopSFX) {
            if (!(HCZSetup->waterfallSFXTimer & 0x1F)) {
                RSDK.StopSfx(HCZSetup->sfxWaterfall);
                RSDK.PlaySfx(HCZSetup->sfxWaterfallLoop, 0, 255);
            }
            ++HCZSetup->waterfallSFXTimer;
        }
        else {
            HCZSetup->waterfallSFXTimer = 0;
            HCZSetup->playingLoopSFX    = true;
            RSDK.PlaySfx(HCZSetup->sfxWaterfall, 0, 255);
            ++HCZSetup->waterfallSFXTimer;
        }
    }
    else {
        HCZSetup->playingLoopSFX = false;
    }

    HCZSetup->activePlayerCount = 0;
    foreach_active(Player, player)
    {
        RSDK.GetEntityID(player);
        if (player->state != Player_State_None) {
            Hitbox *hitbox = Player_GetHitbox(player);
            uint16 tile =
                RSDK.GetTileInfo(Zone->fgHigh, player->position.x >> 20, ((hitbox->bottom << 16) + player->position.y - 0x10000) >> 20) & 0x3FF;
            if (((tile >= 226 && tile <= 224) || (tile >= 880 && tile <= 888)) && player->collisionPlane == 1) {
                if (player->state != Player_State_BubbleBounce && player->state != Player_State_MightyHammerDrop) {
                    if (player->onGround) {
                        if (player->state != Player_State_WaterSlide) {
                            player->interaction    = true;
                            player->tileCollisions = true;
                            player->position.y += player->cameraOffset;
                            player->nextGroundState = StateMachine_None;
                            player->state           = Player_State_WaterSlide;
                        }

                        if (player->onGround) {
                            ++HCZSetup->activePlayerCount;
                        }
                    }
                    else if (player->animator.animationID == ANI_FLUME) {
                        ++HCZSetup->activePlayerCount;
                    }
                }
            }
        }
    }
#endif
}

void HCZSetup_Draw(void) {}

void HCZSetup_Create(void *data) {}

void HCZSetup_StageLoad(void)
{
    HCZSetup->aniTiles1 = RSDK.LoadSpriteSheet("HCZ/AniTiles.gif", SCOPE_STAGE);
    HCZSetup->aniTiles2 = RSDK.LoadSpriteSheet("HCZ/AniTiles2.gif", SCOPE_STAGE);
    HCZSetup->aniTiles3 = RSDK.LoadSpriteSheet("HCZ/AniTiles3.gif", SCOPE_STAGE);

    HCZSetup->bg = RSDK.GetSceneLayer(1);
    for (int32 i = 0; i < 0x200; i += 0x10) {
        int32 val = RSDK.Rand(0, 4);

        int32 pos = i;
        pos     = minVal(0x200, pos);
        pos     = maxVal(0x000, pos);

        int32 *deformData = &HCZSetup->bg->deformationDataW[pos];

        int32 angle = 0;
        for (int32 d = 0; d < 0x10; ++d) {
            *deformData = val * RSDK.Sin1024(angle) >> 10;
            angle += 64;
        }

        memcpy(&HCZSetup->bg->deformationDataW[0x200], HCZSetup->bg->deformationDataW, 0x200 * sizeof(int32));
    }

    if (Zone->actID == 1) {
        HCZSetup->bg->scanlineCallback = HCZSetup_ScanlineCallback;
    }

    int32 id = Zone->fgLow;
    while (id <= Zone->fgHigh) {
        TileLayer *layer = RSDK.GetSceneLayer(id);
        int32 *deformData  = layer->deformationDataW;

        for (int32 i = 0; i < 4; ++i) {
            deformData[0]  = 1;
            deformData[1]  = 1;
            deformData[2]  = 2;
            deformData[3]  = 2;
            deformData[4]  = 3;
            deformData[5]  = 3;
            deformData[6]  = 3;
            deformData[7]  = 3;
            deformData[8]  = 2;
            deformData[9]  = 2;
            deformData[10] = 1;
            deformData[11] = 1;

            deformData[128] = 1;
            deformData[129] = 1;
            deformData[130] = 2;
            deformData[131] = 2;
            deformData[132] = 3;
            deformData[133] = 3;
            deformData[134] = 3;
            deformData[135] = 3;
            deformData[136] = 2;
            deformData[137] = 2;
            deformData[138] = 1;
            deformData[139] = 1;

            deformData[160] = -1;
            deformData[161] = -1;
            deformData[162] = -2;
            deformData[163] = -2;
            deformData[164] = -3;
            deformData[165] = -3;
            deformData[166] = -3;
            deformData[167] = -3;
            deformData[168] = -2;
            deformData[169] = -2;
            deformData[170] = -1;
            deformData[171] = -1;

            deformData += 0x100;
        }
        id++;
    }
    Animals->animalTypes[0] = ANIMAL_POCKY;
    Animals->animalTypes[1] = ANIMAL_ROCKY;
    RSDK.SetDrawLayerProperties(0, false, Water_SetWaterLevel);
    RSDK.SetDrawLayerProperties(Zone->hudDrawOrder, false, Water_RemoveWaterEffect);

    Water->waterPalette = 1;
    if (Zone->actID) {
        if (!PlayerHelpers_CheckStageReload())
            PlayerHelpers_CheckPlayerPos(0x5900000, 0xB00000, 0x2600000, 0x6800000);
        Zone->cameraBoundsL[0] = 168;
        Zone->cameraBoundsL[1] = 168;
        Zone->cameraBoundsL[2] = 168;
        Zone->cameraBoundsL[3] = 168;
        if (isMainGameMode() && globals->atlEnabled && !PlayerHelpers_CheckStageReload()) {
            Zone_ReloadStoredEntities(0x6A00000, 0x1840000, true);
        }
        Zone->stageFinishCallback = HCZSetup_StageFinishCB_Act2;
    }
    else if (isMainGameMode() && PlayerHelpers_CheckAct1()) {
        Zone->forcePlayerOnScreenFlag = true;
        Zone->stageFinishCallback     = HCZSetup_HandleActTransition;
    }

#if RETRO_USE_PLUS
    if (SceneInfo->filter & FILTER_ENCORE) {
        RSDK.LoadPalette(0, "EncoreHCZ.act", 255);
        RSDK.LoadPalette(1, "EncoreHCZw.act", 255);
    }

    HCZSetup->sfxWaterfall     = RSDK.GetSfx("Stage/Waterfall.wav");
    HCZSetup->sfxWaterfallLoop = RSDK.GetSfx("Stage/Waterfall2.wav");
#endif
}

void HCZSetup_ScanlineCallback(ScanlineInfo *scanlines)
{
    RSDK.ProcessParallax(HCZSetup->bg);

    RSDKScreenInfo *screen = &ScreenInfo[SceneInfo->currentScreenID];
    int32 scrH           = screen->height;
    int32 scrX           = screen->position.x;
    int32 scrY           = screen->position.y;
    int32 height         = 0x210 - (scrY >> 2);
    int32 waterPos       = (Water->waterLevel >> 0x10) - scrY;

    int32 div = maxVal(1, abs(height - waterPos));
    int32 val = maxVal(0x10000, 0x640000 / div);

    height   = clampVal(height, 0, scrH);
    waterPos = clampVal(waterPos, 0, scrH);

    ScanlineInfo *scanlinePtr = &scanlines[height];
    if (height >= waterPos) {
        int32 pos = 0x4D00000;
        if (waterPos < height) {
            for (int32 i = 0; i < height - waterPos; ++i) {
                scanlinePtr->position.x = (((0x3540000 - 0xC000 * (pos >> 16)) / 100 + 0x10000) * scrX) & 0x1FFFFFF;
                scanlinePtr->position.y = pos;
                pos -= val;
                --scanlinePtr;
            }
        }
    }
    else {
        int32 pos = 0x40C0000;
        for (int32 i = 0; i < waterPos - height; ++i) {
            int32 distance            = 0x46C - (pos >> 16);
            scanlinePtr->position.x = (((-0xC000 * distance) / 100 + 0x10000) * scrX) & 0x1FFFFFF;
            scanlinePtr->position.y = pos;
            pos += val;
            ++scanlinePtr;
        }
    }
}

void HCZSetup_HandleActTransition(void)
{
    Zone_StoreEntities((Zone->cameraBoundsL[0] + ScreenInfo->centerX) << 16, Zone->cameraBoundsB[0] << 16);
    RSDK.LoadScene();
}

void HCZSetup_StageFinishCB_Act2(void)
{
    if (globals->gameMode == MODE_MANIA && (globals->playerID & 0xFF) == ID_KNUCKLES)
        RSDK.SetScene("Cutscenes", "Mirage Saloon K Intro");
    Zone_StartFadeOut(10, 0x000000);
}

#if RETRO_INCLUDE_EDITOR
void HCZSetup_EditorDraw(void) {}

void HCZSetup_EditorLoad(void) {}
#endif

void HCZSetup_Serialize(void) {}
