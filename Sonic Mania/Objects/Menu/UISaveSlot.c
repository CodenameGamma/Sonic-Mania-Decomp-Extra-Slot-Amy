#include "SonicMania.h"

ObjectUISaveSlot *UISaveSlot;

void UISaveSlot_Update(void)
{
    RSDK_THIS(UISaveSlot);
    UISaveSlot_SetupButtonElements();
    if (self->textSpriteIndex != UIWidgets->textSpriteIndex) {
        UISaveSlot_Unknown6();
        UISaveSlot_Unknown8();
        self->textSpriteIndex = UIWidgets->textSpriteIndex;
    }
    StateMachine_Run(self->state);

    if (++self->field_13C >= 192)
        self->field_13C -= 192;

    EntityUIControl *control = (EntityUIControl *)self->parent;

    // ???
    for (int32 i = 0; i < control->buttonCount; ++i) {
        if (self == (EntityUISaveSlot *)control->buttons[i])
            break;
    }

    if (self->flag) {
        self->field_148 -= 0x600;
        self->field_140 += self->field_148;
        if (self->field_140 >= 0x13600)
            self->field_140 = 0x13600;

        if (self->field_140 <= 0x10000 && self->field_148 < 0) {
            self->field_140 = 0x10000;
            self->field_148 = 0;
        }

        self->field_14C -= 0x1800;
        self->field_144 += self->field_14C;
        if (self->field_144 >= 0x11800)
            self->field_144 = 0x11800;

        if (self->field_144 <= 0x8000 && self->field_14C < 0) {
            self->field_144 = 0x8000;
            self->field_14C = 0;
        }
    }
    else if (self->state != UISaveSlot_Unknown28) {
        if (self->field_140 > 0) {
            self->field_140 -= 0x2000;
            if (self->field_140 < 0)
                self->field_140 = 0;
        }

        if (self->field_144 > 0) {
            self->field_144 -= 0x2000;
            if (self->field_144 < 0)
                self->field_144 = 0;
        }
    }

    if (self->zoneID && (control->buttons[control->field_D8] != (EntityUIButton *)self || control->activeEntityID == -1)) {
        self->flag         = false;
        self->zoneID       = 0;
        self->state        = UISaveSlot_Unknown22;
#if RETRO_USE_PLUS
        self->stateUnknown = StateMachine_None;
#endif
    }
    if (control->state != UIControl_ProcessInputs || control->active != ACTIVE_ALWAYS) {
        if (self->zoneID) {
            self->flag         = false;
            self->zoneID       = 0;
            self->state        = UISaveSlot_Unknown22;
#if RETRO_USE_PLUS
            self->stateUnknown = StateMachine_None;
#endif
        }
        self->active = ACTIVE_BOUNDS;
    }
}

void UISaveSlot_LateUpdate(void) {}

void UISaveSlot_StaticUpdate(void) {}

