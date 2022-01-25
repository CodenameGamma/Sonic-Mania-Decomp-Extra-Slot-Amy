// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: Sol Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectSol *Sol;

void Sol_Update(void)
{
    RSDK_THIS(Sol);
    StateMachine_Run(self->state);
}

void Sol_LateUpdate(void) {}

void Sol_StaticUpdate(void) {}

void Sol_Draw(void)
{
    RSDK_THIS(Sol);
    for (int32 i = 0; i < Sol_MaxFlameOrbs; ++i) {
        if ((1 << i) & self->activeOrbs)
            RSDK.DrawSprite(&self->animator2, &self->positions[i], false);
    }

    RSDK.DrawSprite(&self->animator1, NULL, false);
}

void Sol_Create(void *data)
{
    RSDK_THIS(Sol);
    self->visible   = true;
    self->drawOrder = Zone->drawOrderLow;
    self->drawFX    = FX_FLIP;
    if (data) {
        RSDK.SetSpriteAnimation(Sol->aniFrames, 1, &self->animator1, true, 0);
        self->active = ACTIVE_NORMAL;
        self->drawFX |= FX_ROTATE;
        self->updateRange.x = 0x1000000;
        self->updateRange.y = 0x1000000;
        self->state         = Sol_Unknown8;
    }
    else {
        self->startPos      = self->position;
        self->startDir      = self->direction;
        self->active        = ACTIVE_BOUNDS;
        self->updateRange.x = 0x800000;
        self->updateRange.y = 0x800000;
        self->activeOrbs    = 2 | 8;
        RSDK.SetSpriteAnimation(Sol->aniFrames, 0, &self->animator1, true, 0);
        RSDK.SetSpriteAnimation(Sol->aniFrames, 1, &self->animator2, true, 0);
        self->state = Sol_State_Setup;

        if (self->direction == FLIP_NONE)
            self->velocity.x = -0x4000;
        else
            self->velocity.x = 0x4000;
    }
}

void Sol_StageLoad(void)
{
    if (RSDK.CheckStageFolder("OOZ1") || RSDK.CheckStageFolder("OOZ2"))
        Sol->aniFrames = RSDK.LoadSpriteAnimation("OOZ/Sol.bin", SCOPE_STAGE);

    Sol->hitboxBadnik.left   = -8;
    Sol->hitboxBadnik.top    = -8;
    Sol->hitboxBadnik.right  = 8;
    Sol->hitboxBadnik.bottom = 8;

    Sol->hitboxOrb.left   = -4;
    Sol->hitboxOrb.top    = -4;
    Sol->hitboxOrb.right  = 4;
    Sol->hitboxOrb.bottom = 4;

    DEBUGMODE_ADD_OBJ(Sol);
}

void Sol_DebugSpawn(void)
{
    RSDK_THIS(Sol);
    CREATE_ENTITY(Sol, NULL, self->position.x, self->position.y);
}

void Sol_DebugDraw(void)
{
    RSDK.SetSpriteAnimation(Sol->aniFrames, 0, &DebugMode->animator, true, 0);
    RSDK.DrawSprite(&DebugMode->animator, 0, false);
}

void Sol_HandlePlayerInteractions(void)
{
    RSDK_THIS(Sol);

    int32 storeX = self->position.x;
    int32 storeY = self->position.y;
    for (int32 i = 0; i < Sol_MaxFlameOrbs; ++i) {
        if ((1 << i) & self->activeOrbs) {
            self->position.x = self->positions[i].x;
            self->position.y = self->positions[i].y;
            foreach_active(Player, player)
            {
#if RETRO_USE_PLUS
                if (player->state != Player_State_MightyHammerDrop) {
#endif
                    Sol_HandlePlayerHurt();
#if RETRO_USE_PLUS
                }
#endif
            }
        }
    }

    self->position.x = storeX;
    self->position.y = storeY;

    foreach_active(Player, player)
    {
        if (Player_CheckBadnikTouch(player, self, &Sol->hitboxBadnik) && Player_CheckBadnikBreak(self, player, false)) {
            int32 angle = self->angle;
            for (int32 i = 0; i < Sol_MaxFlameOrbs; ++i) {
                if ((1 << i) & self->activeOrbs) {
                    self->position.x = self->positions[i].x;
                    self->position.y = self->positions[i].y;
                    EntitySol *sol   = CREATE_ENTITY(Sol, intToVoid(true), self->positions[i].x, self->positions[i].y);

                    sol->state = Sol_Unknown9;
#if RETRO_USE_PLUS
                    if (player->state == Player_State_MightyHammerDrop)
                        sol->interaction = false;
#endif
                    sol->velocity.x = 0x380 * RSDK.Cos256(angle);
                    sol->velocity.y = 0x380 * RSDK.Sin256(angle);
                }
                angle += (0x100 / Sol_MaxFlameOrbs);
            }
            destroyEntity(self);
        }
    }
}

