#include "SonicMania.h"

ObjectSpecialRing *SpecialRing;

void SpecialRing_Update(void)
{
    RSDK_THIS(SpecialRing);
    StateMachine_Run(self->state);
}

void SpecialRing_LateUpdate(void) {}

void SpecialRing_StaticUpdate(void) {}

void SpecialRing_Draw(void)
{
    RSDK_THIS(SpecialRing);
    if (self->state == SpecialRing_State_Warp) {
        self->direction = self->warpAnimator.frameID > 8;
        RSDK.DrawSprite(&self->warpAnimator, NULL, false);
    }
    else {
        RSDK.Prepare3DScene(SpecialRing->sceneIndex);
        if (self->enabled)
            RSDK.AddModelTo3DScene(SpecialRing->modelIndex, SpecialRing->sceneIndex, S3D_FLATCLR_SHADED_BLENDED, &self->matWorld, &self->matNormal,
                                   0xF0F000);
        else
            RSDK.AddModelTo3DScene(SpecialRing->modelIndex, SpecialRing->sceneIndex, S3D_FLATCLR_SHADED_WIREFRAME, &self->matWorld, &self->matNormal, 0x609090);
        RSDK.Draw3DScene(SpecialRing->sceneIndex);
    }
}

void SpecialRing_Create(void *data)
{
    RSDK_THIS(SpecialRing);
    if (!SceneInfo->inEditor) {
        self->active        = ACTIVE_BOUNDS;
        self->visible       = true;
        self->updateRange.x = 0x900000;
        self->updateRange.y = 0x900000;
        self->drawFX        = FX_FLIP;
        if (self->planeFilter > 0 && ((uint8)self->planeFilter - 1) & 2)
            self->drawOrder = Zone->drawOrderHigh;
        else
            self->drawOrder = Zone->drawOrderLow;
        self->state = SpecialRing_State_Normal;
        RSDK.SetSpriteAnimation(SpecialRing->aniFrames, 0, &self->warpAnimator, true, 0);
    }
}