void UISaveSlot_Draw(void)
{
    RSDK_THIS(UISaveSlot);
    Vector2 drawPos;

    if (self->type == 0) {
        drawPos.y = self->position.y + 0x30000;
        drawPos.x = self->position.x + 0x30000;
        RSDK.DrawRect(self->position.x - 0x2D0000, self->position.y - 0x4F0000, 0x600000, 0xA40000, 0xFFFFFF, 127, INK_BLEND, false);

        drawPos.x = self->position.x - 0x2D0000;
        drawPos.y = self->position.y - 0x100000;
        UIWidgets_DrawRightTriangle(drawPos.x, drawPos.y, (self->field_140 >> 11), 232, 40, 88);

        drawPos.x = self->position.x + 0x2D0000;
        drawPos.y = 0x4F0000 + self->position.y;
        UIWidgets_DrawRightTriangle(drawPos.x, drawPos.y, (-64 * self->field_140) >> 16, 96, 160, 176);
        UIWidgets_DrawRightTriangle(drawPos.x, drawPos.y, (-44 * self->field_140) >> 16, 88, 112, 224);

        drawPos.x = self->position.x + 0x30000;
        drawPos.y = self->position.y + 0x30000;
        UIWidgets_DrawRectOutline_Blended(164, 96, drawPos.x, drawPos.y);
        RSDK.DrawRect(drawPos.x - 0x2D0000, drawPos.y - 0x130000, 0x5A0000, 0x30000, 0, 255, INK_BLEND, false);

        if (self->flag)
            UIWidgets_DrawRectOutline_Flash(164, 96, self->position.x, self->position.y);
        else
            UIWidgets_DrawRectOutline_Black(164, 96, self->position.x, self->position.y);

        self->animator1.frameID = 0;
        drawPos.x                 = self->position.x;
        drawPos.y                 = self->position.y - 0x300000;
        RSDK.DrawSprite(&self->animator1, &drawPos, false);

        if (self->isNewSave || self->saveZoneID == 255) {
            RSDK.DrawSprite(&self->animator10, &drawPos, false);
            RSDK.DrawSprite(&self->animator11, &drawPos, false);
        }
        else if ((self->flag || self->state == UISaveSlot_Unknown28) && self->saveZoneID <= 13) {
            SpriteFrame *frame = RSDK.GetFrame(UISaveSlot->aniFrames, 5, self->saveZoneID);
            frame->pivotX      = -43;
            frame->width       = 86;
            frame->sprX        = self->field_13C;
            if (frame->sprX > 106) {
                int32 width    = frame->sprX - 106;
                frame->width = 86 - width;
                RSDK.DrawSprite(&self->animator7, &drawPos, false);
                frame->pivotX += frame->width;
                frame->sprX  = 0;
                frame->width = width;
            }
            RSDK.DrawSprite(&self->animator7, &drawPos, false);
            RSDK.DrawSprite(&self->animator8, &drawPos, false);
        }
        else {
            self->drawFX = FX_FLIP;
            RSDK.DrawSprite(&self->animator9, &drawPos, false);

            self->drawFX = FX_NONE;
            RSDK.DrawSprite(&self->animator8, &drawPos, false);
        }

        if (self->isNewSave) {
            drawPos.x = self->position.x;
            drawPos.y = self->position.y + 0x200000;
            UISaveSlot_Unknown3(drawPos.x, drawPos.y);
        }
        else {
            RSDK.DrawRect(self->position.x - 0x2D0000, self->position.y + 0x3D0000, 0x5A0000, 0x100000, 0, 255, INK_NONE, false);

            drawPos.x = self->position.x - 0x240000;
            drawPos.y = 0x450000 + self->position.y;
            for (int32 i = 0; i < 7; ++i) {
                self->animator6.frameID = ((1 << i) & self->saveEmeralds) ? i : 7;
                RSDK.DrawSprite(&self->animator6, &drawPos, false);
                drawPos.x += 0xC0000;
            }

            drawPos.x = self->position.x;
            drawPos.y = self->position.y + 0x100000;
#if RETRO_USE_PLUS
            if (!self->encoreMode || self->type) {
                if (!self->saveContinues) {
                    drawPos.y += 0x20000;
                }
                drawPos.y += 0x20000;
            }
            else {
                drawPos.y += 0x80000;
            }
#else
            if (self->type) {
                drawPos.y += 0x20000;
            }
            else {
                drawPos.y += 0x80000;
            }
#endif
            UISaveSlot_Unknown3(drawPos.x, drawPos.y);

            drawPos.x = self->position.x;
            drawPos.y = self->position.y + 0x330000;
            UISaveSlot_DrawPlayerInfo(drawPos.x, drawPos.y);
        }
    }
    else {
        drawPos.y = self->position.y + 0x30000;
        drawPos.x = self->position.x + 0x30000;
        RSDK.DrawRect(self->position.x - 0x2D0000, self->position.y - 0x1F0000, 0x600000, 0x440000, 0xFFFFFF, 127, INK_BLEND, false);

        drawPos.x = self->position.x + 0x30000;
        drawPos.y = self->position.y + 0x30000;
        UIWidgets_DrawRectOutline_Blended(68, 96, drawPos.x, drawPos.y);

        self->animator1.frameID = 1;
        RSDK.DrawSprite(&self->animator1, 0, false);

        drawPos = self->position;
        UISaveSlot_Unknown3(self->position.x, self->position.y);
        RSDK.DrawSprite(&self->animator8, 0, false);
    }

    if (self->flag || self->type == 0) {
        if (self->type)
            UIWidgets_DrawRectOutline_Flash(68, 96, self->position.x, self->position.y);

        if (self->flag && !(self->field_13C & 8)) {
            drawPos.x = self->position.x;
            drawPos.y = self->position.y;
#if RETRO_USE_PLUS
            if (!self->encoreMode) {
#endif
                if (self->type == 1) {
                    drawPos.y -= 0x60000;
                    UIWidgets_DrawUpDownArrows(drawPos.x, drawPos.y, 40);
                }
                else if (self->isNewSave) {
                    drawPos.y += 0x200000;
                    UIWidgets_DrawUpDownArrows(drawPos.x, drawPos.y, 64);
                }
                else if (self->listID == 1) {
                    drawPos.y -= 0x300000;
                    UIWidgets_DrawUpDownArrows(drawPos.x, drawPos.y, 40);
                }
#if RETRO_USE_PLUS
            }
            else if (self->listID == 1) {
                drawPos.y -= 0x300000;
                UIWidgets_DrawUpDownArrows(drawPos.x, drawPos.y, 40);
            }
#endif
        }
    }
    RSDK.DrawCircle(ScreenInfo->centerX, ScreenInfo->centerY, self->radius, 0, self->alpha, INK_ALPHA, true);
}

void UISaveSlot_Create(void *data)
{
    RSDK_THIS(UISaveSlot);
    if (!SceneInfo->inEditor) {
        self->visible         = true;
        self->drawOrder       = 2;
        self->active          = ACTIVE_BOUNDS;
        self->updateRange.x   = 0x800000;
        self->updateRange.y   = 0x300000;
        self->processButtonCB = UISaveSlot_ProcessButtonCB;
        self->state           = UISaveSlot_Unknown22;
        if (!self->type) {
            UISaveSlot_LoadSaveInfo();
        }
        else {
            self->slotID = NO_SAVE_SLOT;
            UISaveSlot_Unknown6();
        }
        self->textSpriteIndex = UIWidgets->textSpriteIndex;
        UISaveSlot_Unknown8();
    }
}

void UISaveSlot_StageLoad(void) { UISaveSlot->aniFrames = RSDK.LoadSpriteAnimation("UI/SaveSelect.bin", SCOPE_STAGE); }

#if RETRO_USE_PLUS
uint8 UISaveSlot_Unknown1(uint8 id)
{
    switch (id) {
        case 1: return ID_SONIC;
        case 2: return ID_TAILS;
        case 3: return ID_KNUCKLES;
        case 4: return ID_MIGHTY;
        case 5: return ID_RAY;
        default: break;
    }
    return ID_NONE;
}