void Sol_HandlePlayerHurt(void)
{
    RSDK_THIS(Sol);
    foreach_active(Player, player)
    {
        if (Player_CheckCollisionTouch(player, self, &Sol->hitboxOrb)) {
            Player_CheckElementalHit(player, self, SHIELD_FIRE);
        }
    }
}

void Sol_HandleRotation(void)
{
    RSDK_THIS(Sol);

    int32 angle = self->angle;
    if (self->direction)
        self->angle = (angle - 1) & 0xFF;
    else
        self->angle = (angle + 1) & 0xFF;

    for (int32 i = 0; i < Sol_MaxFlameOrbs; ++i) {
        if ((1 << i) & self->activeOrbs) {
            self->positions[i].x = (RSDK.Cos256(angle) << 12) + self->position.x;
            self->positions[i].y = (RSDK.Sin256(angle) << 12) + self->position.y;
        }
        angle += (0x100 / Sol_MaxFlameOrbs);
    }
}

void Sol_CheckOnScreen(void)
{
    RSDK_THIS(Sol);
    if (!RSDK.CheckOnScreen(self, NULL) && !RSDK.CheckPosOnScreen(&self->startPos, &self->updateRange)) {
        self->position  = self->startPos;
        self->direction = self->startDir;
        Sol_Create(NULL);
    }
}

void Sol_State_Setup(void)
{
    RSDK_THIS(Sol);
    self->active = ACTIVE_NORMAL;
    self->state  = Sol_Unknown5;
    Sol_Unknown5();
}

void Sol_Unknown5(void)
{
    RSDK_THIS(Sol);
    RSDK.ProcessAnimation(&self->animator2);

    self->position.y = (RSDK.Sin256(self->field_88) << 10) + self->startPos.y;
    self->field_88 += 4;
    self->position.x += self->velocity.x;
    Sol_HandleRotation();
    Sol_HandlePlayerInteractions();

    if (self->fireOrbs) {
        EntityPlayer *playerPtr = NULL;
        int32 distanceX         = 0x7FFFFFFF;
        int32 distanceY         = 0x7FFFFFFF;
        foreach_active(Player, player)
        {
            if (abs(player->position.y - self->position.y) < distanceY)
                distanceY = abs(player->position.y - self->position.y);

            if (abs(player->position.y - self->position.y) < 0x400000) {
                if (!playerPtr) {
                    if (abs(player->position.x - self->position.x) < distanceX) {
                        distanceX = abs(player->position.x - self->position.x);
                        playerPtr = player;
                    }
                }
                else {
                    if (abs(player->position.x - self->position.x) < distanceX) {
                        distanceX = abs(player->position.x - self->position.x);
                        playerPtr = player;
                    }
                }
            }
        }

        if (!playerPtr)
            playerPtr = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);

        if (distanceX <= 0x800000) {
            self->state             = Sol_Unknown6;
            self->animator1.frameID = 1;
        }

        self->direction = playerPtr->position.x >= self->position.x;
    }

    Sol_CheckOnScreen();
}

void Sol_Unknown6(void)
{
    RSDK_THIS(Sol);
    RSDK.ProcessAnimation(&self->animator2);

    self->position.y = (RSDK.Sin256(self->field_88) << 10) + self->startPos.y;
    self->field_88 += 4;
    self->position.x += self->velocity.x;
    int32 angle = self->angle;
    Sol_HandleRotation();

    for (int32 i = 0; i < Sol_MaxFlameOrbs; ++i) {
        if (angle == 64) {
            if ((1 << i) & self->activeOrbs) {
                self->activeOrbs &= ~(1 << i);
                EntitySol *sol = CREATE_ENTITY(Sol, intToVoid(true), self->positions[i].x, self->positions[i].y);
                if (self->direction == FLIP_NONE)
                    sol->velocity.x = -0x20000;
                else
                    sol->velocity.x = 0x20000;
            }
            angle += (0x100 / Sol_MaxFlameOrbs);
        }
    }
    Sol_HandlePlayerInteractions();

    if (!self->activeOrbs) {
        self->state = Sol_Unknown7;
        if (self->direction == FLIP_NONE)
            self->velocity.x = -0x4000;
        else
            self->velocity.x = 0x4000;
    }

    if (self->animator1.timer >= 0x10)
        self->animator1.frameID = 2;
    else
        self->animator1.timer++;
    Sol_CheckOnScreen();
}

