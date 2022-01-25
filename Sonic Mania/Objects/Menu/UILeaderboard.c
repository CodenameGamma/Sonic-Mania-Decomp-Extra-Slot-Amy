// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: UILeaderboard Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectUILeaderboard *UILeaderboard;

void UILeaderboard_Update(void)
{
    RSDK_THIS(UILeaderboard);
    if (self->textFrames != UIWidgets->textFrames) {
        UILeaderboard_SetupEntrySprites(self);
        self->textFrames = UIWidgets->textFrames;
    }
    StateMachine_Run(self->state);
    if (++self->zoneIconSprX >= 192)
        self->zoneIconSprX -= 192;

    EntityUIControl *parent = (EntityUIControl *)self->parent;
    if (self->state == UILeaderboard_State_Selected && (parent->buttonID != self->zoneID || parent->state != UIControl_ProcessInputs)) {
        self->isSelected = false;
        self->state      = UILeaderboard_State_Unselected;
    }
}

void UILeaderboard_LateUpdate(void) {}

void UILeaderboard_StaticUpdate(void) {}

void UILeaderboard_Draw(void)
{
    UILeaderboard_DrawPrimitives();
    UILeaderboard_DrawEntries();
}

void UILeaderboard_Create(void *data)
{
    RSDK_THIS(UILeaderboard);
    self->startPos.x      = self->position.x;
    self->startPos.y      = self->position.y;
    self->active          = ACTIVE_BOUNDS;
    self->drawOrder       = 2;
    self->visible         = true;
    self->drawFX          = FX_FLIP;
    self->updateRange.x   = 0x800000;
    self->updateRange.y   = 0x300000;
    self->processButtonCB = UILeaderboard_ProcessButtonCB;
    self->state           = UILeaderboard_State_Setup;
    UILeaderboard_SetupEntrySprites(self);
    self->textFrames = UIWidgets->textFrames;
    if (!SceneInfo->inEditor) {
        RSDK.SetText(self->text1, "", 0);
        RSDK.SetText(&self->text1[1], "", 0);
        RSDK.SetText(&self->text1[2], "", 0);
        RSDK.SetText(&self->text1[3], "", 0);
        RSDK.SetText(&self->text1[4], "", 0);
        RSDK.SetText(self->nameText, "", 0);
        RSDK.SetText(&self->nameText[1], "", 0);
        RSDK.SetText(&self->nameText[2], "", 0);
        RSDK.SetText(&self->nameText[3], "", 0);
        RSDK.SetText(&self->nameText[4], "", 0);
    }
}

void UILeaderboard_StageLoad(void) { UILeaderboard->aniFrames = RSDK.LoadSpriteAnimation("UI/SaveSelect.bin", SCOPE_STAGE); }

void UILeaderboard_SetupEntrySprites(EntityUILeaderboard *entity)
{
    RSDK.SetSpriteAnimation(UILeaderboard->aniFrames, 12, &entity->fuzzAnimator, true, 0);
    RSDK.SetSpriteAnimation(UILeaderboard->aniFrames, 13, &entity->zoneIconAnimator, true, entity->zoneID);

    int32 frame = 0;
    if (entity->playerID <= 1)
        frame = 0;
    else
        frame = entity->playerID - 1;
    RSDK.SetSpriteAnimation(UILeaderboard->aniFrames, 1, &entity->playerAnimator, true, frame);
    RSDK.SetSpriteAnimation(UILeaderboard->aniFrames, 2, &entity->playerShadowAnimator, true, frame);

    if (entity->playerID <= 1)
        frame = 5;
    else
        frame = entity->playerID + 4;
    RSDK.SetSpriteAnimation(UIWidgets->textFrames, 11, &entity->playerNameAnimator, true, frame);
    RSDK.SetSpriteAnimation(UIWidgets->textFrames, 11, &entity->actNumAnimator, true, entity->actID + 3);
    RSDK.SetSpriteAnimation(UIWidgets->fontFrames, 0, &entity->textAnimator, true, 0);
    UILeaderboard_LoadEntries(entity);
}

