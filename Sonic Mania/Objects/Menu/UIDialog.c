#include "SonicMania.h"

ObjectUIDialog *UIDialog;

void UIDialog_Update(void)
{
    RSDK_THIS(UIDialog);
    StateMachine_Run(self->state);
    UIDialog_Unknown7();

    if (self->closeDelay > 0) {
        if (!--self->closeDelay)
            UIDialog_HandleAutoClose();
    }
}

void UIDialog_LateUpdate(void) {}

void UIDialog_StaticUpdate(void) {}

void UIDialog_Draw(void)
{
    RSDK_THIS(UIDialog);
    UIDialog_Unknown6();

    Vector2 drawPos;
    switch (self->lineCount) {
        case 0:
        case 1:
            drawPos.x = (self->position.x + 0x100000) + 0x100000;
            drawPos.y = self->position.y - 0x200000;
            break;
        case 2:
            drawPos.x = (self->position.x + 0x100000) + 0x180000;
            drawPos.y = self->position.y - 0x280000;
            break;
        case 3:
            drawPos.x = (self->position.x + 0x100000) + 0x200000;
            drawPos.y = self->position.y - 0x300000;
            break;
        default: break;
    }
    drawPos.x += self->drawPos.x;
    drawPos.y += self->drawPos.y;

    int32 w     = 0;
    int32 count = self->lineCount + 1;
    if (count <= 0) {
        count = self->lineCount + 1;
    }
    else {
        for (int32 i = 0; i < count; ++i) {
            int32 start = 0;
            if (i)
                start = self->lineLength[i - 1] + 1;

            int32 len = 0;
            if (i >= self->lineCount)
                len = self->textInfo.textLength;
            else
                len = self->lineLength[i];
            int32 width = RSDK.GetStringWidth(UIWidgets->labelSpriteIndex, 0, &self->textInfo, start, len, 0);
            if (width > w)
                w = width;
        }
    }

    for (int32 i = 0; i < count; ++i) {
        int32 offset = -0x8000 * w;

        int32 start = 0;
        if (i)
            start = self->lineLength[i - 1] + 1;

        int32 len = 0;
        if (i >= self->lineCount)
            len = self->textInfo.textLength;
        else
            len = self->lineLength[i];
        RSDK.GetStringWidth(UIWidgets->labelSpriteIndex, 0, &self->textInfo, start, len, 0);
        drawPos.x += offset;
        RSDK.DrawText(&self->animator, &drawPos, &self->textInfo, start, len, 0, 0, 0, NULL, false);
        drawPos.y += 0x120000;
        drawPos.x = drawPos.x - offset - 0x120000;
    }
}

void UIDialog_Create(void *data)
{
    RSDK_THIS(UIDialog);
    self->active    = ACTIVE_ALWAYS;
    self->drawOrder = 15;
    self->visible   = true;
    if (data)
        UIDialog_SetupText(self, data);
    self->id = 0;
}

void UIDialog_StageLoad(void)
{
    UIDialog->controlStore = NULL;
    UIDialog->activeDialog = NULL;
}

EntityUIDialog *UIDialog_CreateActiveDialog(TextInfo *msg)
{
    if (UIDialog->activeDialog) {
        LogHelpers_Print("EXCEPTION: Called CreateDialog when an activeDialog already existed.");
    }
    else {
        int32 id = RSDK_GET_ENTITY(SLOT_DIALOG, UIDialog)->objectID;
        if (id) {
            LogHelpers_Print("Can't create UIDialog (%d), entity already exists in slot (class ID: %d)", UIDialog->objectID, id);
        }
        else {
            RSDK.ResetEntitySlot(SLOT_DIALOG, UIDialog->objectID, msg);
            EntityUIDialog *dialog = RSDK_GET_ENTITY(SLOT_DIALOG, UIDialog);
            dialog->position.x     = (ScreenInfo->position.x + ScreenInfo->centerX) << 16;
            dialog->position.y     = (ScreenInfo->position.y + ScreenInfo->centerY) << 16;
            UIDialog->activeDialog = dialog;
            return dialog;
        }
    }
    return NULL;
}