void Sol_Unknown7(void)
{
    RSDK_THIS(Sol);
    RSDK.ProcessAnimation(&self->animator2);
    self->position.y = (RSDK.Sin256(self->field_88) << 10) + self->startPos.y;
    self->field_88 += 4;
    self->position.x += self->velocity.x;
    Sol_HandleRotation();
    Sol_HandlePlayerInteractions();
    Sol_CheckOnScreen();
}

void Sol_Unknown8(void)
{
    RSDK_THIS(Sol);
    RSDK.ProcessAnimation(&self->animator1);
    self->position.x += self->velocity.x;
    if (RSDK.CheckOnScreen(self, &self->updateRange))
        Sol_HandlePlayerHurt();
    else
        destroyEntity(self);
}

void Sol_Unknown9(void)
{
    RSDK_THIS(Sol);
    if (RSDK.CheckOnScreen(self, &self->updateRange)) {
        self->position.x += self->velocity.x;
        self->position.y += self->velocity.y;
        self->velocity.y += 0x3800;
        self->rotation = 2 * RSDK.ATan2(self->velocity.x >> 16, self->velocity.y >> 16) + 384;

        int32 offsetX = RSDK.Sin512(512 - self->rotation) << 10;
        int32 offsetY = RSDK.Cos512(512 - self->rotation) << 10;
        int32 cmode   = 3 - (((self->rotation - 64) >> 7) & 3);

        bool32 collided = RSDK.ObjectTileCollision(self, Zone->fgLayers, cmode, 1, offsetX, offsetY, true);
        if (!collided) {
            collided = RSDK.ObjectTileCollision(self, Zone->fgLayers, cmode, 0, offsetX, offsetY, true);
        }

        if (collided) {
            RSDK.SetSpriteAnimation(Sol->aniFrames, 2, &self->animator1, true, 0);
            self->state    = Sol_Unknown10;
            self->rotation = (self->rotation + 64) & 0x180;
        }

        int32 spawnX = self->position.x + offsetX;
        int32 spawnY = self->position.y + offsetY;
        uint16 tile  = RSDK.GetTileInfo(Zone->fgHigh, spawnX >> 20, (spawnY - 0x10000) >> 20);
        if (tile == (uint16)-1)
            tile = RSDK.GetTileInfo(Zone->fgLow, spawnX >> 20, (spawnY - 0x10000) >> 20);

        int32 behaviour = RSDK.GetTileBehaviour(tile, 0);
        if (((behaviour == 2 || behaviour == 3) && collided) || behaviour == 1) {
            self->position.x = spawnX - 0x40000;
            self->position.y = spawnY - 0x80000;
            self->rotation   = 0;
            self->velocity.x = -0x40000;
            self->velocity.y = 0;
            RSDK.SetSpriteAnimation(Sol->aniFrames, 3, &self->animator1, true, 0);
            self->state = Sol_Unknown11;

            EntitySol *sol  = CREATE_ENTITY(Sol, intToVoid(true), spawnX, spawnY - 0x80000);
            sol->velocity.x = 0x40000;
            sol->velocity.y = 0;
            RSDK.SetSpriteAnimation(Sol->aniFrames, 3, &sol->animator1, true, 0);
            sol->state    = Sol_Unknown11;
            sol->field_88 = sol->position.x & 0xF00000;

            if (behaviour == 1) {
                self->position.y = (self->position.y & 0xFFF00000) + 0x20000;
                sol->position.y  = (sol->position.y & 0xFFF00000) + 0x20000;
                sol->state       = Sol_Unknown12;
                self->state      = Sol_Unknown12;
            }
            else {
                self->position.y -= 0x80000;
            }
        }

        if (self->interaction)
            Sol_HandlePlayerHurt();
    }
    else {
        destroyEntity(self);
    }
}

