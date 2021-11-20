#include "SonicMania.h"

ObjectHUD *HUD;

void HUD_Update(void)
{
    RSDK_THIS(HUD);
    self->enableTimeFlash = false;
    self->enableRingFlash = false;
#if RETRO_USE_PLUS
    if (self->taAnimator2.animationID == 11)
        RSDK.ProcessAnimation(&self->taAnimator2);
#endif
}

void HUD_LateUpdate(void)
{
    RSDK_THIS(HUD);
#if RETRO_USE_PLUS
    if (globals->gameMode == MODE_COMPETITION) {
        for (self->screenID = 0; self->screenID < RSDK.GetSettingsValue(SETTINGS_SCREENCOUNT); ++self->screenID) {
            StateMachine_Run(self->vsStates[self->screenID]);
        }
    }
    else {
        StateMachine_Run(self->state);
    }
#else
    StateMachine_Run(self->state);
#endif

#if RETRO_GAMEVER != VER_100
    if (globals->gameMode < MODE_TIMEATTACK) {
        EntityPlayer *player = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
        if (SceneInfo->timeEnabled && player->rings >= 50 && player->superState < SUPERSTATE_SUPER && SaveGame->saveRAM->chaosEmeralds >= 0x7F) {
            if (sku_platform == PLATFORM_PC || sku_platform == PLATFORM_SWITCH || sku_platform == PLATFORM_DEV)
                HUD_GetSuperFrames();
            if (self->superButtonPos < 0x180000)
                self->superButtonPos += 0x80000;
        }
        else {
            if (self->superButtonPos > -0x200000)
                self->superButtonPos -= 0x80000;
        }
    }
#if RETRO_USE_PLUS
    else if (globals->gameMode == MODE_TIMEATTACK) {
        if (HUD->showTAPrompt) {
            if (sku_platform == PLATFORM_PC || sku_platform == PLATFORM_SWITCH || sku_platform == PLATFORM_DEV) {
                HUD_GetKeyFrame(&self->superButtonAnimator2, KEY_Y);
                HUD_GetKeyFrame(&self->taAnimator3, KEY_Y);
                HUD_GetKeyFrame(&self->taAnimator4, KEY_START);
            }
            if (self->superButtonPos < 0x180000)
                self->superButtonPos += 0x80000;
        }
        else {
            if (self->superButtonPos > -0x400000)
                self->superButtonPos -= 0x80000;
        }
    }
#endif
#endif
}

void HUD_StaticUpdate(void) {}

