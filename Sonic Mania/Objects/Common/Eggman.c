#include "SonicMania.h"

ObjectEggman *Eggman;

void Eggman_Update(void)
{
    RSDK_THIS(Eggman);
    StateMachine_Run(self->state);
}

void Eggman_LateUpdate(void) {}

void Eggman_StaticUpdate(void) {}

void Eggman_Draw(void)
{
    RSDK_THIS(Eggman);
    if (self->parent) {
        self->position.x = self->parent->position.x + self->offset.x;
        self->position.y = self->parent->position.y + self->offset.y;
    }
    RSDK.DrawSprite(&self->animator, NULL, false);
}

void Eggman_Create(void *data)
{
    RSDK_THIS(Eggman);
    if (!SceneInfo->inEditor) {
        if (globals->gameMode < MODE_TIMEATTACK) {
            RSDK.SetSpriteAnimation(Eggman->aniFrames, 0, &self->animator, true, 0);
            self->active        = ACTIVE_NORMAL;
            self->visible       = true;
            self->drawFX        = FX_FLIP;
            self->drawOrder     = Zone->drawOrderLow;
            self->parent        = (Entity *)data;
            self->updateRange.x = 0x400000;
            self->updateRange.y = 0x800000;
            self->hitbox.left   = -10;
            self->hitbox.top    = -24;
            self->hitbox.right  = 10;
            self->hitbox.bottom = 27;
        }
        else {
            destroyEntity(self);
        }
    }
}

void Eggman_StageLoad(void)
{
    if (RSDK.CheckStageFolder("GHZCutscene"))
        Eggman->aniFrames = RSDK.LoadSpriteAnimation("Eggman/EggmanGHZCutt.bin", SCOPE_STAGE);
    else if (RSDK.CheckStageFolder("FBZ"))
        Eggman->aniFrames = RSDK.LoadSpriteAnimation("Eggman/EggmanFBZ.bin", SCOPE_STAGE);
    else if (RSDK.CheckStageFolder("TMZ3"))
        Eggman->aniFrames = RSDK.LoadSpriteAnimation("Eggman/EggmanTMZ.bin", SCOPE_STAGE);
    else
        Eggman->aniFrames = RSDK.LoadSpriteAnimation("Eggman/EggmanAll.bin", SCOPE_STAGE);
}

void Eggman_State_ProcessAnimation(void)
{
    RSDK_THIS(Eggman);
    RSDK.ProcessAnimation(&self->animator);
}

void Eggman_State_ProcessThenSet(void)
{
    RSDK_THIS(Eggman);

    RSDK.ProcessAnimation(&self->animator);
    if (self->animator.frameID >= self->animator.frameCount - 1) {
        RSDK.SetSpriteAnimation(Eggman->aniFrames, self->animID, &self->animator, true, 0);
        self->state = Eggman_State_ProcessAnimation;
    }
}

void Eggman_State_ProcessUntilEnd(void)
{
    RSDK_THIS(Eggman);

    if (self->animator.frameID < self->animator.frameCount - 1)
        RSDK.ProcessAnimation(&self->animator);
}

void Eggman_State_ProcessAirThenSet(void)
{
    RSDK_THIS(Eggman);

    RSDK.ProcessAnimation(&self->animator);
    if (self->onGround) {
        RSDK.SetSpriteAnimation(Eggman->aniFrames, self->animID, &self->animator, true, 0);
        self->state = Eggman_State_ProcessAnimation;
    }
    else {
        self->velocity.y += 0x3800;
        self->position.x += self->velocity.x;
        self->position.y += self->velocity.y;
    }
}

void Eggman_State_FallUntilTimerReset(void)
{
    RSDK_THIS(Eggman);
    RSDK.ProcessAnimation(&self->animator);
    if (self->timer <= 0) {
        RSDK.SetSpriteAnimation(Eggman->aniFrames, self->animID, &self->animator, true, 0);
        self->velocity.x = 0;
        self->velocity.y = 0;
        self->state      = self->stateStore;
    }
    else {
        self->position.x += self->velocity.x;
        self->velocity.y += 0x3800;
        self->position.y += self->velocity.y;
        self->timer--;
    }
}

void Eggman_State_FallAndCollide(void)
{
    RSDK_THIS(Eggman);
    RSDK.ProcessAnimation(&self->animator);
    self->velocity.y += 0x3800;
    self->position.y += self->velocity.y;
    if (RSDK.ObjectTileCollision(self, Zone->fgLayers, CMODE_FLOOR, self->collisionPlane, 0, 0x100000, true)) {
        self->onGround = true;
        self->state    = Eggman_State_ProcessAnimation;
    }
}

void Eggman_State_WalkOffScreen(void)
{
    RSDK_THIS(Eggman);
    RSDK.ProcessAnimation(&self->animator);
    self->position.x += self->velocity.x;
    self->velocity.x += 0x200;

    if (!RSDK.CheckOnScreen(self, NULL)) {
        self->active = ACTIVE_NEVER;
        self->state  = Eggman_State_ProcessAnimation;
    }
}

#if RETRO_INCLUDE_EDITOR
void Eggman_EditorDraw(void)
{
    RSDK_THIS(Eggman);
    RSDK.DrawSprite(&self->animator, NULL, false);
}

void Eggman_EditorLoad(void) { Eggman_StageLoad(); }
#endif

void Eggman_Serialize(void) {}
