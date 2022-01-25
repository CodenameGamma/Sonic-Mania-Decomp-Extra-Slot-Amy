// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: LRZConvControl Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectLRZConvControl *LRZConvControl;

void LRZConvControl_Update(void)
{
    RSDK_THIS(LRZConvControl);

    switch (self->triggerMode) {
        case LRZCONVCTRL_TRIGGER_PLAYER: LRZConvControl_HandlePlayerTrigger(); break;
        case LRZCONVCTRL_TRIGGER_BUTTON: LRZConvControl_HandleButtonTrigger(); break;
    }

    self->visible = DebugMode->debugActive;
}

void LRZConvControl_LateUpdate(void) {}

void LRZConvControl_StaticUpdate(void) {}

void LRZConvControl_Draw(void)
{
    RSDK_THIS(LRZConvControl);

    RSDK.SetSpriteAnimation(LRZConvControl->aniFrames, 0, &self->animator, true, 0);
    RSDK.DrawSprite(&self->animator, NULL, false);

    if (self->triggerMode == LRZCONVCTRL_TRIGGER_PLAYER) {
        self->hitbox.left   = -self->hitboxSize.x >> 17;
        self->hitbox.top    = -self->hitboxSize.y >> 17;
        self->hitbox.right  = self->hitboxSize.x >> 17;
        self->hitbox.bottom = self->hitboxSize.y >> 17;
        DrawHelpers_DrawHitboxOutline(0xFFFFFF, self->direction, self->position.x, self->position.y, &self->hitbox);
    }
}

void LRZConvControl_Create(void *data)
{
    RSDK_THIS(LRZConvControl);

    self->active        = ACTIVE_BOUNDS;
    self->drawOrder     = Zone->drawOrderLow;
    self->visible       = true;
    self->drawFX        = FX_FLIP;
    self->updateRange.x = 0x800000;
    self->updateRange.y = 0x800000;

    switch (self->triggerMode) {
        case LRZCONVCTRL_TRIGGER_PLAYER:
            self->hitbox.left   = -self->hitboxSize.x >> 17;
            self->hitbox.top    = -self->hitboxSize.y >> 17;
            self->hitbox.right  = self->hitboxSize.x >> 17;
            self->hitbox.bottom = self->hitboxSize.y >> 17;
            self->updateRange.x += self->hitboxSize.x;
            self->updateRange.y += self->hitboxSize.y;
            break;
        case LRZCONVCTRL_TRIGGER_BUTTON: self->taggedButton = LRZ2Setup_HandleTagSetup(self->buttonTag, (Entity *)self); break;
    }
}

void LRZConvControl_StageLoad(void) { LRZConvControl->aniFrames = RSDK.LoadSpriteAnimation("LRZ2/LRZConvControl.bin", SCOPE_STAGE); }

void LRZConvControl_HandlePlayerTrigger(void)
{
    RSDK_THIS(LRZConvControl);

    bool32 flag = false;
    foreach_active(Player, player)
    {
        int playerID    = RSDK.GetEntityID(player);
        bool32 collided = Player_CheckCollisionTouch(player, self, &self->hitbox);

        if ((1 << playerID) & self->activePlayers) {
            if (!collided)
                self->activePlayers &= ~(1 << playerID);
        }
        else {
            if (collided) {
                if (!player->sidekick)
                    flag = true;
                self->activePlayers |= (1 << playerID);
            }
        }
    }

    if (flag) {
        switch (self->behavior) {
            case LRZCONVCTRL_BEHAVIOR_ONOFF_SET: LRZ2Setup->conveyorOff = self->flipVal; break;
            case LRZCONVCTRL_BEHAVIOR_ONOFF_SWAP: LRZ2Setup->conveyorOff = !LRZ2Setup->conveyorOff; break;
            case LRZCONVCTRL_BEHAVIOR_CHANGEDIR_SET: LRZ2Setup->conveyorDir = self->flipVal; break;
            case LRZCONVCTRL_BEHAVIOR_CHANGEDIR_SWAP: LRZ2Setup->conveyorDir = !LRZ2Setup->conveyorDir; break;
            default: break;
        }
    }
}

void LRZConvControl_HandleButtonTrigger(void)
{
    RSDK_THIS(LRZConvControl);

    if (self->buttonTag) {
        EntityButton *button = self->taggedButton;
        if (button) {
            switch (self->behavior) {
                case LRZCONVCTRL_BEHAVIOR_ONOFF_SET:
                    if (button->down)
                        LRZ2Setup->conveyorOff = self->flipVal;
                    break;
                case LRZCONVCTRL_BEHAVIOR_ONOFF_SWAP:
                    if (self->field_75 != button->toggled)
                        LRZ2Setup->conveyorOff = !LRZ2Setup->conveyorOff;
                    self->field_75 = button->toggled;
                    break;
                case LRZCONVCTRL_BEHAVIOR_CHANGEDIR_SET:
                    if (button->down)
                        LRZ2Setup->conveyorDir = self->flipVal;
                    break;
                case LRZCONVCTRL_BEHAVIOR_CHANGEDIR_SWAP:
                    if (button->toggled != self->field_75)
                        LRZ2Setup->conveyorDir = !LRZ2Setup->conveyorDir;
                    self->field_75 = button->toggled;
                    break;
                default: break;
            }
        }
    }
}

#if RETRO_INCLUDE_EDITOR
void LRZConvControl_EditorDraw(void) { LRZConvControl_Draw(); }

void LRZConvControl_EditorLoad(void)
{
    LRZConvControl->aniFrames = RSDK.LoadSpriteAnimation("LRZ2/LRZConvControl.bin", SCOPE_STAGE);

    RSDK_ACTIVE_VAR(LRZConvControl, triggerMode);
    RSDK_ENUM_VAR("Player", LRZCONVCTRL_TRIGGER_PLAYER);
    RSDK_ENUM_VAR("Button", LRZCONVCTRL_TRIGGER_BUTTON);

    RSDK_ACTIVE_VAR(LRZConvControl, behavior);
    RSDK_ENUM_VAR("On/Off (Set)", LRZCONVCTRL_BEHAVIOR_ONOFF_SET);
    RSDK_ENUM_VAR("On/Off (Swap)", LRZCONVCTRL_BEHAVIOR_ONOFF_SWAP);
    RSDK_ENUM_VAR("Change Dir (Set)", LRZCONVCTRL_BEHAVIOR_CHANGEDIR_SET);
    RSDK_ENUM_VAR("Change Dir (Swap)", LRZCONVCTRL_BEHAVIOR_CHANGEDIR_SWAP);
}
#endif

void LRZConvControl_Serialize(void)
{
    RSDK_EDITABLE_VAR(LRZConvControl, VAR_UINT8, triggerMode);
    RSDK_EDITABLE_VAR(LRZConvControl, VAR_ENUM, buttonTag);
    RSDK_EDITABLE_VAR(LRZConvControl, VAR_VECTOR2, hitboxSize);
    RSDK_EDITABLE_VAR(LRZConvControl, VAR_UINT8, behavior);
    RSDK_EDITABLE_VAR(LRZConvControl, VAR_UINT8, flipVal);
}
