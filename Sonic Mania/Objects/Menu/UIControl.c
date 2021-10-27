#include "SonicMania.h"

ObjectUIControl *UIControl;

void UIControl_Update(void)
{
    RSDK_THIS(UIControl);

    if (entity->activeEntityID >= 0 && entity->activeEntityID != entity->field_D8)
        entity->field_D8 = entity->activeEntityID;
    if (!UIControl->field_C8 && entity->activeEntityID == -1)
        entity->activeEntityID = entity->field_D8;
    StateMachine_Run(entity->state);
    if (entity->backoutTimer > 0)
        entity->backoutTimer--;
    StateMachine_Run(entity->unknownCallback4);
}

void UIControl_LateUpdate(void) {}

void UIControl_StaticUpdate(void)
{
    if (UIControl->field_C) {
        UIControl->field_C     = 0;
        UIControl->inputLocked = true;
    }
    else {
        UIControl->inputLocked = 0;
    }
    UIControl->flagA = 0;
    ++UIControl->timer;
    UIControl->timer &= 0x7FFF;
}

void UIControl_Draw(void)
{
    RSDK_THIS(UIControl);
    RSDK_screens->position.x = (entity->position.x >> 0x10) - RSDK_screens->centerX;
    RSDK_screens->position.y = (entity->position.y >> 0x10) - RSDK_screens->centerY;
}

void UIControl_Create(void *data)
{
    RSDK_THIS(UIControl);
    if (!RSDK_sceneInfo->inEditor) {
        if (data) {
            Vector2 *size  = (Vector2 *)data;
            entity->size.x = size->x;
            entity->size.y = size->y;
        }
        entity->updateRange.x = entity->size.x >> 1;
        entity->updateRange.y = entity->size.y >> 1;
#if RETRO_USE_PLUS
        entity->promptCount = 0;
#endif
        if (entity->rowCount <= 1)
            entity->rowCount = 1;

        if (entity->columnCount <= 1)
            entity->columnCount = 1;

        if (!entity->dwordC4)
            entity->activeEntityID = entity->startingID;
        else
            entity->activeEntityID = entity->storedEntityID;

        entity->position.x += entity->cameraOffset.x;
        entity->position.y += entity->cameraOffset.y;
        entity->startPos.x = entity->position.x;
        entity->startPos.y = entity->position.y;
#if RETRO_USE_PLUS
        int32 entityID       = RSDK.GetEntityID(entity);
        if (UIButtonPrompt) {
            if (entityID != SLOT_DIALOG_UICONTROL) {
                foreach_all(UIButtonPrompt, prompt)
                {
                    if (entity->promptCount < 4) {
                        Hitbox hitbox;
                        hitbox.right  = entity->size.x >> 17;
                        hitbox.left   = -(entity->size.x >> 17);
                        hitbox.bottom = entity->size.y >> 17;
                        hitbox.top    = -(entity->size.y >> 17);
                        if (MathHelpers_PointInHitbox(FLIP_NONE, entity->startPos.x - entity->cameraOffset.x,
                                                      entity->startPos.y - entity->cameraOffset.y, &hitbox, prompt->position.x, prompt->position.y)) {
                            prompt->parent                            = (Entity *)entity;
                            entity->prompts[entity->promptCount++] = prompt;
                        }
                    }
                }
            }
        }
#endif

        RSDK_stickL[1].deadzone = 0.75f;
        RSDK_stickL[2].deadzone = 0.75f;
        RSDK_stickL[3].deadzone = 0.75f;
        RSDK_stickL[4].deadzone = 0.75f;
        UIControl_Unknown7();
        if (entity->noWidgets) {
            entity->active  = ACTIVE_NORMAL;
            entity->visible = true;
        }
        else {
            entity->dwordCC = 0;
            if (entity->activeOnLoad) {
                UIControl_Unknown4(entity);
            }
            else {
                entity->active = ACTIVE_NEVER;
            }
        }
    }
}

