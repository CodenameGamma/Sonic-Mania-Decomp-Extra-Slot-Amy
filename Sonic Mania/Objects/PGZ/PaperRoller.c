// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: PaperRoller Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectPaperRoller *PaperRoller;

void PaperRoller_Update(void)
{
    RSDK_THIS(PaperRoller);
    RSDK.ProcessAnimation(&self->rollerAnimator);

    int32 startX  = self->position.x - (RSDK.Cos256(self->angle) << 7) * self->length;
    int32 startY  = self->position.y - (RSDK.Sin256(self->angle) << 7) * self->length;
    int32 endX    = self->position.x + (RSDK.Cos256(self->angle) << 7) * self->length;
    int32 endY    = self->position.y + (RSDK.Sin256(self->angle) << 7) * self->length;
    int32 centerX = self->position.x;
    int32 centerY = self->position.y;

    // left roller
    self->position.x = startX;
    self->position.y = startY;
    PaperRoller_HandleRollerCollisions();

    // right roller
    self->position.x = endX;
    self->position.y = endY;
    PaperRoller_HandleRollerCollisions();

    // print 
    self->position.x = centerX;
    self->position.y = centerY;
    PaperRoller_HandlePrintCollisions();

    if (self->direction == FLIP_NONE)
        self->divotAngle += 6;
    else
        self->divotAngle -= 6;

    if (self->divotAngle < 0)
        self->divotAngle += 0x100;
    self->divotAngle &= 0xFF;
}

void PaperRoller_LateUpdate(void) {}

void PaperRoller_StaticUpdate(void) {}

void PaperRoller_Draw(void)
{
    PaperRoller_DrawPaperLines();
    PaperRoller_DrawRollers();
}

void PaperRoller_Create(void *data)
{
    RSDK_THIS(PaperRoller);

    self->active        = ACTIVE_BOUNDS;
    self->drawOrder     = Zone->drawOrderLow;
    self->startPos.x    = self->position.x;
    self->startPos.y    = self->position.y;
    self->visible       = true;
    self->drawFX        = FX_NONE;
    self->updateRange.x = 0x800000;
    self->updateRange.y = 0x800000;

    if ((RSDK.Cos256(self->angle) << 8) * self->length >= 0)
        self->updateRange.x += (RSDK.Cos256(self->angle) << 8) * self->length;
    else
        self->updateRange.x += -((RSDK.Cos256(self->angle) << 8) * self->length);

    if ((RSDK.Sin256(self->angle) << 8) * self->length >= 0)
        self->updateRange.y += (RSDK.Sin256(self->angle) << 8) * self->length;
    else
        self->updateRange.y += -((RSDK.Sin256(self->angle) << 8) * self->length);

    if (self->length > 256)
        self->length = 256;

    self->lastJumpTimer[0] = 0x7FFF;
    self->lastJumpTimer[1] = 0x7FFF;
    self->lastJumpTimer[2] = 0x7FFF;
    self->lastJumpTimer[3] = 0x7FFF;
    self->hitbox.left      = -24;
    self->hitbox.top       = -24;
    self->hitbox.right     = 24;
    self->hitbox.bottom    = 24;
    RSDK.SetSpriteAnimation(PaperRoller->aniFrames, 0, &self->rollerAnimator, true, 0);
    RSDK.SetSpriteAnimation(PaperRoller->aniFrames, 1, &self->divotAnimator, true, 0);
}

void PaperRoller_StageLoad(void)
{
    uint32 colours[]       = { 0x0F0F0E8, 0x0D0B898, 0x987870, 0x586868 };
    PaperRoller->aniFrames = RSDK.LoadSpriteAnimation("PSZ1/PaperRoller.bin", SCOPE_STAGE);
    for (int32 i = 0; i < 0x40; ++i) PaperRoller->colours[i] = colours[i & 3];
    PaperRoller->sfxPaper = RSDK.GetSfx("PSZ/Paper.wav");
}