void SpecialRing_StageLoad(void)
{
    SpecialRing->aniFrames = RSDK.LoadSpriteAnimation("Global/SpecialRing.bin", SCOPE_STAGE);
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

    SpecialRing->sfxSpecialRing = RSDK.GetSfx("Global/SpecialRing.wav");
    SpecialRing->sfxSpecialWarp = RSDK.GetSfx("Global/SpecialWarp.wav");

    DEBUGMODE_ADD_OBJ(SpecialRing);

    foreach_all(SpecialRing, ring) {
        if (ring->id <= 0 || globals->gameMode == MODE_TIMEATTACK || globals->gameMode == MODE_COMPETITION) {
            ring->enabled = false;
        }
        else {
            ring->enabled = !(SaveGame->saveRAM->collectedSpecialRings & (1 << ((16 * Zone->actID) + ring->id - 1)));
            if (globals->specialRingID == ring->id) {
                for (int32 p = 0; p < Player->playerCount; ++p) {
                    EntityPlayer *player = RSDK_GET_ENTITY(p, Player);

                    player->position.x = ring->position.x;
                    player->position.y = ring->position.y;
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
                SceneInfo->milliseconds = globals->tempMilliseconds;
                SceneInfo->seconds      = globals->tempSeconds;
                SceneInfo->minutes      = globals->tempMinutes;
            }
        }
    }
    globals->specialRingID = 0;
}

void SpecialRing_DebugDraw(void)
{
    RSDK.SetSpriteAnimation(Ring->aniFrames, 1, &DebugMode->animator, true, 0);
    RSDK.DrawSprite(&DebugMode->animator, NULL, false);
}
void SpecialRing_DebugSpawn(void)
{
    RSDK_THIS(SpecialRing);
    EntitySpecialRing *specialRing = CREATE_ENTITY(SpecialRing, NULL, self->position.x, self->position.y);
    specialRing->enabled           = true;
}
void SpecialRing_StartWarp(void)
{
    RSDK_THIS(SpecialRing);
    if (++self->warpTimer == 30) {
        SaveGame_SaveGameState();
        RSDK.PlaySfx(SpecialRing->sfxSpecialWarp, 0, 254);
        destroyEntity(self);
        EntitySaveGame *saveRAM = SaveGame->saveRAM;
        saveRAM->storedStageID  = SceneInfo->listPos;
        RSDK.SetScene("Special Stage", "");
        SceneInfo->listPos += saveRAM->nextSpecialStage;
#if RETRO_USE_PLUS
        if (globals->gameMode == MODE_ENCORE)
            SceneInfo->listPos += 7;
#endif
        Zone_StartFadeOut(10, 0xF0F0F0);
        RSDK.StopChannel(Music->channelID);
    }
}
void SpecialRing_State_Warp(void)
{
    RSDK_THIS(SpecialRing);
    RSDK.ProcessAnimation(&self->warpAnimator);
    if (!(Zone->timer & 3)) {
        for (int32 i = 0; i < 3; ++i) {
            EntityRing *ring = CREATE_ENTITY(Ring, NULL, (RSDK.Rand(-0x200000, 0x20000) + self->sparkleRadius) + self->position.x,
                                             self->position.y + RSDK.Rand(-0x200000, 0x200000));
            ring->state     = Ring_State_Sparkle;
            ring->stateDraw = Ring_Draw_Sparkle;
            ring->active    = ACTIVE_NORMAL;
            ring->visible   = 0;
            ring->drawOrder = Zone->drawOrderLow;
            RSDK.SetSpriteAnimation(Ring->aniFrames, i % 3 + 2, &ring->animator, true, 0);
            int32 cnt = ring->animator.frameCount;
            if (ring->animator.animationID == 2) {
                ring->alpha = 224;
                cnt >>= 1;
            }
            ring->maxFrameCount           = cnt - 1;
            ring->animator.animationSpeed = RSDK.Rand(6, 8);
            ring->timer                   = 2 * i;
        }
        self->sparkleRadius -= 0x80000;
    }

    if (SaveGame->saveRAM->chaosEmeralds == 0x7F || !self->id) {
        destroyEntity(self);
    }
    else {
        if (self->warpAnimator.frameID == self->warpAnimator.frameCount - 1) {
            self->warpTimer = 0;
            self->visible   = false;
            self->state     = SpecialRing_StartWarp;
        }
    }
}
void SpecialRing_State_Normal(void)
{
    RSDK_THIS(SpecialRing);
    self->angleZ            = (self->angleZ + 4) & 0x3FF;
    self->angleY            = (self->angleY + 4) & 0x3FF;

    Vector2 updateRange;
    updateRange.x = 0x800000;
    updateRange.y = 0x800000;
    if (!RSDK.CheckOnScreen(self, &updateRange)) {
        self->scale.x = 0;
    }

    if (self->scale.x >= 0x140)
        self->scale.x = 0x140;
    else
        self->scale.x += ((0x168 - self->scale.x) >> 5);

    RSDK.MatrixScaleXYZ(&self->matTransform, self->scale.x, self->scale.x, self->scale.x);
    RSDK.MatrixTranslateXYZ(&self->matTransform, self->position.x, self->position.y, 0, false);
    RSDK.MatrixRotateXYZ(&self->matWorld, 0, self->angleY, self->angleZ);
    RSDK.MatrixMultiply(&self->matWorld, &self->matWorld, &self->matTransform);
    RSDK.MatrixRotateX(&self->matTempRot, 0x1E0);
    RSDK.MatrixRotateXYZ(&self->matNormal, 0, self->angleY, self->angleZ);
    RSDK.MatrixMultiply(&self->matNormal, &self->matNormal, &self->matTempRot);

    if (self->enabled && self->scale.x > 0x100) {
        foreach_active(Player, player) {
            if ((self->planeFilter <= 0 || player->collisionPlane == (((uint8)self->planeFilter - 1) & 1)) && !player->sidekick) {
                if (Player_CheckCollisionTouch(player, self, &SpecialRing->hitbox) && SceneInfo->timeEnabled) {
                    self->sparkleRadius = 0x100000;
                    self->state   = SpecialRing_State_Warp;
                    EntitySaveGame *saveRAM = SaveGame->saveRAM;
#if RETRO_GAMEVER != VER_100
                    // rings spawned via debug mode give you 50 rings, always
                    if (saveRAM->chaosEmeralds != 0x7F && self->id) {
#else
                    // rings spawned via debug mode take you to special stage, always
                    if (saveRAM->chaosEmeralds != 0x7F) {
#endif
                        player->visible             = false;
                        player->active              = ACTIVE_NEVER;
                        SceneInfo->timeEnabled = false;
                    }
                    else {
                        Player_GiveRings(50, player, true);
                    }

                    if (self->id > 0) {
                        if (saveRAM->chaosEmeralds != 0x7F)
                            globals->specialRingID = self->id;
                        saveRAM->collectedSpecialRings |= 1 << (16 * Zone->actID - 1 + self->id);
                    }
                    RSDK.PlaySfx(SpecialRing->sfxSpecialRing, false, 0xFE);
                }
            }
        }
    }
}

#if RETRO_INCLUDE_EDITOR
void SpecialRing_EditorDraw(void)
{
    RSDK_THIS(SpecialRing);
    RSDK.SetSpriteAnimation(SpecialRing->aniFrames, 0, &self->warpAnimator, true, 7);
    RSDK.DrawSprite(&self->warpAnimator, NULL, false);
}

void SpecialRing_EditorLoad(void)
{
    SpecialRing->aniFrames = RSDK.LoadSpriteAnimation("Global/SpecialRing.bin", SCOPE_STAGE);

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
