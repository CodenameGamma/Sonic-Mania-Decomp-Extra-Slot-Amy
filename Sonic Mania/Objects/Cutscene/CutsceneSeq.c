#include "SonicMania.h"

ObjectCutsceneSeq *CutsceneSeq;

void CutsceneSeq_Update(void)
{
    RSDK_THIS(CutsceneSeq);
    CutsceneSeq_CheckSkip(entity->skipType, entity, entity->skipCallback);
}

void CutsceneSeq_LateUpdate(void)
{
    RSDK_THIS(CutsceneSeq);
    entity->currentState      = (bool32(*)(Entity *))entity->cutsceneStates[entity->stateID];
    if (entity->currentState) {
        RSDK_sceneInfo->entity         = entity->cutsceneCurEntity;
        bool32 stateComplete           = entity->currentState((Entity*)entity);
        RSDK_sceneInfo->entity         = (Entity *)entity;
        ++entity->timer;
        if (stateComplete) {
            LogHelpers_Print("State completed");
            CutsceneSeq_NewState(entity->stateID + 1, entity);
        }

        if (!entity->cutsceneStates[entity->stateID]) {
            LogHelpers_Print("Sequence completed");
            RSDK.ResetEntityPtr(entity, TYPE_BLANK, NULL);
        }
    }

    for (int32 i = 0; i < 8; ++i) {
        Vector2 *point = &entity->points[i];
        if (point->x && point->y) {
            break;
        }
        else {
            entity->visible = true;
        }
    }
    if (entity->fillTimerA <= 0 && entity->fillTimerB <= 0) {
        entity->visible = false;
    }
}

void CutsceneSeq_StaticUpdate(void) {}

void CutsceneSeq_Draw(void)
{
    RSDK_THIS(CutsceneSeq);

    colour colours[8];
    colours[0] = 0xFF00;
    colours[1] = 0xFF0000;
    colours[2] = 0xFF;
    colours[3] = 0xFF00FF;
    colours[4] = 0xFFFF00;
    colours[5] = 0xFFFF;
    colours[6] = 0x9933FF;
    colours[7] = 0xFF9900;

    for (int32 i = 0; i < 8; ++i) {
        Vector2 *point = &entity->points[i];
        if (point->x || point->y) {
#if RETRO_USE_PLUS
            RSDK.PrintVector2(0, "Draw poi ", point->x, point->y);
#endif
            RSDK.DrawLine(point->x - 0x100000, point->y - 0x100000, point->x + 0x100000, point->y + 0x100000, colours[i], 0x7F, INK_NONE, 0);
            RSDK.DrawLine(point->x + 0x100000, point->y - 0x100000, point->x - 0x100000, point->y + 0x100000, colours[i], 0x7F, INK_NONE, 0);
        }
    }

    if (entity->fillTimerA > 0)
        RSDK.FillScreen(0xFFF0F0, entity->fillTimerA, entity->fillTimerA - 256, entity->fillTimerA - 256);
    if (entity->fillTimerB > 0)
        RSDK.FillScreen(0x000000, entity->fillTimerB, entity->fillTimerB - 128, entity->fillTimerB - 256);
}

void CutsceneSeq_Create(void *data)
{
    RSDK_THIS(CutsceneSeq);
    entity->active            = ACTIVE_NORMAL;
    entity->visible           = false;
    entity->fillTimerA        = 0;
    entity->drawOrder         = Zone->hudDrawOrder + 1;
    entity->fillTimerB        = 0;
    CutsceneSeq_CheckSkip(entity->skipType, entity, entity->skipCallback);
}

void CutsceneSeq_StageLoad(void) {}

Entity *CutsceneSeq_GetEntity(int32 type)
{
    Entity *entity = NULL;
    if (RSDK.GetEntities(type, (Entity **)&entity)) {
        foreach_return entity;
    }
    return NULL;
}

void CutsceneSeq_LockAllPlayerControl(void)
{
    for (int32 i = 0; i < Player->playerCount; ++i) {
        CutsceneSeq_LockPlayerControl(RSDK.GetEntityByID(i));
    }
}
void CutsceneSeq_LockPlayerControl(void *plr)
{
    EntityPlayer *player   = (EntityPlayer *)plr;
    player->up             = false;
    player->down           = false;
    player->left           = false;
    player->right          = false;
    player->jumpHold       = false;
    player->jumpPress      = false;
    Player->upState        = 0;
    Player->downState      = 0;
    Player->leftState      = 0;
    Player->rightState     = 0;
    Player->jumpPressState = 0;
    Player->jumpHoldState  = 0;
}