void UIControl_StageLoad(void)
{
    UIControl->inputLocked = false;
    UIControl->field_C     = 0;
    UIControl->active      = ACTIVE_ALWAYS;
    UIControl->field_4     = 0;
}

EntityUIControl *UIControl_GetUIControl(void)
{
    foreach_all(UIControl, control)
    {
        if (control->active == ACTIVE_ALWAYS) {
            foreach_return control;
        }
    }
    return NULL;
}

void UIControl_ClearInputs(char id)
{
    for (int32 i = 0; i < 4; ++i) {
        UIControl->upPress[i]      = false;
        UIControl->downPress[i]    = false;
        UIControl->leftPress[i]    = false;
        UIControl->rightPress[i]   = false;
        UIControl->yPress[i]       = false;
        UIControl->xPress[i]       = false;
        UIControl->backPress[i]    = false;
        UIControl->confirmPress[i] = false;
#if RETRO_USE_PLUS
        UIControl->startPress[i] = false;
#endif
    }

    UIControl->keyUp    = false;
    UIControl->keyDown  = false;
    UIControl->keyLeft  = false;
    UIControl->keyRight = false;
    UIControl->keyY     = id == 3;
    UIControl->keyX     = id == 2;
#if RETRO_USE_PLUS
    UIControl->keyStart = id == 5;
#endif

    if (API_GetConfirmButtonFlip()) {
        UIControl->keyConfirm = id == 1;
        UIControl->keyBack    = id == 0;
        UIControl->flagA      = id == 0;
    }
    else {
        UIControl->keyConfirm = id == 0;
        UIControl->keyBack    = id == 1;
        UIControl->flagA      = id == 1;
    }
    UIControl->field_C     = true;
    UIControl->inputLocked = true;
}