void UIDialog_SetupText(EntityUIDialog *dialog, TextInfo *text)
{
    if (text) {
        dialog->lineCount = 0;
        int32 charPos       = 0;
        for (int32 i = 0; i < text->textLength; ++i) {
            if (text->text[charPos] == '\n' && dialog->lineCount < 3) {
                dialog->lineLength[dialog->lineCount] = charPos;
                ++dialog->lineCount;
            }
            ++charPos;
        }
        RSDK.CopyString(&dialog->textInfo, text);
        RSDK.SetSpriteAnimation(UIWidgets->labelSpriteIndex, 0, &dialog->animator, true, 0);
        RSDK.SetSpriteString(UIWidgets->labelSpriteIndex, 0, &dialog->textInfo);
    }
}

void UIDialog_AddButton(uint8 frame, EntityUIDialog *dialog, void (*callback)(void), bool32 flag)
{
    int32 id = dialog->id;
    if (dialog->id < UIDialog_OptionCount) {
        dialog->buttonFrames[dialog->id] = frame;
        dialog->callbacks[dialog->id]    = callback;
        dialog->flags[dialog->id]        = flag;
        RSDK.ResetEntitySlot(dialog->id + 23, UIButton->objectID, NULL);
        EntityUIButton *button = RSDK.GetEntityByID(dialog->id + 23);
        button->position.x     = (ScreenInfo->position.x + ScreenInfo->centerX) << 16;
        button->position.y     = (ScreenInfo->position.y + ScreenInfo->centerY) << 16;
        RSDK.SetSpriteAnimation(UIWidgets->textSpriteIndex, 9, &button->animator, true, frame);
        button->textSpriteIndex = UIWidgets->textSpriteIndex;

        if (frame == DIALOG_CONTINUE)
            button->size.x = 0x640000;
        else
            button->size.x = 0x320000;
        button->size.y              = 0x180000;
        button->options2            = UIDialog_Unknown10;
        button->dword138            = 24;
        button->align               = 1;
        button->active              = ACTIVE_ALWAYS;
        button->drawOrder           = dialog->drawOrder;
        dialog->entPtrs[dialog->id] = button;
        ++dialog->id;
        EntityUIControl *parent = dialog->parent;
        if (parent) {
            button->parent        = (Entity *)parent;
            parent->buttons[id]  = button;
            parent->buttonCount = dialog->id;
        }
    }
}

void UIDialog_Setup(EntityUIDialog *dialog)
{
    if (dialog) {
        bool32 flag = false;
        Vector2 size;
        size.x = ScreenInfo->width << 16;
        size.y = ScreenInfo->height << 16;
        foreach_all(UIControl, control)
        {
            if (control->active == ACTIVE_ALWAYS) {
                flag                        = true;
                control->dialogHasFocus     = true;
                UIDialog->controlStore      = control;
                UIDialog->controlStateStore = control->state;
                foreach_break;
            }
        }

        control = NULL;
        RSDK.ResetEntitySlot(SLOT_DIALOG_UICONTROL, UIControl->objectID, &size);
        control                      = RSDK_GET_ENTITY(SLOT_DIALOG_UICONTROL, UIControl);
        control->dwordCC             = 1;
        control->position.x          = (ScreenInfo->position.x + ScreenInfo->centerX) << 16;
        control->position.y          = (ScreenInfo->position.y + ScreenInfo->centerY) << 16;
        control->rowCount            = 1;
        control->columnCount         = dialog->id;
        control->activeEntityID      = 0;
        control->backPressCB         = UIDialog_HandleAutoClose;
        control->selectionDisabled = true;
        dialog->parent               = control;
        if (!flag) {
            UIDialog->controlStore      = NULL;
            UIDialog->controlStateStore = StateMachine_None;
        }

        int32 i = 0;
        for (; i < UIDialog_OptionCount; ++i) {
            if (!dialog->entPtrs[i])
                break;
            dialog->entPtrs[i]->parent = (Entity *)control;
            control->buttons[i]       = dialog->entPtrs[i];
        }
        control->buttonCount = i;
        dialog->timer       = 0;
        dialog->state          = UIDialog_Unknown11;
    }
}

void UIDialog_Unknown4(EntityUIDialog *self, void (*callback)(void))
{
    if (self) {
        if (self->state != UIDialog_Unknown13) {
            self->parent->selectionDisabled = true;
            self->timer                    = 0;
            self->state                       = UIDialog_Unknown13;
            self->curCallback                 = callback;
        }
    }
}