#if !RETRO_USE_PLUS
void UILeaderboard_InitLeaderboard(EntityUILeaderboard *leaderboard)
{
    int32 count              = APICallback_LeaderboardEntryCount();
    leaderboard->entryLength = count;
    if (leaderboard->entryIsUser && count >= 1) {
        int32 i = 0;
        for (; i < leaderboard->entryLength; ++i) {
            bool32 isUser = API_ReadLeaderboardEntry(i)->isUser;
            if (isUser)
                break;
        }

        int32 offset = leaderboard->entryLength - 6;
        if (i - 2 < offset)
            offset = i - 2;
        if (offset <= 0)
            offset = 0;
        leaderboard->entryOffset = offset;
    }
    else {
        leaderboard->entryOffset = 0;
    }
    UILeaderboard_SetupEntrySprites(leaderboard);
}

void UILeaderboard_SetupLeaderboard(EntityUILeaderboard *leaderboard, uint8 player, uint8 zone, uint8 act)
{
    leaderboard->zoneIconSprX = 0;
    leaderboard->entryOffset  = 0;

    leaderboard->entryLength = APICallback_LeaderboardEntryCount();
    leaderboard->playerID    = player;
    leaderboard->zoneID      = zone;
    leaderboard->actID       = act;
    RSDK.CopyPalette((zone >> 3) + 1, 32 * zone, 0, 224, 32);
    UILeaderboard_SetupEntrySprites(leaderboard);
    Localization_GetZoneName(&leaderboard->zoneName, zone);
    RSDK.SetSpriteAnimation(UIWidgets->uiSpriteIndex, 3, &leaderboard->zoneNameAnimator, true, 0);
    if (!SceneInfo->inEditor)
        RSDK.SetSpriteString(UIWidgets->uiSpriteIndex, 3, &leaderboard->zoneName);
}

#endif