void UIControl_ProcessInputs(void)
{
    RSDK_THIS(UIControl);
    UIControl_Unknown16();

    if (!UIControl->inputLocked) {
        for (int32 i = 0; i < 4; ++i) {
            UIControl->upPress[i]    = RSDK_controller[i + 1].keyUp.press || RSDK_stickL[i + 1].keyUp.press;
            UIControl->downPress[i]  = RSDK_controller[i + 1].keyDown.press || RSDK_stickL[i + 1].keyDown.press;
            UIControl->leftPress[i]  = RSDK_controller[i + 1].keyLeft.press || RSDK_stickL[i + 1].keyLeft.press;
            UIControl->rightPress[i] = RSDK_controller[i + 1].keyRight.press || RSDK_stickL[i + 1].keyRight.press;

            if (UIControl->upPress[i] && UIControl->downPress[i]) // up and down? neither!
            {
                UIControl->upPress[i]   = false;
                UIControl->downPress[i] = false;
            }
            if (UIControl->leftPress[i] && UIControl->rightPress[i]) // left and right? neither!
            {
                UIControl->leftPress[i]  = false;
                UIControl->rightPress[i] = false;
            }

            UIControl->yPress[i] = RSDK_controller[i + 1].keyY.press;
            UIControl->xPress[i] = RSDK_controller[i + 1].keyX.press;
#if RETRO_USE_PLUS
            UIControl->startPress[i] = RSDK_controller[i + 1].keyStart.press;
#endif
            if (API_GetConfirmButtonFlip()) {
                UIControl->confirmPress[i] = RSDK_controller[i + 1].keyStart.press || RSDK_controller[i + 1].keyB.press;
                UIControl->backPress[i]    = RSDK_controller[i + 1].keyA.press;
            }
            else {
                UIControl->confirmPress[i] = RSDK_controller[i + 1].keyStart.press || RSDK_controller[i + 1].keyA.press;
                UIControl->backPress[i]    = RSDK_controller[i + 1].keyB.press;
            }
        }

        UIControl->keyUp    = RSDK_controller->keyUp.press || RSDK_stickL->keyUp.press;
        UIControl->keyDown  = RSDK_controller->keyDown.press || RSDK_stickL->keyDown.press;
        UIControl->keyLeft  = RSDK_controller->keyLeft.press || RSDK_stickL->keyLeft.press;
        UIControl->keyRight = RSDK_controller->keyRight.press || RSDK_stickL->keyRight.press;
        UIControl->keyY     = RSDK_controller->keyY.press;
        UIControl->keyX     = RSDK_controller->keyX.press;
#if RETRO_USE_PLUS
        UIControl->keyStart = RSDK_controller->keyStart.press;
#endif
        if (API_GetConfirmButtonFlip()) {
            UIControl->keyConfirm    = RSDK_controller->keyStart.press || RSDK_controller->keyB.press;
            UIControl->keyBack = RSDK_controller->keyA.press;
        }
        else {
            UIControl->keyConfirm    = RSDK_controller->keyStart.press || RSDK_controller->keyA.press;
            UIControl->keyBack = RSDK_controller->keyB.press;
        }
#if RETRO_USE_PLUS
        UIControl->keyBack |= RSDK_unknown->field_10;
#else

#endif
        UIControl->keyBack |= UIControl->flagA;

        if (UIControl->keyBack) {
            UIControl->keyConfirm = false;
            UIControl->keyY    = false;
        }
        if (UIControl->keyConfirm) {
            UIControl->keyY = false;
        }
        UIControl->inputLocked = true;
    }

    if (!entity->selectionDisabled) {
        bool32 flag = false;
        if (UIControl->keyBack) {
            if (!entity->childHasFocus && !entity->dialogHasFocus
#if RETRO_USE_PLUS
                && !entity->popoverHasFocus 
#endif
                && entity->backoutTimer <= 0) {
                if (entity->backPressCB) {
                    flag = entity->backPressCB();
                    if (!flag) {
                        UIControl->keyBack = false;
                    }
                    else {
                        if (entity->buttons[entity->activeEntityID])
                            entity->buttons[entity->activeEntityID]->flag = false;
                    }
                }
                else {
                    if (entity->parentTag.textLength <= 0) {
                        UIControl->keyBack = false;
                    }
                    else {
                        entity->selectionDisabled = true;
                        UITransition_StartTransition(UIControl_Unknown13, 0);
                        flag = false;

                        if (entity->buttons[entity->activeEntityID])
                            entity->buttons[entity->activeEntityID]->flag = false;
                    }
                }

                if (flag)
                    return;
            }
#if RETRO_USE_PLUS
            else {
                LogHelpers_Print("Backout prevented");
                LogHelpers_Print("childHasFocus = %d", entity->childHasFocus);
                LogHelpers_Print("dialogHasFocus = %d", entity->dialogHasFocus);
                LogHelpers_Print("popoverHasFocus = %d", entity->popoverHasFocus);
                LogHelpers_Print("backoutTimer = %d", entity->backoutTimer);
            }
#endif
        }

        if (entity->processButtonInputCB)
            entity->processButtonInputCB();
        else
            UIControl_ProcessButtonInput();

        if (!entity->selectionDisabled) {
            if (UIControl->keyY) {
                if (!entity->childHasFocus
                    && !entity->dialogHasFocus
#if RETRO_USE_PLUS
                    && !entity->popoverHasFocus
#endif
                    && entity->backoutTimer <= 0) {
                    StateMachine_Run(entity->yPressCB);
                }
                UIControl->keyY = false;
            }
            if (UIControl->keyX) {
                if (!entity->childHasFocus 
                    && !entity->dialogHasFocus
#if RETRO_USE_PLUS
                    && !entity->popoverHasFocus 
#endif
                    && entity->backoutTimer <= 0) {
                    StateMachine_Run(entity->xPressCB);
                }
                UIControl->keyX = false;
            }
        }
    }
}

int32 UIControl_Unknown1(EntityUIControl *control, EntityUIButton *entity)
{
    for (int32 i = 0; i < control->buttonCount; ++i) {
        if (entity == control->buttons[i])
            return i;
    }
    return -1;
}