void HUD_Draw(void)
{
    RSDK_THIS(HUD);
    EntityPlayer *player = RSDK_GET_ENTITY(SceneInfo->currentScreenID, Player);

    Vector2 lifePos;
    Vector2 offset[4];

    offset[HUDOFF_SCORE].x = self->offsets[HUDOFF_SCORE].x;
    offset[HUDOFF_SCORE].y = self->offsets[HUDOFF_SCORE].y;
    offset[HUDOFF_TIME].x  = self->offsets[HUDOFF_TIME].x;
    offset[HUDOFF_TIME].y  = self->offsets[HUDOFF_TIME].y;
    offset[HUDOFF_RINGS].x = self->offsets[HUDOFF_RINGS].x;
    offset[HUDOFF_RINGS].y = self->offsets[HUDOFF_RINGS].y;
    offset[HUDOFF_LIFE].x  = self->offsets[HUDOFF_LIFE].x;
    offset[HUDOFF_LIFE].y  = self->offsets[HUDOFF_LIFE].y;
    if (globals->gameMode == MODE_COMPETITION) {
#if RETRO_USE_PLUS
        offset[HUDOFF_SCORE].x = self->vsScoreOffsets[SceneInfo->currentScreenID].x;
        offset[HUDOFF_SCORE].y = self->vsScoreOffsets[SceneInfo->currentScreenID].y;
        offset[HUDOFF_TIME].x  = self->vsTimeOffsets[SceneInfo->currentScreenID].x;
        offset[HUDOFF_TIME].y  = self->vsTimeOffsets[SceneInfo->currentScreenID].y;
        offset[HUDOFF_RINGS].x = self->vsRingsOffsets[SceneInfo->currentScreenID].x;
        offset[HUDOFF_RINGS].y = self->vsRingsOffsets[SceneInfo->currentScreenID].y;
        offset[HUDOFF_LIFE].x  = self->vsLifeOffsets[SceneInfo->currentScreenID].x;
        offset[HUDOFF_LIFE].y  = self->vsLifeOffsets[SceneInfo->currentScreenID].y;
#endif

        foreach_active(Player, plr)
        {
            if (plr != player) {
                self->playerIDAnimator.frameID = plr->playerID;
                RSDK.DrawSprite(&self->playerIDAnimator, &plr->position, false);
            }
        }
    }
#if RETRO_USE_PLUS
    else {
        if (HUD->swapCooldown > 0) {
            RSDK.ProcessAnimation(&self->playerIDAnimator);
            RSDK.DrawSprite(&self->playerIDAnimator, &player->position, false);
            --HUD->swapCooldown;
        }
    }
#endif

    if (player->rings)
        self->ringFlashFrame = 0;
    else
        self->ringFlashFrame = (Zone->timer2 >> 3) & 1;

#if RETRO_GAMEVER != VER_100
#if RETRO_USE_PLUS
    self->timeFlashFrame = 0;
    if ((SceneInfo->minutes == 9 && isMainGameMode() && !(globals->medalMods & getMod(MEDAL_NOTIMEOVER))) && ActClear->disableTimeBonus)
        self->timeFlashFrame = (Zone->timer2 >> 3) & 1;
#else
    if (SceneInfo->minutes == 9 && globals->gameMode < MODE_TIMEATTACK)
        self->timeFlashFrame = (Zone->timer2 >> 3) & 1;
#endif
#endif

    self->hudElementsAnimator.frameID = 0;
    RSDK.DrawSprite(&self->hudElementsAnimator, &offset[HUDOFF_SCORE], true);

    lifePos.x = offset[HUDOFF_SCORE].x + 0x610000;
    lifePos.y = offset[HUDOFF_SCORE].y + 0xE0000;
    HUD_DrawNumbersBase10(&lifePos, player->score, 0);
    self->hudElementsAnimator.frameID = self->timeFlashFrame + 1;
    RSDK.DrawSprite(&self->hudElementsAnimator, &offset[HUDOFF_TIME], true);

    if (!self->enableTimeFlash || Zone->timer2 & 8) {
        lifePos.x                       = offset[HUDOFF_TIME].x + 0x340000;
        lifePos.y                       = offset[HUDOFF_TIME].y - 0x20000;
        self->hudElementsAnimator.frameID = 12;
        RSDK.DrawSprite(&self->hudElementsAnimator, &lifePos, true);

        lifePos.x = offset[HUDOFF_TIME].x + 0x610000;
        lifePos.y = offset[HUDOFF_TIME].y + 0xE0000;
        HUD_DrawNumbersBase10(&lifePos, SceneInfo->milliseconds, 2);

        lifePos.x -= 0x90000;
        HUD_DrawNumbersBase10(&lifePos, SceneInfo->seconds, 2);

        lifePos.x -= 0x90000;

#if RETRO_USE_PLUS
        if (SceneInfo->minutes > 9 && globals->medalMods & getMod(MEDAL_NOTIMEOVER))
            HUD_DrawNumbersBase10(&lifePos, SceneInfo->minutes, 2);
        else
#endif
            HUD_DrawNumbersBase10(&lifePos, SceneInfo->minutes, 1);
    }

    self->hudElementsAnimator.frameID = self->ringFlashFrame + 3;
    RSDK.DrawSprite(&self->hudElementsAnimator, &offset[HUDOFF_RINGS], true);
    if (!self->enableRingFlash || Zone->timer2 & 8) {
        lifePos.x = offset[HUDOFF_RINGS].x + 0x610000;
        lifePos.y = offset[HUDOFF_RINGS].y + 0xE0000;

        if (player->hyperRing)
            HUD_DrawNumbersHyperRing(&lifePos, player->rings);
        else
            HUD_DrawNumbersBase10(&lifePos, player->rings, 0);
    }

    if (RSDK_GET_ENTITY(SLOT_PLAYER1, Player)->objectID == DebugMode->objectID) {
        if (player->camera) {
            lifePos.y = 0x180000;
            lifePos.x = (ScreenInfo[player->camera->screenID].width - 16) << 16;
            HUD_DrawNumbersBase16(&lifePos, ScreenInfo[player->camera->screenID].position.y);

            lifePos.x -= 0x90000;
            HUD_DrawNumbersBase16(&lifePos, ScreenInfo[player->camera->screenID].position.x);

            lifePos.y += 0x100000;
            lifePos.x = (ScreenInfo[player->camera->screenID].width - 16) << 16;
            HUD_DrawNumbersBase16(&lifePos, (player->position.y >> 0x10));

            lifePos.x -= 0x90000;
            HUD_DrawNumbersBase16(&lifePos, (player->position.x >> 0x10));
        }
    }
#if RETRO_USE_PLUS
    else if (self->superButtonPos > -0x400000 && globals->gameMode == MODE_TIMEATTACK) {
        // TA Save Icon
        lifePos.y = 0x140000;
        lifePos.x = (ScreenInfo[SceneInfo->currentScreenID].width << 16) - self->superButtonPos;

        if (API.CheckDLC(DLC_PLUS)) {
            RSDK.DrawSprite(&self->taAnimator2, &lifePos, true);
            lifePos.x -= 0x1C0000;

            if (HUD->replaySaveEnabled) {
                RSDK.DrawSprite(&self->taAnimator3, &lifePos, true);
            }
            else {
                self->inkEffect = INK_BLEND;
                RSDK.DrawSprite(&self->taAnimator3, &lifePos, true);
                self->inkEffect = INK_NONE;
            }

            lifePos.y += 0x1C0000;
            lifePos.x = (ScreenInfo[SceneInfo->currentScreenID].width << 16) - self->superButtonPos;
        }
        RSDK.DrawSprite(&self->taAnimator1, &lifePos, true);
        lifePos.x -= 0x1C0000;
        RSDK.DrawSprite(&self->taAnimator4, &lifePos, true);
    }
#endif
#if RETRO_GAMEVER != VER_100
    else if (self->superButtonPos > -0x200000) {
        lifePos.y = 0x140000;
        lifePos.x = (ScreenInfo[SceneInfo->currentScreenID].width << 16) - self->superButtonPos;
        RSDK.DrawSprite(&self->superButtonAnimator1, &lifePos, true);
        lifePos.x -= 0x140000;
        bool32 flag = true;
#if RETRO_USE_PLUS
        if (Player->canSuperCB)
            flag = Player->canSuperCB(true);
#endif
        if (player->state == Player_State_Air && player->jumpAbilityTimer == 1 && flag) {
            RSDK.DrawSprite(&self->superButtonAnimator2, &lifePos, true);
        }
        else {
            self->inkEffect = INK_BLEND;
            RSDK.DrawSprite(&self->superButtonAnimator2, &lifePos, true);
            self->inkEffect = INK_NONE;
        }
    }
#endif

    int32 cID    = -1;
    lifePos.x  = offset[HUDOFF_LIFE].x;
    lifePos.y  = offset[HUDOFF_LIFE].y;
    int32 charID = player->characterID;
#if RETRO_USE_PLUS
    int32 lives = self->lives[player->playerID];
    for (; charID > 0; ++cID) charID >>= 1;
    self->lifeIconAnimator.frameID = cID;
    if (cID < 0) {
        self->lifeIconAnimator.frameID = self->lifeFrameIDs[player->playerID];
        lives                         = lives - 1;
    }
    else {
        self->lifeFrameIDs[player->playerID] = cID;
        self->lives[player->playerID]        = player->lives;
    }
#else
    switch (player->characterID) {
        default:
        case ID_SONIC: self->lifeIconAnimator.frameID = 0; break;
        case ID_TAILS: self->lifeIconAnimator.frameID = 1; break;
        case ID_KNUCKLES: self->lifeIconAnimator.frameID = 2; break;
    }
#endif
    RSDK.DrawSprite(&self->lifeIconAnimator, &lifePos, true);

#if RETRO_USE_PLUS
    if (globals->gameMode == MODE_ENCORE) {
        for (int32 p = 0; p < PLAYER_MAX; ++p) {
            if (HUD->stockFlashTimers[p] > 0)
                HUD->stockFlashTimers[p]--;
        }

        lifePos.x += 0x140000;
        EntityPlayer *sidekick = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
        if (sidekick->objectID) {
            int32 charID = sidekick->characterID;
            int32 id     = -1;
            for (id = -1; charID > 0; ++id) charID >>= 1;
            self->lifeIconAnimator.frameID = id;
            if (id >= 0 && !(HUD->stockFlashTimers[0] & 4)) {
                if ((sidekick->state != Player_State_Die && sidekick->state != Player_State_Drown && sidekick->state != Player_State_EncoreRespawn)
                    || !sidekick->abilityValues[0]) {
                    RSDK.DrawSprite(&self->lifeIconAnimator, &lifePos, true);
                }
            }
            lifePos.x += 0x140000;
            RSDK.SetSpriteAnimation(HUD->aniFrames, 12, &self->lifeIconAnimator, true, 0);

            for (int32 i = 0; i < 3; ++i) {
                id        = -1;
                int32 stock = (globals->stock >> (i * 8)) & 0xFF;
                if (stock) {
                    do {
                        stock >>= 1;
                        ++id;
                    } while (stock > 0);
                }
                self->lifeIconAnimator.frameID = id;
                if (id >= 0 && !(HUD->stockFlashTimers[i + 1] & 4))
                    RSDK.DrawSprite(&self->lifeIconAnimator, &lifePos, true);

                lifePos.x += 0x100000;
            }

            RSDK.SetSpriteAnimation(HUD->aniFrames, 2, &self->lifeIconAnimator, true, 0);
        }
    }
    else {
        self->hudElementsAnimator.frameID = 14;
        RSDK.DrawSprite(&self->hudElementsAnimator, &lifePos, true);
        lifePos.x += 0x300000;
        if (player->lives < 10)
            lifePos.x -= 0x80000;
        HUD_DrawNumbersBase10(&lifePos, lives, 0);
    }
#else
    self->hudElementsAnimator.frameID = 14;
    RSDK.DrawSprite(&self->hudElementsAnimator, &lifePos, true);
    lifePos.x += 0x300000;
    if (player->lives < 10)
        lifePos.x -= 0x80000;
    HUD_DrawNumbersBase10(&lifePos, player->lives, 0);
#endif

    if (globals->gameMode == MODE_COMPETITION) {
#if RETRO_USE_PLUS
        switch (HUD->compSession[SceneInfo->currentScreenID]) {
            case 1:
                RSDK.DrawRect(ScreenInfo->width - 1, 0, 1, ScreenInfo->height, 0, 255, INK_NONE, true);
                RSDK.DrawRect(0, ScreenInfo->height - 1, ScreenInfo->width, 1, 0, 255, INK_NONE, true);
                break;
            case 2: RSDK.DrawRect(0, ScreenInfo->height - 1, ScreenInfo->width, 1, 0, 255, INK_NONE, true); break;
            case 3:
                RSDK.DrawRect(0, 0, 1, ScreenInfo[1].height, 0, 255, INK_NONE, true);
                RSDK.DrawRect(0, ScreenInfo->height - 1, ScreenInfo->width, 1, 0, 255, INK_NONE, true);
                break;
            case 4:
                RSDK.DrawRect(ScreenInfo->width - 1, 0, 1, ScreenInfo->height, 0, 255, INK_NONE, true);
                RSDK.DrawRect(0, 0, ScreenInfo[1].width, 1, 0, 255, INK_NONE, true);
                break;
            case 5: RSDK.DrawRect(0, 0, ScreenInfo[1].width, 1, 0, 255, INK_NONE, true); break;
            case 6:
                RSDK.DrawRect(0, 0, 1, ScreenInfo[1].height, 0, 255, INK_NONE, true);
                RSDK.DrawRect(0, 0, ScreenInfo[1].width, 1, 0, 255, INK_NONE, true);
                break;
            default: break;
        }
#else
        switch (SceneInfo->currentScreenID) {
            default: break;
            case 0: RSDK.DrawRect(0, ScreenInfo->height - 1, ScreenInfo->width, 1, 0, 255, INK_NONE, true); break;
            case 1: RSDK.DrawRect(0, 0, ScreenInfo[1].width, 1, 0, 255, INK_NONE, true); break;
        }
#endif
    }
}