void PaperRoller_DrawDeformedLine(uint32 colour, int32 len, int32 startX, int32 startY, int32 endX, int32 endY, int32 offsetX, int32 offsetY,
                                  int32 deformX, int32 deformY)
{
    RSDK_THIS(PaperRoller);

    int32 count = (self->length / 4) + (self->length % 4 > 0);
    if (count) {
        int32 negAngle = -(uint8)(self->angle);

        int32 posX  = startX;
        int32 posY  = startY;
        int32 moveX = (endX - startX) / count;
        int32 moveY = (endY - startY) / count;

        for (int32 i = 0; i < count; ++i) {
            int32 clrOffset = 0;
            if (self->direction)
                clrOffset = len % count;
            else
                clrOffset = count - len % count - 1;
            uint32 lineClr = PaperRoller->colours[(Zone->timer + clrOffset) % count];

            if (!deformY) {
                RSDK.DrawLine(offsetX + posX, offsetY + posY, offsetX + (posX + moveX), offsetY + (posY + moveY), colour ? colour : lineClr, 127,
                              INK_NONE, false);
            }
            else {
                int32 distX = (posX - self->position.x) >> 8;
                int32 distY = (posY - self->position.y) >> 8;

                int32 angValX = distY * RSDK.Sin256(self->angle) + distX * RSDK.Cos256(self->angle);

                int32 angValY = self->position.y - distX * RSDK.Sin256(self->angle) + distY * RSDK.Cos256(self->angle) - self->position.y;
                int32 lenY    = 0;

                if (angValX >= deformX) {
                    int32 length = (self->length << 15) - deformX;
                    if (length < 0)
                        length = deformX - (self->length << 15);
                    if (length >> 16 > 0)
                        lenY = (deformY >> 8) * ((length + deformX - angValX) >> 8) / (length >> 16);
                }
                else {
                    int32 length = 0;
                    if ((self->length << 15) + deformX >= 0)
                        length = (self->length << 15) + deformX;
                    else
                        length = -(deformX + (self->length << 15));

                    if (length >> 16 > 0)
                        lenY = (deformY >> 8) * (((self->length << 15) + angValX) >> 8) / (length >> 16);
                }

                distX = angValX >> 8;
                distY = (lenY + angValY) >> 8;

                int32 offsetX1 = self->position.x + distY * RSDK.Sin256(negAngle) + distX * RSDK.Cos256(negAngle);
                int32 offsetY1 = self->position.y - distX * RSDK.Sin256(negAngle) + distY * RSDK.Cos256(negAngle);

                distX = ((posX + moveX) - self->position.x) >> 8;
                distY = ((posY + moveY) - self->position.y) >> 8;

                angValX = distY * RSDK.Sin256(self->angle) + distX * RSDK.Cos256(self->angle);
                angValY = self->position.y - distX * RSDK.Sin256(self->angle) + distY * RSDK.Cos256(self->angle) - self->position.y;

                lenY = 0;
                if (angValX >= deformX) {
                    int32 length = (self->length << 15) - deformX;
                    if (length < 0)
                        length = deformX - (self->length << 15);

                    if (length >> 16 > 0)
                        lenY = (deformY >> 8) * ((length + deformX - angValX) >> 8) / (length >> 16);
                }
                else {
                    int32 length = 0;
                    if ((self->length << 15) + deformX >= 0)
                        length = (self->length << 15) + deformX;
                    else
                        length = -(deformX + (self->length << 15));

                    if (length >> 16 > 0)
                        lenY = (deformY >> 8) * (((self->length << 15) + angValX) >> 8) / (length >> 16);
                }

                distX          = angValX >> 8;
                distY          = (lenY + angValY) >> 8;
                int32 offsetX2 = self->position.x + distY * RSDK.Sin256(negAngle) + distX * RSDK.Cos256(negAngle);
                int32 offsetY2 = self->position.y - distX * RSDK.Sin256(negAngle) + distY * RSDK.Cos256(negAngle);
                RSDK.DrawLine(offsetX + offsetX1, offsetY + offsetY1, offsetX + offsetX2, offsetY + offsetY2, colour ? colour : lineClr, 127,
                              INK_NONE, false);
            }
            ++len;
            posY += moveY;
            posX += moveX;
        }
    }
}

