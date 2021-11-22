#include "SonicMania.h"

ObjectGrabber *Grabber;

void Grabber_Update(void)
{
    RSDK_THIS(Grabber);
    StateMachine_Run(self->state);
}

void Grabber_LateUpdate(void) {}

void Grabber_StaticUpdate(void) {}

void Grabber_Draw(void)
{
    RSDK_THIS(Grabber);
    if (SceneInfo->currentDrawGroup == self->drawOrder) {
        RSDK.DrawLine(self->position.x, self->startPos.y - 0x100000, self->position.x, self->position.y, 0x202020, 0, INK_NONE, false);
        RSDK.DrawLine(self->position.x - 0x10000, self->startPos.y - 0x100000, self->position.x - 0x10000, self->position.y, 0xE0E0E0, 0,
                      INK_NONE, false);

        Vector2 drawPos;
        int32 dir           = self->direction;
        drawPos.x         = self->position.x;
        drawPos.y         = self->startPos.y;
        self->direction = 0;
        RSDK.DrawSprite(&self->animator3, &drawPos, false);
        self->direction = dir;
        RSDK.DrawSprite(&self->animator1, NULL, false);
        if (!SceneInfo->currentScreenID)
            RSDK.AddDrawListRef(Zone->drawOrderHigh, SceneInfo->entitySlot);
    }
    else {
        RSDK.DrawSprite(&self->animator2, NULL, false);
    }
}

void Grabber_Create(void *data)
{
    RSDK_THIS(Grabber);
    self->visible   = true;
    self->drawOrder = Zone->drawOrderLow;
    self->startPos  = self->position;
    self->startDir  = self->direction;
    self->active    = ACTIVE_BOUNDS;
    if (!self->direction)
        self->velocity.x = -0x4000;
    else
        self->velocity.x = 0x4000;
    self->updateRange.x = 0x800000;
    self->updateRange.y = 0x800000;
    self->drawFX        = FX_FLIP;
    RSDK.SetSpriteAnimation(Grabber->aniFrames, 0, &self->animator1, true, 0);
    RSDK.SetSpriteAnimation(Grabber->aniFrames, 1, &self->animator2, true, 0);
    RSDK.SetSpriteAnimation(Grabber->aniFrames, 4, &self->animator3, true, 0);
    self->state = Grabber_State_Setup;
}

void Grabber_StageLoad(void)
{
    Grabber->aniFrames          = RSDK.LoadSpriteAnimation("CPZ/Grabber.bin", SCOPE_STAGE);
    Grabber->hitbox1.left       = -8;
    Grabber->hitbox1.top        = -8;
    Grabber->hitbox1.right      = 8;
    Grabber->hitbox1.bottom     = 8;
    Grabber->hitboxRange.left   = -64;
    Grabber->hitboxRange.top    = 0;
    Grabber->hitboxRange.right  = 64;
    Grabber->hitboxRange.bottom = 128;
    Grabber->hitbox2.left       = -8;
    Grabber->hitbox2.top        = 0;
    Grabber->hitbox2.right      = 8;
    Grabber->hitbox2.bottom     = 16;
    DEBUGMODE_ADD_OBJ(Grabber);
    Grabber->sfxGrab    = RSDK.GetSfx("Global/Grab.wav");
    Grabber->sfxDestroy = RSDK.GetSfx("Global/Destroy.wav");
}

void Grabber_DebugDraw(void)
{
    RSDK.SetSpriteAnimation(Grabber->aniFrames, 0, &DebugMode->animator, true, 0);
    RSDK.DrawSprite(&DebugMode->animator, NULL, false);
}

void Grabber_DebugSpawn(void)
{
    RSDK_THIS(Grabber);
    EntityGrabber *grabber = CREATE_ENTITY(Grabber, NULL, self->position.x, self->position.y);
    grabber->direction     = self->direction;
    grabber->startDir      = self->direction;

    if (!self->direction)
        grabber->velocity.x = -0x4000;
    else
        grabber->velocity.x = 0x4000;
}

