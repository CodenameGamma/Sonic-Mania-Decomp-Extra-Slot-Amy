#include "SonicMania.h"

ObjectGHZCutsceneST *GHZCutsceneST;

void GHZCutsceneST_Update(void)
{
    void *states[5] = { GHZCutsceneST_Cutscene_FadeIn, GHZCutsceneST_Cutscene_FinishRubyWarp, GHZCutsceneST_Cutscene_ExitHBH,
                        GHZCutsceneST_Cutscene_SetupGHZ1, NULL };

    RSDK_THIS(GHZCutsceneST);
    if (!self->activated) {
        foreach_active(Player, player)
        {
            if (Player_CheckCollisionTouch(player, self, &self->hitbox) && !player->sidekick) {
                CutsceneSeq_StartSequence((Entity *)self, states);
                if (RSDK_GET_ENTITY(SLOT_CUTSCENESEQ, CutsceneSeq)->objectID) {
                    EntityCutsceneSeq *cutsceneSeq = RSDK_GET_ENTITY(SLOT_CUTSCENESEQ, CutsceneSeq);
                    cutsceneSeq->skipType         = SKIPTYPE_CALLBACK;
                    cutsceneSeq->skipCallback      = GHZCutsceneST_SkipCB;
                }
                self->activated = true;
            }
        }
    }
}

void GHZCutsceneST_LateUpdate(void) {}

void GHZCutsceneST_StaticUpdate(void) {}

void GHZCutsceneST_Draw(void) {}

void GHZCutsceneST_Create(void *data)
{
    RSDK_THIS(GHZCutsceneST);
    if (!SceneInfo->inEditor) {
        INIT_ENTITY(self);
        CutsceneRules_SetupEntity(self, &self->size, &self->hitbox);
        self->active = ACTIVE_BOUNDS;

        GHZCutsceneST_SetupObjects();

        SceneInfo->timeEnabled = false;
    }
}

void GHZCutsceneST_StageLoad(void) {}

void GHZCutsceneST_SetupObjects(void)
{
    foreach_all(Platform, platform)
    {
        if (!platform->frameID) {
            platform->drawOrder     = Zone->drawOrderHigh - 1;
            GHZCutsceneST->platform = (Entity *)platform;
            foreach_break;
        }
    }

    foreach_all(AIZKingClaw, claw)
    {
        GHZCutsceneST->claw = (Entity *)claw;
        foreach_break;
    }

    foreach_all(PhantomRuby, phantomRuby)
    {
        GHZCutsceneST->phantomRuby = (Entity *)phantomRuby;
        foreach_break;
    }

    foreach_all(FXRuby, fxRuby)
    {
        GHZCutsceneST->fxRuby  = (Entity *)fxRuby;
        fxRuby->state          = StateMachine_None;
        fxRuby->fadeBlack      = 0x200;
        fxRuby->fadeWhite      = 0x200;
        fxRuby->outerRadius    = ScreenInfo->width;
        fxRuby->timer      = 64;
        foreach_break;
    }

    foreach_all(CutsceneHBH, cutsceneHBH) { GHZCutsceneST->cutsceneHBH[cutsceneHBH->characterID] = (Entity *)cutsceneHBH; }
}

void GHZCutsceneST_SkipCB(void)
{
#if RETRO_USE_PLUS
    if (globals->gameMode == MODE_ENCORE)
        RSDK.SetScene("Encore Mode", "Green Hill Zone+ 1");
    else
#endif
        RSDK.SetScene("Mania Mode", "Green Hill Zone 1");
}