void PaperRoller_DrawPaperLines(void)
{
    RSDK_THIS(PaperRoller);

    int32 x1 = self->position.x - (RSDK.Cos256(self->angle) << 7) * self->length;
    int32 y1 = self->position.y - (RSDK.Sin256(self->angle) << 7) * self->length;
    int32 x2 = self->position.x + (RSDK.Cos256(self->angle) << 7) * self->length;
    int32 y2 = self->position.y + (RSDK.Sin256(self->angle) << 7) * self->length;

    int32 len     = self->length / 4 + (self->length % 4 > 0);
    int32 startX1 = x1 + 0x1800 * RSDK.Cos256(self->angle - 64);
    int32 startY1 = y1 + 0x1800 * RSDK.Sin256(self->angle - 64);
    int32 endX1   = x2 + 0x1800 * RSDK.Cos256(self->angle - 64);
    int32 endY1   = y2 + 0x1800 * RSDK.Sin256(self->angle - 64);

    int32 startX2 = x2 + 0x1800 * RSDK.Cos256(self->angle + 64);
    int32 startY2 = y2 + 0x1800 * RSDK.Sin256(self->angle + 64);
    int32 endX2   = x1 + 0x1800 * RSDK.Cos256(self->angle + 64);
    int32 endY2   = y1 + 0x1800 * RSDK.Sin256(self->angle + 64);

    PaperRoller_DrawDeformedLine(0x000000, 0, startX1, startY1, endX1, endY1, 0, 0, self->deformPosTop.x, self->deformPosTop.y);
    PaperRoller_DrawDeformedLine(0x000000, len, startX2, startY2, endX2, endY2, 0, 0, self->deformPosBottom.x, self->deformPosBottom.y);

    int32 angle = self->angle + 32;
    if (angle < 0)
        angle = ((-1 - (self->angle + 32)) & 0xFFFFFF00) + self->angle + 288;

    int32 offsetY1 = 0, offsetY2 = 0, offsetX2 = 0, offsetX1 = 0;
    switch (angle >> 6) {
        case 0:
            offsetY1 = 0x10000;
            offsetY2 = -0x10000;
            break;
        case 1:
            offsetX1 = -0x10000;
            offsetX2 = 0x10000;
            break;
        case 2:
            offsetY1 = -0x10000;
            offsetY2 = 0x10000;
            break;
        case 3:
            offsetX1 = 0x10000;
            offsetX2 = -0x10000;
            break;
    }
    PaperRoller_DrawDeformedLine(0x000000, 0, startX1, startY1, endX1, endY1, offsetX1, offsetY1, self->deformPosTop.x, self->deformPosTop.y);
    PaperRoller_DrawDeformedLine(0x000000, len, startX2, startY2, endX2, endY2, offsetX2, offsetY2, self->deformPosBottom.x, self->deformPosBottom.y);

    offsetX1 <<= 1;
    offsetY1 <<= 1;
    offsetX2 <<= 1;
    offsetY2 <<= 1;
    PaperRoller_DrawDeformedLine(0xD0B898, 0, startX1, startY1, endX1, endY1, offsetX1, offsetY1, self->deformPosTop.x, self->deformPosTop.y);
    PaperRoller_DrawDeformedLine(0xD0B898, len, startX2, startY2, endX2, endY2, offsetX2, offsetY2, self->deformPosBottom.x, self->deformPosBottom.y);
}

void PaperRoller_DrawRollers(void)
{
    RSDK_THIS(PaperRoller);
    Vector2 rollerPos;
    Vector2 divotPos;

    rollerPos = self->position;
    rollerPos.x -= (RSDK.Cos256(self->angle) << 7) * self->length;
    rollerPos.y -= (RSDK.Sin256(self->angle) << 7) * self->length;
    RSDK.DrawSprite(&self->rollerAnimator, &rollerPos, false);

    divotPos = rollerPos;
    divotPos.x += -0x900 * RSDK.Cos256(self->divotAngle);
    divotPos.y += -0x900 * RSDK.Sin256(self->divotAngle);
    RSDK.DrawSprite(&self->divotAnimator, &divotPos, false);

    divotPos = rollerPos;
    divotPos.x += 0x900 * RSDK.Cos256(self->divotAngle);
    divotPos.y += 0x900 * RSDK.Sin256(self->divotAngle);
    RSDK.DrawSprite(&self->divotAnimator, &divotPos, false);

    rollerPos = self->position;
    rollerPos.x += (RSDK.Cos256(self->angle) << 7) * self->length;
    rollerPos.y += (RSDK.Sin256(self->angle) << 7) * self->length;
    RSDK.DrawSprite(&self->rollerAnimator, &rollerPos, false);

    divotPos = rollerPos;
    divotPos.x += -0x900 * RSDK.Cos256(self->divotAngle);
    divotPos.y += -0x900 * RSDK.Sin256(self->divotAngle);
    RSDK.DrawSprite(&self->divotAnimator, &divotPos, false);

    divotPos = rollerPos;
    divotPos.x += 0x900 * RSDK.Cos256(self->divotAngle);
    divotPos.y += 0x900 * RSDK.Sin256(self->divotAngle);
    RSDK.DrawSprite(&self->divotAnimator, &divotPos, false);
}