void UIControl_Unknown2(EntityUIControl *control)
{
    Entity *storeEntity = RSDK_sceneInfo->entity;
    for (int32 i = 0; i < SCENEENTITY_COUNT; ++i) {
        EntityUIButton *entity = RSDK.GetEntityByID(i);
        if (entity) {
            int32 h          = RSDK_screens->height;
            int32 centerX    = -RSDK_screens->width >> 1;
            int32 centerX2   = RSDK_screens->width >> 1;
            int32 centerY    = h >> 1;
            int32 negCenterY = -h >> 1;

            int32 x = centerX2;
            int32 y = centerY;
            if (centerX < centerX2)
                x = centerX;
            if (centerX2 > centerX)
                centerX = centerX2;
            if (negCenterY < centerY)
                y = negCenterY;
            if (centerY > negCenterY)
                negCenterY = centerY;
            if (entity->position.x >= control->position.x + (x << 16) && entity->position.x <= control->position.x + (centerX << 16)) {
                if (entity->position.y >= control->position.y + (y << 16) && entity->position.y <= control->position.y + (negCenterY << 16)) {
                    int32 id                 = RSDK.GetEntityID(entity);
                    RSDK_sceneInfo->entity = (Entity *)entity;
                    if (UIButton && entity->objectID == UIButton->objectID) {
                        UIButton_Unknown1(entity);
                        UIButton_Update();
                    }
                    else if (UIChoice && entity->objectID == UIChoice->objectID) {
                        UIChoice_Update();
                    }
                    else if (UITAZoneModule && entity->objectID == UITAZoneModule->objectID) {
                        UITAZoneModule_Update();
                    }
#if RETRO_USE_PLUS
                    else if (UIReplayCarousel && entity->objectID == UIReplayCarousel->objectID) {
                        UIReplayCarousel_Update();
                    }
#endif
                    else if (UIModeButton && entity->objectID == UIModeButton->objectID) {
#if RETRO_USE_PLUS
                        UIModeButton_Update();
#else
                        EntityUIModeButton *modeButton = (EntityUIModeButton *)RSDK_sceneInfo->entity;
                        modeButton->touchPosStart.x    = 0xB80000;
                        modeButton->touchPosStart.y    = 0x3E0000;
                        modeButton->touchPosEnd.x      = 0;
                        modeButton->touchPosEnd.y      = -0x120000;
                        if (modeButton->textSpriteIndex != UIWidgets->textSpriteIndex || modeButton->wasDisabled != modeButton->disabled) {
                            UIModeButton_Unknown1();
                            modeButton->textSpriteIndex = UIWidgets->textSpriteIndex;
                            modeButton->wasDisabled     = modeButton->disabled;
                        }
#endif
                    }
                    else if (UIVsZoneButton && entity->objectID == UIVsZoneButton->objectID) {
                        UIVsZoneButton_Update();
                    }
                    else if (UIHeading && entity->objectID == UIHeading->objectID) {
                        EntityUIHeading *heading = (EntityUIHeading *)entity;
                        if (heading->textSpriteIndex != UIHeading->textSpriteIndex) {
                            RSDK.SetSpriteAnimation(UIHeading->textSpriteIndex, heading->headingID, &heading->animator, true, 0);
                            heading->textSpriteIndex = UIHeading->textSpriteIndex;
                        }
                        StateMachine_Run(heading->state);
                    }
                    if (entity->visible)
                        RSDK.AddDrawListRef(entity->drawOrder, id);
                    RSDK_sceneInfo->entity = storeEntity;
                }
            }
        }
    }
}

#if RETRO_USE_PLUS
void UIControl_Unknown3(EntityUIControl *entity)
{
    for (int32 i = 0; i < entity->promptCount; ++i) {
        entity->prompts[i]->active = ACTIVE_NORMAL;
    }
}
#endif