void UISaveSlot_Unknown2(uint8 playerID, bool32 isSilhouette, uint8 buddyID, uint8 *friendIDs, uint8 friendCount, int32 drawX, int32 drawY)
{
    RSDK_THIS(UISaveSlot);
    Vector2 drawPos;

    Animator *animator = NULL;
    if (!isSilhouette)
        animator = &self->animator2;
    else
        animator = &self->animator3;

    RSDK.SetSpriteAnimation(UISaveSlot->aniFrames, (isSilhouette != 0) + 19, animator, true, 0);
    self->drawFX |= FX_FLIP;
    self->direction = FLIP_NONE;
    switch (buddyID) {
        case ID_TAILS: animator->frameID = 1; break;
        case ID_KNUCKLES: animator->frameID = 2; break;
        case ID_MIGHTY: animator->frameID = 3; break;
        case ID_RAY: animator->frameID = 4; break;
        default: animator->frameID = 0; break;
    }

    int32 y = drawY;
    if (friendCount)
        y = drawY - 0x80000;
    drawPos.x = drawX + 0x100000;
    drawPos.y = y - 0x40000;

    self->direction = !animator->frameID || animator->frameID == 3;
    RSDK.DrawSprite(animator, &drawPos, false);

    switch (playerID) {
        case ID_TAILS: animator->frameID = 1; break;
        case ID_KNUCKLES: animator->frameID = 2; break;
        case ID_MIGHTY: animator->frameID = 3; break;
        case ID_RAY: animator->frameID = 4; break;
        default: animator->frameID = 0; break;
    }

    y = drawY;
    if (friendCount)
        y = drawY - 0x80000;
    drawPos.x         = drawX - 0xF0000;
    drawPos.y         = y + 0x30000;
    self->direction = animator->frameID && animator->frameID != 3;
    RSDK.DrawSprite(animator, &drawPos, false);

    self->direction = FLIP_NONE;
    self->drawFX    = FX_NONE;
    if (friendCount && !isSilhouette) {
        RSDK.SetSpriteAnimation(UISaveSlot->aniFrames, 3, animator, true, 0);
        int32 x      = drawX;
        int32 offset = 0x120000;
        if (self->saveContinues > 0) {
            offset = 0xA0000;
            x      = drawX - 0x160000;
        }
        drawPos.x = x + ((offset * (friendCount - 1)) >> 1);
        drawPos.y = drawY + 0x1A0000;

        for (int32 f = friendCount - 1; f >= 0; --f) {
            switch (friendIDs[f]) {
                case ID_TAILS: animator->frameID = 1; break;
                case ID_KNUCKLES: animator->frameID = 2; break;
                case ID_MIGHTY: animator->frameID = 3; break;
                case ID_RAY: animator->frameID = 4; break;
                default: animator->frameID = 0; break;
            }
            RSDK.DrawSprite(animator, &drawPos, false);
            drawPos.x -= offset;
        }
    }
}
#endif

void UISaveSlot_Unknown3(int32 drawX, int32 drawY)
{
    RSDK_THIS(UISaveSlot);
    Vector2 drawPos;

    RSDK.SetSpriteAnimation(UISaveSlot->aniFrames, 1, &self->animator2, true, 3);
    RSDK.SetSpriteAnimation(UISaveSlot->aniFrames, 2, &self->animator3, true, 3);

    uint8 friendIDs[3];
    int32 playerID    = 0;
    int32 buddyID     = 0;
    int32 friendCount = 0;

#if RETRO_USE_PLUS
    if (!self->encoreMode) {
#endif
        int32 frames[]              = { 3, 0, 1, 2, 4, 5 };
        self->animator3.frameID = frames[self->frameID];
        self->animator2.frameID = frames[self->frameID];
#if RETRO_USE_PLUS
    }
    else if (!SceneInfo->inEditor) {
        if (!self->isNewSave && self->type != 1) {
            playerID = self->saveEncorePlayer;
            buddyID  = self->saveEncoreBuddy;

            for (int32 i = 0; i < 3; ++i) {
                friendIDs[i] = 0;
                if (!self->saveEncoreFriends[i])
                    continue;
                ++friendCount;
                friendIDs[i] = self->saveEncoreFriends[i];
            }

            if (!buddyID) {
                switch (playerID) {
                    case ID_TAILS: playerID = 1; break;
                    case ID_KNUCKLES: playerID = 2; break;
                    case ID_MIGHTY: playerID = 4; break;
                    case ID_RAY: playerID = 5; break;
                    default: playerID = 0; break;
                }
                self->animator3.frameID = playerID;
                self->animator2.frameID = playerID;
            }
            else if (playerID) {
                drawX += 4 * self->field_144;
                drawY += 4 * self->field_144;
                UISaveSlot_Unknown2(playerID, true, buddyID, friendIDs, friendCount, drawX, drawY);
                UISaveSlot_Unknown2(playerID, false, buddyID, friendIDs, friendCount, drawX - 8 * self->field_144, drawY - 8 * self->field_144);
                return;
            }
        }
        else {
            self->animator2.frameID = 6;
            self->animator3.frameID = 6;
        }
    }
    else if (self->debugEncoreDraw) {
        playerID     = UISaveSlot_Unknown1(self->dCharPoint);
        buddyID      = UISaveSlot_Unknown1(self->dCharPartner);
        friendIDs[0] = UISaveSlot_Unknown1(self->dCharStock1);
        friendIDs[1] = UISaveSlot_Unknown1(self->dCharStock2);
        friendIDs[2] = UISaveSlot_Unknown1(self->dCharStock3);
        if (friendIDs[0]) {
            friendCount = 1;
            if (friendIDs[1]) {
                friendCount = 2;
                if (friendIDs[2])
                    friendCount = 3;
            }
        }

        if (!buddyID) {
            switch (playerID) {
                case ID_TAILS: playerID = 1; break;
                case ID_KNUCKLES: playerID = 2; break;
                case ID_MIGHTY: playerID = 4; break;
                case ID_RAY: playerID = 5; break;
                default: playerID = 0; break;
            }
            self->animator3.frameID = playerID;
            self->animator2.frameID = playerID;
        }
        else if (playerID) {
            drawX += 4 * self->field_144;
            drawY += 4 * self->field_144;
            UISaveSlot_Unknown2(playerID, true, buddyID, friendIDs, friendCount, drawX, drawY);
            UISaveSlot_Unknown2(playerID, false, buddyID, friendIDs, friendCount, drawX - 8 * self->field_144, drawY - 8 * self->field_144);
            return;
        }
    }
    else {
        self->animator2.frameID = 6;
        self->animator3.frameID = 6;
    }
#endif

    drawX += 4 * self->field_144;
    drawY += 4 * self->field_144;
    drawPos.x = drawX;
    drawPos.y = drawY;
    RSDK.DrawSprite(&self->animator3, &drawPos, false);

    drawX -= 8 * self->field_144;
    drawY -= 8 * self->field_144;
    drawPos.x = drawX;
    drawPos.y = drawY;
    RSDK.DrawSprite(&self->animator2, &drawPos, false);
}