void UILeaderboard_LoadEntries(EntityUILeaderboard *entity)
{
    if (!SceneInfo->inEditor) {
        for (int32 i = 0; i < 5; ++i) {
            LeaderboardEntry *entry = API_ReadLeaderboardEntry(i + entity->entryOffset);
#if RETRO_USE_PLUS
            if (entry && entry->status == STATUS_OK) {
#else
            if (entry) {
#endif
                char buffer[0x10];

                if (entry->globalRank < 1)
                    strcpy(buffer, "--");
                else
                    sprintf(buffer, "%d", entry->globalRank);
                RSDK.PrependText(&entity->rankText[i], buffer);
                RSDK.SetSpriteString(UIWidgets->fontFrames, 0, &entity->rankText[i]);

                RSDK.CopyString(&entity->nameText[i], &entry->username);
                if (entity->nameText[i].length > 19) {
                    entity->nameText[i].length = 19;
                    RSDK.AppendText(&entity->nameText[i], "...");
                }
                RSDK.SetSpriteString(UIWidgets->fontFrames, 0, &entity->nameText[i]);
                entity->times[i]  = entry->score;
                entity->ranks[i]  = entry->globalRank;
                entity->isUser[i] = entry->isUser;
            }
            else {
                RSDK.PrependText(&entity->rankText[i], "--");
                RSDK.SetSpriteString(UIWidgets->fontFrames, 0, &entity->rankText[i]);

                RSDK.PrependText(&entity->nameText[i], "--------");
                RSDK.SetSpriteString(UIWidgets->fontFrames, 0, &entity->nameText[i]);

                entity->times[i] = 0;
                if (entry)
                    entity->ranks[i] = entry->globalRank;
                else
                    entity->ranks[i] = 0;
                entity->isUser[i] = false;
            }
        }
    }
}

void UILeaderboard_DrawPrimitives(void)
{
    RSDK_THIS(UILeaderboard);
    Vector2 drawPos;

    if (!SceneInfo->inEditor)
        UIWidgets_DrawRectOutline_Blended(158, 414, self->position.x + 0x30000, self->position.y + 0x30000);

    UIWidgets_DrawRectOutline_Black(158, 414, self->position.x, self->position.y);

    drawPos.x = self->position.x - 0x840000;
    drawPos.y = self->position.y - 0x4C0000;
    RSDK.DrawRect(drawPos.x, drawPos.y, 0x1120000, 0x180000, 0x000000, 255, INK_NONE, false);

    drawPos.y += 0x180000;
    RSDK.DrawRect(drawPos.x, drawPos.y, 0x580000, 0x100000, 0xF0F0F0, 255, INK_NONE, false);

    drawPos.x += 0x580000;
    UIWidgets_DrawRightTriangle(drawPos.x, drawPos.y, 15, 240, 240, 240);

    drawPos.x += 0xF0000;
    drawPos.y += 0xF0000;
    UIWidgets_DrawRightTriangle(drawPos.x, drawPos.y, -15, 232, 40, 88);

    drawPos.y -= 0xF0000;
    RSDK.DrawRect(drawPos.x, drawPos.y, 0x3A0000, 0x100000, 0xE82858, 255, INK_NONE, false);

    drawPos.x += 0x3A0000;
    UIWidgets_DrawRightTriangle(drawPos.x, drawPos.y, 15, 232, 40, 88);

    drawPos.x += 0xF0000;
    drawPos.y += 0xF0000;
    UIWidgets_DrawRightTriangle(drawPos.x, drawPos.y, -15, 96, 160, 176);

    drawPos.y -= 0xF0000;
    RSDK.DrawRect(drawPos.x, drawPos.y, 0x3A0000, 0x100000, 0x60A0B0, 255, INK_NONE, false);

    drawPos.x += 0x3A0000;
    UIWidgets_DrawRightTriangle(drawPos.x, drawPos.y, 15, 96, 160, 176);

    drawPos.x += 0x270000;
    drawPos.y += 0xF0000;
    UIWidgets_DrawRightTriangle(drawPos.x, drawPos.y, -39, 88, 112, 224);

    RSDK.DrawRect(drawPos.x, drawPos.y - 0x270000, 0x3F0000, 0x280000, 0x5870E0, 255, INK_NONE, false);

    drawPos.x = self->position.x - 0xCC0000;
    drawPos.y = self->position.y - 0x240000;
    RSDK.DrawRect(drawPos.x, drawPos.y, 0x680000, 0x200000, 0x60A0AB, 255, INK_NONE, false);

    drawPos.x += 0x680000;
    UIWidgets_DrawRightTriangle(drawPos.x, drawPos.y, 31, 96, 160, 176);

    drawPos.x += 0x1F0000;
    drawPos.y += 0x1F0000;
    UIWidgets_DrawRightTriangle(drawPos.x, drawPos.y, -31, 88, 112, 224);

    drawPos.y -= 0x1F0000;
    RSDK.DrawRect(drawPos.x, drawPos.y, 0x790000, 0x200000, 0x5870E0, 255, INK_NONE, false);

    drawPos.x += 0x790000;
    UIWidgets_DrawRightTriangle(drawPos.x, drawPos.y, 31, 88, 112, 224);

    drawPos.x += 0x1F0000;
    drawPos.y += 0x1F0000;
    UIWidgets_DrawRightTriangle(drawPos.x, drawPos.y, -31, 232, 40, 88);

    RSDK.DrawRect(drawPos.x, drawPos.y - 0x1F0000, 0x790000, 0x200000, 0xE82858, 255, INK_NONE, false);

    RSDK.DrawRect(self->position.x - 0xCC0000, self->position.y - 0x40000, 0x1980000, 0x500000, 0x000000, 255, INK_NONE, false);
}

void UILeaderboard_DrawEntries(void)
{
    RSDK_THIS(UILeaderboard);
    Vector2 drawPos;

    UILeaderboard_DrawZonePreview();
    drawPos.x = self->position.x - 0x8C0000;
    drawPos.y = self->position.y - 0x4C0000;
    if (!SceneInfo->inEditor) {
        drawPos.x += 0xC0000;
        drawPos.y += 0x70000;
        RSDK.DrawText(&self->zoneNameAnimator, &drawPos, &self->zoneName, 0, self->zoneName.length, ALIGN_LEFT, 0, 0, 0, false);

        drawPos.x -= 0xC0000;
        drawPos.y -= 0x70000;
    }
    drawPos.y += 0x1C0000;
    RSDK.SetSpriteAnimation(UILeaderboard->aniFrames, 9, &self->taAnimator, true, 10);
    RSDK.DrawSprite(&self->taAnimator, &drawPos, false);

    drawPos.x += 0x6F0000;
    drawPos.y += 0x40000;
    RSDK.DrawSprite(&self->actNumAnimator, &drawPos, false);

    drawPos.x += 0x490000;
    RSDK.DrawSprite(&self->playerNameAnimator, &drawPos, false);

    drawPos.x = self->position.x + 0xA10000;
    drawPos.y = self->position.y - 0x350000;
    RSDK.DrawSprite(&self->playerShadowAnimator, &drawPos, false);

    drawPos.x -= 0x30000;
    drawPos.y -= 0x30000;
    RSDK.DrawSprite(&self->playerAnimator, &drawPos, false);

    drawPos.x = self->position.x - 0x980000;
    drawPos.y = self->position.y - 0x100000;
    RSDK.SetSpriteAnimation(UIWidgets->textFrames, 11, &self->taAnimator, true, 0);
    RSDK.DrawSprite(&self->taAnimator, &drawPos, false);

    drawPos.x += 0x8E0000;
    RSDK.SetSpriteAnimation(UIWidgets->textFrames, 11, &self->taAnimator, true, 1);
    RSDK.DrawSprite(&self->taAnimator, &drawPos, false);

    drawPos.x += 0x8E0000;
    RSDK.SetSpriteAnimation(UIWidgets->textFrames, 11, &self->taAnimator, true, 2);
    RSDK.DrawSprite(&self->taAnimator, &drawPos, false);

    for (int32 i = 0; i < 5; ++i) UILeaderboard_DrawRank(i);
}

void UILeaderboard_DrawZonePreview(void)
{
    RSDK_THIS(UILeaderboard);
    Vector2 drawPos;

    drawPos.x = self->position.x - 0xA80000;
    drawPos.y = self->position.y - 0x380000;
    UIWidgets_DrawRectOutline_Black(40, 72, drawPos.x, drawPos.y);

    if (SceneInfo->inEditor) {
        self->direction = self->fuzzDir;
        self->drawFX    = FX_FLIP;
        RSDK.DrawSprite(&self->fuzzAnimator, &drawPos, false);
        self->direction = FLIP_NONE;
        self->drawFX    = FX_NONE;
    }
    else {
        SpriteFrame *frame = RSDK.GetFrame(UILeaderboard->aniFrames, 13, self->zoneID);
        frame->pivotX      = -33;
        frame->width       = 66;
        frame->sprX        = self->zoneIconSprX;
        if (self->zoneIconSprX <= 126) {
            RSDK.DrawSprite(&self->zoneIconAnimator, &drawPos, false);
        }
        else {
            frame->width = 66 - (self->zoneIconSprX - 126);
            RSDK.DrawSprite(&self->zoneIconAnimator, &drawPos, false);

            frame->pivotX += frame->width;
            frame->sprX  = 0;
            frame->width = self->zoneIconSprX - 126;
            RSDK.DrawSprite(&self->zoneIconAnimator, &drawPos, false);
        }
    }
}

void UILeaderboard_DrawTime(int32 mins, int32 secs, int32 millisecs, int32 x, int32 y)
{
    RSDK_THIS(UILeaderboard);
    Vector2 drawPos;
    char scoreText[0x10];

    if (mins == 99 && secs == 99 && millisecs == 99)
        sprintf(scoreText, "<<:<<;<<");
    else if (!mins && !secs && !millisecs)
        sprintf(scoreText, "<<:<<;<<");
    else
        sprintf(scoreText, "%02d:%02d;%02d", mins, secs, millisecs);

    drawPos.x = x;
    drawPos.y = y;
    for (int32 i = 0; i < 8; ++i) {
        if (!scoreText[i])
            break;
        RSDK.SetSpriteAnimation(UILeaderboard->aniFrames, 8, &self->timeAnimator, true, (scoreText[i] - '0'));
        RSDK.DrawSprite(&self->timeAnimator, &drawPos, false);
        drawPos.x += 0x80000;
    }
}

void UILeaderboard_DrawRank(int32 id)
{
    RSDK_THIS(UILeaderboard);
    Vector2 drawPos;

    drawPos.x = (id << 20) + self->position.x - 0x200000;
    drawPos.y = (id << 20) + self->position.y + 0x40000;

    int32 colour = 0x5870E0;
    switch (self->ranks[id]) {
        default: break;
        case 1: colour = 0xD9AD04; break;
        case 2: colour = 0x98C0C8; break;
        case 3: colour = 0xC05802; break;
    }
    UIWidgets_DrawParallelogram(15, 169, 15, (colour >> 16) & 0xFF, (colour >> 8) & 0xFF, colour & 0xFF, drawPos.x, drawPos.y);

    drawPos.x -= 0x7A0000;
    if (!self->isUser[id] || !(UIControl->timer & 4)) {
        if (SceneInfo->inEditor) {
            drawPos.x += 0xE60000;
        }
        else {
            RSDK.DrawText(&self->textAnimator, &drawPos, &self->rankText[id], 0, self->rankText[id].length, ALIGN_RIGHT, 0, 0, 0, false);
            drawPos.x += 0x320000;
            RSDK.DrawText(&self->textAnimator, &drawPos, &self->nameText[id], 0, self->nameText[id].length, ALIGN_LEFT, 0, 0, 0, false);
            drawPos.x += 0xB40000;
        }

        int32 min, sec, ms;
        TimeAttackData_GetTimeFromValue(self->times[id], &min, &sec, &ms);
        drawPos.y -= 0x40000;
        UILeaderboard_DrawTime(min, sec, ms, drawPos.x, drawPos.y);
    }
}

void UILeaderboard_State_Setup(void)
{
    RSDK_THIS(UILeaderboard);
    self->state = UILeaderboard_State_Unselected;
    RSDK.ProcessAnimation(&self->fuzzAnimator);
    self->fuzzDir = self->fuzzAnimator.frameID & 3;
}

void UILeaderboard_State_Unselected(void)
{
    RSDK_THIS(UILeaderboard);
    RSDK.ProcessAnimation(&self->fuzzAnimator);
    self->fuzzDir = self->fuzzAnimator.frameID & 3;
}

void UILeaderboard_State_Selected(void)
{
    RSDK_THIS(UILeaderboard);
    RSDK.ProcessAnimation(&self->fuzzAnimator);
    self->fuzzDir = self->fuzzAnimator.frameID & 3;
}

void UILeaderboard_ProcessButtonCB(void)
{
    RSDK_THIS(UILeaderboard);
#if RETRO_USE_PLUS
    Vector2 entryCount = API.LeaderboardEntryCount();

    int32 newID = self->entryOffset;
    if (UIControl->keyUp)
        newID--;
    else if (UIControl->keyDown)
        newID++;
    else if (UIControl->keyLeft)
        newID -= 5;
    else if (UIControl->keyRight)
        newID += 5;

    int32 end = entryCount.x + entryCount.y;
    if (newID >= end)
        newID = end - 5;
    if (newID < entryCount.x)
        newID = entryCount.x;

    if (entryCount.y && self->entryOffset != newID) {
        LogHelpers_Print("old: %d, new: %d", self->entryOffset, newID);
        self->entryOffset = newID;
        UILeaderboard_LoadEntries(self);
        RSDK.PlaySfx(UIWidgets->sfxBleep, false, 255);
        if (entryCount.x <= 1 || newID >= entryCount.x + 2) {
            if (newID > (entryCount.x + entryCount.y - 7)) {
                LogHelpers_Print("Load down");
                API.LoadNewLeaderboardEntries(entryCount.x, entryCount.y + 20, 2);
            }
        }
        else {
            LogHelpers_Print("Load up");
            API.LoadNewLeaderboardEntries(entryCount.x - 20, entryCount.y + 20, 1);
        }
    }

    if (UIControl->keyY && self->yPressCBEnabled) {
#else
    int32 newID = self->entryOffset;
    if (UIControl->keyUp)
        newID--;
    else if (UIControl->keyDown)
        newID++;
    else if (UIControl->keyLeft)
        newID -= 5;
    else if (UIControl->keyRight)
        newID += 5;

    int32 end = self->entryOffset + self->entryLength;
    if (newID >= end)
        newID = end - 5;
    if (newID < 0)
        newID = 0;

    if (self->entryLength && self->entryOffset != newID) {
        self->entryOffset = newID;
        UILeaderboard_LoadEntries(self);
        RSDK.PlaySfx(UIWidgets->sfxBleep, false, 255);
    }

    if (UIControl->keyY && self->taRecord) {
#endif
        self->yPressCB();
        RSDK.PlaySfx(UIWidgets->sfxBleep, false, 255);
    }

    if (self->state == UILeaderboard_State_Unselected || !self->isSelected) {
        self->isSelected = true;
        self->state      = UILeaderboard_State_Selected;
        self->lostFocus  = false; // used literally just here so idk what it actually is
    }
}

#if RETRO_INCLUDE_EDITOR
void UILeaderboard_EditorDraw(void)
{
    RSDK_THIS(UILeaderboard);
    self->inkEffect = self->disabled ? INK_BLEND : INK_NONE;
    UILeaderboard_Draw();
}

void UILeaderboard_EditorLoad(void) { UILeaderboard->aniFrames = RSDK.LoadSpriteAnimation("UI/SaveSelect.bin", SCOPE_STAGE); }
#endif

void UILeaderboard_Serialize(void) { RSDK_EDITABLE_VAR(UILeaderboard, VAR_BOOL, disabled); }
