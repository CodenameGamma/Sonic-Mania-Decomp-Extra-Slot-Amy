#include "SonicMania.h"

ObjectUIResPicker *UIResPicker;

void UIResPicker_Update(void)
{
    RSDK_THIS(UIResPicker);
    self->touchPosStart.x = self->size.x;
    self->touchPosStart.y = self->size.y;
    self->touchPosEnd.y   = 0;
    self->touchPosStart.x = (self->touchPosStart.x + 3 * self->size.y) >> 1;
    self->touchPosStart.y = self->size.y + 0x60000;
    self->touchPosEnd.x   = -self->touchPosStart.x;
    if (self->selection != self->prevSelection) {
        UIResPicker_GetDisplayInfo(self);
        self->prevSelection = self->selection;
    }

    StateMachine_Run(self->state);

    EntityUIControl *control = (EntityUIControl *)self->parent;
    if (control && control->state == UIButton_Unknown16)
        UIResPicker_Unknown3(self);
    self->visible = true;
}

void UIResPicker_LateUpdate(void) {}

void UIResPicker_StaticUpdate(void) {}

void UIResPicker_Draw(void)
{
    RSDK_THIS(UIResPicker);
    Vector2 drawPos;

    int32 size  = (self->size.y + self->size.x) >> 16;
    drawPos.x = self->position.x;
    drawPos.y = self->position.y;
    drawPos.x -= self->offset;
    drawPos.y -= self->offset;
#if RETRO_USE_PLUS
    UIWidgets_DrawRhombus(self->size.y >> 16, size, self->field_11C, (UIWidgets->buttonColour >> 16) & 0xFF, (UIWidgets->buttonColour >> 8) & 0xFF,
                       (UIWidgets->buttonColour) & 0xFF, drawPos.x, drawPos.y);
#else
    UIWidgets_DrawRhombus(self->size.y >> 16, size, self->field_11C, 0xF0, 0xF0, 0xF0, drawPos.x, drawPos.y);
#endif

    drawPos = self->position;
    drawPos.x += self->offset;
    drawPos.y += self->offset;
    UIWidgets_DrawRhombus((self->size.y >> 16), size, self->field_11C, 0, 0, 0, drawPos.x, drawPos.y);

    if (self->arrowWidth > 0 && self->flag) {
        drawPos = self->position;
        drawPos.y += self->field_120;
        drawPos.x -= self->arrowWidth << 15;
        drawPos.x += self->offset;
        drawPos.y += self->offset;
        RSDK.DrawSprite(&self->animator1, &drawPos, false);

        drawPos = self->position;
        drawPos.y += self->field_120;
        drawPos.x += self->arrowWidth << 15;
        drawPos.x += self->offset;
        drawPos.y += self->offset;
        RSDK.DrawSprite(&self->animator2, &drawPos, false);
    }

    if (self->textFlag) {
        drawPos.x = self->position.x;
        drawPos.y = self->position.y + self->field_120 + self->offset;

        switch (self->align) {
            case 0: drawPos.x = -0x60000 - (self->size.x >> 1) + drawPos.x; break;
            case 2: drawPos.x = drawPos.x + (self->size.x >> 1) - 0x60000; break;
        }

        int32 width = RSDK.GetStringWidth(UIWidgets->labelSpriteIndex, 0, &self->text, 0, self->text.textLength, 0);
        drawPos.x -= width << 15;
        RSDK.DrawText(&self->textAnimator, &drawPos, &self->text, 0, self->text.textLength, ALIGN_LEFT, 0, 0, 0, false);
    }
}

