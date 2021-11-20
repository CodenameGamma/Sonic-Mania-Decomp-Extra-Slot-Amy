#include "SonicMania.h"

ObjectWisp *Wisp = NULL;

void Wisp_Update(void)
{
    RSDK_THIS(Wisp);
    StateMachine_Run(self->state);
}

void Wisp_LateUpdate(void) {}

void Wisp_StaticUpdate(void) {}

void Wisp_Draw(void)
{
    RSDK_THIS(Wisp);
    RSDK.DrawSprite(&self->bodyAnimator, NULL, false);

    self->inkEffect = INK_ALPHA;
    RSDK.DrawSprite(&self->wingAnimator, NULL, false);

    self->inkEffect = INK_NONE;
}

void Wisp_Create(void *data)
{
    RSDK_THIS(Wisp);
    self->visible   = true;
    self->drawOrder = Zone->drawOrderLow;
    self->drawFX |= FX_FLIP;
    self->startPos      = self->position;
    self->active        = ACTIVE_BOUNDS;
    self->updateRange.x = 0x800000;
    self->updateRange.y = 0x800000;
    self->direction     = FLIP_NONE;
    self->targetPtr     = NULL;
    self->alpha         = 0xC0;
    self->timer         = 16;
    self->buzzCount        = 4;
    RSDK.SetSpriteAnimation(Wisp->aniFrames, 0, &self->bodyAnimator, true, 0);
    RSDK.SetSpriteAnimation(Wisp->aniFrames, 1, &self->wingAnimator, true, 0);
    self->state = Wisp_State_Setup;
}

void Wisp_StageLoad(void)
{
    Wisp->aniFrames     = RSDK.LoadSpriteAnimation("Blueprint/Wisp.bin", SCOPE_STAGE);
    Wisp->hitbox.left   = -8;
    Wisp->hitbox.top    = -8;
    Wisp->hitbox.right  = 8;
    Wisp->hitbox.bottom = 8;
    DEBUGMODE_ADD_OBJ(Wisp);
}

void Wisp_DebugSpawn(void)
{
    RSDK_THIS(DebugMode);

    CREATE_ENTITY(Wisp, NULL, self->position.x, self->position.y);
}

void Wisp_DebugDraw(void)
{
    RSDK.SetSpriteAnimation(Wisp->aniFrames, 0, &DebugMode->animator, true, 0);
    RSDK.DrawSprite(&DebugMode->animator, NULL, false);
}

void Wisp_HandlePlayerInteractions(void)
{
    RSDK_THIS(Wisp);
    foreach_active(Player, player)
    {
        EntityPlayer *target = self->targetPtr;
        if (target) {
            if (abs(player->position.x - self->position.x) < abs(target->position.x - self->position.x))
                self->targetPtr = player;
        }
        else {
            self->targetPtr = player;
        }
        if (Player_CheckBadnikTouch(player, self, &Wisp->hitbox))
            Player_CheckBadnikBreak(self, player, true);
    }
}

void Wisp_CheckOnScreen(void)
{
    RSDK_THIS(Wisp);
    if (!RSDK.CheckOnScreen(self, NULL) && !RSDK.CheckPosOnScreen(&self->startPos, &self->updateRange)) {
        self->position.x = self->startPos.x;
        self->position.y = self->startPos.y;
        Wisp_Create(NULL);
    }
}

void Wisp_State_Setup(void)
{
    RSDK_THIS(Wisp);
    self->active     = ACTIVE_NORMAL;
    self->velocity.x = 0;
    self->velocity.y = 0;
    self->state      = Wisp_WaitInPlace;
    Wisp_WaitInPlace();
}

void Wisp_WaitInPlace(void)
{
    RSDK_THIS(Wisp);

    if (!--self->timer) {
        if (--self->buzzCount) {
            self->velocity.y = -0x10000;
            self->timer      = 96;
            self->state      = Wisp_FlyTowardTarget;
        }
        else {
            self->velocity.x = -0x20000;
            self->velocity.y = -0x20000;
            self->state      = Wisp_BasicFly;
        }
    }
    RSDK.ProcessAnimation(&self->wingAnimator);
    Wisp_HandlePlayerInteractions();
    Wisp_CheckOnScreen();
}

void Wisp_FlyTowardTarget(void)
{
    RSDK_THIS(Wisp);

    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;

    EntityPlayer *target = self->targetPtr;
    if (target) {
        if (target->position.x >= self->position.x) {
            self->velocity.x += 0x1000;
            self->direction = FLIP_X;
            if (self->velocity.x > 0x20000)
                self->velocity.x = 0x20000;
        }
        else {
            self->velocity.x -= 0x1000;
            self->direction = FLIP_NONE;
            if (self->velocity.x < -0x20000)
                self->velocity.x = -0x20000;
        }

        if (target->position.y >= self->position.y) {
            self->velocity.y += 0x1000;
            if (self->velocity.y > 0x20000)
                self->velocity.y = 0x20000;
        }
        else {
            self->velocity.y -= 0x1000;
            if (self->velocity.y < -0x20000)
                self->velocity.y = -0x20000;
        }
    }

    self->timer--;
    if (!self->timer) {
        self->timer      = RSDK.Rand(0, 32);
        self->state      = Wisp_WaitInPlace;
        self->velocity.x = 0;
        self->velocity.y = 0;
        self->direction  = FLIP_NONE;
    }
    RSDK.ProcessAnimation(&self->wingAnimator);
    Wisp_HandlePlayerInteractions();
    Wisp_CheckOnScreen();
}

void Wisp_BasicFly(void)
{
    RSDK_THIS(Wisp);
    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;
    RSDK.ProcessAnimation(&self->wingAnimator);
    Wisp_HandlePlayerInteractions();
    Wisp_CheckOnScreen();
}

#if RETRO_INCLUDE_EDITOR
void Wisp_EditorDraw(void) {}

void Wisp_EditorLoad(void) {}
#endif

void Wisp_Serialize(void) { RSDK_EDITABLE_VAR(Wisp, VAR_UINT8, direction); }
