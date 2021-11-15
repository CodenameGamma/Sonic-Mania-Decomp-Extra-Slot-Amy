#include "SonicMania.h"

ObjectSpecialRing *SpecialRing;

void SpecialRing_Update(void)
{
    RSDK_THIS(SpecialRing);
    StateMachine_Run(entity->state);
}

void SpecialRing_LateUpdate(void) {}

void SpecialRing_StaticUpdate(void) {}

void SpecialRing_Draw(void)
{
    RSDK_THIS(SpecialRing);
    if (entity->state == SpecialRing_State_Warp) {
        entity->direction = entity->warpAnimator.frameID > 8;
        RSDK.DrawSprite(&entity->warpAnimator, 0, 0);
    }
    else {
        RSDK.Prepare3DScene(SpecialRing->sceneIndex);
        if (entity->enabled)
            RSDK.AddModelTo3DScene(SpecialRing->modelIndex, SpecialRing->sceneIndex, S3D_FLATCLR_SHADED_BLENDED, &entity->matrix2, &entity->matrix3,
                                0xF0F000);
        else
            RSDK.AddModelTo3DScene(SpecialRing->modelIndex, SpecialRing->sceneIndex, S3D_FLATCLR_SHADED_WIREFRAME, &entity->matrix2, &entity->matrix3, 0x609090);
        RSDK.Draw3DScene(SpecialRing->sceneIndex);
    }
}

void SpecialRing_Create(void *data)
{
    RSDK_THIS(SpecialRing);
    if (!RSDK_sceneInfo->inEditor) {
        entity->active        = ACTIVE_BOUNDS;
        entity->visible       = true;
        entity->updateRange.x = 0x900000;
        entity->updateRange.y = 0x900000;
        entity->drawFX        = FX_FLIP;
        if (entity->planeFilter > 0 && ((uint8)entity->planeFilter - 1) & 2)
            entity->drawOrder = Zone->drawOrderHigh;
        else
            entity->drawOrder = Zone->drawOrderLow;
        entity->state = SpecialRing_State_Normal;
        RSDK.SetSpriteAnimation(SpecialRing->spriteIndex, 0, &entity->warpAnimator, true, 0);
    }
}

void SpecialRing_StageLoad(void)
{
    SpecialRing->spriteIndex = RSDK.LoadSpriteAnimation("Global/SpecialRing.bin", SCOPE_STAGE);
    SpecialRing->modelIndex  = RSDK.LoadMesh("Global/SpecialRing.bin", SCOPE_STAGE);
    SpecialRing->sceneIndex  = RSDK.Create3DScene("View:SpecialRing", 512, SCOPE_STAGE);

    SpecialRing->hitbox.left   = -18;
    SpecialRing->hitbox.top    = -18;
    SpecialRing->hitbox.right  = 18;
    SpecialRing->hitbox.bottom = 18;

    // sets diffuse colour (overrides)
    RSDK.SetDiffuseColour(SpecialRing->sceneIndex, 160, 160, 160);
    // sets diffuse intensity (0-8 means more diffuse, any more and it'll start darkening to black (9-12), any greater than 11 is full black)
    RSDK.SetDiffuseIntensity(SpecialRing->sceneIndex, 8, 8, 8);
    // sets specular (highlight) intensity (16-0, 16 = none, 0 = all)
    RSDK.SetSpecularIntensity(SpecialRing->sceneIndex, 14, 14, 14);
    SpecialRing->sfx_SpecialRing = RSDK.GetSFX("Global/SpecialRing.wav");
    SpecialRing->sfx_SpecialWarp = RSDK.GetSFX("Global/SpecialWarp.wav");

    DEBUGMODE_ADD_OBJ(SpecialRing);

    foreach_all(SpecialRing, entity) {
        if (entity->id <= 0 || globals->gameMode == MODE_TIMEATTACK || globals->gameMode == MODE_COMPETITION) {
            entity->enabled = false;
        }
        else {
            entity->enabled = !(SaveGame->saveRAM->collectedSpecialRings & (1 << ((16 * Zone->actID) + entity->id - 1)));
            if (globals->specialRingID == entity->id) {
                for (int32 p = 0; p < Player->playerCount; ++p) {
                    EntityPlayer *player = RSDK_GET_ENTITY(p, Player);

                    player->position.x = entity->position.x;
                    player->position.y = entity->position.y;
                    player->position.y += 0x100000;
                    if (!p) {
                        EntityPlayer *player2 = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
                        if (globals->gameMode != MODE_COMPETITION) {
                            player2->position.x = player->position.x;
                            player2->position.y = player->position.y;
                            player2->direction  = player->direction;
                            if (player->direction)
                                player2->position.x += 0x100000;
                            else
                                player2->position.x -= 0x100000;

                            for (int32 f = 0; f < 0x10; ++f) {
                                Player->flyCarryPositions[f].x = player->position.x;
                                Player->flyCarryPositions[f].y = player->position.y;
                            }
                        }
                    }
                }
                RSDK_sceneInfo->milliseconds = globals->tempMilliseconds;
                RSDK_sceneInfo->seconds      = globals->tempSeconds;
                RSDK_sceneInfo->minutes      = globals->tempMinutes;
            }
        }
    }
    globals->specialRingID = 0;
}