void UIDialog_Unknown6(void)
{
    RSDK_THIS(UIDialog);
    RSDK.DrawRect(((ScreenInfo->position.x + ScreenInfo->centerX) << 16) - (self->field_70.x >> 1),
                  ((ScreenInfo->position.y + ScreenInfo->centerY) << 16) - (self->field_70.y >> 1), self->field_70.x, self->field_70.y,
                  self->field_B8 ? 0x282028 : 0x000000, 255, INK_NONE, false);
    UIWidgets_DrawRhombus(0x8F, 0xC8, 0x8F, 0x30, 0xA0, 0xF0, self->drawPos.x + ((ScreenInfo->position.x + ScreenInfo->centerX) << 16),
                       self->drawPos.y + ((ScreenInfo->position.y + ScreenInfo->centerY) << 16));
}

void UIDialog_Unknown7(void)
{
    int32 offsets[] = { 0, 0, 0x80, 0x70 };

    RSDK_THIS(UIDialog);
    int32 offset = offsets[self->id] << 16;
    int32 x      = self->position.x - 0x240000 + self->drawPos.x - ((offset * maxVal(self->id - 1, 0)) >> 1);
    int32 y      = self->position.y + 0x2C0000 + self->drawPos.y;

    for (int32 i = 0; i < UIDialog_OptionCount; ++i) {
        if (!self->entPtrs[i])
            break;
        EntityUIButton *button = self->entPtrs[i];
        button->posUnknown2.x  = x;
        button->position.x     = x;
        button->posUnknown2.y  = y;
        button->position.y     = y;
        x += offset;
    }
}

void UIDialog_Close(void)
{
    RSDK_THIS(UIDialog);
    EntityUIControl *control = self->parent;

    if (control) {
        UIControl_Unknown6(control);
        destroyEntity(control);
    }

    for (int32 i = 0; i < UIDialog_OptionCount; ++i) {
        if (self->entPtrs[i])
            destroyEntity(self->entPtrs[i]);
    }

    EntityUIControl *storedControl = UIDialog->controlStore;
    if (storedControl) {
        UIControl_Unknown5(UIDialog->controlStore);
        storedControl->state          = UIDialog->controlStateStore;
        storedControl->dialogHasFocus = false;
    }
    UIDialog->controlStore      = NULL;
    UIDialog->controlStateStore = StateMachine_None;
    UIDialog->activeDialog      = NULL;
    StateMachine_Run(self->curCallback);
    destroyEntity(self);
}

bool32 UIDialog_HandleAutoClose(void)
{
    EntityUIDialog *entity   = UIDialog->activeDialog;
    EntityUIControl *control = entity->parent;

    for (int32 i = 0; i < control->buttonCount; ++i) {
        int32 frame = entity->buttonFrames[i];
        if (frame == 1 || frame == 3) {
            if (entity->flags[i]) {
                if (entity->state != UIDialog_Unknown13) {
                    entity->parent->selectionDisabled = true;
                    entity->curCallback                 = entity->callbacks[i];
                    entity->timer                    = 0;
                    entity->state                       = UIDialog_Unknown13;
                }
            }
            else if (entity->callbacks[i]) {
                entity->callbacks[i]();
            }
            return true;
        }
    }

    return false;
}

void UIDialog_Unknown10(void)
{
    EntityUIDialog *entity = UIDialog->activeDialog;
    if (entity->parent) {
        int32 id = entity->parent->activeEntityID;
        if (id >= 0 && id < entity->parent->buttonCount) {
            if (entity->flags[id]) {
                UIDialog_Unknown4(entity, entity->callbacks[id]);
            }
            else {
                StateMachine_Run(entity->callbacks[id]);
            }
        }
    }
}