void UIControl_Unknown4(EntityUIControl *entity)
{
#if RETRO_USE_PLUS
    RSDK.PrintText(PRINT_NORMAL, &entity->tag);
#endif
    entity->active  = ACTIVE_ALWAYS;
    entity->visible = true;
    if (entity->dwordC4) {
        entity->activeEntityID = entity->storedEntityID;
        entity->storedEntityID = 0;
        entity->dwordC4        = 0;
    }
    else if (entity->resetSelection) {
        entity->activeEntityID = entity->startingID;
    }

    RSDK.ClearCameras();
    RSDK.AddCamera(&entity->position, RSDK_screens->width << 16, RSDK_screens->height << 16, true);
    UIControl_Unknown2(entity);
    if (!entity->childHasFocus && (entity->resetSelection || !entity->dwordCC)) {
        entity->position.x   = entity->startPos.x;
        entity->position.y   = entity->startPos.y;
        entity->posUnknown.x = entity->startPos.x;
        entity->posUnknown.y = entity->startPos.y;
    }
    entity->state         = UIControl_ProcessInputs;
    entity->childHasFocus = false;
    entity->dwordCC       = 1;

#if RETRO_USE_PLUS
    for (int32 i = 0; i < entity->promptCount; ++i) {
        entity->prompts[i]->active = ACTIVE_NORMAL;
    }
#endif
    if (entity->unknownCallback3) {
        Entity *storeEntity    = RSDK_sceneInfo->entity;
        RSDK_sceneInfo->entity = (Entity *)entity;
        entity->unknownCallback3();
        RSDK_sceneInfo->entity = storeEntity;
    }
}

void UIControl_Unknown5(EntityUIControl *entity)
{
    entity->active  = ACTIVE_ALWAYS;
    entity->visible = true;
    if (!entity->dialogHasFocus
#if RETRO_USE_PLUS
        && !entity->popoverHasFocus
#endif
        && !entity->childHasFocus) {
        if (entity->dwordC4) {
            entity->activeEntityID = entity->storedEntityID;
            entity->storedEntityID = 0;
            entity->dwordC4        = 0;
        }
        else if (entity->resetSelection) {
            entity->activeEntityID = entity->startingID;
#if !RETRO_USE_PLUS
            entity->position.x   = entity->startPos.x;
            entity->position.y   = entity->startPos.y;
            entity->posUnknown.x = entity->startPos.x;
            entity->posUnknown.y = entity->startPos.y;
            entity->state        = UIControl_ProcessInputs;
#endif
        }

#if RETRO_USE_PLUS
        if (entity->resetSelection) {
            entity->position.x   = entity->startPos.x;
            entity->position.y   = entity->startPos.y;
            entity->posUnknown.x = entity->startPos.x;
            entity->posUnknown.y = entity->startPos.y;
            entity->dwordCC      = 1;
            UIControl_Unknown3(entity);
            entity->state = UIControl_ProcessInputs;
        }
        else {
            if (entity->dwordCC) {
                entity->dwordCC = 1;
                UIControl_Unknown3(entity);
                entity->state = UIControl_ProcessInputs;
            }
            else {
                entity->position.x   = entity->startPos.x;
                entity->position.y   = entity->startPos.y;
                entity->posUnknown.x = entity->startPos.x;
                entity->posUnknown.y = entity->startPos.y;
                entity->dwordCC      = 1;
                UIControl_Unknown3(entity);
                entity->state = UIControl_ProcessInputs;
            }
        }
#endif
    }
}

void UIControl_Unknown6(EntityUIControl *control)
{
    RSDK_THIS(UIControl);
    UIControl->field_C8 = 0;
    control->active     = ACTIVE_NEVER;
    control->visible    = false;
    control->state      = StateMachine_None;
#if RETRO_USE_PLUS
    if (entity->promptCount) {
        for (int32 i = 0; i < control->promptCount; ++i) {
            control->prompts[i]->active = ACTIVE_BOUNDS;
        }
    }
#endif
}