void HUD_Create(void *data)
{
    RSDK_THIS(HUD);
    if (!SceneInfo->inEditor) {
#if RETRO_USE_PLUS
        ActClear->disableTimeBonus = false;
#endif
        self->active                  = ACTIVE_NORMAL;
        self->visible                 = true;
        self->drawOrder               = Zone->hudDrawOrder;
        self->offsets[HUDOFF_SCORE].x = 0x100000;
        self->offsets[HUDOFF_SCORE].y = 0xC0000;
        self->offsets[HUDOFF_TIME].x  = 0x100000;
        self->offsets[HUDOFF_TIME].y  = 0x1C0000;
        self->offsets[HUDOFF_RINGS].x = 0x100000;
        self->offsets[HUDOFF_RINGS].y = 0x2C0000;
        self->offsets[HUDOFF_LIFE].x  = 0x100000;
        self->offsets[HUDOFF_LIFE].y  = (ScreenInfo->height - 12) << 16;
#if RETRO_GAMEVER != VER_100
        self->superButtonPos = -0x200000;
#endif

#if RETRO_USE_PLUS
        for (int32 i = 0; i < SCREEN_MAX; ++i) {
            self->vsScoreOffsets[i].x = self->offsets[HUDOFF_SCORE].x;
            self->vsScoreOffsets[i].y = self->offsets[HUDOFF_SCORE].y;
            self->vsTimeOffsets[i].x  = self->offsets[HUDOFF_TIME].x;
            self->vsTimeOffsets[i].y  = self->offsets[HUDOFF_TIME].y;
            self->vsRingsOffsets[i].x = self->offsets[HUDOFF_RINGS].x;
            self->vsRingsOffsets[i].y = self->offsets[HUDOFF_RINGS].y;
            self->vsLifeOffsets[i].x  = self->offsets[HUDOFF_LIFE].x;
            self->vsLifeOffsets[i].y  = self->offsets[HUDOFF_LIFE].y;
        }
#endif
        RSDK.SetSpriteAnimation(HUD->aniFrames, 0, &self->hudElementsAnimator, true, 0);
        RSDK.SetSpriteAnimation(HUD->aniFrames, 1, &self->numbersAnimator, true, 0);
        RSDK.SetSpriteAnimation(HUD->aniFrames, 9, &self->hyperNumbersAnimator, true, 0);
        RSDK.SetSpriteAnimation(HUD->aniFrames, 2, &self->lifeIconAnimator, true, 0);
#if RETRO_USE_PLUS
        if (globals->gameMode == MODE_ENCORE)
            RSDK.SetSpriteAnimation(HUD->aniFrames, 13, &self->playerIDAnimator, true, 0);
        else
            RSDK.SetSpriteAnimation(HUD->aniFrames, 8, &self->playerIDAnimator, true, 0);
        RSDK.SetSpriteAnimation(HUD->aniFrames, 10, &self->taAnimator1, true, 2);
        RSDK.SetSpriteAnimation(HUD->aniFrames, 10, &self->taAnimator2, true, 1);
#else
        RSDK.SetSpriteAnimation(HUD->aniFrames, 8, &self->playerIDAnimator, true, 0);
#endif
#if RETRO_GAMEVER != VER_100
        RSDK.SetSpriteAnimation(HUD->superButtonFrames, 0, &self->superButtonAnimator1, true, 0);
        HUD_GetSuperFrames();
#endif
#if RETRO_USE_PLUS
        RSDK.SetDebugValue("Show HUD", &self->visible, DTYPE_UINT8, false, true);
#endif
    }
}

