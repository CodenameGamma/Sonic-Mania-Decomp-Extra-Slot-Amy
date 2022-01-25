// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: SP500MkII Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectSP500MkII *SP500MkII;

void SP500MkII_Update(void)
{
    RSDK_THIS(SP500MkII);
    self->pos2.x = self->position.x + self->printPos.x;
    self->pos2.y = self->position.y + self->printPos.y;

    StateMachine_Run(self->state);

    self->showRedLight   = false;
    self->showGreenLight = false;
    if (self->isActive && self->printRowID < self->printRows)
        self->showGreenLight = true;
    if (self->buttonDown && !self->isActive && self->state != SP500MkII_State_PrintY && self->printRowID < self->printRows)
        self->showRedLight = true;
    RSDK.ProcessAnimation(&self->animator);
}

void SP500MkII_LateUpdate(void) {}

void SP500MkII_StaticUpdate(void) {}

void SP500MkII_Draw(void)
{
    SP500MkII_DrawRails();
    SP500MkII_DrawPrinter();
    SP500MkII_DrawDebugOverlay();
}

void SP500MkII_Create(void *data)
{
    RSDK_THIS(SP500MkII);
    self->active        = ACTIVE_BOUNDS;
    self->drawOrder     = Zone->drawOrderLow;
    self->pos           = self->position;
    self->drawFX        = FX_FLIP;
    self->visible       = true;
    self->updateRange.x = (self->length + 16) << 19;
    self->updateRange.y = (self->printRows + 4) << 21;
    self->state         = SP500MkII_State_Setup;
}

void SP500MkII_StageLoad(void)
{
    SP500MkII->aniFrames = RSDK.LoadSpriteAnimation("PSZ1/SP500MkII.bin", SCOPE_STAGE);

    SP500MkII->hitboxButton.left   = -32;
    SP500MkII->hitboxButton.top    = -24;
    SP500MkII->hitboxButton.right  = 32;
    SP500MkII->hitboxButton.bottom = 23;

    SP500MkII->hitboxBody.left   = -32;
    SP500MkII->hitboxBody.top    = -38;
    SP500MkII->hitboxBody.right  = 32;
    SP500MkII->hitboxBody.bottom = -24;

    SP500MkII->hitboxSolid.left   = -23;
    SP500MkII->hitboxSolid.top    = 24;
    SP500MkII->hitboxSolid.right  = 22;
    SP500MkII->hitboxSolid.bottom = 39;
}

void SP500MkII_DrawDebugOverlay(void)
{
    RSDK_THIS(SP500MkII);
    if (SceneInfo->inEditor || DebugMode->debugActive) {
        int32 storeX = self->position.x;
        int32 storeY = self->position.y;
        self->position.x += self->printPos.x;
        self->position.y += self->printPos.y;
        DrawHelpers_DrawHitboxOutline(0xFFFF00, self->direction, self->position.x, self->position.y, &SP500MkII->hitboxBody);
        DrawHelpers_DrawHitboxOutline(0xFFFFFF, self->direction, self->position.x, self->position.y, &SP500MkII->hitboxButton);
        DrawHelpers_DrawHitboxOutline(0xFFFFFF, self->direction, self->position.x, self->position.y, &SP500MkII->hitboxSolid);
        self->position.x = storeX;
        self->position.y = storeY;
    }
}

