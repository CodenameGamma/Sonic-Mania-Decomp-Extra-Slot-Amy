#include "SonicMania.h"

ObjectIwamodoki *Iwamodoki;

void Iwamodoki_Update(void)
{
    RSDK_THIS(Iwamodoki);

    if (LRZConvItem && self->lrzConvPhys) {
        self->preMovePos.x = self->position.x;
        self->preMovePos.y = self->position.y;
        self->moveOffset   = LRZConvItem_Unknown2(self);
    }
    else {
        self->preMovePos.x = self->position.x;
        self->preMovePos.y = self->position.y;
        self->moveOffset.x = 0;
        self->moveOffset.y = 0;
    }
    self->postMovePos.x = self->position.x;
    self->postMovePos.y = self->position.y;

    StateMachine_Run(self->state);
}

void Iwamodoki_LateUpdate(void) {}

void Iwamodoki_StaticUpdate(void) {}

void Iwamodoki_Draw(void)
{
    RSDK_THIS(Iwamodoki);

    RSDK.DrawSprite(&self->animator, NULL, false);
}

void Iwamodoki_Create(void *data)
{
    RSDK_THIS(Iwamodoki);

    self->drawFX |= FX_FLIP;
    self->startPos.x    = self->position.x;
    self->startPos.y    = self->position.y;
    self->startDir      = self->direction;
    self->visible       = true;
    self->active        = ACTIVE_BOUNDS;
    self->updateRange.x = 0x800000;
    self->updateRange.y = 0x800000;
    self->drawOrder     = Zone->drawOrderLow;

    if (data) {
        self->active = ACTIVE_NORMAL;
        RSDK.SetSpriteAnimation(Iwamodoki->aniFrames, voidToInt(data) + 2, &self->animator, true, 0);
        self->state = Iwamodoki_State_Debris;
    }
    else {
        RSDK.SetSpriteAnimation(Iwamodoki->aniFrames, 0, &self->animator, true, 0);
        self->state = Iwamodoki_State_Setup;
    }
}

void Iwamodoki_StageLoad(void)
{
    if (RSDK.CheckStageFolder("LRZ1"))
        Iwamodoki->aniFrames = RSDK.LoadSpriteAnimation("LRZ1/Iwamodoki.bin", SCOPE_STAGE);
    else if (RSDK.CheckStageFolder("LRZ2") || RSDK.CheckStageFolder("LRZ3"))
        Iwamodoki->aniFrames = RSDK.LoadSpriteAnimation("LRZ2/Iwamodoki.bin", SCOPE_STAGE);

    Iwamodoki->hitbox1.left   = -12;
    Iwamodoki->hitbox1.top    = -11;
    Iwamodoki->hitbox1.right  = 12;
    Iwamodoki->hitbox1.bottom = 11;

    Iwamodoki->hitbox2.left   = -64;
    Iwamodoki->hitbox2.top    = -64;
    Iwamodoki->hitbox2.right  = 64;
    Iwamodoki->hitbox2.bottom = 64;

    Iwamodoki->hitbox3.left   = -4;
    Iwamodoki->hitbox3.top    = -4;
    Iwamodoki->hitbox3.right  = 4;
    Iwamodoki->hitbox3.bottom = 4;

    DEBUGMODE_ADD_OBJ(Iwamodoki);
}

void Iwamodoki_DebugSpawn(void)
{
    RSDK_THIS(DebugMode);

    CREATE_ENTITY(Iwamodoki, NULL, self->position.x, self->position.y);
}

void Iwamodoki_DebugDraw(void)
{
    RSDK.SetSpriteAnimation(Iwamodoki->aniFrames, 2, &DebugMode->animator, true, 0);
    RSDK.DrawSprite(&DebugMode->animator, NULL, false);
}

void Iwamodoki_HandlePlayerCollisions(void)
{
    RSDK_THIS(Iwamodoki);

    self->position.x = self->preMovePos.x;
    self->position.y = self->preMovePos.y;

    foreach_active(Player, player)
    {
        if (Player_CheckCollisionBox(player, self, &Iwamodoki->hitbox1) == C_TOP) {
            player->position.x += self->moveOffset.x;
            player->position.y += self->moveOffset.y;
        }
    }

    self->position.x = self->postMovePos.x;
    self->position.y = self->postMovePos.y;
}

void Iwamodoki_CheckOnScreen(void)
{
    RSDK_THIS(Iwamodoki);
    if (!RSDK.CheckOnScreen(self, NULL) && !RSDK.CheckPosOnScreen(&self->startPos, &self->updateRange)) {
        self->position.x = self->startPos.x;
        self->position.y = self->startPos.y;
        self->direction  = self->startDir;
        Iwamodoki_Create(NULL);
    }
}

void Iwamodoki_State_Setup(void)
{
    RSDK_THIS(Iwamodoki);

    self->active     = ACTIVE_NORMAL;
    self->velocity.x = -0x10000;
    self->state      = Iwamodoki_State_Unknown1;
    Iwamodoki_State_Unknown1();
}