void UISaveSlot_DrawPlayerInfo(int32 drawX, int32 drawY)
{
    RSDK_THIS(UISaveSlot);
    RSDK.SetSpriteAnimation(UISaveSlot->aniFrames, 3, &self->animator4, true, 0);
    RSDK.SetSpriteAnimation(UISaveSlot->aniFrames, 21, &self->animator5, true, 0);

    int32 playerID = 0;
#if RETRO_USE_PLUS
    if (self->encoreMode) {
        if (self->isNewSave || self->type == 1)
            return;

        if (self->debugEncoreDraw && SceneInfo->inEditor) {
            switch (UISaveSlot_Unknown1(self->dCharPoint)) {
                case ID_TAILS: playerID = 1; break;
                case ID_KNUCKLES: playerID = 2; break;
                case ID_MIGHTY: playerID = 3; break;
                case ID_RAY: playerID = 4; break;
                default: playerID = 0; break;
            }
        }
        else {
            switch (self->saveEncorePlayer) {
                case ID_TAILS: playerID = 1; break;
                case ID_KNUCKLES: playerID = 2; break;
                case ID_MIGHTY: playerID = 3; break;
                case ID_RAY: playerID = 4; break;
                default: playerID = 0; break;
            }
        }
    }
    else {
#endif
        int32 frames[] = { 0, 0, 1, 2, 3, 4 };
        playerID     = frames[self->frameID];
#if RETRO_USE_PLUS
    }
#endif
    self->animator4.frameID = playerID;
    self->animator5.frameID = playerID;

    Animator *animators[2];
    animators[0] = &self->animator4;
    animators[1] = &self->animator5;

    int32 retries[2];
    retries[0] = minVal(self->saveLives, 99);
#if RETRO_USE_PLUS
    retries[1] = minVal(self->saveContinues, 99);
#else
    retries[1] = 0;
#endif

    Vector2 positions[2];
    positions[0].x = drawX;
    positions[0].y = drawY;
    positions[1].y = drawY;
    positions[1].x = drawX;

    int32 count = 1;
    if (retries[1] <= 0) {
        positions[0].x = drawX - 0x20000;
    }
    else {
        positions[0].x = drawX - 0x170000;
        positions[1].x = drawX + 0x170000;
        count          = 2;
    }

    for (int32 i = 0; i < count; ++i) {
        Vector2 drawPos;

        drawPos.x = positions[i].x;
        drawPos.y = positions[i].y;
        if (
#if RETRO_USE_PLUS
            i || !self->encoreMode
#else
            true
#endif
        ) {
            self->animator1.frameID = 3;
            RSDK.DrawSprite(&self->animator1, &drawPos, false);

            drawPos.x -= 0xC0000;
            RSDK.DrawSprite(animators[i], &drawPos, false);

            drawPos.x += 0x140000;
            self->animator12.frameID = retries[i] / 10;
            RSDK.DrawSprite(&self->animator12, &drawPos, false);

            drawPos.x += 0x80000;
            self->animator12.frameID = retries[i] % 10;
            RSDK.DrawSprite(&self->animator12, &drawPos, false);
        }
    }
}

