// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: OOZ1Outro Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectOOZ1Outro *OOZ1Outro;

void OOZ1Outro_Update(void)
{
    RSDK_THIS(OOZ1Outro);

    if (isMainGameMode() && globals->enableIntro && !PlayerHelpers_CheckStageReload()) {
        self->activated = true;
        CutsceneSeq_StartSequence(self, OOZ1Outro_Cutscene_FadeIn, OOZ1Outro_Cutscene_PostActClearSetup, OOZ1Outro_Cutscene_FallIntoAct2, OOZ1Outro_Cutscene_BeginAct, StateMachine_None);
    }
    self->active = ACTIVE_NEVER;
}

void OOZ1Outro_LateUpdate(void) {}

void OOZ1Outro_StaticUpdate(void) {}

void OOZ1Outro_Draw(void) {}

void OOZ1Outro_Create(void *data)
{
    RSDK_THIS(OOZ1Outro);
    if (!SceneInfo->inEditor) {
        self->active  = ACTIVE_NORMAL;
        self->visible = false;
        if (!self->size.x)
            self->size.x = 0x1A80000;
        if (!self->size.y)
            self->size.y = 0xF00000;
        self->updateRange.x += self->size.x;
        self->updateRange.y += self->size.y;

        self->hitbox.left   = -self->size.x >> 17;
        self->hitbox.right  = self->size.x >> 17;
        self->hitbox.top    = -self->size.y >> 17;
        self->hitbox.bottom = self->size.y >> 17;
    }
}

void OOZ1Outro_StageLoad(void) { OOZ1Outro->sfxDrop = RSDK.GetSfx("Stage/Drop.wav"); }

bool32 OOZ1Outro_Cutscene_FadeIn(EntityCutsceneSeq *host)
{
    RSDK_THIS(OOZ1Outro);

    if (host->timer) {
        if (host->fadeWhite > 0)
            host->fadeWhite -= 4;
    }
    else {
        host->fadeWhite = 512;
    }

    if (host->timer == 1) {
        foreach_all(Player, player)
        {
            player->position.x = self->position.x;
            player->position.y = self->position.y;
            player->position.y += (self->size.y >> 1) - 0x400000;
        }

        self->boundsR         = Zone->cameraBoundsR[0];
        self->boundsT         = Zone->cameraBoundsT[0];
        self->boundsB         = Zone->cameraBoundsB[0];
        Zone->cameraBoundsL[0] = (self->position.x >> 16) - ScreenInfo->centerX;
        Zone->cameraBoundsR[0] = (self->position.x >> 16) + ScreenInfo->centerX;
        Zone->cameraBoundsT[0] = (self->position.y >> 16) - ScreenInfo->centerY;
        Zone->cameraBoundsB[0] = (self->position.y >> 16) + ScreenInfo->centerY;
        EntityCamera *camera    = RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);
        camera->boundsL         = Zone->cameraBoundsL[0];
        camera->boundsR         = Zone->cameraBoundsR[0];
        camera->boundsT         = Zone->cameraBoundsT[0];
        camera->boundsB         = Zone->cameraBoundsB[0];
        Smog->forceEnabled      = true;
    }

    if (host->timer == 30) {
        ActClear->displayedActID = 1;

        foreach_active(SignPost, signpost)
        {
            signpost->state  = SignPost_State_Fall;
            signpost->active = ACTIVE_NORMAL;
        }

        RSDK.PlaySfx(SignPost->sfxTwinkle, false, 0xFF);
        return true;
    }
    return false;
}

bool32 OOZ1Outro_Cutscene_PostActClearSetup(EntityCutsceneSeq *host)
{
    RSDK_THIS(OOZ1Outro);
    if (host->fadeWhite > 0)
        host->fadeWhite -= 4;

    CutsceneSeq_LockAllPlayerControl();
    if (ActClear->finished) {
        foreach_active(Player, player)
        {
            player->state          = Player_State_Air;
            player->onGround       = false;
            player->collisionPlane = 1;
            player->interaction    = false;
            player->velocity.y     = -0x20000;
            player->stateInput     = StateMachine_None;
            RSDK.SetSpriteAnimation(player->aniFrames, ANI_HURT, &player->animator, false, 0);
        }
        Zone->cameraBoundsL[0] = self->boundsL;
        Zone->cameraBoundsR[0] = self->boundsR;
        Zone->cameraBoundsT[0] = self->boundsT;
        Zone->cameraBoundsB[0] = self->boundsB;
#if RETRO_USE_PLUS
        if (RSDK_GET_ENTITY(SLOT_CUTSCENESEQ, CutsceneSeq)->objectID)
            RSDK_GET_ENTITY(SLOT_CUTSCENESEQ, CutsceneSeq)->skipType = SKIPTYPE_RELOADSCN;
#endif
        return true;
    }
    return false;
}

bool32 OOZ1Outro_Cutscene_FallIntoAct2(EntityCutsceneSeq *host)
{
    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityPlayer *player2 = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
    if (host->timer == 48) {
        player1->interaction = true;
        if (player2->objectID == Player->objectID)
            player2->interaction = true;
    }
    if (Smog->forceEnabled && player1->animator.animationID)
        return false;
    player1->direction  = FLIP_NONE;
    player1->stateInput = Player_ProcessP1Input;
    if (player2->objectID == Player->objectID)
        player2->stateInput = Player_ProcessP2Input_AI;
    return true;
}

bool32 OOZ1Outro_Cutscene_BeginAct(EntityCutsceneSeq *host)
{
    if (host->timer == 120) {
        globals->suppressTitlecard = true;
        foreach_all(TitleCard, titlecard)
        {
            titlecard->active    = ACTIVE_NORMAL;
            titlecard->state     = TitleCard_State_Initial;
            titlecard->stateDraw = TitleCard_Draw_SlideIn;
            foreach_break;
        }

        Music_PlayTrack(TRACK_STAGE);
        return true;
    }
    return false;
}

void OOZ1Outro_EditorDraw(void)
{
    RSDK_THIS(OOZ1Outro);
    Vector2 drawPos;

    drawPos.x = self->position.x;
    drawPos.y = self->position.y;
    drawPos.x -= self->size.x >> 1;
    drawPos.y -= self->size.y >> 1;
    RSDK.DrawLine(drawPos.x, drawPos.y, drawPos.x + self->size.x, drawPos.y, 0xFFFF00, 0, INK_NONE, false);
    RSDK.DrawLine(drawPos.x, self->size.y + drawPos.y, drawPos.x + self->size.x, self->size.y + drawPos.y, 0xFFFF00, 0, INK_NONE, false);
    RSDK.DrawLine(drawPos.x, drawPos.y, drawPos.x, drawPos.y + self->size.y, 0xFFFF00, 0, INK_NONE, false);
    RSDK.DrawLine(drawPos.x + self->size.x, drawPos.y, drawPos.x + self->size.x, drawPos.y + self->size.y, 0xFFFF00, 0, INK_NONE, false);
}

void OOZ1Outro_EditorLoad(void) {}

void OOZ1Outro_Serialize(void) { RSDK_EDITABLE_VAR(OOZ1Outro, VAR_VECTOR2, size); }
