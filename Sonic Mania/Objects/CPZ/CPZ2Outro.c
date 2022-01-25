// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: CPZ2Outro Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectCPZ2Outro *CPZ2Outro;

void CPZ2Outro_Update(void)
{
    RSDK_THIS(CPZ2Outro);
    if (!self->activated) {
        CPZ2Outro_SetupCutscene();
        self->activated = true;
    }
}

void CPZ2Outro_LateUpdate(void) {}

void CPZ2Outro_StaticUpdate(void) {}

void CPZ2Outro_Draw(void) {}

void CPZ2Outro_Create(void *data)
{
    RSDK_THIS(CPZ2Outro);

    INIT_ENTITY(self);
    CutsceneRules_SetupEntity(self, &self->size, &self->hitbox);
    self->active = ACTIVE_NEVER;
}

void CPZ2Outro_StageLoad(void)
{
    CPZ2Outro->prisonPtr = NULL;
    foreach_all(EggPrison, prison)
    {
        CPZ2Outro->prisonPtr = (Entity *)prison;
        foreach_break;
    }
}

void CPZ2Outro_SetupCutscene(void)
{
    RSDK_THIS(CPZ2Outro);
    EntityCutsceneSeq *seq = RSDK_GET_ENTITY(SLOT_CUTSCENESEQ, CutsceneSeq);

    CutsceneSeq_StartSequence(self, CPZ2Outro_Cutscene_Outro, StateMachine_None);
#if RETRO_USE_PLUS
    if (seq->objectID)
        seq->skipType = SKIPTYPE_RELOADSCN;
#endif

    foreach_active(HUD, hud) { hud->state = HUD_State_GoOffScreen; }
}

bool32 CPZ2Outro_Cutscene_Outro(void *h)
{
    EntityCutsceneSeq *host = (EntityCutsceneSeq *)h;

    RSDK_GET_PLAYER(player1, player2, camera);
    unused(camera);

    Vector2 size;
    RSDK.GetLayerSize(Zone->fgLow, &size, true);

    if (!host->timer) {
        ((EntityEggPrison *)CPZ2Outro->prisonPtr)->notSolid = true;
        Zone->cameraBoundsT[0]                              = 0;
        Zone->cameraBoundsT[1]                              = 0;
        Zone->cameraBoundsR[0]                              = size.x;
        Zone->cameraBoundsR[1]                              = size.x;
        Zone->playerBoundActiveR[0]                         = 0;
        Zone->playerBoundActiveR[1]                         = 0;
        CutsceneSeq_LockAllPlayerControl();
        player1->stateInput = StateMachine_None;
        player1->state      = Player_State_Ground;
        player1->groundVel  = 0;
        player1->right      = true;
        if (player2->objectID == Player->objectID) {
            player2->state      = Player_State_Ground;
            player2->stateInput = Player_ProcessP2Input_AI;
            player2->groundVel  = 0;
        }
    }

    if (player1->jumpPress)
        player1->jumpPress = false;

    if (player1->onGround && player1->position.x >= host->activeEntity->position.x && !host->values[0]) {
        player1->jumpPress = true;
        host->values[0]    = 1;
    }

    if (player1->position.x > size.x << 16) {
        Zone_StartFadeOut(10, 0x000000);
        return true;
    }
    return false;
}

#if RETRO_INCLUDE_EDITOR
void CPZ2Outro_EditorDraw(void)
{
    RSDK_THIS(CPZ2Outro);
    CutsceneRules_DrawCutsceneBounds(self, &self->size);
}

void CPZ2Outro_EditorLoad(void) {}
#endif

void CPZ2Outro_Serialize(void) { RSDK_EDITABLE_VAR(CPZ2Outro, VAR_VECTOR2, size); }
