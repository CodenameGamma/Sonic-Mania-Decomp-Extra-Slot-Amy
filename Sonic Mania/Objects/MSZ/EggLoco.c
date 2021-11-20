#include "SonicMania.h"

ObjectEggLoco *EggLoco;

void EggLoco_Update(void)
{
    RSDK_THIS(EggLoco);
    RSDK.ProcessAnimation(&self->animator1);
    RSDK.ProcessAnimation(&self->animator10);
    RSDK.ProcessAnimation(&self->animator7);
    self->field_60 = (self->field_60 + 16) & 0x1FF;

    StateMachine_Run(self->state);

    if (!(Zone->timer & 0x3F)) {
        RSDK.PlaySfx(EggLoco->sfxSmoke, false, 255);
        CREATE_ENTITY(LocoSmoke, NULL, self->position.x + 0x450000, self->position.y - 0x480000);
    }
}

void EggLoco_LateUpdate(void) {}

void EggLoco_StaticUpdate(void) {}

void EggLoco_Draw(void)
{
    RSDK_THIS(EggLoco);
    Vector2 drawPos;

    drawPos.x = self->position.x;
    drawPos.y = self->position.y - 2 * self->field_64;
    RSDK.DrawSprite(&self->animator2, 0, false);
    RSDK.DrawSprite(&self->animator4, &drawPos, false);

    drawPos.x                  = self->position.x;
    drawPos.x                  = self->position.x - 0x2D0000;
    drawPos.y                  = self->position.y - 0x3F0000;
    self->drawFX             = FX_ROTATE;
    self->rotation           = -(self->field_64 >> 15);
    self->animator12.frameID = 0;
    RSDK.DrawSprite(&self->animator12, &drawPos, false);

    drawPos.x                  = self->position.x - 0x5F0000;
    drawPos.y                  = self->field_64 + self->position.y - 0x3F0000;
    self->drawFX             = FX_NONE;
    self->animator12.frameID = 1;
    RSDK.DrawSprite(&self->animator12, &drawPos, false);

    self->animator12.frameID = 2;
    RSDK.DrawSprite(&self->animator12, &drawPos, false);
    RSDK.DrawSprite(&self->animator2, 0, false);
    RSDK.DrawSprite(&self->animator1, 0, false);
    RSDK.DrawSprite(&self->animator3, 0, false);

    drawPos.x        = self->position.x - 0x680000;
    drawPos.y        = self->position.y + 0x380000;
    self->rotation = self->field_60;
    self->drawFX   = FX_ROTATE;
    RSDK.DrawSprite(&self->animator5, &drawPos, false);

    self->drawFX = FX_NONE;
    RSDK.DrawSprite(&self->animator6, &drawPos, false);

    drawPos.x += 0x4A0000;
    self->drawFX   = FX_ROTATE;
    self->rotation = self->field_60 + 256;
    RSDK.DrawSprite(&self->animator5, &drawPos, false);

    self->drawFX = FX_NONE;
    RSDK.DrawSprite(&self->animator6, &drawPos, false);

    drawPos.x = self->position.x + 0x160000;
    drawPos.y = self->position.y + 0x4B0000;
    RSDK.DrawSprite(&self->animator7, &drawPos, false);

    drawPos.x += 0x3E0000;
    RSDK.DrawSprite(&self->animator7, &drawPos, false);

    drawPos.x                 = (2304 * RSDK.Cos512(self->field_60) + self->position.x - 0x680000) + 0x4C0000;
    drawPos.y                 = ((9 * RSDK.Sin512(self->field_60)) << 8) + self->position.y + 0x380000;
    self->animator8.frameID = 1;
    self->drawFX            = FX_ROTATE;
    self->rotation          = RSDK.Sin512(-self->field_60) / 18 - 4;
    RSDK.DrawSprite(&self->animator8, &drawPos, false);

    self->drawFX = FX_NONE;
    drawPos.x -= 0x4C0000;
    self->animator8.frameID = 0;
    RSDK.DrawSprite(&self->animator8, &drawPos, false);

    self->animator11.frameID = 0;
    drawPos.x                  = self->position.x + ((RSDK.Cos512(self->field_60) + 256) << 11);
    drawPos.y                  = self->position.y;
    RSDK.DrawSprite(&self->animator11, &drawPos, false);

    self->animator11.frameID = 2;
    RSDK.DrawSprite(&self->animator11, &drawPos, false);

    self->animator11.frameID = 1;
    RSDK.DrawSprite(&self->animator11, 0, false);
    RSDK.DrawSprite(&self->animator9, 0, false);
    RSDK.DrawSprite(&self->animator10, 0, false);
}