void UIResPicker_Create(void *data)
{
    RSDK_THIS(UIResPicker);
    if (!SceneInfo->inEditor) {
        self->selection     = -2;
        self->visible       = true;
        self->drawOrder     = 2;
        self->active        = ACTIVE_BOUNDS;
        self->updateRange.x = 0x800000;
        self->updateRange.y = 0x400000;
        self->field_11C     = self->size.y >> 16;
        self->size.y        = abs(self->size.y);
        self->textFlag      = true;
        RSDK.SetText(&self->text, "", 0x100);
        self->processButtonCB = UIResPicker_ProcessButtonCB;
        self->touchCB         = UIResPicker_ProcessTouchCB;
        RSDK.SetSpriteAnimation(UIWidgets->uiSpriteIndex, 2, &self->animator1, true, 0);
        RSDK.SetSpriteAnimation(UIWidgets->uiSpriteIndex, 2, &self->animator2, true, 1);
        RSDK.SetSpriteAnimation(UIWidgets->labelSpriteIndex, 0, &self->textAnimator, true, 0);
    }
}

void UIResPicker_StageLoad(void) { UIResPicker->aniFrames = RSDK.LoadSpriteAnimation("UI/SaveSelect.bin", SCOPE_STAGE); }

void UIResPicker_GetDisplayInfo(EntityUIResPicker *entity)
{
    if (sku_platform == PLATFORM_PC || sku_platform == PLATFORM_DEV) {
        TextInfo info;
        RSDK.GetDisplayInfo(&entity->selection, &entity->displayWidth, &entity->displayHeight, &entity->displayRefreshRate, &info);

        char buffer[0x20];
        RSDK.GetCString(buffer, &info);
        if (strcmp(buffer, "DEFAULT") != 0)
            RSDK.CopyString(&entity->text, &info);
        else
            Localization_GetString(&entity->text, STR_DEFAULT);
        RSDK.SetSpriteString(UIWidgets->labelSpriteIndex, 0, &entity->text);
    }
}

void UIResPicker_ApplySettings(void)
{
    RSDK_THIS(UIResPicker);
    RSDK.PlaySfx(UIWidgets->sfxBleep, false, 255);
    self->field_128 = -0x20000;
    self->field_12C = -0x20000;
    self->field_120 = 0;
    self->offset    = -0x20000;
    UIResPicker_GetDisplayInfo(self);

    RSDK.SetSettingsValue(SETTINGS_FSWIDTH, self->displayWidth);
    RSDK.SetSettingsValue(SETTINGS_FSHEIGHT, self->displayHeight);
    if (!self->displayRefreshRate)
        RSDK.SetSettingsValue(SETTINGS_REFRESHRATE, 60);
    else
        RSDK.SetSettingsValue(SETTINGS_REFRESHRATE, self->displayRefreshRate);
#if RETRO_USE_PLUS
    RSDK.SetSettingsValue(SETTINGS_CHANGED, true);
#else
    RSDK.UpdateWindow();
#endif
}

void UIResPicker_ProcessButtonCB(void)
{
    RSDK_THIS(UIResPicker);

    int32 sel = self->selection;
    if (UIControl->keyLeft)
        self->selection--;
    if (UIControl->keyRight)
        ++self->selection;

    if (sel != self->selection)
        UIResPicker_ApplySettings();
}

bool32 UIResPicker_ProcessTouchCB(void)
{
    RSDK_THIS(UIResPicker);

    void (*callbacks[2])(void);
    Vector2 touchStart[2];
    Vector2 touchEnd[2];

    callbacks[0] = UIResPicker_TouchedCB_Left;
    callbacks[1] = UIResPicker_TouchedCB_Right;

    touchStart[0].x = self->touchPosStart.x;
    touchStart[0].y = self->touchPosStart.y;
    touchStart[1].x = self->touchPosStart.x;
    touchStart[1].y = self->touchPosStart.y;

    touchEnd[0].x = self->touchPosEnd.x;
    touchEnd[0].y = self->touchPosEnd.y;
    touchEnd[1].x = -self->touchPosEnd.x;
    touchEnd[1].y = self->touchPosEnd.y;

    bool32 pressed = false;
    for (int32 i = 0; i < 2; ++i) {
        if (TouchInfo->count) {
            int32 sizeX = touchStart[i].x >> 1;
            int32 sizeY = touchStart[i].y >> 1;
            for (int32 t = 0; t < TouchInfo->count; ++t) {
                int32 x = (ScreenInfo->position.x << 16) - ((TouchInfo->x[t] * ScreenInfo->width) * -65536.0f);
                int32 y = (ScreenInfo->position.y << 16) - ((TouchInfo->y[t] * ScreenInfo->height) * -65536.0f);

                int32 touchX = abs(touchEnd[i].x + self->position.x - x);
                int32 touchY = abs(touchEnd[i].y + self->position.y - y);
                if (touchX < sizeX && touchY < sizeY) {
                    self->field_13C = i;
                    pressed           = true;
                }
            }
        }
        else if (self->touchPressed && self->field_13C == i && !self->disabled) {
            callbacks[i]();
        }
    }

    self->touchPressed = pressed;
    return self->touchPressed;
}

