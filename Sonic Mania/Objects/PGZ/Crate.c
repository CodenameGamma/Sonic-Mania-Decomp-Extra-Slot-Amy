#include "SonicMania.h"

ObjectCrate *Crate;

void Crate_Update(void)
{
    RSDK_THIS(Crate);
    if (self->collision != 1 || !Crate_Collide()) {
        if (self->animator.animationID) {
            StateMachine_Run(self->state);
        }
        else {
            Platform_Update();
        }
    }
}

void Crate_LateUpdate(void) {}

void Crate_StaticUpdate(void) {}

void Crate_Draw(void)
{
    RSDK_THIS(Crate);
    RSDK.DrawSprite(&self->animator, &self->drawPos, 0);
}

void Crate_Create(void *data)
{
    RSDK_THIS(Crate);

    int32 frameID = self->frameID;
    if (frameID == 3)
        self->collision = PLATFORM_C_SOLID_NONE;
    else
        self->collision = PLATFORM_C_SOLID_ALL;
    Platform_Create(NULL);
    self->frameID = frameID;

    if (!SceneInfo->inEditor) {
        self->hitbox.left   = -24;
        self->hitbox.top    = -24;
        self->hitbox.right  = 24;
        self->hitbox.bottom = 24;
        self->active        = ACTIVE_XBOUNDS;
        self->drawFX        = FX_SCALE | FX_FLIP;
        self->scale.x       = 0x200;
        self->scale.y       = 0x200;
        if (frameID == 3)
            self->drawOrder = Zone->drawOrderLow;
        else
            self->drawOrder = Zone->drawOrderHigh;
        RSDK.SetSpriteAnimation(Crate->aniFrames, 0, &self->animator, true, frameID);
        self->state = Crate_Null1;
    }
}

void Crate_StageLoad(void)
{
    if (RSDK.CheckStageFolder("PSZ1"))
        Crate->aniFrames = RSDK.LoadSpriteAnimation("PSZ1/Crate.bin", SCOPE_STAGE);
    Crate->sfxExplosion2 = RSDK.GetSFX("Stage/Explosion2.wav");
}