void HUD_StageLoad(void)
{
    HUD->aniFrames = RSDK.LoadSpriteAnimation("Global/HUD.bin", SCOPE_STAGE);
#if RETRO_GAMEVER != VER_100
    HUD->superButtonFrames = RSDK.LoadSpriteAnimation("Global/SuperButtons.bin", SCOPE_STAGE);
#endif

#if RETRO_USE_PLUS
    HUD->sfxClick    = RSDK.GetSFX("Stage/Click.wav");
    HUD->sfxStarpost = RSDK.GetSFX("Global/StarPost.wav");
    HUD->showTAPrompt      = false;

    EntityCompetitionSession *session = (EntityCompetitionSession *)globals->competitionSession;
    if (globals->gameMode == MODE_COMPETITION) {
        HUD->compSession[0] = session->unknown84;
        HUD->compSession[1] = session->unknown85;
        HUD->compSession[2] = session->splitScreenMode;
        HUD->compSession[3] = session->unknown87;
    }
#endif
}

void HUD_DrawNumbersBase10(Vector2 *drawPos, int32 value, int32 maxDigits)
{
    RSDK_THIS(HUD);
    int32 mult = 1;
    if (!maxDigits) {
        if (value <= 0) {
            maxDigits = 1;
        }
        else {
            int32 v = value;
            do {
                ++maxDigits;
                v /= 10;
            } while (v > 0);
        }
    }

    while (maxDigits--) {
        self->numbersAnimator.frameID = value / mult % 10;
        RSDK.DrawSprite(&self->numbersAnimator, drawPos, true);
        mult *= 10;
        drawPos->x -= 0x80000;
    }
}