void EggLoco_Create(void *data)
{
    RSDK_THIS(EggLoco);
    if (!SceneInfo->inEditor) {
        self->visible       = true;
        self->drawOrder     = Zone->drawOrderLow;
        self->active        = ACTIVE_BOUNDS;
        self->updateRange.x = 0xA00000;
        self->updateRange.y = 0x600000;
        self->state         = EggLoco_State_Unknown1;
        self->timer         = 60;
        RSDK.SetSpriteAnimation(EggLoco->aniFrames, 0, &self->animator3, true, 0);
        RSDK.SetSpriteAnimation(EggLoco->aniFrames, 6, &self->animator4, true, 0);
        RSDK.SetSpriteAnimation(EggLoco->aniFrames, 1, &self->animator5, true, 0);
        RSDK.SetSpriteAnimation(EggLoco->aniFrames, 1, &self->animator6, true, 1);
        RSDK.SetSpriteAnimation(EggLoco->aniFrames, 2, &self->animator7, true, 0);
        RSDK.SetSpriteAnimation(EggLoco->aniFrames, 3, &self->animator8, true, 0);
        RSDK.SetSpriteAnimation(EggLoco->aniFrames, 5, &self->animator9, true, 0);
        RSDK.SetSpriteAnimation(EggLoco->aniFrames, 7, &self->animator10, true, 0);
        RSDK.SetSpriteAnimation(EggLoco->aniFrames, 4, &self->animator11, true, 0);
        RSDK.SetSpriteAnimation(EggLoco->aniFrames, 10, &self->animator1, true, 0);
        RSDK.SetSpriteAnimation(EggLoco->aniFrames, 12, &self->animator2, true, 0);
        RSDK.SetSpriteAnimation(EggLoco->aniFrames, 13, &self->animator12, true, 0);
    }
}

void EggLoco_StageLoad(void)
{
    EggLoco->aniFrames  = RSDK.LoadSpriteAnimation("MSZ/Train.bin", SCOPE_STAGE);
    EggLoco->sfxSmoke   = RSDK.GetSFX("MSZ/LocoSmoke.wav");
    EggLoco->sfxWhistle = RSDK.GetSFX("MSZ/LocoWhistle.wav");
    Soundboard_LoadSFX("MSZ/LocoChugga.wav", true, EggLoco_CheckCB, EggLoco_UpdateCB);
}

bool32 EggLoco_CheckCB(void)
{
    int32 count   = 0;
    int32 screenX = (ScreenInfo->position.x + ScreenInfo->centerX) << 16;
    int32 screenY = (ScreenInfo->position.y + ScreenInfo->centerY) << 16;
    foreach_all(EggLoco, eggLoco)
    {
        int32 distX = abs(screenX - eggLoco->position.x);
        int32 distY = abs(screenY - eggLoco->position.y);
        int32 rad   = MathHelpers_SquareRoot((distY >> 16) * (distY >> 16) + (distX >> 16) * (distX >> 16));
        if (rad <= 840)
            count++;
    }
    return count > 0;
}

void EggLoco_UpdateCB(int32 sfx)
{
    int32 screenX = (ScreenInfo->position.x + ScreenInfo->centerX) << 16;
    int32 screenY = (ScreenInfo->position.y + ScreenInfo->centerY) << 16;
    foreach_all(EggLoco, eggLoco)
    {
        int32 distX = abs(screenX - eggLoco->position.x);
        int32 distY = abs(screenY - eggLoco->position.y);
        int32 vol   = minVal(MathHelpers_SquareRoot((distX >> 16) * (distX >> 16) + (distY >> 16) * (distY >> 16)), 840);
        RSDK.SetChannelAttributes(Soundboard->sfxChannel[sfx], 1.0 - (vol / 840.0), 0.0, 1.0);
        foreach_break;
    }
}

void EggLoco_State_Unknown1(void)
{
    RSDK_THIS(EggLoco);

    if (self->timer <= 0) {
        RSDK.SetSpriteAnimation(EggLoco->aniFrames, 11, &self->animator1, true, 0);
        RSDK.SetSpriteAnimation(EggLoco->aniFrames, 12, &self->animator2, true, 0);
        self->state = EggLoco_State_Unknown2;
    }
    else {
        self->timer--;
    }
}

void EggLoco_State_Unknown2(void)
{
    RSDK_THIS(EggLoco);
    if (++self->timer == 4) {
        RSDK.PlaySfx(EggLoco->sfxWhistle, false, 255);
        self->timer = 0;
        self->state = EggLoco_State_Unknown3;
    }
}

void EggLoco_State_Unknown3(void)
{
    RSDK_THIS(EggLoco);

    RSDK.ProcessAnimation(&self->animator2);
    self->field_64 = (RSDK.GetFrameID(&self->animator2) - 97) << 16;
    if (self->field_64 >= 0x10000 && !(Zone->timer & 3)) {
        EntityDust *dust = CREATE_ENTITY(Dust, NULL, self->position.x - 0x240000, self->position.y - 0x4E0000);
        dust->state      = Dust_State_MoveGravity;
        dust->velocity.x = 0x20000;
        dust->velocity.y = -0x18000;
        dust->drawOrder  = Zone->drawOrderLow;
    }
    if (self->animator1.animationID == 11 && self->animator1.frameID == self->animator1.frameCount - 1)
        RSDK.SetSpriteAnimation(EggLoco->aniFrames, 10, &self->animator1, true, 0);

    if (self->animator2.frameID == self->animator2.frameCount - 1) {
        self->timer = RSDK.Rand(60, 180);
        self->state = EggLoco_State_Unknown1;
    }
}

#if RETRO_INCLUDE_EDITOR
void EggLoco_EditorDraw(void) {}

void EggLoco_EditorLoad(void) {}
#endif

void EggLoco_Serialize(void) {}