void Iwamodoki_State_Unknown1(void)
{
    RSDK_THIS(Iwamodoki);

    foreach_active(Player, player)
    {
        if (Player_CheckCollisionTouch(player, self, &Iwamodoki->hitbox2)) {
            self->state = Iwamodoki_State_Unknown2;
        }
    }

    Iwamodoki_HandlePlayerCollisions();
    Iwamodoki_CheckOnScreen();
}

void Iwamodoki_State_Unknown2(void)
{
    RSDK_THIS(Iwamodoki);

    RSDK.ProcessAnimation(&self->animator);
    if (self->animator.frameID == 6) {
        RSDK.SetSpriteAnimation(Iwamodoki->aniFrames, 1, &self->animator, true, 0);
        self->timer2 = 15;
        self->timer1 = 15;
        self->state  = Iwamodoki_State_Unknown3;
    }
    Iwamodoki_HandlePlayerCollisions();
    Iwamodoki_CheckOnScreen();
}

void Iwamodoki_State_Unknown3(void)
{
    RSDK_THIS(Iwamodoki);

    RSDK.ProcessAnimation(&self->animator);

    if (!--self->timer1) {
        if (!--self->timer2) {
            self->timer1 = 32;
            RSDK.SetSpriteAnimation(Iwamodoki->aniFrames, 2, &self->animator, true, 0);
            self->state = Iwamodoki_State_Unknown4;
        }
        else {
            RSDK.SetSpriteAnimation(Iwamodoki->aniFrames, 1, &self->animator, true, 0);
            self->timer1 = self->timer2;
        }
    }
    Iwamodoki_HandlePlayerCollisions();
    Iwamodoki_CheckOnScreen();
}

void Iwamodoki_State_Unknown4(void)
{
    RSDK_THIS(Iwamodoki);

    RSDK.ProcessAnimation(&self->animator);
    if (!--self->timer1) {
        CREATE_ENTITY(Explosion, intToVoid(EXPLOSION_ENEMY), self->position.x, self->position.y)->drawOrder = Zone->drawOrderHigh;
        if (self->activeScreens == 1)
            RSDK.PlaySfx(Explosion->sfxDestroy, false, 255);

        EntityIwamodoki *debris = CREATE_ENTITY(Iwamodoki, intToVoid(1), self->position.x, self->position.y);
        debris->velocity.x      = -0x20000;
        debris->velocity.y      = -0x40000;

        debris             = CREATE_ENTITY(Iwamodoki, intToVoid(1), self->position.x, self->position.y);
        debris->velocity.x = 0x20000;
        debris->velocity.y = -0x40000;

        debris             = CREATE_ENTITY(Iwamodoki, intToVoid(2), self->position.x, self->position.y);
        debris->velocity.x = -0x40000;
        debris->velocity.y = -0x20000;

        debris             = CREATE_ENTITY(Iwamodoki, intToVoid(2), self->position.x, self->position.y);
        debris->velocity.x = 0x40000;
        debris->velocity.y = -0x20000;

        destroyEntity(self);
    }
    else {
        Iwamodoki_HandlePlayerCollisions();
        Iwamodoki_CheckOnScreen();
    }
}

void Iwamodoki_State_Debris(void)
{
    RSDK_THIS(Iwamodoki);

    RSDK.ProcessAnimation(&self->animator);
    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;
    self->velocity.y += 0x3800;

    foreach_active(Player, player)
    {
        if (Player_CheckCollisionTouch(player, self, &Iwamodoki->hitbox3)) {
            Player_CheckProjectileHit(player, self);
        }
    }

    if (!RSDK.CheckOnScreen(self, NULL))
        destroyEntity(self);
}

#if RETRO_INCLUDE_EDITOR
void Iwamodoki_EditorDraw(void) { Iwamodoki_Draw(); }

void Iwamodoki_EditorLoad(void)
{
    if (RSDK.CheckStageFolder("LRZ1"))
        Iwamodoki->aniFrames = RSDK.LoadSpriteAnimation("LRZ1/Iwamodoki.bin", SCOPE_STAGE);
    else if (RSDK.CheckStageFolder("LRZ2") || RSDK.CheckStageFolder("LRZ3"))
        Iwamodoki->aniFrames = RSDK.LoadSpriteAnimation("LRZ2/Iwamodoki.bin", SCOPE_STAGE);

    RSDK_ACTIVE_VAR(Iwamodoki, direction);
    RSDK_ENUM_VAR("No Flip", FLIP_NONE);
    RSDK_ENUM_VAR("Flip X", FLIP_X);
    RSDK_ENUM_VAR("Flip Y", FLIP_Y);
    RSDK_ENUM_VAR("Flip XY", FLIP_XY);
}
#endif

void Iwamodoki_Serialize(void)
{
    RSDK_EDITABLE_VAR(Iwamodoki, VAR_UINT8, direction);
    RSDK_EDITABLE_VAR(Iwamodoki, VAR_BOOL, lrzConvPhys);
}