void HUD_DrawNumbersBase16(Vector2 *drawPos, int32 value)
{
    RSDK_THIS(HUD);
    int32 mult = 1;
    for (int32 i = 4; i; --i) {
        self->numbersAnimator.frameID = value / mult & 0xF;
        RSDK.DrawSprite(&self->numbersAnimator, drawPos, true);
        drawPos->x -= 0x80000;
        mult *= 16;
    }
}

void HUD_DrawNumbersHyperRing(Vector2 *drawPos, int32 value)
{
    RSDK_THIS(HUD);
    int32 cnt   = 0;
    int32 mult  = 1;
    int32 mult2 = 1;
    if (value <= 0) {
        cnt = 1;
    }
    else {
        int32 v = value;
        while (v) {
            ++cnt;
            v /= 10;
        }
    }

    if (cnt > 0) {
        while (cnt--) {
            self->hyperNumbersAnimator.frameID = value / mult % 10;
            RSDK.DrawSprite(&self->hyperNumbersAnimator, drawPos, true);
            drawPos->x -= 0x80000;
            mult = 10 * mult2;
            mult2 *= 10;
        }
    }

    self->hyperNumbersAnimator.frameID = 10;
    drawPos->x -= 0x40000;
    RSDK.DrawSprite(&self->hyperNumbersAnimator, drawPos, true);
}