void Crate_Break(EntityCrate *entity)
{
    for (int32 i = 0; i < 64; ++i) {
        // the ice is used to create a shattering effect
        EntityIce *ice  = CREATE_ENTITY(Ice, intToVoid(3), (RSDK.Rand(-24, 25) << 16) + entity->position.x,
                                                        (RSDK.Rand(-24, 25) << 16) + entity->position.y);
        ice->velocity.x = RSDK.Rand(-6, 8) << 15;
        ice->velocity.y = RSDK.Rand(-10, 2) << 15;
        ice->direction                = RSDK.Rand(0, 4);
        ice->animator1.animationSpeed = RSDK.Rand(1, 4);
        ice->drawOrder = Zone->drawOrderLow + 1;
        switch (entity->animator.frameID) {
            case 0:
            case 3: RSDK.SetSpriteAnimation(Crate->aniFrames, 1, &ice->animator1, true, 0); break;
            case 1:
            case 2:
                if (RSDK.Rand(0, 6) >= 2)
                    RSDK.SetSpriteAnimation(Crate->aniFrames, 2, &ice->animator1, true, 0);
                else
                    RSDK.SetSpriteAnimation(Crate->aniFrames, 3, &ice->animator1, true, 0);
                break;
            default: break;
        }
    }
    RSDK.PlaySfx(Crate->sfxExplosion2, 0, 255);
    entity->position.y -= 0x10000;

    foreach_active(Crate, crate)
    {
        if (crate != entity && crate->state == Crate_Null1
            && RSDK.CheckObjectCollisionTouchBox(entity, &entity->hitbox, crate, &crate->hitbox)) {
            crate->state = Crate_ApplyGravity;
        }
    }
    destroyEntity(entity);
}
void Crate_MoveY(EntityCrate *self, int32 offset)
{
    self->drawPos.y += offset;
    self->centerPos.x = self->drawPos.x;
    self->centerPos.y = self->drawPos.y;
    self->position.x  = self->drawPos.x;
    self->position.y  = self->drawPos.y;
    int32 start           = (self->drawPos.y - 0x300000) & 0xFFFF0000;
    foreach_active(Crate, crate)
    {
        if (crate != self && RSDK.CheckObjectCollisionBox(self, &self->hitbox, crate, &crate->hitbox, true) == C_TOP)
            Crate_MoveY(crate, start - crate->drawPos.y);
    }
}
bool32 Crate_Collide(void)
{
    RSDK_THIS(Crate);

    foreach_active(Player, player)
    {
        int32 storeX           = player->position.x;
        int32 storeY           = player->position.y;
        int32 storeXVel        = player->velocity.x;
        int32 storeYVel        = player->velocity.y;
        int32 storeVel         = player->groundVel;
        int32 storeGrounded    = player->onGround;
        Hitbox *playerHitbox = Player_GetHitbox(player);
        switch (RSDK.CheckObjectCollisionBox(self, &self->hitbox, player, playerHitbox, false)) {
            default:
            case 0:
                player->velocity.x = storeXVel;
                player->velocity.y = storeYVel;
                player->position.x = storeX;
                player->position.y = storeY;
                Player_CheckCollisionBox(player, self, &self->hitbox);
                break;
            case 1:
                if (self->frameID != 1) {
                    player->velocity.x = storeXVel;
                    player->velocity.y = storeYVel;
                    player->position.x = storeX;
                    player->position.y = storeY;
                    Player_CheckCollisionBox(player, self, &self->hitbox);
                    break;
                }
#if RETRO_USE_PLUS
                else if (player->state == Player_State_MightyHammerDrop) {
                    player->velocity.y -= 0x10000;
                    Crate_Break(self);
                    player->velocity.x = storeXVel;
                    player->velocity.y = storeYVel;
                    player->position.x = storeX;
                    player->position.y = storeY;
                    player->groundVel  = storeVel;
                    player->onGround   = storeGrounded;
                    foreach_return true;
                }
#endif
                else if (player->shield == SHIELD_BUBBLE && player->invincibleTimer <= 0) {
                    if (RSDK_GET_ENTITY(Player->playerCount + RSDK.GetEntityID(player), Shield)->animator.animationID == 8
                        && player->velocity.y >= 0x80000) {
                        Crate_Break(self);
                        player->velocity.x = storeXVel;
                        player->velocity.y = storeYVel;
                        player->position.x = storeX;
                        player->position.y = storeY;
                        player->groundVel  = storeVel;
                        player->onGround   = storeGrounded;
                        foreach_return true;
                    }
                }
                player->velocity.x = storeXVel;
                player->velocity.y = storeYVel;
                player->position.x = storeX;
                player->position.y = storeY;
                Player_CheckCollisionBox(player, self, &self->hitbox);
                break;
            case 2:
            case 3:
                if (self->frameID != 1 || player->shield != SHIELD_FIRE || player->invincibleTimer > 0
                    || RSDK_GET_ENTITY(Player->playerCount + RSDK.GetEntityID(player), Shield)->animator.animationID != 2) {
                    player->velocity.x = storeXVel;
                    player->velocity.y = storeYVel;
                    player->position.x = storeX;
                    player->position.y = storeY;
                    Player_CheckCollisionBox(player, self, &self->hitbox);
                }
                else if (player->position.x < self->position.x) {
                    if (player->velocity.x >= 0x78000) {
                        Crate_Break(self);
                        player->velocity.x = storeXVel;
                        player->velocity.y = storeYVel;
                        player->position.x = storeX;
                        player->position.y = storeY;
                        player->groundVel  = storeVel;
                        player->onGround   = storeGrounded;
                        foreach_return true;
                    }
                    else {
                        player->velocity.x = storeXVel;
                        player->velocity.y = storeYVel;
                        player->position.x = storeX;
                        player->position.y = storeY;
                        Player_CheckCollisionBox(player, self, &self->hitbox);
                    }
                }
                else {
                    if (player->velocity.x <= -0x78000) {
                        Crate_Break(self);
                        player->velocity.x = storeXVel;
                        player->velocity.y = storeYVel;
                        player->position.x = storeX;
                        player->position.y = storeY;
                        player->groundVel  = storeVel;
                        player->onGround   = storeGrounded;
                        foreach_return true;
                    }
                    else {
                        player->velocity.x = storeXVel;
                        player->velocity.y = storeYVel;
                        player->position.x = storeX;
                        player->position.y = storeY;
                        Player_CheckCollisionBox(player, self, &self->hitbox);
                    }
                }
                break;
            case 4:
                if (self->collisionOffset.y >= 0)
                    player->collisionFlagV |= 2;
                player->velocity.x = storeXVel;
                player->velocity.y = storeYVel;
                player->position.x = storeX;
                player->position.y = storeY;
                Player_CheckCollisionBox(player, self, &self->hitbox);
                break;
        }
    }
    return false;
}
void Crate_Null1(void) {
    //hehe
}
void Crate_ApplyGravity(void)
{
    RSDK_THIS(Crate);
    self->position.y -= 0x10000;
    foreach_active(Crate, crate)
    {
        if (crate != self && crate->state == Crate_Null1
            && RSDK.CheckObjectCollisionTouchBox(self, &self->hitbox, crate, &crate->hitbox)) {
            crate->state = Crate_ApplyGravity;
        }
    }
    self->position.y += 0x10000;
    self->timer = 15;
    self->state         = Crate_WaitToFall;
}
void Crate_WaitToFall(void)
{
    RSDK_THIS(Crate);
    if (--self->timer <= 0)
        self->state = Crate_Fall;
}
void Crate_Fall(void)
{
    RSDK_THIS(Crate);
    self->drawPos.y += self->velocity.y;
    int32 x = self->position.x;
    int32 y = self->position.y;
    self->velocity.y += 0x3800;
    self->position.x = self->drawPos.x;
    self->position.y = self->drawPos.y;
    if (RSDK.ObjectTileCollision(self, Zone->fgLayers, 0, CMODE_FLOOR, 0, 0x180000, true)) {
        self->velocity.y = 0;
        self->state      = Crate_Null1;
    }
    else {
        foreach_active(Crate, crate)
        {
            if (crate != self && !crate->velocity.y && RSDK.CheckObjectCollisionBox(crate, &crate->hitbox, self, &self->hitbox, true) == C_TOP) {
                self->velocity.y = 0;
                self->state      = Crate_Null1;
            }
        }
    }
    self->drawPos.x   = self->position.x;
    self->drawPos.y   = self->position.y;
    self->centerPos.x = self->position.x;
    self->centerPos.y = self->position.y;
    self->position.x  = x;
    self->position.y  = y;
}

void Crate_EditorDraw(void) {
    RSDK_THIS(Crate);
    RSDK.SetSpriteAnimation(Crate->aniFrames, 0, &self->animator, true, self->frameID);
    self->drawPos = self->position;
    Crate_Draw();
}

void Crate_EditorLoad(void) { Crate_StageLoad(); }

void Crate_Serialize(void)
{
    RSDK_EDITABLE_VAR(Crate, VAR_UINT8, frameID);
    RSDK_EDITABLE_VAR(Crate, VAR_ENUM, childCount);
    RSDK_EDITABLE_VAR(Crate, VAR_BOOL, ignoreItemBox);
}
