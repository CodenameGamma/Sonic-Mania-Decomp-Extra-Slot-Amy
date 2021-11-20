#include "SonicMania.h"

ObjectKanabun *Kanabun;

void Kanabun_Update(void)
{
    RSDK_THIS(Kanabun);
    StateMachine_Run(self->state);
}

void Kanabun_LateUpdate(void) {}

void Kanabun_StaticUpdate(void) {}

void Kanabun_Draw(void)
{
    RSDK_THIS(Kanabun);
    RSDK.DrawSprite(&self->animator, NULL, false);
}

void Kanabun_Create(void *data)
{
    RSDK_THIS(Kanabun);

    if (!self->angleVel)
        self->angleVel = 1;
    if (!self->hVel) {
        self->hDist    = 1;
        self->bobDist  = 2;
        self->hVel     = 0x4000;
        self->angleVel = 2;
    }

    self->drawFX = FX_FLIP | FX_ROTATE | FX_SCALE;
    if (!SceneInfo->inEditor) {
        self->visible       = true;
        self->drawOrder     = Zone->drawOrderLow;
        self->startPos      = self->position;
        self->startDir      = self->direction;
        self->active        = ACTIVE_BOUNDS;
        self->updateRange.x = 0x800000;
        self->updateRange.y = 0x800000;
        RSDK.SetSpriteAnimation(Kanabun->aniFrames, 0, &self->animator, true, 0);
        self->state = Kanabun_State_Setup;
    }
}

void Kanabun_StageLoad(void)
{
    if (RSDK.CheckStageFolder("SSZ1"))
        Kanabun->aniFrames = RSDK.LoadSpriteAnimation("SSZ1/Kanabun.bin", SCOPE_STAGE);
    else if (RSDK.CheckStageFolder("SSZ2"))
        Kanabun->aniFrames = RSDK.LoadSpriteAnimation("SSZ2/Kanabun.bin", SCOPE_STAGE);

    Kanabun->hitbox.left   = -6;
    Kanabun->hitbox.top    = -6;
    Kanabun->hitbox.right  = 6;
    Kanabun->hitbox.bottom = 6;
    DEBUGMODE_ADD_OBJ(Kanabun);
}

void Kanabun_DebugSpawn(void)
{
    RSDK_THIS(DebugMode);
    CREATE_ENTITY(Kanabun, NULL, self->position.x, self->position.y);
}

void Kanabun_DebugDraw(void)
{
    RSDK.SetSpriteAnimation(Kanabun->aniFrames, 0, &DebugMode->animator, true, 0);
    RSDK.DrawSprite(&DebugMode->animator, NULL, false);
}

void Kanabun_CheckPlayerCollisions(void)
{
    RSDK_THIS(Kanabun);

    foreach_active(Player, player)
    {
        if (Player_CheckBadnikTouch(player, self, &Kanabun->hitbox))
            Player_CheckBadnikBreak(self, player, true);
    }
}

void Kanabun_CheckOnScreen(void)
{
    RSDK_THIS(Kanabun);
    if (!RSDK.CheckOnScreen(self, NULL) && !RSDK.CheckPosOnScreen(&self->startPos, &self->updateRange)) {
        self->position.x = self->startPos.x;
        self->position.y = self->startPos.y;
        self->direction  = self->startDir;
        Kanabun_Create(NULL);
    }
}

void Kanabun_HandleMovement(void)
{
    RSDK_THIS(Kanabun);

    self->position.x += self->velocity.x;
    self->position.y = ((self->bobDist * RSDK.Sin512(self->angle)) << 8) + self->startPos.y;
    self->angle += self->angleVel;

    self->scale.x = (abs(RSDK.Sin512(((self->angle >> 1) + 0x80) & 0x1FF)) >> 1) + 0x100;
    self->scale.y = self->scale.x;
    if (((uint32)(self->angle - 0x80) & 0x1FF) >= 0x100)
        self->drawOrder = Zone->drawOrderHigh;
    else
        self->drawOrder = Zone->drawOrderLow;
}

void Kanabun_State_Setup(void)
{
    RSDK_THIS(Kanabun);

    self->velocity.x = self->hVel;
    if (self->direction == FLIP_NONE)
        self->velocity.x = -self->velocity.x;
    self->active = ACTIVE_NORMAL;

    self->state = Kanabun_State_Unknown1;
    Kanabun_State_Unknown1();
}

void Kanabun_State_Unknown1(void)
{
    RSDK_THIS(Kanabun);

    Kanabun_HandleMovement();
    if (self->position.y >= self->startPos.y) {
        if (self->groundVel == 1)
            self->groundVel = 0;
    }
    else if (!self->groundVel) {
        self->groundVel = 1;
    }

    int vel = self->hVel * self->hDist * (0x100 / self->angleVel);
    if ((self->direction == FLIP_NONE && self->position.x <= (self->startPos.x - vel))
        || (self->direction == FLIP_X && self->position.x >= (vel + self->startPos.x))) {
        RSDK.SetSpriteAnimation(Kanabun->aniFrames, 1, &self->animator, true, 0);
        self->state = Kanabun_State_Unknown2;
    }
    RSDK.ProcessAnimation(&self->animator);
    if (self->drawOrder == Zone->drawOrderHigh)
        Kanabun_CheckPlayerCollisions();
    Kanabun_CheckOnScreen();
}

void Kanabun_State_Unknown2(void)
{
    RSDK_THIS(Kanabun);
    Kanabun_HandleMovement();
    RSDK.ProcessAnimation(&self->animator);

    if (self->animator.frameID == self->animator.frameCount - 1) {
        RSDK.SetSpriteAnimation(Kanabun->aniFrames, 0, &self->animator, true, 0);
        self->velocity.x = -self->velocity.x;
        self->groundVel  = 0;
        self->direction ^= FLIP_X;
        self->state = Kanabun_State_Unknown1;
    }
    else {
        if (self->drawOrder == Zone->drawOrderHigh)
            Kanabun_CheckPlayerCollisions();
        Kanabun_CheckOnScreen();
    }
}

#if RETRO_INCLUDE_EDITOR
void Kanabun_EditorDraw(void)
{
    RSDK_THIS(Kanabun);
    RSDK.SetSpriteAnimation(Kanabun->aniFrames, 0, &self->animator, false, 0);
    self->scale.x = 0x200;
    self->scale.y = 0x200;

    Kanabun_Draw();
}

void Kanabun_EditorLoad(void)
{
    if (RSDK.CheckStageFolder("SSZ1"))
        Kanabun->aniFrames = RSDK.LoadSpriteAnimation("SSZ1/Kanabun.bin", SCOPE_STAGE);
    else if (RSDK.CheckStageFolder("SSZ2"))
        Kanabun->aniFrames = RSDK.LoadSpriteAnimation("SSZ2/Kanabun.bin", SCOPE_STAGE);
}
#endif

void Kanabun_Serialize(void)
{
    RSDK_EDITABLE_VAR(Kanabun, VAR_ENUM, hVel);
    RSDK_EDITABLE_VAR(Kanabun, VAR_UINT8, hDist);
    RSDK_EDITABLE_VAR(Kanabun, VAR_UINT8, bobDist);
    RSDK_EDITABLE_VAR(Kanabun, VAR_UINT8, angleVel);
    RSDK_EDITABLE_VAR(Kanabun, VAR_UINT8, direction);
}