bool32 GHZCutsceneST_Cutscene_FadeIn(EntityCutsceneSeq *host)
{
    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityPlayer *player2 = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
    RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);
    EntityFXRuby *fxRuby    = (EntityFXRuby *)GHZCutsceneST->fxRuby;
    EntityPhantomRuby *ruby = (EntityPhantomRuby *)GHZCutsceneST->phantomRuby;
    if (host->timer) {
        if (host->timer >= 60) {
            if (fxRuby->fadeBlack <= 0) {
                if (fxRuby->fadeWhite <= 0) {
                    PhantomRuby_PlaySFX(RUBYSFX_ATTACK4);
                    return true;
                }
                fxRuby->fadeWhite -= 16;
            }
            else {
                fxRuby->fadeBlack -= 16;
            }
        }
    }
    else {
        player1->position.y = ruby->position.y;
        player1->state      = Player_State_None;
        player1->stateInput = StateMachine_None;
        CutsceneSeq_LockAllPlayerControl();
        if (player2->objectID == Player->objectID) {
            player2->position.y = ruby->position.y;
            player2->state      = Player_State_None;
            player2->stateInput = StateMachine_None;
        }
    }

    int32 id = 0;
    for (int32 angle = 0; angle < 0x80; angle += 0x40) {
        EntityPlayer *player = RSDK_GET_ENTITY(id++, Player);
        if (!player || player->objectID == TYPE_BLANK)
            break;
        RSDK.SetSpriteAnimation(player->aniFrames, ANI_FAN, &player->animator, 0, 0);
        player->position.x += (player->position.x - player->position.x) >> 3;
        player->position.y += (0xA00 * RSDK.Sin256(2 * (host->timer + angle - host->storedValue2)) + ruby->position.y - player->position.y) >> 3;
        player->state = Player_State_None;
    }
    return false;
}
bool32 GHZCutsceneST_Cutscene_FinishRubyWarp(EntityCutsceneSeq *host)
{
    EntityPlayer *player1   = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityCamera *camera    = RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);
    EntityPhantomRuby *ruby = (EntityPhantomRuby *)GHZCutsceneST->phantomRuby;
    EntityFXRuby *fxRuby    = (EntityFXRuby *)GHZCutsceneST->fxRuby;
    if (!host->timer)
        fxRuby->state = FXRuby_State_ShrinkRing;

    EntityPlayer **curPlayer = &player1;
    if (fxRuby->outerRadius <= 0) {
        int32 id = 0;
        for (int32 angle = 0; angle < 0x80; angle += 0x10) {
            EntityPlayer *player = RSDK_GET_ENTITY(id++, Player);
            if (!player || player->objectID == TYPE_BLANK)
                break;
            RSDK.SetSpriteAnimation(player->aniFrames, ANI_FAN, &player->animator, false, 0);
            int32 x              = (player->position.x - player->position.x) >> 3;
            int32 y              = (0xA00 * RSDK.Sin256(2 * (angle + host->timer - host->storedValue2)) + ruby->position.y - player->position.y) >> 3;
            player->velocity.y = (y >> 8) * (y >> 8);
            player->velocity.x = (x >> 8) * (x >> 8);
            player->state      = Player_State_Air;
            player->up         = true;
            player->camera     = NULL;
            ++curPlayer;
        }
        host->values[0] = 1;
        Camera_SetupLerp(0, 0, camera->position.x, camera->position.y, 0);
        return true;
    }
    else {
        int32 id = 0;
        for (int32 angle = 0; angle < 0x80; angle += 0x10) {
            EntityPlayer *player = RSDK_GET_ENTITY(id++, Player);
            if (!player || player->objectID == TYPE_BLANK)
                break;
            RSDK.SetSpriteAnimation(player->aniFrames, ANI_FAN, &player->animator, 0, 0);
            player->position.x += (player->position.x - player->position.x) >> 3;
            player->position.y += (0xA00 * RSDK.Sin256(2 * (host->timer + angle - host->storedValue2)) + ruby->position.y - player->position.y) >> 3;
            player->state = Player_State_None;
            ++curPlayer;
        }
        return false;
    }
    return false;
}
bool32 GHZCutsceneST_Cutscene_ExitHBH(EntityCutsceneSeq *host)
{
    RSDK_GET_PLAYER(player1, player2, camera);
    unused(player2);
    EntityPhantomRuby *ruby  = (EntityPhantomRuby *)GHZCutsceneST->phantomRuby;
    EntityAIZKingClaw *claw  = (EntityAIZKingClaw *)GHZCutsceneST->claw;
    EntityPlatform *platform = (EntityPlatform *)GHZCutsceneST->platform;
    if (host->timer >= 60) {
        if (host->timer == 60) {
            int32 id = 0;
            for (int32 i = 0; i < 2; ++i) {
                EntityPlayer *player = RSDK_GET_ENTITY(id++, Player);
                if (!player || player->objectID == TYPE_BLANK)
                    break;
                player->up = false;
            }
            Music_FadeOut(0.025);
        }
        
        if (host->timer == 180) {
            player1->camera = camera;
            Camera_SetTargetEntity(0, player1);
            return true;
        }
        else {
            claw->velocity.y -= 0x1800;
            for (int32 hbhChar = 0; hbhChar < 5; ++hbhChar) {
                EntityCutsceneHBH *hbh = (EntityCutsceneHBH *)GHZCutsceneST->cutsceneHBH[hbhChar];
                switch (hbhChar) {
                    case HBH_GUNNER:
                        hbh->velocity.x += 0x800;
                        hbh->velocity.y -= 0x1800;
                        hbh->position.x += hbh->velocity.x;
                        hbh->position.y += hbh->velocity.y;
                        break;
                    case HBH_SHINOBI:
                        if (host->timer == 60) {
                            RSDK.SetSpriteAnimation(hbh->aniFrames, 3, &hbh->animator, true, 0);
                            RSDK.SetSpriteAnimation(0xFFFF, 0, &hbh->animator2, true, 0);
                        }
                        hbh->position.x -= 0x4000;
                        hbh->position.y -= 0x40000;
                        break;
                    case HBH_MYSTIC:
                        hbh->velocity.x += 0x2000;
                        hbh->velocity.y -= 0x1000;
                        hbh->position.x += hbh->velocity.x;
                        hbh->position.y += hbh->velocity.y;
                        break;
                    case HBH_RIDER:
                    case HBH_KING:
                        hbh->position.x += claw->velocity.x;
                        hbh->position.y += claw->velocity.y;
                        break;
                    default: break;
                }
            }

            ruby->position.x += claw->velocity.x;
            ruby->position.y += claw->velocity.y;
            platform->position.x += claw->velocity.x;
            platform->position.y += claw->velocity.y;
            platform->drawPos.x   = platform->position.x;
            platform->drawPos.y   = platform->position.y;
            platform->centerPos.x = platform->position.x;
            platform->centerPos.y = platform->position.y;
            claw->position.x += claw->velocity.x;
            claw->position.y += claw->velocity.y;
        }
    }
    return false;
}

bool32 GHZCutsceneST_Cutscene_SetupGHZ1(EntityCutsceneSeq *host)
{
    RSDK_THIS(GHZCutsceneST);
#if RETRO_USE_PLUS
    if (globals->gameMode == MODE_ENCORE)
        RSDK.SetScene("Encore Mode", "");
    else
#endif
        RSDK.SetScene("Mania Mode", "");
    globals->parallaxOffset[0] = self->field_68;
    EntityPlayer *player       = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    player->onGround           = true;
    player->state              = Player_State_Ground;
    Zone_StoreEntities((ScreenInfo->position.x + ScreenInfo->centerX) << 16, (ScreenInfo->height + ScreenInfo->position.y) << 16);
    RSDK.LoadScene();
    return true;
}

#if RETRO_INCLUDE_EDITOR
void GHZCutsceneST_EditorDraw(void)
{
    RSDK_THIS(GHZCutsceneST);
    CutsceneRules_DrawCutsceneBounds(self, &self->size);
}

void GHZCutsceneST_EditorLoad(void) {}
#endif

void GHZCutsceneST_Serialize(void) { RSDK_EDITABLE_VAR(GHZCutsceneST, VAR_VECTOR2, size); }