void UISaveSlot_SetupButtonElements(void)
{
    RSDK_THIS(UISaveSlot);
    self->touchCB  = UIButton_TouchCB_Alt;
    self->options3 = UISaveSlot_SelectedSave;
    self->failCB   = NULL;
    self->options5 = UISaveSlot_Unknown19;
    self->options6 = UISaveSlot_Unknown20;
    self->options7 = UISaveSlot_Unknown17;
    self->options8 = UISaveSlot_Unknown18;

#if RETRO_USE_PLUS
    EntitySaveGame *saveRAM = (EntitySaveGame *)SaveGame_GetDataPtr(self->slotID, self->encoreMode);
#else
    EntitySaveGame *saveRAM = (EntitySaveGame *)SaveGame_GetDataPtr(self->slotID);
#endif
    if (self->type == 1) {
        self->touchPos1[0].x       = 0x600000;
        self->touchPos1[0].y       = 0x440000;
        self->touchPos2[0].x       = 0;
        self->touchPos2[0].y       = 0;
        self->touchPosCallbacks[0] = UISaveSlot_SelectedSave;
        self->touchPos1[1].x       = 0x200000;
        self->touchPos1[1].y       = 0x200000;
        self->touchPos2[1].x       = 0;
        self->touchPos2[1].y       = -0x1A0000;
        self->touchPosCallbacks[1] = UISaveSlot_NextCharacter;
        self->touchPos1[2].x       = 0x200000;
        self->touchPos1[2].y       = 0x200000;
        self->touchPos2[2].x       = 0;
        self->touchPos2[2].y       = 0xE0000;
        self->touchPosCallbacks[2] = UISaveSlot_PrevCharacter;
        self->touchPosCount        = 3;
    }
    else {
        self->touchPos1[0].x       = 0x600000;
        self->touchPos1[0].y       = 0xA40000;
        self->touchPos2[0].x       = 0;
        self->touchPos2[0].y       = 0;
        self->touchPosCallbacks[0] = UISaveSlot_SelectedSave;

        switch (saveRAM->saveState) {
            default:
            case 0:
                self->touchPos1[1].x       = 0x200000;
                self->touchPos1[1].y       = 0x200000;
                self->touchPos2[1].x       = 0;
                self->touchPos2[1].y       = 0;
                self->touchPosCallbacks[1] = UISaveSlot_NextCharacter;
                self->touchPos1[2].x       = 0x200000;
                self->touchPos1[2].y       = 0x200000;
                self->touchPos2[2].x       = 0;
                self->touchPos2[2].y       = 0x400000;
                self->touchPosCallbacks[2] = UISaveSlot_PrevCharacter;
                self->touchPosCount        = 3;
                break;
            case 1: self->touchPosCount = 1; break;
            case 2:
                self->touchPos1[1].x       = 0x200000;
                self->touchPos1[1].y       = 0x200000;
                self->touchPos2[1].y       = -0x440000;
                self->touchPos2[1].x       = 0;
                self->touchPosCallbacks[1] = UISaveSlot_NextZone;
                self->touchPos1[2].x       = 0x200000;
                self->touchPos1[2].y       = 0x200000;
                self->touchPos2[2].x       = 0;
                self->touchPos2[2].y       = 0x280000 - 0x440000;
                self->touchPosCallbacks[2] = UISaveSlot_PrevZone;
                self->touchPosCount        = 3;
                break;
        }
    }
}

void UISaveSlot_Unknown6(void)
{
    RSDK_THIS(UISaveSlot);
    RSDK.SetSpriteAnimation(UISaveSlot->aniFrames, 0, &self->animator1, true, 0);
    RSDK.SetSpriteAnimation(UISaveSlot->aniFrames, 1, &self->animator2, true, 3);
    RSDK.SetSpriteAnimation(UISaveSlot->aniFrames, 2, &self->animator3, true, 3);
    RSDK.SetSpriteAnimation(UISaveSlot->aniFrames, 3, &self->animator4, true, 0);
    RSDK.SetSpriteAnimation(UISaveSlot->aniFrames, 0, &self->animator10, true, 2);
    RSDK.SetSpriteAnimation(UIWidgets->textSpriteIndex, 2, &self->animator11, true, 0);
    RSDK.SetSpriteAnimation(UISaveSlot->aniFrames, 7, &self->animator9, true, 0);
    RSDK.SetSpriteAnimation(UISaveSlot->aniFrames, 4, &self->animator6, true, 0);
    RSDK.SetSpriteAnimation(UISaveSlot->aniFrames, 5, &self->animator7, true, 0);

    if (self->type)
        RSDK.SetSpriteAnimation(UIWidgets->textSpriteIndex, 2, &self->animator8, true, 2);
    else
        RSDK.SetSpriteAnimation(UISaveSlot->aniFrames, 6, &self->animator8, true, 0);
    RSDK.SetSpriteAnimation(UISaveSlot->aniFrames, 8, &self->animator12, true, 0);
}

