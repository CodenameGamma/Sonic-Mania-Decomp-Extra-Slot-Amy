#include "SonicMania.h"

ObjectAnnouncer *Announcer;

void Announcer_Update(void)
{
    RSDK_THIS(Announcer);
    StateMachine_Run(entity->state);
}

void Announcer_LateUpdate(void) {}

void Announcer_StaticUpdate(void) {}

void Announcer_Draw(void)
{
    RSDK_THIS(Announcer);
    StateMachine_Run(entity->stateDraw);
}

void Announcer_Create(void *data)
{
    RSDK_THIS(Announcer);
    entity->active        = ACTIVE_NORMAL;
    entity->drawOrder     = 13;
    entity->visible       = true;
    entity->drawFX        = FX_FLIP | FX_SCALE;
    entity->isPermanent   = true;
    entity->updateRange.x = 0x800000;
    entity->updateRange.y = 0x800000;
}

void Announcer_StageLoad(void)
{
    Announcer->finishedCountdown = false;
    if (RSDK.CheckStageFolder("Menu")) {
#if !RETRO_USE_PLUS
        Announcer->sfx_NewRecordTop = RSDK.GetSFX("VO/NewRecordTop.wav");
        Announcer->sfx_NewRecordMid = RSDK.GetSFX("VO/NewRecordMid.wav");
#endif
        Announcer->sfx_Sonic        = RSDK.GetSFX("VO/Sonic.wav");
        Announcer->sfx_Tails        = RSDK.GetSFX("VO/Tails.wav");
        Announcer->sfx_Knuckles     = RSDK.GetSFX("VO/Knuckles.wav");
#if RETRO_USE_PLUS
        Announcer->sfx_Mighty       = RSDK.GetSFX("VO/Mighty.wav");
        Announcer->sfx_Ray          = RSDK.GetSFX("VO/Ray.wav");
#endif
        Announcer->sfx_TheWinnerIs  = RSDK.GetSFX("VO/TheWinnerIs.wav");
        Announcer->sfx_Player1      = RSDK.GetSFX("VO/Player1.wav");
        Announcer->sfx_Player2      = RSDK.GetSFX("VO/Player2.wav");
#if RETRO_USE_PLUS
        Announcer->sfx_Player3      = RSDK.GetSFX("VO/Player3.wav");
        Announcer->sfx_Player4      = RSDK.GetSFX("VO/Player4.wav");
#endif
        Announcer->sfx_SonicWins    = RSDK.GetSFX("VO/SonicWins.wav");
        Announcer->sfx_TailsWins    = RSDK.GetSFX("VO/TailsWins.wav");
        Announcer->sfx_KnuxWins     = RSDK.GetSFX("VO/KnuxWins.wav");
#if RETRO_USE_PLUS
        Announcer->sfx_MightyWins   = RSDK.GetSFX("VO/MightyWins.wav");
        Announcer->sfx_RayWins      = RSDK.GetSFX("VO/RayWins.wav");
        Announcer->sfx_ItsADraw     = RSDK.GetSFX("VO/ItsADraw.wav");
        Announcer->sfx_ItsADraw_Set = RSDK.GetSFX("VO/ItsADraw_Set.wav");
        LogHelpers_Print("sfxDrawRound = %d", Announcer->sfx_ItsADraw);
        LogHelpers_Print("sfxDrawSet = %d", Announcer->sfx_ItsADraw_Set);
#endif
    }
    else if (globals->gameMode == MODE_COMPETITION) {
        Announcer->spriteIndex = RSDK.LoadSpriteAnimation("Global/Announcer.bin", SCOPE_STAGE);
        Announcer->sfx_Three   = RSDK.GetSFX("VO/Three.wav");
        Announcer->sfx_Two     = RSDK.GetSFX("VO/Two.wav");
        Announcer->sfx_One     = RSDK.GetSFX("VO/One.wav");
        Announcer->sfx_Go      = RSDK.GetSFX("VO/Go.wav");
        Announcer->sfx_Goal    = RSDK.GetSFX("VO/Goal.wav");
    }
    else if (globals->gameMode == MODE_TIMEATTACK) {
        Announcer->spriteIndex = RSDK.LoadSpriteAnimation("Global/Announcer.bin", SCOPE_STAGE);
#if RETRO_USE_PLUS
        Announcer->sfx_NewRecordTop = RSDK.GetSFX("VO/NewRecordTop.wav");
        Announcer->sfx_NewRecordMid = RSDK.GetSFX("VO/NewRecordMid.wav");
#endif
        Announcer->sfx_Goal         = RSDK.GetSFX("VO/Goal.wav");
    }
}