void Sol_Unknown10(void)
{
    RSDK_THIS(Sol);
    RSDK.ProcessAnimation(&self->animator1);
    if (!RSDK.CheckOnScreen(self, &self->updateRange) || self->animator1.frameID == self->animator1.frameCount - 1) {
        destroyEntity(self);
    }
}

void Sol_Unknown11(void)
{
    RSDK_THIS(Sol);
    if (RSDK.CheckOnScreen(self, &self->updateRange)) {
        bool32 collided = RSDK.ObjectTileGrip(self, Zone->fgLayers, CMODE_FLOOR, 1, 0, 0x80000, 16);
        if (!collided)
            collided = RSDK.ObjectTileGrip(self, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x80000, 16);

        if (collided) {
            uint16 tile = RSDK.GetTileInfo(Zone->fgHigh, self->position.x >> 20, (self->position.y + 0x90000) >> 20);
            if (tile == (uint16)-1)
                tile = RSDK.GetTileInfo(Zone->fgLow, self->position.x >> 20, (self->position.y + 0x90000) >> 20);
            self->rotation = 2 * RSDK.GetTileAngle(tile, 0, 0);
        }

        uint16 tile = RSDK.GetTileInfo(Zone->fgHigh, self->position.x >> 20, (self->position.y + 0x70000) >> 20);
        if (tile == (uint16)-1)
            tile = RSDK.GetTileInfo(Zone->fgLow, self->position.x >> 20, (self->position.y + 0x70000) >> 20);

        int32 behaviour = RSDK.GetTileBehaviour(tile, 0);
        if (!behaviour || behaviour == 4) {
            if (collided) {
                RSDK.SetSpriteAnimation(Sol->aniFrames, 2, &self->animator1, true, 0);
                self->state = Sol_Unknown10;
            }
            else {
                self->state = Sol_Unknown9;
            }
        }
        else {
            self->position.y -= 0x80000;
            if ((self->position.x & 0xF00000) != self->field_88)
                OOZSetup_StartFire(self->position.y & 0xFFFF0000, (self->position.x & 0xFFF00000) + 0x70000, self->rotation >> 1);
            self->field_88 = self->position.x & 0xF00000;
        }
        self->position.x += self->velocity.x;
        self->position.y += 0x80000;
        RSDK.ProcessAnimation(&self->animator1);
        Sol_HandlePlayerHurt();
    }
    else {
        destroyEntity(self);
    }
}

void Sol_Unknown12(void)
{
    RSDK_THIS(Sol);
    if (RSDK.CheckOnScreen(self, &self->updateRange)) {
        uint16 tile = RSDK.GetTileInfo(Zone->fgHigh, self->position.x >> 20, (self->position.y + 0xF0000) >> 20);
        if (tile == (uint16)-1)
            tile = RSDK.GetTileInfo(Zone->fgLow, self->position.x >> 20, (self->position.y + 0xF0000) >> 20);

        if (RSDK.GetTileBehaviour(tile, 0) == 1) {
            if ((self->position.x & 0xF00000) != self->field_88)
                OOZSetup_StartFire(self->position.y & 0xFFFF0000, (self->position.x & 0xFFF00000) + 0x70000, self->rotation >> 1);
            self->field_88 = self->position.x & 0xF00000;
        }
        else {
            RSDK.SetSpriteAnimation(Sol->aniFrames, 2, &self->animator1, true, 0);
            self->state = Sol_Unknown10;
        }
        self->position.x += self->velocity.x;
        RSDK.ProcessAnimation(&self->animator1);
        Sol_HandlePlayerHurt();
    }
    else {
        destroyEntity(self);
    }
}

#if RETRO_INCLUDE_EDITOR
void Sol_EditorDraw(void)
{
    RSDK_THIS(Sol);

    int32 angle = self->angle;
    Sol_HandleRotation();
    self->angle = angle;

    Sol_Draw();
}

void Sol_EditorLoad(void) { Sol->aniFrames = RSDK.LoadSpriteAnimation("OOZ/Sol.bin", SCOPE_STAGE); }
#endif

void Sol_Serialize(void)
{
    RSDK_EDITABLE_VAR(Sol, VAR_UINT8, direction);
    RSDK_EDITABLE_VAR(Sol, VAR_BOOL, fireOrbs);
}