void UISaveSlot_LoadSaveInfo(void)
{
    RSDK_THIS(UISaveSlot);
#if RETRO_USE_PLUS
    EntitySaveGame *saveRAM = (EntitySaveGame*)SaveGame_GetDataPtr(self->slotID, self->encoreMode);
#else                         
    EntitySaveGame *saveRAM = (EntitySaveGame*)SaveGame_GetDataPtr(self->slotID);
#endif
    int32 saveState = saveRAM->saveState;
    if (saveState == 1 || saveState == 2) {
#if RETRO_USE_PLUS
        if (self->encoreMode) {
            self->saveEncorePlayer = saveRAM->playerID & 0xFF;
            self->saveEncoreBuddy  = (saveRAM->playerID >> 8) & 0xFF;

            int32 friends = saveRAM->stock;
            for (int32 i = 0; i < 3; ++i) {
                self->saveEncoreFriends[i] = ID_NONE;
                if (!friends)
                    continue;
                self->saveEncoreFriends[i] = friends & 0xFF;
                friends >>= 8;
            }

            self->frameID = self->saveEncorePlayer;
        }
        else {
#endif
            self->frameID = saveRAM->characterID;
#if RETRO_USE_PLUS
        }
#endif
    }

    switch (saveState) {
        default: break;
        case 0:
            self->saveZoneID   = 0;
            self->saveEmeralds = 0;
            self->saveLives    = 3;
#if RETRO_USE_PLUS
            self->saveContinues = 0;
            self->frameID       = self->encoreMode ? 6 : 0;
#else
            self->frameID = 0;
#endif
            self->isNewSave = true;
            self->listID    = 0;
            break;
        case 1:
            self->saveZoneID   = saveRAM->zoneID;
            self->saveEmeralds = saveRAM->chaosEmeralds;
            self->saveLives    = saveRAM->lives;
#if RETRO_USE_PLUS
            self->saveContinues = saveRAM->continues;
#endif
            self->isNewSave = false;
            self->listID    = 0;
            break;
        case 2:
            self->saveZoneID   = 255;
            self->saveEmeralds = saveRAM->chaosEmeralds;
            self->saveLives    = saveRAM->lives;
#if RETRO_USE_PLUS
            self->saveContinues = saveRAM->continues;
#endif
            self->listID    = 1;
            self->isNewSave = false;
            break;
    }
    UISaveSlot_Unknown6();
    UISaveSlot_Unknown8();
}
void UISaveSlot_Unknown8(void)
{
    RSDK_THIS(UISaveSlot);

    self->animator11.frameID = self->listID;
    self->animator7.frameID  = self->saveZoneID;
    if (self->type == 0) {
        self->animator8.frameID = self->saveZoneID;
        if (self->zoneID) {
#if RETRO_USE_PLUS
            if (self->encoreMode)
                RSDK.CopyPalette(((self->saveZoneID >> 3) + 4), 32 * self->saveZoneID, 0, 224, 32);
            else
#endif
                RSDK.CopyPalette(((self->saveZoneID >> 3) + 1), 32 * self->saveZoneID, 0, 224, 32);
        }
    }
}

void UISaveSlot_DeleteDLG_CB(void)
{
    EntityUIDialog *dialog     = (EntityUIDialog *)UIDialog->activeDialog;
    EntityUISaveSlot *saveSlot = (EntityUISaveSlot *)dialog->entityPtr;
    dialog->parent->state      = StateMachine_None;
    UIWaitSpinner_Wait();
#if RETRO_USE_PLUS
    int32 *saveRAM = SaveGame_GetDataPtr(saveSlot->slotID % 8, saveSlot->encoreMode);
#else
    int32 *saveRAM = SaveGame_GetDataPtr(saveSlot->slotID % 8);
#endif
    memset(saveRAM, 0, 0x400);
    SaveGame_SaveFile(UISaveSlot_DeleteSaveCB);
}

void UISaveSlot_DeleteSaveCB(int32 status)
{
    EntityUIDialog *dialog     = (EntityUIDialog *)UIDialog->activeDialog;
    EntityUISaveSlot *saveSlot = (EntityUISaveSlot *)dialog->entityPtr;
    UIWaitSpinner_Wait2();
    if (dialog->state != UIDialog_Unknown13) {
        dialog->parent->selectionDisabled = true;
        dialog->timer                     = 0;
        dialog->state                     = UIDialog_Unknown13;
        dialog->curCallback               = NULL;
    }

    Entity *storeEntity    = SceneInfo->entity;
    SceneInfo->entity = (Entity *)saveSlot;
    UISaveSlot_LoadSaveInfo();
    UISaveSlot_Unknown8();
    SceneInfo->entity = storeEntity;
}

void UISaveSlot_ProcessButtonCB(void)
{
    RSDK_THIS(UISaveSlot);
    EntityUIControl *control = (EntityUIControl *)self->parent;
#if RETRO_USE_PLUS
    EntitySaveGame *saveRAM = (EntitySaveGame*)SaveGame_GetDataPtr(self->slotID, self->encoreMode);
#else                         
    EntitySaveGame *saveRAM = (EntitySaveGame*)SaveGame_GetDataPtr(self->slotID);
#endif

    self->active = ACTIVE_NORMAL;
    UIControl_Unknown15(control, self->position.x, 0);
    if (control->position.x == control->posUnknown.x) {
        if (control->columnCount > 1) {
            if (UIControl->keyLeft) {
                if (control->activeEntityID > 0) {
                    control->activeEntityID--;
                    RSDK.PlaySfx(UIWidgets->sfxBleep, false, 255);
                    UISaveSlot_Unknown21();
                    return;
                }
            }
            else if (UIControl->keyRight) {
                if (control->activeEntityID < control->buttonCount - 1) {
                    control->activeEntityID++;
                    RSDK.PlaySfx(UIWidgets->sfxBleep, false, 255);
                    UISaveSlot_Unknown21();
                    return;
                }
            }
        }

#if RETRO_USE_PLUS
        StateMachine_Run(self->stateUnknown);
#endif

        TextInfo msg;
        INIT_TEXTINFO(msg);

        if (UIControl->keyConfirm) {
#if RETRO_USE_PLUS
            if (API.CheckDLC(DLC_PLUS) || self->frameID < 4) {
                UISaveSlot_SelectedSave();
            }
            else {
                Localization_GetString(&msg, STR_ENCOREREQUIRED);
                UIDialog_CreateDialogOk(&msg, NULL, true);
            }
#else
            UISaveSlot_SelectedSave();
#endif
        }
        else if (UIControl->keyX && saveRAM->saveState && !self->type) {
            Localization_GetString(&msg, STR_DELETEPOPUP);
            UIDialog_CreateDialogYesNo(&msg, UISaveSlot_DeleteDLG_CB, NULL, false, true);
        }
        else if (!self->zoneID && control->buttons[control->field_D8] == (EntityUIButton *)self && control->state == UIControl_ProcessInputs) {
            UISaveSlot_Unknown19();
        }
    }
}