void SP500MkII_DrawPrinter(void)
{
    RSDK_THIS(SP500MkII);
    int32 storeX = self->position.x;
    int32 storeY = self->position.y;
    self->position.x += self->printPos.x;
    self->position.y += self->printPos.y;

    Vector2 drawPos;
    drawPos.x = self->position.x;
    drawPos.y = self->printDistance + self->position.y;
    RSDK.SetSpriteAnimation(SP500MkII->aniFrames, 1, &self->animator, true, 0);
    RSDK.DrawSprite(&self->animator, &drawPos, false);

    drawPos.y -= self->printDistance;
    RSDK.SetSpriteAnimation(SP500MkII->aniFrames, 0, &self->animator, true, 0);
    RSDK.DrawSprite(&self->animator, &drawPos, false);

    if (self->showGreenLight) {
        self->inkEffect = INK_ADD;
        self->alpha     = 255;
        RSDK.SetSpriteAnimation(SP500MkII->aniFrames, 2, &self->animator, true, 0);
        RSDK.DrawSprite(&self->animator, &drawPos, false);

        self->inkEffect = INK_NONE;
    }

    if (self->showRedLight) {
        self->inkEffect = INK_ADD;
        self->alpha     = 255;
        RSDK.SetSpriteAnimation(SP500MkII->aniFrames, 3, &self->animator, true, 0);
        RSDK.DrawSprite(&self->animator, &drawPos, false);

        self->inkEffect = INK_NONE;
    }
    self->position.x = storeX;
    self->position.y = storeY;
}

void SP500MkII_DrawRails(void)
{
    RSDK_THIS(SP500MkII);

    Vector2 drawPos;
    drawPos.x = self->position.x - 0x280000;
    drawPos.y = self->printPos.y + self->position.y;
    RSDK.SetSpriteAnimation(SP500MkII->aniFrames, 4, &self->animator, true, 0);
    RSDK.DrawSprite(&self->animator, &drawPos, false);

    self->animator.frameID = 1;
    RSDK.DrawSprite(&self->animator, &drawPos, false);

    RSDK.SetSpriteAnimation(SP500MkII->aniFrames, 5, &self->animator, true, 0);
    RSDK.DrawSprite(&self->animator, &drawPos, false);

    drawPos.x += 0x80000;
    self->animator.frameID = 1;
    for (int32 l = 0; l < self->length + 8; ++l) {
        RSDK.DrawSprite(&self->animator, &drawPos, false);
        drawPos.x += 0x80000;
    }

    self->animator.frameID = 2;
    RSDK.DrawSprite(&self->animator, &drawPos, false);

    drawPos.x += 0x80000;
    self->direction = FLIP_X;
    RSDK.SetSpriteAnimation(SP500MkII->aniFrames, 4, &self->animator, true, 0);
    RSDK.DrawSprite(&self->animator, &drawPos, false);

    self->animator.frameID = 1;
    RSDK.DrawSprite(&self->animator, &drawPos, false);

    self->direction = FLIP_NONE;
}

void SP500MkII_CheckPlayerCollisions(void)
{
    RSDK_THIS(SP500MkII);

    int32 storeX     = self->position.x;
    int32 storeY     = self->position.y;
    self->position.x = self->pos2.x;
    self->position.y = self->pos2.y;
    int32 top        = SP500MkII->hitboxBody.top << 16;

    int32 dist = 0;
    foreach_active(Player, player)
    {
        int32 playerID = RSDK.GetEntityID(player);
        int32 bottom   = player->position.y + (Player_GetHitbox(player)->bottom << 16);
        Player_CheckCollisionBox(player, self, &SP500MkII->hitboxSolid);
        int32 side     = Player_CheckCollisionBox(player, self, &SP500MkII->hitboxButton);
        bool32 touched = Player_CheckCollisionTouch(player, self, &SP500MkII->hitboxBody);

        if (side == C_TOP) {
            player->position.x += self->printMoveOffset.x;
            player->position.y += self->printMoveOffset.y;
        }

        if (touched) {
            int32 distY = minVal(bottom - self->position.y + top, 0xD0000);

            if (dist <= distY)
                dist = distY;

            if (side == C_TOP)
                self->activePlayers |= (1 << playerID);
            else
                self->activePlayers &= ~(1 << playerID);
        }
        else {
            self->activePlayers &= ~(1 << playerID);
        }
    }

    self->printDistance = dist;
    self->buttonDown  = self->activePlayers != 0;
    self->position.x    = storeX;
    self->position.y    = storeY;
}

