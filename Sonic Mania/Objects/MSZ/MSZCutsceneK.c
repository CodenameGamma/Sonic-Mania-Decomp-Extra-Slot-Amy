#include "SonicMania.h"

ObjectMSZCutsceneK *MSZCutsceneK;

void MSZCutsceneK_Update(void)
{
    RSDK_THIS(MSZCutsceneK);
    if (!entity->activated) {
        MSZCutsceneK_StartCutscene();
        entity->activated = true;
    }
}

void MSZCutsceneK_LateUpdate(void) {}

void MSZCutsceneK_StaticUpdate(void) {}

void MSZCutsceneK_Draw(void) {}

void MSZCutsceneK_Create(void *data)
{
    RSDK_THIS(MSZCutsceneK);

    INIT_ENTITY(entity);
    CutsceneRules_SetupEntity(entity);
    entity->active = ACTIVE_BOUNDS;
}

void MSZCutsceneK_StageLoad(void)
{
    MSZCutsceneK->playerFrames = RSDK.LoadSpriteAnimation("Players/KnuxCutsceneAIZ.bin", SCOPE_STAGE);
    MSZCutsceneK->sfxImpact    = RSDK.GetSFX("Stage/Impact5.wav");
    MSZCutsceneK->sfxDrop      = RSDK.GetSFX("Stage/Drop.wav");

    MSZCutsceneK->mystic  = NULL;
    MSZCutsceneK->tornado = NULL;

    foreach_all(HeavyMystic, mystic)
    {
        MSZCutsceneK->mystic = (Entity *)mystic;
        foreach_break;
    }

    foreach_all(Tornado, tornado)
    {
        MSZCutsceneK->tornado = (Entity *)tornado;
        foreach_break;
    }
}

void MSZCutsceneK_EditorDraw(void) {}

void MSZCutsceneK_EditorLoad(void) {}

void MSZCutsceneK_SkipCB(void)
{
    RSDK.SetScene("Mania Mode", "");
    RSDK_sceneInfo->listPos += TimeAttackData_GetManiaListPos(7, 3, 0);
}

void MSZCutsceneK_StartCutscene(void)
{
    RSDK_THIS(MSZCutsceneK);

    void *states[] = { MSZCutsceneK_CutsceneState_Unknown1, MSZCutsceneK_CutsceneState_Unknown2, NULL };

    CutsceneSeq_StartSequence((Entity *)entity, states);

    EntityCutsceneSeq *sequence = RSDK_GET_ENTITY(SLOT_CUTSCENESEQ, CutsceneSeq);
    if (sequence->objectID) {
        sequence->skipType     = SKIPTYPE_CALLBACK;
        sequence->skipCallback = MSZCutsceneK_SkipCB;
    }
}

void MSZCutsceneK_SetupP2(int posX, int posY)
{
    Player->sonicSpriteIndex = RSDK.LoadSpriteAnimation("Players/Sonic.bin", SCOPE_STAGE);
    Player->superSpriteIndex = RSDK.LoadSpriteAnimation("Players/SuperSonic.bin", SCOPE_STAGE);
    RSDK.ResetEntitySlot(SLOT_PLAYER2, Player->objectID, NULL);

    EntityPlayer *player2 = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
    ++Player->playerCount;
    player2->characterID     = ID_SONIC;
    player2->position.x      = posX;
    player2->position.y      = posY;
    player2->spriteIndex     = Player->sonicSpriteIndex;
    player2->tailSpriteIndex = -1;
    player2->cameraOffset    = 0x50000;
    player2->movesetState    = Player_SonicJumpAbility;
    player2->sensorY         = 0x140000;
    player2->stateInput      = 0;
    player2->state           = Player_State_None;
    RSDK.SetSpriteAnimation(Player->sonicSpriteIndex, ANI_RIDE, &player2->playerAnimator, true, 0);
}