void UIControl_Unknown7(void)
{
    RSDK_THIS(UIControl);
    int32 slotID = RSDK.GetEntityID(entity);
    Hitbox bounds;

    if (UIHeading && slotID != SLOT_DIALOG_UICONTROL) {
        foreach_all(UIHeading, heading)
        {
            int32 x         = entity->startPos.x - entity->cameraOffset.x;
            int32 y         = entity->startPos.y - entity->cameraOffset.y;
            bounds.right  = entity->size.x >> 17;
            bounds.left   = -(entity->size.x >> 17);
            bounds.bottom = entity->size.y >> 17;
            bounds.top    = -(entity->size.y >> 17);
            if (MathHelpers_PointInHitbox(FLIP_NONE, x, y, &bounds, heading->position.x, heading->position.y))
                entity->heading = (Entity *)heading;
        }
    }

#if RETRO_USE_PLUS
    if (UIShifter && slotID != SLOT_DIALOG_UICONTROL) {
        foreach_all(UIShifter, shifter)
        {
            int32 x         = entity->startPos.x - entity->cameraOffset.x;
            int32 y         = entity->startPos.y - entity->cameraOffset.y;
            bounds.right  = entity->size.x >> 17;
            bounds.left   = -(entity->size.x >> 17);
            bounds.bottom = (entity->size.y >> 17);
            bounds.top    = -(entity->size.y >> 17);
            if (MathHelpers_PointInHitbox(FLIP_NONE, x, y, &bounds, shifter->position.x, shifter->position.y)) {
                entity->shifter = (Entity *)shifter;
                shifter->parent = entity;
            }
        }
    }

    if (UICarousel && slotID != SLOT_DIALOG_UICONTROL) {
        foreach_all(UICarousel, carousel)
        {
            int32 x         = entity->startPos.x - entity->cameraOffset.x;
            int32 y         = entity->startPos.y - entity->cameraOffset.y;
            bounds.right  = entity->size.x >> 17;
            bounds.left   = -(entity->size.x >> 17);
            bounds.bottom = entity->size.y >> 17;
            bounds.top    = -(entity->size.y >> 17);
            if (MathHelpers_PointInHitbox(FLIP_NONE, x, y, &bounds, carousel->position.x, carousel->position.y)) {
                entity->carousel = carousel;
                carousel->parent = (Entity *)entity;
            }
        }
    }
#endif

    for (int32 i = 0; i < SCENEENTITY_COUNT; ++i) {
        EntityUIButton *button = RSDK_GET_ENTITY(i, UIButton);
        if (button) {
            int32 id = button->objectID;
            if (id != UIButton->objectID && (!UIModeButton || id != UIModeButton->objectID) && (!UISaveSlot || id != UISaveSlot->objectID)
                && (!UICharButton || id != UICharButton->objectID) && (!UITAZoneModule || id != UITAZoneModule->objectID)
#if RETRO_USE_PLUS
                && (!UIRankButton || id != UIRankButton->objectID) && (!UIReplayCarousel || id != UIReplayCarousel->objectID)
#endif
                && (!UILeaderboard || id != UILeaderboard->objectID) && (!UIVsCharSelector || id != UIVsCharSelector->objectID)
                && (!UIVsZoneButton || id != UIVsZoneButton->objectID) && (!UIVsResults || id != UIVsResults->objectID)
                && (!UISlider || id != UISlider->objectID) && (!UIKeyBinder || id != UIKeyBinder->objectID)) {
            }
            else {
                int32 x         = entity->startPos.x - entity->cameraOffset.x;
                int32 y         = entity->startPos.y - entity->cameraOffset.y;
                bounds.left   = -(entity->size.x >> 17);
                bounds.top    = -(entity->size.y >> 17);
                bounds.right  = entity->size.x >> 17;
                bounds.bottom = entity->size.y >> 17;
                if (MathHelpers_PointInHitbox(FLIP_NONE, x, y, &bounds, button->position.x, button->position.y)) {
                    if (entity->buttonCount < 64) {
                        if (!button->parent)
                            button->parent = (Entity *)entity;
                        entity->buttons[entity->buttonCount++] = button;
                    }
                }
            }
        }
    }
}