void UISaveSlot_SelectedSave(void)
{
    RSDK_THIS(UISaveSlot);
    EntityUIControl *control = (EntityUIControl *)self->parent;

    if (control->position.x == control->posUnknown.x) {
        control->state          = 0;
        self->state           = UISaveSlot_Unknown28;
        self->flag            = false;
        self->zoneID          = 0;
        self->processButtonCB = NULL;
        foreach_all(UISaveSlot, saveSlot)
        {
            if (saveSlot != self) {
                if (saveSlot->position.x >= self->position.x) {
                    if (saveSlot->position.x > self->position.x)
                        saveSlot->velocity.x = 0x200000;
                }
                else {
                    saveSlot->velocity.x = -0x200000;
                }
                saveSlot->state = UISaveSlot_Unknown23;
            }
        }

        foreach_all(UIButtonPrompt, prompt) { prompt->visible = false; }

#if RETRO_USE_PLUS
        int32 id = API_MostRecentActiveControllerID(1, 0, 5);
#else
        int32 id = API_MostRecentActiveControllerID(0);
#endif
        API_ResetControllerAssignments();
        API_AssignControllerID(1, id);
        RSDK.PlaySfx(UIWidgets->sfxAccept, false, 255);
        RSDK.StopChannel(Music->channelID);
    }
}

void UISaveSlot_NextCharacter(void)
{
    RSDK_THIS(UISaveSlot);
    ++self->frameID;

    int32 player = self->frameID;
#if RETRO_USE_PLUS
    int32 max = API.CheckDLC(DLC_PLUS) ? 6 : 4;
#else
    int32 max = 4;
#endif
    while (player >= max) {
        player -= max;
    }
    self->frameID   = player;
    self->field_14C = 0x8000;
    self->field_144 = 0;
    RSDK.PlaySfx(UIWidgets->sfxBleep, false, 255);
    UISaveSlot_Unknown8();
}

void UISaveSlot_PrevCharacter(void)
{
    RSDK_THIS(UISaveSlot);
    --self->frameID;

    int32 player = self->frameID;
#if RETRO_USE_PLUS
    int32 max = API.CheckDLC(DLC_PLUS) ? 6 : 4;
#else
    int32 max = 4;
#endif
    while (player < 0) {
        player += max;
    }
    self->frameID   = player;
    self->field_14C = 0x8000;
    self->field_144 = 0;
    RSDK.PlaySfx(UIWidgets->sfxBleep, false, 255);
    UISaveSlot_Unknown8();
}

void UISaveSlot_NextZone(void)
{
    RSDK_THIS(UISaveSlot);

    if (self->saveZoneID == 255) {
        self->saveZoneID = 0;
    }
    else {
        self->saveZoneID++;
        if (self->saveZoneID > 11)
            self->saveZoneID = 0;
    }
    RSDK.PlaySfx(UIWidgets->sfxBleep, false, 255);
    UISaveSlot_Unknown8();
}

void UISaveSlot_PrevZone(void)
{
    RSDK_THIS(UISaveSlot);

    if (self->saveZoneID == 255) {
        self->saveZoneID = 11;
    }
    else {
        self->saveZoneID--;
        if (self->saveZoneID < 0)
            self->saveZoneID = 11;
    }
    RSDK.PlaySfx(UIWidgets->sfxBleep, false, 255);
    UISaveSlot_Unknown8();
}

bool32 UISaveSlot_Unknown17(void)
{
    RSDK_THIS(UISaveSlot);
    return self->flag;
}
bool32 UISaveSlot_Unknown18(void)
{
    RSDK_THIS(UISaveSlot);
    return self->state == UISaveSlot_Unknown28;
}

void UISaveSlot_Unknown19(void)
{
    RSDK_THIS(UISaveSlot);
    if (!self->zoneID) {
        self->flag   = true;
        self->zoneID = 1;
        UISaveSlot_Unknown8();
        self->field_148 = 0x4000;
        self->field_14C = 0x8000;
#if RETRO_USE_PLUS
        self->stateUnknown = StateMachine_None;
#endif
        if (!self->isNewSave && self->type != 1) {
            if (self->listID) {
                self->state = UISaveSlot_Unknown27;
            }
            else {
                self->state = UISaveSlot_Unknown25;
            }
        }
#if RETRO_USE_PLUS
        else if (self->encoreMode) {
            self->state = UISaveSlot_Unknown25;
        }
#endif
        else {
            self->state = UISaveSlot_Unknown24;
#if RETRO_USE_PLUS
            self->stateUnknown = UISaveSlot_Unknown26;
#endif
        }
    }
}

void UISaveSlot_Unknown20(void)
{
    RSDK_THIS(UISaveSlot);
    self->flag         = false;
    self->zoneID       = 0;
    self->state        = UISaveSlot_Unknown22;
#if RETRO_USE_PLUS
    self->stateUnknown = StateMachine_None;
#endif
}

void UISaveSlot_Unknown21(void)
{
    RSDK_THIS(UISaveSlot);
#if RETRO_USE_PLUS
    EntitySaveGame *saveRAM = (EntitySaveGame*)SaveGame_GetDataPtr(self->slotID, self->encoreMode);
#else
    EntitySaveGame *saveRAM = (EntitySaveGame*)SaveGame_GetDataPtr(self->slotID);
#endif

    if (!saveRAM->saveState) {
#if RETRO_USE_PLUS
        int32 frame = self->encoreMode ? 6 : 0;
#else
        int32 frame = 0;
#endif
        if (self->frameID != frame) {
            self->frameID = frame;
            UISaveSlot_Unknown8();
        }
    }
    if (!self->type && saveRAM->saveState == 2) {
        self->saveZoneID = 255;
        UISaveSlot_Unknown8();
    }
}