bool32 MSZCutsceneK_CutsceneState_Unknown1(EntityCutsceneSeq *host)
{
    RSDK_GET_PLAYER(player1, player2, camera);
    unused(player2);
    unused(camera);

    EntityHeavyMystic *mystic = (EntityHeavyMystic *)MSZCutsceneK->mystic;
    EntityTornado *tornado    = (EntityTornado *)MSZCutsceneK->tornado;
    if (!host->timer) {
        RSDK_sceneInfo->timeEnabled  = false;
        RSDK_sceneInfo->milliseconds = 0;
        player1->state               = Player_State_None;
        player1->stateInput          = 0;
        CutsceneSeq_LockAllPlayerControl();
        player1->velocity.x = 0;
        player1->velocity.y = 0;
        RSDK.SetSpriteAnimation(MSZCutsceneK->playerFrames, 6, &player1->playerAnimator, true, 0);
        MSZCutsceneK_SetupP2(0xCC0000, 0x29E0000);
    }
    if (mystic->position.x > tornado->position.x - 0x100000) {
        MSZCutsceneK->pos1.x = player1->position.x - tornado->position.x;
        MSZCutsceneK->pos1.y = player1->position.y - tornado->position.y;
        MSZCutsceneK->pos2.x = player2->position.x - tornado->position.x;
        MSZCutsceneK->pos2.y = player2->position.y - tornado->position.y;
        return true;
    }
    return false;
}

bool32 MSZCutsceneK_CutsceneState_Unknown2(EntityCutsceneSeq *host)
{
    RSDK_GET_PLAYER(player1, player2, camera);
    unused(player2);
    unused(camera);
    EntityTornado *tornado = (EntityTornado *)MSZCutsceneK->tornado;

    if (!host->timer) {
        RSDK.StopChannel(Music->channelID);
        RSDK.PlaySfx(MSZCutsceneK->sfxImpact, false, 255);
        tornado->velocity.y = -0x10000;
        if (!host->field_6C[0]) {
            host->field_6C[0] = true;
            RSDK.PlaySfx(MSZCutsceneK->sfxDrop, false, 255);
            RSDK.SetSpriteAnimation(MSZCutsceneK->playerFrames, 4, &player1->playerAnimator, true, 0);
            player1->velocity.x = -0x10000;
            player1->velocity.y = -0x30000;
        }
    }
    tornado->position.x += tornado->velocity.x;
    tornado->position.y += tornado->velocity.y;
    if (tornado->velocity.y >= tornado->field_84.y) {
        tornado->velocity.y = 0;
        tornado->position.y = tornado->field_84.y;
    }
    else {
        tornado->velocity.y += 0x3800;
    }

    if (host->field_6C[0]) {
        if (!host->field_6C[2])
            player1->velocity.y += 0x3800;
    }
    else {
        player1->position.x = tornado->position.x + MSZCutsceneK->pos1.x;
        player1->position.y = tornado->position.y + MSZCutsceneK->pos1.y;
    }

    if (host->field_6C[1]) {
        if (host->timer - host->field_68 == 15) {
            globals->suppressTitlecard = true;
            globals->suppressAutoMusic = true;
            globals->enableIntro       = 1;
            RSDK.SetScene("Mania Mode", "");
            RSDK_sceneInfo->listPos += TimeAttackData_GetManiaListPos(7, 3, 0);
            Zone_StartFadeOut(10, 0x000000);
        }
        else if (host->timer - host->field_68 == 60) {
            player1->velocity.y = 0;
            host->field_6C[2]   = true;
        }
    }
    else if (player1->position.y > tornado->position.y) {
        TornadoPath->cameraPtr = NULL;
        host->field_6C[1]      = 1;
        host->field_68         = host->timer;
        MSZCutsceneK->pos3     = camera->position;
        MSZCutsceneK->pos3.y += 0x1E00000;
        Camera_SetupLerp(0, 0, MSZCutsceneK->pos3.x, MSZCutsceneK->pos3.y, 3);
    }
    player2->position.x = tornado->position.x + MSZCutsceneK->pos2.x;
    player2->position.y = tornado->position.y + MSZCutsceneK->pos2.y;
    return false;
}

void MSZCutsceneK_Serialize(void) { RSDK_EDITABLE_VAR(MSZCutsceneK, VAR_VECTOR2, size); }