void SpecialRing_DebugDraw(void)
{
    RSDK.SetSpriteAnimation(Ring->spriteIndex, 1, &DebugMode->animator, true, 0);
    RSDK.DrawSprite(&DebugMode->animator, NULL, false);
}
void SpecialRing_DebugSpawn(void)
{
    RSDK_THIS(SpecialRing);
    EntitySpecialRing *specialRing = CREATE_ENTITY(SpecialRing, NULL, entity->position.x, entity->position.y);
    specialRing->enabled           = true;
}
void SpecialRing_StartWarp(void)
{
    RSDK_THIS(SpecialRing);
    if (++entity->warpTimer == 30) {
        SaveGame_SaveGameState();
        RSDK.PlaySfx(SpecialRing->sfx_SpecialWarp, 0, 254);
        destroyEntity(entity);
        EntitySaveGame *saveRAM = SaveGame->saveRAM;
        saveRAM->storedStageID  = RSDK_sceneInfo->listPos;
        RSDK.SetScene("Special Stage", "");
        RSDK_sceneInfo->listPos += saveRAM->nextSpecialStage;
#if RETRO_USE_PLUS
        if (globals->gameMode == MODE_ENCORE)
            RSDK_sceneInfo->listPos += 7;
#endif
        Zone_StartFadeOut(10, 0xF0F0F0);
        RSDK.StopChannel(Music->channelID);
    }
}
void SpecialRing_State_Warp(void)
{
    RSDK_THIS(SpecialRing);
    RSDK.ProcessAnimation(&entity->warpAnimator);
    if (!(Zone->timer & 3)) {
        for (int32 i = 0; i < 3; ++i) {
            EntityRing *ring = CREATE_ENTITY(Ring, NULL, (RSDK.Rand(-0x200000, 0x20000) + entity->dword68) + entity->position.x,
                                             entity->position.y + RSDK.Rand(-0x200000, 0x200000));
            ring->state     = Ring_State_Sparkle;
            ring->stateDraw = Ring_StateDraw_Sparkle;
            ring->active    = ACTIVE_NORMAL;
            ring->visible   = 0;
            ring->drawOrder = Zone->drawOrderLow;
            RSDK.SetSpriteAnimation(Ring->spriteIndex, i % 3 + 2, &ring->animator, true, 0);
            int32 cnt = ring->animator.frameCount;
            if (ring->animator.animationID == 2) {
                ring->alpha = 224;
                cnt >>= 1;
            }
            ring->maxFrameCount           = cnt - 1;
            ring->animator.animationSpeed = RSDK.Rand(6, 8);
            ring->timer                   = 2 * i;
        }
        entity->dword68 -= 0x80000;
    }

    if (SaveGame->saveRAM->chaosEmeralds == 0x7F || !entity->id) {
        destroyEntity(entity);
    }
    else {
        if (entity->warpAnimator.frameID == entity->warpAnimator.frameCount - 1) {
            entity->warpTimer = 0;
            entity->visible   = false;
            entity->state     = SpecialRing_StartWarp;
        }
    }
}
void SpecialRing_State_Normal(void)
{
    RSDK_THIS(SpecialRing);
    entity->angleZ            = (entity->angleZ + 4) & 0x3FF;
    entity->angleY            = (entity->angleY + 4) & 0x3FF;

    Vector2 updateRange;
    updateRange.x = 0x800000;
    updateRange.y = 0x800000;
    if (!RSDK.CheckOnScreen(entity, &updateRange)) {
        entity->scale.x = 0;
    }

    if (entity->scale.x >= 0x140)
        entity->scale.x = 0x140;
    else
        entity->scale.x += ((0x168 - entity->scale.x) >> 5);

    RSDK.MatrixScaleXYZ(&entity->matrix, entity->scale.x, entity->scale.x, entity->scale.x);
    RSDK.MatrixTranslateXYZ(&entity->matrix, entity->position.x, entity->position.y, 0, false);
    RSDK.MatrixRotateXYZ(&entity->matrix2, 0, entity->angleY, entity->angleZ);
    RSDK.MatrixMultiply(&entity->matrix2, &entity->matrix2, &entity->matrix);
    RSDK.MatrixRotateX(&entity->matrix4, 0x1E0);
    RSDK.MatrixRotateXYZ(&entity->matrix3, 0, entity->angleY, entity->angleZ);
    RSDK.MatrixMultiply(&entity->matrix3, &entity->matrix3, &entity->matrix4);

    if (entity->enabled && entity->scale.x > 0x100) {
        foreach_active(Player, player) {
            if ((entity->planeFilter <= 0 || player->collisionPlane == (((uint8)entity->planeFilter - 1) & 1)) && !player->sidekick) {
                if (Player_CheckCollisionTouch(player, entity, &SpecialRing->hitbox) && RSDK_sceneInfo->timeEnabled) {
                    entity->dword68 = 0x100000;
                    entity->state   = SpecialRing_State_Warp;
                    EntitySaveGame *saveRAM = SaveGame->saveRAM;
#if RETRO_GAMEVER != VER_100
                    // rings spawned via debug mode give you 50 rings, always
                    if (saveRAM->chaosEmeralds != 0x7F && entity->id) {
#else
                    // rings spawned via debug mode take you to special stage, always
                    if (saveRAM->chaosEmeralds != 0x7F) {
#endif
                        player->visible             = false;
                        player->active              = ACTIVE_NEVER;
                        RSDK_sceneInfo->timeEnabled = false;
                    }
                    else {
                        Player_GiveRings(50, player, true);
                    }

                    if (entity->id > 0) {
                        if (saveRAM->chaosEmeralds != 0x7F)
                            globals->specialRingID = entity->id;
                        saveRAM->collectedSpecialRings |= 1 << (16 * Zone->actID - 1 + entity->id);
                    }
                    RSDK.PlaySfx(SpecialRing->sfx_SpecialRing, 0, 254);
                }
            }
        }
    }
}

#if RETRO_INCLUDE_EDITOR
void SpecialRing_EditorDraw(void)
{
    RSDK_THIS(SpecialRing);
    RSDK.SetSpriteAnimation(SpecialRing->spriteIndex, 0, &entity->warpAnimator, true, 7);
    RSDK.DrawSprite(&entity->warpAnimator, NULL, false);
}

void SpecialRing_EditorLoad(void)
{
    SpecialRing->spriteIndex = RSDK.LoadSpriteAnimation("Global/SpecialRing.bin", SCOPE_STAGE);

    RSDK_ACTIVE_VAR(SpecialRing, planeFilter);
    RSDK_ENUM_VAR("No Filter", PLANEFILTER_NONE);
    RSDK_ENUM_VAR("Plane A", PLANEFILTER_A);
    RSDK_ENUM_VAR("Plane B", PLANEFILTER_B);
}
#endif

void SpecialRing_Serialize(void)
{
    RSDK_EDITABLE_VAR(SpecialRing, VAR_ENUM, id);
    RSDK_EDITABLE_VAR(SpecialRing, VAR_ENUM, planeFilter);
}
