#include "SonicMania.h"

ObjectMSZ1KIntro *MSZ1KIntro;

void MSZ1KIntro_Update(void)
{
    RSDK_THIS(MSZ1KIntro);
    if (!self->activated) {
        if (isMainGameMode() || !globals->enableIntro || PlayerHelpers_CheckStageReload()) {
            self->active = ACTIVE_NEVER;
        }
        else {
            void *states[] = { MSZ1KIntro_CutsceneState_Unknown1, MSZ1KIntro_CutsceneState_Unknown2, MSZ1KIntro_CutsceneState_Unknown3, NULL };

            self->activated = true;
            CutsceneSeq_StartSequence((Entity *)self, states);
            if (RSDK_GET_ENTITY(SLOT_CUTSCENESEQ, CutsceneSeq)->objectID)
                RSDK_GET_ENTITY(SLOT_CUTSCENESEQ, CutsceneSeq)->skipType = SKIPTYPE_RELOADSCN;
        }
    }
}

void MSZ1KIntro_LateUpdate(void) {}

void MSZ1KIntro_StaticUpdate(void) {}

void MSZ1KIntro_Draw(void) {}

void MSZ1KIntro_Create(void *data)
{
    RSDK_THIS(MSZ1KIntro);

    INIT_ENTITY(self);
    CutsceneRules_SetupEntity(self, &self->size, &self->hitbox);
    self->active = ACTIVE_NORMAL;
    if (!globals->suppressTitlecard)
        destroyEntity(self);
}

void MSZ1KIntro_StageLoad(void)
{
    MSZ1KIntro->playerFrames = RSDK.LoadSpriteAnimation("Players/KnuxCutsceneAIZ.bin", SCOPE_STAGE);
    MSZ1KIntro->sfxImpact    = RSDK.GetSFX("Stage/Impact3.wav");
}

bool32 MSZ1KIntro_CutsceneState_Unknown1(EntityCutsceneSeq *host)
{
    RSDK_GET_PLAYER(player1, player2, camera);
    unused(player2);
    unused(camera);

    if (!host->timer) {
        player1->position.y -= 0x4000000;
        player1->state           = Player_State_Air;
        player1->nextGroundState = StateMachine_None;
        player1->nextAirState    = StateMachine_None;
        player1->onGround        = false;
        player1->velocity.y      = 0x80000;
        player1->stateInput      = StateMachine_None;
        CutsceneSeq_LockAllPlayerControl();
    }

    RSDK.SetSpriteAnimation(MSZ1KIntro->playerFrames, 4, &player1->animator, false, 0);
    if (player1->onGround) {
        RSDK.SetSpriteAnimation(MSZ1KIntro->playerFrames, 5, &player1->animator, true, 0);
        player1->state = Player_State_None;
        return true;
    }
    return false;
}

bool32 MSZ1KIntro_CutsceneState_Unknown2(EntityCutsceneSeq *host)
{
    RSDK_GET_PLAYER(player1, player2, camera);
    unused(player2);
    unused(camera);

    if (!host->timer) {
        RSDK.SetSpriteAnimation(MSZ1KIntro->playerFrames, 5, &player1->animator, true, 0);
        player1->state          = Player_State_None;
        player1->tileCollisions = false;
    }
    if (player1->animator.animationID == 5 && player1->animator.frameID == 1 && !host->values[0]) {
        RSDK.PlaySfx(MSZ1KIntro->sfxImpact, false, 255);
        Camera_ShakeScreen(0, 0, 4);
        host->values[0] = 1;
    }
    if (host->timer == 60)
        RSDK.SetSpriteAnimation(MSZ1KIntro->playerFrames, 7, &player1->animator, false, 0);
    if (host->timer == 180)
        RSDK.SetSpriteAnimation(MSZ1KIntro->playerFrames, 3, &player1->animator, true, 0);
    if (player1->animator.animationID == 3 && player1->animator.frameID == player1->animator.frameCount - 1) {
        RSDK.SetSpriteAnimation(player1->aniFrames, 0, &player1->animator, true, 0);
        return true;
    }
    return false;
}

bool32 MSZ1KIntro_CutsceneState_Unknown3(EntityCutsceneSeq *host)
{
    RSDK_GET_PLAYER(player1, player2, camera);
    unused(player2);
    unused(camera);
    if (host->timer == 30) {
        player1->stateInput     = Player_ProcessP1Input;
        player1->tileCollisions = true;
        player1->onGround       = true;
        player1->state          = Player_State_Ground;
        foreach_all(TitleCard, titleCard)
        {
            titleCard->active    = ACTIVE_NORMAL;
            titleCard->state     = TitleCard_State_Initial;
            titleCard->stateDraw = TitleCard_Draw_Default;
            foreach_break;
        }
        Music_PlayTrack(TRACK_STAGE);
        return true;
    }
    return false;
}

#if RETRO_INCLUDE_EDITOR
void MSZ1KIntro_EditorDraw(void)
{
    RSDK_THIS(MSZ1KIntro);
    CutsceneRules_DrawCutsceneBounds(self, &self->size);
}

void MSZ1KIntro_EditorLoad(void) {}
#endif

void MSZ1KIntro_Serialize(void) { RSDK_EDITABLE_VAR(MSZ1KIntro, VAR_VECTOR2, size); }