void Grabber_CheckPlayerCollisions(void)
{
    RSDK_THIS(Grabber);
    foreach_active(Player, player)
    {
        if (player != (EntityPlayer *)self->grabbedPlayer) {
            if (self->state == Grabber_State_Unknown3) {
                if (Player_CheckCollisionTouch(player, self, &Grabber->hitbox2)) {
                    RSDK.PlaySfx(Grabber->sfxGrab, 0, 255);
                    self->state           = Grabber_State_Unknown7;
                    self->isPermanent     = true;
                    self->grabbedPlayer   = (Entity*)player;
                    player->velocity.x      = 0;
                    player->velocity.y      = 0;
                    player->groundVel       = 0;
                    player->state           = Player_State_None;
                    player->nextAirState    = StateMachine_None;
                    player->nextGroundState = StateMachine_None;
                    player->onGround        = false;
                    player->direction       = self->direction;
                    RSDK.SetSpriteAnimation(player->aniFrames, ANI_FAN, &player->animator, true, 0);
                    player->animator.animationSpeed = 0;
                    player->direction                     = self->direction ^ 1;
                }
            }
            if (player != (EntityPlayer*)self->grabbedPlayer && !self->timer && Player_CheckBadnikTouch(player, self, &Grabber->hitbox1)
                && Player_CheckBadnikBreak(self, player, false)) {
                EntityPlayer *player = (EntityPlayer *)self->grabbedPlayer;
                if (player)
                    player->state = Player_State_Air;
                destroyEntity(self);
            }
        }
    }
}

void Grabber_CheckOnScreen(void)
{
    RSDK_THIS(Grabber);
    if (!RSDK.CheckOnScreen(self, NULL) && !RSDK.CheckPosOnScreen(&self->startPos, &self->updateRange)) {
        self->position      = self->startPos;
        self->field_A8      = 0;
        self->struggleFlags = 0;
        self->struggleFlags = 0;
        self->direction     = self->startDir;
        self->grabbedPlayer = NULL;
        self->field_BC      = 0;
        self->explodeTimer  = 0;
        Grabber_Create(NULL);
    }
}

void Grabber_HandleExplode(void)
{
    RSDK_THIS(Grabber);
    if (!--self->field_BC) {
        self->animator1.frameID ^= 1;
        self->field_BC = self->explodeTimer;
        self->explodeTimer--;
        if (!self->explodeTimer) {
            EntityPlayer *player = (EntityPlayer *)self->grabbedPlayer;
            if (player && player->state == Player_State_None) {
                Player_CheckHit(player, self);
                if (player->state != Player_State_Hit && Player_CheckValidState(player))
                    player->state = Player_State_Air;
                self->grabbedPlayer = NULL;
            }
            CREATE_ENTITY(Explosion, intToVoid(EXPLOSION_ENEMY), self->position.x, self->position.y)->drawOrder = Zone->drawOrderHigh;
            RSDK.PlaySfx(Explosion->sfxDestroy, false, 255);
            destroyEntity(self);
        }
    }
}

void Grabber_State_Setup(void)
{
    RSDK_THIS(Grabber);
    self->active = ACTIVE_NORMAL;
    self->state  = Grabber_State_Unknown5;
    Grabber_State_Unknown5();
}

void Grabber_State_Unknown5(void)
{
    RSDK_THIS(Grabber);
    RSDK.ProcessAnimation(&self->animator3);
    if (++self->field_BC >= 256) {
        self->field_BC = 0;
        RSDK.SetSpriteAnimation(Grabber->aniFrames, 3, &self->animator1, true, 0);
        RSDK.SetSpriteAnimation(0xFFFF, 0, &self->animator2, true, 0);
        self->state = Grabber_State_Unknown6;
    }
    self->position.x += self->velocity.x;

    foreach_active(Player, player)
    {
        if (Player_CheckCollisionTouch(player, self, &Grabber->hitboxRange)) {
            self->explodeTimer = 16;
            self->state        = Grabber_State_Unknown2;
        }
    }

    Grabber_CheckPlayerCollisions();
    Grabber_CheckOnScreen();
}

void Grabber_State_Unknown2(void)
{
    RSDK_THIS(Grabber);
    if (--self->explodeTimer < 0) {
        self->explodeTimer = 32;
        self->state        = Grabber_State_Unknown3;
    }
    Grabber_CheckPlayerCollisions();
    Grabber_CheckOnScreen();
}

void Grabber_State_Unknown6(void)
{
    RSDK_THIS(Grabber);

    RSDK.ProcessAnimation(&self->animator1);
    if (self->animator1.frameID == self->animator1.frameCount - 1) {
        RSDK.SetSpriteAnimation(Grabber->aniFrames, 0, &self->animator1, true, 0);
        RSDK.SetSpriteAnimation(Grabber->aniFrames, 1, &self->animator2, true, 0);
        if (self->direction == FLIP_X)
            self->position.x -= 0x10000;
        else
            self->position.x += 0x10000;

        self->state      = Grabber_State_Unknown5;
        self->velocity.x = -self->velocity.x;
        self->direction ^= 1;
        Grabber_State_Unknown5();
    }
    else {
        Grabber_CheckPlayerCollisions();
        Grabber_CheckOnScreen();
    }
}