bool32 UIControl_Unknown9(EntityUIControl *entity)
{
    if (entity->scrollSpeed.x && entity->scrollSpeed.y) {
        return entity->position.x != entity->posUnknown.x || entity->position.y != entity->posUnknown.y;
    }
    else if (entity->scrollSpeed.x) {
        return entity->position.x != entity->posUnknown.x;
    }
    else if (entity->scrollSpeed.y) {
        return entity->position.y != entity->posUnknown.y;
    }
    return false;
}

void UIControl_MatchMenuTag(const char *text)
{
    TextInfo info;
    info.text       = NULL;
    info.length     = 0;
    info.textLength = 0;
    RSDK.PrependText(&info, text);
    foreach_all(UIControl, entity)
    {
        if (entity->active == ACTIVE_ALWAYS) {
            UIControl_Unknown6(entity);
        }
        else if (RSDK.StringCompare(&info, &entity->tag, false)) {
            UIControl_Unknown4(entity);
        }
        else {
            UIControl_Unknown6(entity);
        }
    }
}

void UIControl_Unknown11(TextInfo *info)
{
    if (info->textLength) {
        foreach_all(UIControl, entity)
        {
            if (entity->active == ACTIVE_ALWAYS || !RSDK.StringCompare(info, &entity->tag, false))
                UIControl_Unknown6(entity);
            else
                UIControl_Unknown4(entity);
        }
    }
}

void UIControl_Unknown12(Entity *control)
{
    foreach_all(UIControl, entity)
    {
        if (entity->active == ACTIVE_ALWAYS) {
            UIControl_Unknown6(entity);
        }
        else if (entity == (EntityUIControl *)control) {
            UIControl_Unknown5(entity);
        }
        else {
            UIControl_Unknown6(entity);
        }
    }
}

void UIControl_Unknown13(void)
{
    EntityUIControl *entity   = UIControl_GetUIControl();
    entity->selectionDisabled = false;
    UIControl_Unknown11(&entity->parentTag);
}

void UIControl_Unknown15(EntityUIControl *entity, int32 x, int32 y)
{
    int32 finalX = x;
    if (!x) {
        finalX = entity->position.x;
        x      = entity->position.x;
    }
    int32 finalY = y;
    if (!y) {
        finalY = entity->position.y;
        y      = entity->position.y;
    }

#if RETRO_USE_PLUS
    if (!entity->noClamp) {
#endif
        int32 startX = entity->startPos.x - entity->cameraOffset.x;
        int32 startY = entity->startPos.y - entity->cameraOffset.y;
        int32 x1     = startX + (RSDK_screens->width << 15) - (entity->size.x >> 1);
        int32 x2     = startX + (entity->size.x >> 1) - (RSDK_screens->width << 15);
        int32 y1     = startY + (RSDK_screens->height << 15) - (entity->size.y >> 1);
        int32 y2     = startY + (entity->size.y >> 1) - (RSDK_screens->height << 15);
        if (x < x2)
            x2 = x;
        finalX = x2;
        if (x1 > x2)
            finalX = x1;

        if (y < y2)
            y2 = y;
        finalY = y2;
        if (y1 > y2)
            finalY = y1;
#if RETRO_USE_PLUS
    }
#endif
    entity->posUnknown.x = finalX;
    entity->posUnknown.y = finalY;
}

void UIControl_Unknown16(void)
{
    RSDK_THIS(UIControl);
    if (entity->position.x < entity->posUnknown.x) {
        entity->position.x += entity->scrollSpeed.x;
        if (entity->position.x > entity->posUnknown.x)
            entity->position.x = entity->posUnknown.x;
    }
    if (entity->position.x > entity->posUnknown.x) {
        entity->position.x -= entity->scrollSpeed.x;
        if (entity->position.x < entity->posUnknown.x)
            entity->position.x = entity->posUnknown.x;
    }

    if (entity->position.y < entity->posUnknown.y) {
        entity->position.y += entity->scrollSpeed.y;
        if (entity->position.y > entity->posUnknown.y)
            entity->position.y = entity->posUnknown.y;
    }
    if (entity->position.y > entity->posUnknown.y) {
        entity->position.y -= entity->scrollSpeed.y;
        if (entity->position.y < entity->posUnknown.y)
            entity->position.y = entity->posUnknown.y;
    }
}