void PaperRoller_HandleRollerCollisions(void)
{
    RSDK_THIS(PaperRoller);

    foreach_active(Player, player)
    {
        int32 playerID = RSDK.GetEntityID(player);
        if (self->playerTimer[playerID] <= 0) {
            int32 distX = abs(self->position.x - player->position.x);
            int32 distY = abs(self->position.y - player->position.y);
            if (MathHelpers_SquareRoot((distX >> 16) * (distX >> 16) + (distY >> 16) * (distY >> 16)) <= 40 && !self->playerTimer[playerID]) {
                RSDK.PlaySfx(Player->sfxRelease, false, 255);
                int32 angle = RSDK.ATan2(player->position.x - self->position.x, player->position.y - self->position.y);

                int32 ang = 0;
                if (self->direction == FLIP_NONE)
                    ang = angle + 64;
                else
                    ang = angle - 64;

                player->position.x = self->position.x;
                player->position.y = self->position.y;
                player->position.x += 0x2800 * RSDK.Cos256(angle);
                player->position.y += 0x2800 * RSDK.Sin256(angle);
                player->velocity.x = RSDK.Cos256(ang) << 11;
                player->velocity.y = RSDK.Sin256(ang) << 11;
                player->state      = Player_State_Air;
                RSDK.SetSpriteAnimation(player->aniFrames, ANI_JUMP, &player->animator, false, 0);
                player->nextAirState        = StateMachine_None;
                player->nextGroundState     = StateMachine_None;
                player->onGround            = false;
                player->groundVel           = 0;
                player->tileCollisions      = true;
                player->jumpAbility         = 0;
                player->jumpAbilityTimer    = 0;
                self->playerTimer[playerID] = 10;
            }
        }
        else {
            self->playerTimer[playerID]--;
        }
    }
}

void PaperRoller_HandlePrintCollisions(void)
{
    RSDK_THIS(PaperRoller);
    bool32 hasDeformedTop    = false;
    bool32 hasDeformedBottom = false;

    int32 negAngle          = -self->angle;
    self->deformPosTop.x    = 0;
    self->deformPosTop.y    = 0;
    self->deformPosBottom.x = 0;
    self->deformPosBottom.y = 0;

    foreach_active(Player, player)
    {
        int32 playerID = RSDK.GetEntityID(player);
        if (player->jumpPress)
            self->lastJumpTimer[playerID] = 0;
        else
            ++self->lastJumpTimer[playerID];

        int32 distX = (player->position.x - self->position.x) >> 8;
        int32 distY = (player->position.y - self->position.y) >> 8;
        int32 playerX  = distY * RSDK.Sin256(self->angle) + distX * RSDK.Cos256(self->angle) + self->position.x;
        int32 playerY  = distY * RSDK.Cos256(self->angle) - distX * RSDK.Sin256(self->angle) + self->position.y;

        int32 deformX = playerX - self->position.x;
        int32 defY    = playerY - self->position.y;
        if (abs(deformX) <= self->length << 15 && abs(defY) <= 0x280000) {
            int32 deformY = 0;
            if (defY < 0) {
                if (abs(defY) > 0x180000) {
                    deformY = defY + 0x280000;
                }
                else {
                    Vector2 playerPos;
                    playerPos.x = self->position.x + deformX;
                    playerPos.y = self->position.y - 0x180000;
                    Zone_RotateOnPivot(&playerPos, &self->position, negAngle);

                    Vector2 pivotPos;
                    pivotPos.x       = 0;
                    pivotPos.y       = 0;
                    player->position = playerPos;

                    Vector2 playerVel = player->velocity;
                    Zone_RotateOnPivot(&playerVel, &pivotPos, self->angle);
                    int32 angle = RSDK.ATan2(playerVel.x, -playerVel.y);

                    int32 force = 6;
                    if (player->jumpHold)
                        force = 12;

                    playerVel.x = force * (RSDK.Cos256(angle) << 8);
                    playerVel.y = force * (RSDK.Sin256(angle) << 8);
                    if (abs(playerVel.x) < 0x10000)
                        playerVel.x += ((2 * (self->direction == FLIP_NONE) - 1) << 16);
                    Zone_RotateOnPivot(&playerVel, &pivotPos, negAngle);
                    player->velocity = playerVel;

                    player->state = Player_State_Air;
                    RSDK.SetSpriteAnimation(player->aniFrames, ANI_JUMP, &player->animator, false, 0);
                    player->nextAirState     = StateMachine_None;
                    player->nextGroundState  = StateMachine_None;
                    player->onGround         = false;
                    player->groundVel        = 0;
                    player->tileCollisions   = true;
                    player->jumpAbility      = 0;
                    player->jumpAbilityTimer = 0;
                    RSDK.PlaySfx(PaperRoller->sfxPaper, false, 0xFF);
                    deformY = 0x100000;
                }

                if (player->sidekick) {
                    if (!hasDeformedTop) {
                        self->deformPosTop.x = deformX;
                        self->deformPosTop.y = deformY;
                    }
                }
                else {
                    self->deformPosTop.x = deformX;
                    self->deformPosTop.y = deformY;
                    hasDeformedTop       = true;
                }
            }
            else {
                if (abs(defY) > 0x180000) {
                    deformY = defY - 0x280000;
                }
                else {
                    Vector2 playerPos;
                    playerPos.x = self->position.x + deformX;
                    playerPos.y = self->position.y + 0x180000;
                    Zone_RotateOnPivot(&playerPos, &self->position, negAngle);

                    Vector2 pivotPos;
                    pivotPos.x       = 0;
                    pivotPos.y       = 0;
                    player->position = playerPos;

                    Vector2 playerVel = player->velocity;
                    Zone_RotateOnPivot(&playerVel, &pivotPos, self->angle);

                    int32 angle = RSDK.ATan2(playerVel.x, -playerVel.y);
                    int32 force = 6;
                    if (player->jumpHold)
                        force = 12;

                    playerVel.x = force * (RSDK.Cos256(angle) << 8);
                    playerVel.y = force * (RSDK.Sin256(angle) << 8);
                    if ((self->direction == FLIP_NONE && playerVel.x > -0x10000) || (self->direction == FLIP_X && playerVel.x < 0x10000))
                        playerVel.x += ((2 * (self->direction != FLIP_NONE) - 1) << 18);
                    Zone_RotateOnPivot(&playerVel, &pivotPos, negAngle);
                    player->velocity = playerVel;

                    player->state = Player_State_Air;
                    RSDK.SetSpriteAnimation(player->aniFrames, ANI_JUMP, &player->animator, false, 0);
                    player->nextAirState     = StateMachine_None;
                    player->nextGroundState  = StateMachine_None;
                    player->onGround         = false;
                    player->groundVel        = 0;
                    player->tileCollisions   = true;
                    player->jumpAbility      = 0;
                    player->jumpAbilityTimer = 0;
                    RSDK.PlaySfx(PaperRoller->sfxPaper, false, 0xFF);
                    deformY = -0x100000;
                }

                if (!player->sidekick) {
                    self->deformPosBottom.x = deformX;
                    self->deformPosBottom.y = deformY;
                    hasDeformedBottom       = true;
                }
                else if (!hasDeformedBottom) {
                    self->deformPosBottom.x = deformX;
                    self->deformPosBottom.y = deformY;
                }
            }
        }
    }
}