void SP500MkII_State_Setup(void)
{
    RSDK_THIS(SP500MkII);
    self->printPos.x = 0;
    self->printPos.y = 0;
    if (self->start == 1)
        self->printPos.x = self->length << 19;
    self->timer          = 0;
    self->activePlayers  = 0;
    self->printDistance  = 0;
    self->buttonDown   = false;
    self->isActive       = false;
    self->xDir           = 2 * (self->start == 0) - 1;
    self->showGreenLight = false;
    self->showRedLight   = false;
    self->state          = SP500MkII_State_CheckActivated;
}

void SP500MkII_State_CheckActivated(void)
{
    RSDK_THIS(SP500MkII);
    self->printMoveOffset.x = 0;
    self->printMoveOffset.y = 0;
    SP500MkII_CheckPlayerCollisions();

    if (self->activePlayers & 1) {
        ++self->timer;
    }
    else {
        if (self->isActive) {
            if (self->printRowID < self->printRows)
                self->state = SP500MkII_State_PrintX;
        }
        self->timer = 0;
    }
    if (self->timer == 30)
        self->isActive = true;
}

void SP500MkII_State_PrintX(void)
{
    RSDK_THIS(SP500MkII);
    self->printMoveOffset = self->printPos;

    if (self->xDir <= 0) {
        if (self->printPos.x > 0)
            self->printPos.x -= 0x40000;
        if (self->printPos.x < 0)
            self->printPos.x = 0;

        if (!self->printPos.x) {
            self->xDir = 1;
            ++self->printRowID;
            self->state    = SP500MkII_State_PrintY;
            self->isActive = false;
        }
    }
    else {
        if (self->printPos.x < self->length << 19)
            self->printPos.x += 0x40000;
        if (self->printPos.x > self->length << 19)
            self->printPos.x = self->length << 19;

        if (self->printPos.x == self->length << 19) {
            self->xDir = -1;
            ++self->printRowID;
            self->state    = SP500MkII_State_PrintY;
            self->isActive = false;
        }
    }

    self->printMoveOffset.x = self->printPos.x - self->printMoveOffset.x;
    self->printMoveOffset.y = 0;
    SP500MkII_CheckPlayerCollisions();
}

void SP500MkII_State_PrintY(void)
{
    RSDK_THIS(SP500MkII);
    self->printMoveOffset.x = self->printPos.x;
    self->printMoveOffset.y = self->printPos.y;

    int32 dist = (self->printRowID * (2 * (self->yDir != 0) - 1)) << 21;
    if (self->yDir == 1) {
        if (self->printPos.y < dist)
            self->printPos.y += 0x40000;

        if (self->printPos.y > dist)
            self->printPos.y = dist;
    }
    else {
        if (self->printPos.y > dist)
            self->printPos.y -= 0x40000;

        if (self->printPos.y < dist)
            self->printPos.y = dist;
    }

    if (self->printPos.y == dist)
        self->state = SP500MkII_State_CheckActivated;
    self->printMoveOffset.x = 0;
    self->printMoveOffset.y = self->printPos.y - self->printMoveOffset.y;
    SP500MkII_CheckPlayerCollisions();
}

#if RETRO_INCLUDE_EDITOR
void SP500MkII_EditorDraw(void)
{
    SP500MkII_DrawRails();
    SP500MkII_DrawPrinter();
    SP500MkII_DrawDebugOverlay();
}

void SP500MkII_EditorLoad(void) { SP500MkII->aniFrames = RSDK.LoadSpriteAnimation("PSZ1/SP500MkII.bin", SCOPE_STAGE); }
#endif

void SP500MkII_Serialize(void)
{
    RSDK_EDITABLE_VAR(SP500MkII, VAR_UINT8, start);
    RSDK_EDITABLE_VAR(SP500MkII, VAR_UINT8, yDir);
    RSDK_EDITABLE_VAR(SP500MkII, VAR_UINT32, printRows);
    RSDK_EDITABLE_VAR(SP500MkII, VAR_UINT32, length);
}