#if RETRO_GAMEVER != VER_100
void HUD_GetKeyFrame(Animator *animator, int32 buttonID)
{
    int32 val = UIButtonPrompt_GetGamepadType();
    if (API_GetConfirmButtonFlip && buttonID <= 1)
        buttonID ^= 1;
    if (val != 1 && (val <= 8 || val > 12)) {
        RSDK.SetSpriteAnimation(HUD->superButtonFrames, val, animator, true, buttonID);
    }
    else {
        EntityPlayer *player = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
#if RETRO_USE_PLUS
        int32 id = RSDK.ControllerIDForInputID(player->controllerID);
#else
        int32 id                 = CONT_ANY;
#endif
        int32 contID = player->controllerID;
        if (id == CONT_UNASSIGNED)
            contID = CONT_P1;

        int32 map = 0;
        switch (buttonID) {
            default: break;
            case 0: map = ControllerInfo[contID].keyA.keyMap; break;
            case 1: map = ControllerInfo[contID].keyB.keyMap; break;
            case 2: map = ControllerInfo[contID].keyX.keyMap; break;
            case 3: map = ControllerInfo[contID].keyY.keyMap; break;
            case 4: map = ControllerInfo[contID].keyStart.keyMap; break;
        }

        int32 frame = UIButtonPrompt_MappingsToFrame(map);
        RSDK.SetSpriteAnimation(HUD->superButtonFrames, 1, animator, true, frame);
    }
}
void HUD_GetSuperFrames(void)
{
    RSDK_THIS(HUD);
    HUD_GetKeyFrame(&self->superButtonAnimator2, KEY_Y);
#if RETRO_USE_PLUS
    HUD_GetKeyFrame(&self->taAnimator3, KEY_Y);
    HUD_GetKeyFrame(&self->taAnimator4, KEY_START);
#endif
}
#endif