void UISaveSlot_Unknown22(void)
{
    RSDK_THIS(UISaveSlot);
    RSDK.ProcessAnimation(&self->animator9);
    self->direction = self->animator9.frameID & 3;
}

void UISaveSlot_Unknown23(void)
{
    RSDK_THIS(UISaveSlot);
    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;
}

void UISaveSlot_Unknown24(void)
{
    // so true!
}

void UISaveSlot_Unknown25(void)
{
    RSDK_THIS(UISaveSlot);
    if (!self->zoneID)
        self->state = UISaveSlot_Unknown22;
}

void UISaveSlot_Unknown26(void)
{
    RSDK_THIS(UISaveSlot);
    EntityUIControl *control = (EntityUIControl *)self->parent;

    if (control->active == ACTIVE_ALWAYS) {
        if (UIControl->keyUp) {
            UISaveSlot_NextCharacter();
        }
        else if (UIControl->keyDown) {
            UISaveSlot_PrevCharacter();
        }
    }
}

void UISaveSlot_Unknown27(void)
{
    RSDK_THIS(UISaveSlot);
    EntityUIControl *control = (EntityUIControl *)self->parent;

    if (control->active == ACTIVE_ALWAYS) {
        if (UIControl->keyUp) {
            UISaveSlot_NextZone();
        }
        else if (UIControl->keyDown) {
            UISaveSlot_PrevZone();
        }
    }
}

void UISaveSlot_Unknown28(void)
{
    RSDK_THIS(UISaveSlot);

    ++self->timer;
#if RETRO_USE_PLUS
    if (self->encoreMode && (self->isNewSave || self->type == 1)) {
        EntityFXRuby *fxRuby = self->fxRuby;
        int32 rubyY            = self->position.y;

        int32 val = (4 * self->field_144);
        if (self->type != 1)
            rubyY += 0x200000;

        if (self->timer == 32) {
            PhantomRuby_PlaySFX(RUBYSFX_REDCUBE);
            fxRuby            = CREATE_ENTITY(FXRuby, NULL, self->position.x - val, rubyY - val);
            fxRuby->fadeWhite = 0;
            fxRuby->fadeBlack = 0;
            fxRuby->drawOrder = 15;
            self->fxRuby    = fxRuby;
        }
        if (fxRuby) {
            fxRuby->position.x = self->position.x - val;
            fxRuby->position.y = rubyY - val;
        }
        if (self->timer == 152)
            PhantomRuby_PlaySFX(RUBYSFX_ATTACK1);

        if (self->timer >= 92) {
            if (fxRuby->fadeWhite < 512) {
                fxRuby->fadeWhite += 8;
            }
            else if (fxRuby->fadeBlack < 512) {
                fxRuby->fadeBlack += 8;
            }
            else if (self->timer == 302) {
                StateMachine_Run(self->options2);
                self->state = StateMachine_None;
            }
        }
    }
    else {
#endif
        if (self->timer == 32)
            RSDK.PlaySfx(UIWidgets->sfxWarp, false, 255);
        if (self->timer > 32) {
            self->alpha += 32;
            self->radius += 12;
        }
        if (self->radius > 512) {
            StateMachine_Run(self->options2);
            self->state = StateMachine_None;
        }
#if RETRO_USE_PLUS
    }
#endif
}

#if RETRO_INCLUDE_EDITOR
void UISaveSlot_EditorDraw(void)
{
    RSDK_THIS(UISaveSlot);
    self->drawOrder       = 2;
    self->updateRange.x   = 0x800000;
    self->updateRange.y   = 0x300000;
    if (!self->type) {
        UISaveSlot_LoadSaveInfo();
    }
    else {
        self->slotID = NO_SAVE_SLOT;
        UISaveSlot_Unknown6();
    }
    self->textSpriteIndex = UIWidgets->textSpriteIndex;
    UISaveSlot_Unknown8();

    UISaveSlot_Draw();
}

void UISaveSlot_EditorLoad(void) { UISaveSlot->aniFrames = RSDK.LoadSpriteAnimation("UI/SaveSelect.bin", SCOPE_STAGE); }
#endif

void UISaveSlot_Serialize(void)
{
    RSDK_EDITABLE_VAR(UISaveSlot, VAR_BOOL, disabled);
    RSDK_EDITABLE_VAR(UISaveSlot, VAR_ENUM, type);
    RSDK_EDITABLE_VAR(UISaveSlot, VAR_ENUM, slotID);
#if RETRO_USE_PLUS
    RSDK_EDITABLE_VAR(UISaveSlot, VAR_BOOL, encoreMode);
    RSDK_EDITABLE_VAR(UISaveSlot, VAR_BOOL, debugEncoreDraw);
    RSDK_EDITABLE_VAR(UISaveSlot, VAR_UINT8, dCharPoint);
    RSDK_EDITABLE_VAR(UISaveSlot, VAR_UINT8, dCharPartner);
    RSDK_EDITABLE_VAR(UISaveSlot, VAR_UINT8, dCharStock1);
    RSDK_EDITABLE_VAR(UISaveSlot, VAR_UINT8, dCharStock2);
    RSDK_EDITABLE_VAR(UISaveSlot, VAR_UINT8, dCharStock3);
#endif
}