#if RETRO_INCLUDE_EDITOR
void PaperRoller_EditorDraw(void)
{
    RSDK_THIS(PaperRoller);
    self->drawOrder     = Zone->drawOrderLow;
    self->startPos.x    = self->position.x;
    self->startPos.y    = self->position.y;
    self->visible       = true;
    self->drawFX        = FX_NONE;
    self->updateRange.x = 0x800000;
    self->updateRange.y = 0x800000;

    int32 len = self->length;

    if ((RSDK.Cos256(self->angle) << 8) * self->length >= 0)
        self->updateRange.x += (RSDK.Cos256(self->angle) << 8) * self->length;
    else
        self->updateRange.x += -((RSDK.Cos256(self->angle) << 8) * self->length);

    if ((RSDK.Sin256(self->angle) << 8) * self->length >= 0)
        self->updateRange.y += (RSDK.Sin256(self->angle) << 8) * self->length;
    else
        self->updateRange.y += -((RSDK.Sin256(self->angle) << 8) * self->length);

    if (self->length > 256)
        self->length = 256;

    RSDK.SetSpriteAnimation(PaperRoller->aniFrames, 0, &self->rollerAnimator, true, 0);
    RSDK.SetSpriteAnimation(PaperRoller->aniFrames, 1, &self->divotAnimator, true, 0);

    PaperRoller_DrawPaperLines();
    PaperRoller_DrawRollers();

    self->length = len;
}

void PaperRoller_EditorLoad(void)
{
    PaperRoller_StageLoad();

    RSDK_ACTIVE_VAR(PaperRoller, direction);
    RSDK_ENUM_VAR("Left", FLIP_NONE);
    RSDK_ENUM_VAR("Right", FLIP_X);
}
#endif

void PaperRoller_Serialize(void)
{
    RSDK_EDITABLE_VAR(PaperRoller, VAR_UINT8, direction);
    RSDK_EDITABLE_VAR(PaperRoller, VAR_ENUM, length);
    RSDK_EDITABLE_VAR(PaperRoller, VAR_ENUM, angle);
}