void HUD_State_ComeOnScreen(void)
{
    RSDK_THIS(HUD);
#if RETRO_USE_PLUS
    Vector2 *offset[4];
    void **state = NULL;

    int32 *max = NULL;
    if (globals->gameMode == MODE_COMPETITION) {
        state                = (void **)&self->vsStates[self->screenID];
        offset[HUDOFF_SCORE] = &self->vsScoreOffsets[SceneInfo->currentScreenID];
        offset[HUDOFF_TIME]  = &self->vsTimeOffsets[SceneInfo->currentScreenID];
        offset[HUDOFF_RINGS] = &self->vsRingsOffsets[SceneInfo->currentScreenID];
        offset[HUDOFF_LIFE]  = &self->vsLifeOffsets[SceneInfo->currentScreenID];
        max                  = &self->vsMaxOffsets[SceneInfo->currentScreenID];
    }
    else {
        state                = (void **)&self->state;
        offset[HUDOFF_SCORE] = &self->offsets[HUDOFF_SCORE];
        offset[HUDOFF_TIME]  = &self->offsets[HUDOFF_TIME];
        offset[HUDOFF_RINGS] = &self->offsets[HUDOFF_RINGS];
        offset[HUDOFF_LIFE]  = &self->offsets[HUDOFF_LIFE];
        max                  = &self->maxOffset;
    }
    if (offset[HUDOFF_SCORE]->x < *max)
        offset[HUDOFF_SCORE]->x += 0x80000;
    if (offset[HUDOFF_TIME]->x < *max)
        offset[HUDOFF_TIME]->x += 0x80000;
    if (offset[HUDOFF_RINGS]->x < *max)
        offset[HUDOFF_RINGS]->x += 0x80000;
    if (offset[HUDOFF_LIFE]->x < *max)
        offset[HUDOFF_LIFE]->x += 0x80000;
    else
        *state = NULL;
#else
    if (self->offsets[HUDOFF_SCORE].x < self->maxOffset)
        self->offsets[HUDOFF_SCORE].x += 0x80000;

    if (self->offsets[HUDOFF_TIME].x < self->maxOffset)
        self->offsets[HUDOFF_TIME].x += 0x80000;

    if (self->offsets[HUDOFF_RINGS].x < self->maxOffset)
        self->offsets[HUDOFF_RINGS].x += 0x80000;

    if (self->offsets[HUDOFF_LIFE].x < self->maxOffset)
        self->offsets[HUDOFF_LIFE].x += 0x80000;
    else
        self->state = StateMachine_None;
#endif
}