void Grabber_State_Unknown3(void)
{
    RSDK_THIS(Grabber);
    if (--self->explodeTimer < 0)
        self->state = Grabber_State_Unknown4;
    self->position.y += 0x20000;
    Grabber_CheckPlayerCollisions();
    Grabber_CheckOnScreen();
}

void Grabber_State_Unknown4(void)
{
    RSDK_THIS(Grabber);
    if (++self->explodeTimer >= 32) {
        self->explodeTimer = 0;
        self->state        = Grabber_State_Unknown5;
    }
    self->position.y -= 0x20000;
    Grabber_CheckPlayerCollisions();
    Grabber_CheckOnScreen();
}

void Grabber_State_Unknown7(void)
{
    RSDK_THIS(Grabber);

    RSDK.ProcessAnimation(&self->animator1);
    RSDK.ProcessAnimation(&self->animator2);
    if (++self->explodeTimer < 32) {
        self->position.y -= 0x20000;
        EntityPlayer *player = (EntityPlayer *)self->grabbedPlayer;
        if (player) {
            player->animator.animationSpeed = 0;
            player->position.x                    = self->position.x;
            player->position.y                    = self->position.y;
            player->position.y += 0x100000;
            player->velocity.x = 0;
            player->direction  = self->direction ^ 1;
            player->velocity.y = 0;
        }
        Grabber_CheckPlayerCollisions();
    }
    else if (self->animator1.frameID != self->animator1.frameCount - 1) {
        Grabber_CheckPlayerCollisions();
    }
    else {
        self->field_BC     = 1;
        self->explodeTimer = 16;
        RSDK.SetSpriteAnimation(Grabber->aniFrames, 2, &self->animator1, true, 0);
        self->state = Grabber_State_Struggle;
        Grabber_CheckPlayerCollisions();
    }

    Grabber_CheckOnScreen();
}

void Grabber_State_Struggle(void)
{
    RSDK_THIS(Grabber);
    EntityPlayer *player = (EntityPlayer *)self->grabbedPlayer;
    if (player) {
        player->animator.animationSpeed = 0;
        if (self->struggleFlags) {
            player->velocity.x = 0;
            player->velocity.y = 0;
            if (self->field_A8) {
                uint8 flags = 0;
                self->field_A8--;
                if (player->left)
                    flags = 1;
                if (player->right)
                    flags |= 2;
                if (flags) {
                    if (flags != 3 && flags != self->struggleFlags) {
                        self->struggleFlags = flags;
                        if (++self->struggleTimer >= 4) {
                            player->state         = Player_State_Air;
                            self->grabbedPlayer = 0;
                            self->timer         = 16;
                            self->state         = Grabber_State_Unknown9;
                        }
                    }
                }
            }
            else {
                self->struggleTimer = 0;
                self->struggleFlags = 0;
            }
        }
        else {
            if (player->left) {
                self->struggleFlags = 1;
                self->field_A8      = 64;
            }
            else if (player->right) {
                self->struggleFlags = 2;
                self->field_A8      = 64;
            }
        }
        player->position.x = self->position.x;
        player->position.y = self->position.y;
        player->position.y += 0x100000;
        player->direction = self->direction ^ 1;
    }

    Grabber_CheckPlayerCollisions();
    Grabber_HandleExplode();
    Grabber_CheckOnScreen();
}

void Grabber_State_Unknown9(void)
{
    RSDK_THIS(Grabber);
    if (self->timer)
        self->timer--;
    Grabber_CheckPlayerCollisions();
    Grabber_HandleExplode();
    Grabber_CheckOnScreen();
}

#if RETRO_INCLUDE_EDITOR
void Grabber_EditorDraw(void)
{
    RSDK_THIS(Grabber);

    RSDK.DrawLine(self->position.x, self->startPos.y - 0x100000, self->position.x, self->position.y, 0x202020, 0, INK_NONE, false);
    RSDK.DrawLine(self->position.x - 0x10000, self->startPos.y - 0x100000, self->position.x - 0x10000, self->position.y, 0xE0E0E0, 0,
                  INK_NONE, false);

    Vector2 drawPos;
    int32 dir         = self->direction;
    drawPos.x         = self->position.x;
    drawPos.y         = self->startPos.y;
    self->direction = FLIP_NONE;
    RSDK.DrawSprite(&self->animator3, &drawPos, false);
    self->direction = dir;
    RSDK.DrawSprite(&self->animator1, NULL, false);
    RSDK.DrawSprite(&self->animator2, NULL, false);
}

void Grabber_EditorLoad(void) { Grabber->aniFrames = RSDK.LoadSpriteAnimation("CPZ/Grabber.bin", SCOPE_STAGE); }
#endif

void Grabber_Serialize(void) { RSDK_EDITABLE_VAR(Grabber, VAR_UINT8, direction); }