void Announcer_StartCountdown(void)
{
    Announcer->finishedCountdown = false;
    EntityAnnouncer *announcer = CREATE_ENTITY(Announcer, NULL, 0, 0);
    announcer->state     = Announcer_Unknown5;
    announcer->stateDraw = Announcer_Unknown3;
    announcer->playerID  = 3;
    announcer->scale.x   = 0x200;
    announcer->scale.y   = 0x200;
}
void Announcer_AnnounceGoal(int32 screen)
{
    EntityAnnouncer *entity = CREATE_ENTITY(Announcer, NULL, 0, 0);
    entity->state           = Announcer_Unknown6;
    entity->stateDraw       = Announcer_Unknown4;
    entity->screen          = screen;
    RSDK.SetSpriteAnimation(Announcer->spriteIndex, 0, &entity->animator, true, 0);
    RSDK.PlaySfx(Announcer->sfx_Goal, 0, 255);
}
void Announcer_Unknown3(void)
{
    Vector2 drawPos;
    RSDK_THIS(Announcer);
    drawPos.y = (RSDK_screens->centerY - 32) << 16;
    drawPos.x = RSDK_screens->centerX << 16;
    drawPos.x += entity->pos.x;
    drawPos.y += entity->pos.y;
    RSDK.DrawSprite(&entity->animator, &drawPos, true);

    if (entity->playerID > 0) {
        entity->inkEffect = INK_NONE;

        int32 frame                         = 0;
        EntityCompetitionSession *session = (EntityCompetitionSession *)globals->competitionSession;
        switch (session->characterFlags[RSDK_sceneInfo->currentScreenID]) {
            case ID_TAILS: frame = 1; break;
            case ID_KNUCKLES: frame = 2; break;
#if RETRO_USE_PLUS
            case ID_MIGHTY: frame = 3; break;
            case ID_RAY: frame = 4; break;
#endif
            default: frame = 0; break;
        }
        RSDK.SetSpriteAnimation(Announcer->spriteIndex, 2, &entity->animator2, true, frame);

        drawPos.x = RSDK_screens->centerX << 16;
        drawPos.y = (RSDK_screens->centerY + 48) << 16;
        RSDK.DrawSprite(&entity->animator2, &drawPos, true);
        entity->inkEffect = INK_ALPHA;
    }
}
void Announcer_Unknown4(void)
{
    Vector2 drawPos;
    RSDK_THIS(Announcer);

    if (RSDK_sceneInfo->currentScreenID == entity->screen) {
        drawPos.x = RSDK_screens->centerX << 16;
        drawPos.y = (RSDK_screens->centerY - 32) << 16;
        drawPos.x += entity->pos.x;
        drawPos.y += entity->pos.y;
        RSDK.DrawSprite(&entity->animator, &drawPos, true);
    }
}
void Announcer_Unknown5(void)
{
    RSDK_THIS(Announcer);

    entity->inkEffect = INK_ALPHA;
    if (entity->playerID <= 0) {
        if (entity->timer >= 60) {
            destroyEntity(entity);
        }
        else {
            if (!entity->timer) {
                RSDK.PlaySfx(Announcer->sfx_Go, 0, 255);
                RSDK.SetSpriteAnimation(Announcer->spriteIndex, 1, &entity->animator, true, 3);
            }

            int32 val = 0;
            if (entity->timer - 15 > 0)
                val = (entity->timer - 15) << 9;
            entity->alpha = 512 - val / 45;
            entity->timer++;
        }
    }
    else {
        if (entity->timer >= 45) {
            entity->timer = 0;
            entity->playerID--;
            if (!entity->playerID) {
                Announcer->finishedCountdown = true;
                RSDK_sceneInfo->timeEnabled  = true;
            }
        }
        else {

            if (!entity->timer) {
                switch (entity->playerID) {
                    default: break;
                    case 1:
                        RSDK.PlaySfx(Announcer->sfx_One, 0, 255);
                        RSDK.SetSpriteAnimation(Announcer->spriteIndex, 1, &entity->animator, true, 2);
                        break;
                    case 2:
                        RSDK.PlaySfx(Announcer->sfx_Two, 0, 255);
                        RSDK.SetSpriteAnimation(Announcer->spriteIndex, 1, &entity->animator, true, 1);
                        break;
                    case 3:
                        RSDK.PlaySfx(Announcer->sfx_Three, 0, 255);
                        RSDK.SetSpriteAnimation(Announcer->spriteIndex, 1, &entity->animator, true, 0);
                        break;
                }
            }
            entity->pos.x = 0;
            entity->pos.y = 0;
            entity->alpha = 512 - (entity->timer << 9) / 45;
            entity->timer++;
        }
    }
}
void Announcer_Unknown6(void)
{
    RSDK_THIS(Announcer);
    entity->scale.x = 0x200;
    entity->scale.y = 0x200;
    if (entity->timer >= 16) {
        if (entity->timer >= 76) {
            if (entity->timer >= 92) {
                destroyEntity(entity);
            }
            else {
                entity->visible = true;
                MathHelpers_Lerp1(&entity->pos, ((entity->timer - 76) << 8) / 16, 0, 0, RSDK_screens->width << 16, 0);
                ++entity->timer;
            }
        }
        else {
            ++entity->timer;
            entity->pos.x = 0;
            entity->pos.y = 0;
        }
    }
    else {
        entity->visible = true;
        int32 t           = 16 * entity->timer;
        int32 val         = -0x10000 * RSDK_screens->width;
        if (t > 0) {
            if (t < 256)
                entity->pos.x = val + t * (-val >> 8);
            else
                entity->pos.x = 0;
            entity->pos.y = 0;
            ++entity->timer;
        }
        else {
            entity->pos.x = val;
            entity->pos.y = 0;
            ++entity->timer;
        }
    }
}
void Announcer_State_AnnounceWinner(void)
{
    RSDK_THIS(Announcer);
    if (entity->timer >= 150) {
        switch (entity->playerID) {
            case 0: RSDK.PlaySfx(Announcer->sfx_Player1, 0, 255); break;
            case 1: RSDK.PlaySfx(Announcer->sfx_Player2, 0, 255); break;
#if RETRO_USE_PLUS
            case 2: RSDK.PlaySfx(Announcer->sfx_Player3, 0, 255); break;
            case 3: RSDK.PlaySfx(Announcer->sfx_Player4, 0, 255); break;
#endif
            default: break;
        }
        destroyEntity(entity);
    }
    else {
        if (entity->timer == 30)
            RSDK.PlaySfx(Announcer->sfx_TheWinnerIs, 0, 255);
        ++entity->timer;
    }
}
void Announcer_State_AnnounceDraw(void)
{
    RSDK_THIS(Announcer);
    if (entity->timer < 30) {
        entity->timer++;
    }
    else {
        if (!entity->playerID) {
            RSDK.PlaySfx(Announcer->sfx_ItsADraw, 0, 255);
        }
        else if (entity->playerID == 1) {
            RSDK.PlaySfx(Announcer->sfx_ItsADraw_Set, 0, 255);
        }
        destroyEntity(entity);
    }
}
void Announcer_State_AnnounceWinPlayer(void)
{
    RSDK_THIS(Announcer);
    if (entity->timer >= 30) {
        switch (entity->playerID) {
            case ID_SONIC: RSDK.PlaySfx(Announcer->sfx_SonicWins, 0, 255); break;
            case ID_TAILS: RSDK.PlaySfx(Announcer->sfx_TailsWins, 0, 255); break;
            case ID_KNUCKLES: RSDK.PlaySfx(Announcer->sfx_KnuxWins, 0, 255); break;
#if RETRO_USE_PLUS
            case ID_MIGHTY: RSDK.PlaySfx(Announcer->sfx_MightyWins, 0, 255); break;
            case ID_RAY: RSDK.PlaySfx(Announcer->sfx_RayWins, 0, 255); break;
#endif
            default: break;
        }
        destroyEntity(entity);
    }
    else {
        entity->timer++;
    }
}

#if RETRO_INCLUDE_EDITOR
void Announcer_EditorDraw(void) {}

void Announcer_EditorLoad(void) {}
#endif

void Announcer_Serialize(void) {}