void UIResPicker_TouchedCB_Left(void)
{
    RSDK_THIS(UIResPicker);
    self->selection--;
    if (self->selection != self->selection - 1)
        UIResPicker_ApplySettings();
}

void UIResPicker_TouchedCB_Right(void)
{
    RSDK_THIS(UIResPicker);

    self->selection++;
    if (self->selection != self->selection + 1)
        UIResPicker_ApplySettings();
}

void UIResPicker_Unknown3(EntityUIResPicker *entity)
{
    if (entity) {
        entity->field_120 = 0;
        entity->offset    = 0;
        entity->field_128 = 0;
        entity->field_12C = 0;
        entity->flag      = false;
        entity->state     = UIResPicker_Unknown7;
    }
}

void UIResPicker_Unknown7(void)
{
    RSDK_THIS(UIResPicker);

    self->textFlag = true;
    if (self->field_120) {
        int32 dist = -(self->field_120 / abs(self->field_120));
        self->field_120 += dist << 15;
        if (dist < 0) {
            if (self->field_120 < 0) {
                self->field_120 = 0;
            }
            else if (dist > 0 && self->field_120 > 0)
                self->field_120 = 0;
        }
        else if (dist > 0 && self->field_120 > 0)
            self->field_120 = 0;
    }

    if (self->offset) {
        int32 dist = -(self->offset / abs(self->offset));
        self->offset += dist << 16;
        if (dist < 0) {
            if (self->offset < 0) {
                self->offset = 0;
            }
            else if (dist > 0 && self->offset > 0)
                self->offset = 0;
        }
        else if (dist > 0 && self->offset > 0)
            self->offset = 0;
    }
}

void UIResPicker_Unknown8(void)
{
    RSDK_THIS(UIResPicker);

    self->field_128 += 0x4000;
    self->field_120 += self->field_128;
    self->textFlag = true;
    if (self->field_120 >= 0 && self->field_128 > 0) {
        self->field_120 = 0;
        self->field_128 = 0;
    }

    self->field_12C += 0x4800;
    self->offset += self->field_12C;
    if (self->offset >= -0x20000 && self->field_12C > 0) {
        self->offset    = -0x20000;
        self->field_12C = 0;
    }
}

#if RETRO_INCLUDE_EDITOR
void UIResPicker_EditorDraw(void)
{
    RSDK_THIS(UIResPicker);
    self->selection     = -2;
    self->drawOrder     = 2;
    self->updateRange.x = 0x800000;
    self->updateRange.y = 0x400000;
    self->field_11C     = self->size.y >> 16;
    self->size.y        = abs(self->size.y);
    RSDK.SetSpriteAnimation(UIWidgets->uiSpriteIndex, 2, &self->animator1, true, 0);
    RSDK.SetSpriteAnimation(UIWidgets->uiSpriteIndex, 2, &self->animator2, true, 1);
    RSDK.SetSpriteAnimation(UIWidgets->labelSpriteIndex, 0, &self->textAnimator, true, 0);

    UIResPicker_Draw();
}

void UIResPicker_EditorLoad(void) {}
#endif

void UIResPicker_Serialize(void)
{
    RSDK_EDITABLE_VAR(UIResPicker, VAR_BOOL, disabled);
    RSDK_EDITABLE_VAR(UIResPicker, VAR_VECTOR2, size);
    RSDK_EDITABLE_VAR(UIResPicker, VAR_ENUM, align);
    RSDK_EDITABLE_VAR(UIResPicker, VAR_ENUM, arrowWidth);
}