void HUD_State_GoOffScreen(void)
{
    RSDK_THIS(HUD);

#if RETRO_USE_PLUS
    Vector2 *offset[4];
    void **statePtr = NULL;

    if (globals->gameMode == MODE_COMPETITION) {
        statePtr             = (void **)&self->vsStates[self->screenID];
        offset[HUDOFF_SCORE] = &self->vsScoreOffsets[self->screenID];
        offset[HUDOFF_TIME]  = &self->vsTimeOffsets[self->screenID];
        offset[HUDOFF_RINGS] = &self->vsRingsOffsets[self->screenID];
        offset[HUDOFF_LIFE]  = &self->vsLifeOffsets[self->screenID];
    }
    else {
        statePtr             = (void **)&self->state;
        offset[HUDOFF_SCORE] = &self->offsets[HUDOFF_SCORE];
        offset[HUDOFF_TIME]  = &self->offsets[HUDOFF_TIME];
        offset[HUDOFF_RINGS] = &self->offsets[HUDOFF_RINGS];
        offset[HUDOFF_LIFE]  = &self->offsets[HUDOFF_LIFE];
    }
    offset[HUDOFF_SCORE]->x -= 0x80000;
    if (offset[HUDOFF_TIME]->x - offset[HUDOFF_SCORE]->x > 0x100000)
        offset[HUDOFF_TIME]->x -= 0x80000;
    if (offset[HUDOFF_RINGS]->x - offset[HUDOFF_TIME]->x > 0x100000)
        offset[HUDOFF_RINGS]->x -= 0x80000;
    if (offset[HUDOFF_LIFE]->x - offset[HUDOFF_RINGS]->x > 0x100000)
        offset[HUDOFF_LIFE]->x -= 0x80000;

    if (offset[HUDOFF_LIFE]->x < -0x500000) {
        if (globals->gameMode == MODE_COMPETITION) {
            *statePtr = StateMachine_None;
            Competition_CalculateScore(self->screenID, 1);
            EntityGameOver *gameOver   = RSDK_GET_ENTITY(self->screenID + Player->playerCount, GameOver);
            EntityCompetition *manager = (EntityCompetition *)Competition->activeEntity;
            if (!manager || manager->timer) {
                RSDK.ResetEntityPtr(gameOver, GameOver->objectID, intToVoid(false));
                gameOver->playerID = self->screenID;
            }
            else {
                RSDK.ResetEntityPtr(gameOver, GameOver->objectID, intToVoid(true));
                RSDK.SetGameMode(ENGINESTATE_FROZEN);
                SceneInfo->timeEnabled = false;
                gameOver->playerID          = self->screenID;
            }
        }
        else {
            destroyEntity(self);
        }
    }
#else
    self->offsets[HUDOFF_SCORE].x -= 0x80000;
    if (self->offsets[HUDOFF_TIME].x - self->offsets[HUDOFF_SCORE].x > 0x100000)
        self->offsets[HUDOFF_TIME].x -= 0x80000;
    if (self->offsets[HUDOFF_RINGS].x - self->offsets[HUDOFF_TIME].x > 0x100000)
        self->offsets[HUDOFF_RINGS].x -= 0x80000;
    if (self->offsets[HUDOFF_LIFE].x - self->offsets[HUDOFF_RINGS].x > 0x100000)
        self->offsets[HUDOFF_LIFE].x -= 0x80000;

    if (self->offsets[HUDOFF_LIFE].x < -0x400000)
        destroyEntity(self);
#endif
}

#if RETRO_INCLUDE_EDITOR
void HUD_EditorDraw(void)
{
    RSDK_THIS(HUD);
    RSDK.SetSpriteAnimation(HUD->aniFrames, 0, &self->hudElementsAnimator, true, 0);
    RSDK.DrawSprite(&self->hudElementsAnimator, NULL, false);
}

void HUD_EditorLoad(void) { HUD->aniFrames = RSDK.LoadSpriteAnimation("Editor/EditorIcons.bin", SCOPE_STAGE); }
#endif

void HUD_Serialize(void) {}