void UIControl_ProcessButtonInput(void)
{
    RSDK_THIS(UIControl);

    bool32 flag = false;
    if (RSDK_touchMouse->count || UIControl->field_C8) {
        EntityUIButton *activeButton = 0;
        UIControl->field_C8          = RSDK_touchMouse->count != 0;
        UIControl->field_4           = 1;

        for (int32 i = 0; i < entity->buttonCount; ++i) {
            if (entity->buttons[i]) {
                EntityUIButton *button = entity->buttons[i];

                Entity *storeEntity    = RSDK_sceneInfo->entity;
                RSDK_sceneInfo->entity = (Entity *)button;
                if (button->touchCB && !entity->dialogHasFocus
#if RETRO_USE_PLUS
                    && !entity->popoverHasFocus
#endif
                    ) {
                    if (!button->options8 || !button->options8()) {
                        if (flag || button->touchCB()) {
                            flag = true;
                            if (button->touchCB && !activeButton)
                                activeButton = button;
                        }
                        else {
                            flag = false;
                        }
                    }
                }
                RSDK_sceneInfo->entity = storeEntity;
            }
        }

        if (RSDK_touchMouse->count) {
            if (flag) {
                int32 id = -1;
                for (int32 i = 0; i < entity->buttonCount; ++i) {
                    if (activeButton == entity->buttons[i]) {
                        id = i;
                        break;
                    }
                }

                entity->activeEntityID = id;
                if (activeButton->flag) {
                    Entity *storeEntity    = RSDK_sceneInfo->entity;
                    RSDK_sceneInfo->entity = (Entity *)activeButton;
                    activeButton->flag     = true;
                    StateMachine_Run(activeButton->options5);
                    RSDK_sceneInfo->entity = storeEntity;
                }
            }
            else {
                entity->activeEntityID = -1;
            }
        }

        UIControl->field_4 = 0;
    }

    if (entity->activeEntityID >= 0) {
        if (entity->activeEntityID < entity->buttonCount) {
            EntityUIButton *button = entity->buttons[entity->activeEntityID];
            if (button) {
                Entity *storeEntity    = RSDK_sceneInfo->entity;
                RSDK_sceneInfo->entity = (Entity *)button;
                if (button->processButtonCB) {
                    if (!button->options8 || !button->options8())
                        button->processButtonCB();
                }
                RSDK_sceneInfo->entity = storeEntity;
            }
        }
    }
}

#if RETRO_INCLUDE_EDITOR
void UIControl_EditorDraw(void) {}

void UIControl_EditorLoad(void) {}
#endif

void UIControl_Serialize(void)
{
    RSDK_EDITABLE_VAR(UIControl, VAR_STRING, tag);
    RSDK_EDITABLE_VAR(UIControl, VAR_STRING, parentTag);
    RSDK_EDITABLE_VAR(UIControl, VAR_BOOL, activeOnLoad);
    RSDK_EDITABLE_VAR(UIControl, VAR_BOOL, noWidgets);
    RSDK_EDITABLE_VAR(UIControl, VAR_BOOL, resetSelection);
    RSDK_EDITABLE_VAR(UIControl, VAR_UINT8, rowCount);
    RSDK_EDITABLE_VAR(UIControl, VAR_UINT8, columnCount);
    RSDK_EDITABLE_VAR(UIControl, VAR_UINT8, startingID);
    RSDK_EDITABLE_VAR(UIControl, VAR_VECTOR2, size);
    RSDK_EDITABLE_VAR(UIControl, VAR_VECTOR2, cameraOffset);
    RSDK_EDITABLE_VAR(UIControl, VAR_VECTOR2, scrollSpeed);
#if RETRO_USE_PLUS
    RSDK_EDITABLE_VAR(UIControl, VAR_BOOL, noClamp);
    RSDK_EDITABLE_VAR(UIControl, VAR_BOOL, noWrap);
#endif
}