void CutsceneSeq_CheckSkip(uint8 skipType, EntityCutsceneSeq *entity, void (*skipCallback)(void))
{
    bool32 skipPress = RSDK_controller->keyStart.press;
#if RETRO_USE_TOUCH_CONTROLS
    skipPress |= RSDK_touchMouse->count;
#endif

    if (skipType && skipPress && (RSDK_sceneInfo->state & 1)) {
        bool32 load = false;
        if (skipType == SKIPTYPE_NEXTSCENE) {
            ++RSDK_sceneInfo->listPos;
            load = true;
        }
        else {
            if (skipCallback && skipType == SKIPTYPE_CALLBACK)
                skipCallback();
            load = entity && (entity->skipType == SKIPTYPE_CALLBACK || entity->skipType == SKIPTYPE_RELOADSCN);
        }

        if (load) {
            globals->suppressTitlecard = false;
            globals->suppressAutoMusic = false;
            globals->enableIntro       = false;
            RSDK.SetGameMode(ENGINESTATE_FROZEN);
            Zone_StartFadeOut(20, 0x000000);
            Music_FadeOut(0.03);
        }
    }
}
void CutsceneSeq_NewState(int32 nextState, EntityCutsceneSeq *CutsceneSeq)
{
    CutsceneSeq->stateID     = nextState;
    CutsceneSeq->timer       = 0;
    CutsceneSeq->field_68    = 0;
    CutsceneSeq->field_6C[0] = 0;
    CutsceneSeq->points[0].x = 0;
    CutsceneSeq->points[0].y = 0;
    CutsceneSeq->field_6C[1] = 0;
    CutsceneSeq->points[1].x = 0;
    CutsceneSeq->points[1].y = 0;
    CutsceneSeq->field_6C[2] = 0;
    CutsceneSeq->points[2].x = 0;
    CutsceneSeq->points[2].y = 0;
    CutsceneSeq->field_6C[3] = 0;
    CutsceneSeq->points[3].x = 0;
    CutsceneSeq->points[3].y = 0;
    CutsceneSeq->field_6C[4] = 0;
    CutsceneSeq->points[4].x = 0;
    CutsceneSeq->points[4].y = 0;
    CutsceneSeq->field_6C[5] = 0;
    CutsceneSeq->points[5].x = 0;
    CutsceneSeq->points[5].y = 0;
    CutsceneSeq->field_6C[6] = 0;
    CutsceneSeq->points[6].x = 0;
    CutsceneSeq->points[6].y = 0;
    CutsceneSeq->field_6C[7] = 0;
    CutsceneSeq->points[7].x = 0;
    CutsceneSeq->points[7].y = 0;
}
void CutsceneSeq_StartSequence(Entity *host, void **states)
{
    if (RSDK_GET_ENTITY(SLOT_CUTSCENESEQ, CutsceneSeq)->objectID)
        return;
    RSDK.ResetEntitySlot(SLOT_CUTSCENESEQ, CutsceneSeq->objectID, 0);
    EntityCutsceneSeq *cutsceneSeq = (EntityCutsceneSeq *)RSDK.GetEntityByID(SLOT_CUTSCENESEQ);
    cutsceneSeq->position.x   = 0;
    cutsceneSeq->position.y   = 0;
    cutsceneSeq->cutsceneCurEntity = RSDK_sceneInfo->entity;
    cutsceneSeq->hostEntity        = host;
    int32 id                         = 0;
    while (states[id]) {
        LogHelpers_Print("state = %x", states[id]);
        cutsceneSeq->cutsceneStates[id] = states[id];
        id++;
    }
    LogHelpers_Print("Starting sequence with %d states", id);
}

#if RETRO_INCLUDE_EDITOR
void CutsceneSeq_EditorDraw(void) {}

void CutsceneSeq_EditorLoad(void) {}
#endif

void CutsceneSeq_Serialize(void) {}