void UIDialog_Unknown11(void)
{
    RSDK_THIS(UIDialog);
    Vector2 pos;

    pos.x = 0;
    pos.y = 0;
    if (self->timer == 1) {
        RSDK.PlaySfx(UIWidgets->sfxWoosh, false, 255);
        UIControl_Unknown12((Entity *)self->parent);
    }

    if (self->timer >= 8) {
        if (self->timer >= 16) {
            if (self->timer >= 26) {
                self->parent->selectionDisabled = false;
                self->timer                    = 0;
                self->state                       = UIDialog_Unknown12;
            }
            else {
                self->field_70.x = ScreenInfo->width << 16;
                self->field_70.y = 0x900000;
                MathHelpers_Lerp3(&pos, maxVal(((self->timer - 16) << 8) / 10, 0), -0x400000 - (ScreenInfo->width << 16), 0, 0, 0);
                self->drawPos.x = pos.x;
                self->drawPos.y = pos.y;
                if (self->timer - 16 == 1 && self->playEventSfx)
                    RSDK.PlaySfx(UIWidgets->sfxEvent, false, 255);
                ++self->timer;
            }
        }
        else {
            self->drawPos.x = -0x400000 - (ScreenInfo->width << 16);
            self->drawPos.y = 0;
            MathHelpers_Lerp1(&pos, maxVal(((self->timer - 8) << 8) / 8, 0), ScreenInfo->width << 16, 0x10000, ScreenInfo->width << 16,
                                 0x900000);
            self->field_70 = pos;
            self->timer++;
        }
    }
    else {
        self->drawPos.x = -0x400000 - (ScreenInfo->width << 16);
        self->drawPos.y = 0;
        MathHelpers_Lerp3(&pos, maxVal((self->timer << 8) / 8, 0), 0, 0x10000, ScreenInfo->width << 16, 0x10000);
        ++self->timer;
        self->field_70.x = pos.x;
        self->field_70.y = pos.y;
    }
}

void UIDialog_Unknown12(void)
{
    RSDK_THIS(UIDialog);

    self->field_70.x = ScreenInfo->width << 16;
    self->field_70.y = 0x900000;
    self->drawPos.x  = 0;
    self->drawPos.y  = 0;
}

void UIDialog_Unknown13(void)
{
    RSDK_THIS(UIDialog);
    Vector2 pos;

    if (self->timer >= 8) {
        if (self->timer >= 16) {
            UIDialog_Close();
        }
        else {
            self->drawPos.x = (ScreenInfo->width + 64) << 16;
            self->drawPos.y = 0;
            MathHelpers_Lerp3(&pos, maxVal(((self->timer - 8) << 8) / 8, 0), ScreenInfo->width << 16, 0x900000, ScreenInfo->width << 16, 0);
            ++self->timer;
            self->field_70.x = pos.x;
            self->field_70.y = pos.y;
        }
    }
    else {
        self->field_70.x = ScreenInfo->width << 16;
        self->field_70.y = 0x900000;
        MathHelpers_Lerp2(&pos, maxVal((self->timer << 8) / 8, 0), 0, 0, (ScreenInfo->width + 64) << 16, 0);
        ++self->timer;
        self->drawPos.x = pos.x;
        self->drawPos.y = pos.y;
    }
}


EntityUIDialog *UIDialog_CreateDialogOk(TextInfo *text, void (*callback)(void), bool32 flag)
{
    EntityUIDialog *dialog = UIDialog_CreateActiveDialog(text);
    if (dialog) {
        UIDialog_AddButton(DIALOG_OK, dialog, callback, flag);
        UIDialog_Setup(dialog);
    }
    return dialog;
}
EntityUIDialog *UIDialog_CreateDialogYesNo(TextInfo *text, void (*callbackYes)(void), void (*callbackNo)(void), bool32 flagYes, bool32 flagNo)
{
    EntityUIDialog *dialog = UIDialog_CreateActiveDialog(text);
    if (dialog) {
        UIDialog_AddButton(DIALOG_NO, dialog, callbackNo, flagNo);
        UIDialog_AddButton(DIALOG_YES, dialog, callbackYes, flagYes);
        UIDialog_Setup(dialog);
    }
    return dialog;
}
EntityUIDialog *UIDialog_CreateDialogOkCancel(TextInfo *text, void (*callbackOk)(void), void (*callbackCancel)(void), bool32 flagOk,
                                              bool32 flagCancel)
{
    EntityUIDialog *dialog = UIDialog_CreateActiveDialog(text);
    if (dialog) {
        UIDialog_AddButton(DIALOG_OK, dialog, callbackOk, flagOk);
        UIDialog_AddButton(DIALOG_CANCEL, dialog, callbackCancel, flagCancel);
        UIDialog_Setup(dialog);
    }
    return dialog;
}

#if RETRO_INCLUDE_EDITOR
void UIDialog_EditorDraw(void) {}

void UIDialog_EditorLoad(void) {}
#endif

void UIDialog_Serialize(void) {}
