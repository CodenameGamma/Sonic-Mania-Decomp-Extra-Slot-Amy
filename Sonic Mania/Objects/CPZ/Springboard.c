// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: Springboard Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectSpringboard *Springboard;

void Springboard_Update(void)
{
    RSDK_THIS(Springboard);
    RSDK.ProcessAnimation(&self->animator);

    //bounceDelay is unused, but if it was used, it'd prolly be "if (!--self->bounceDelay) {" around this foreach loop. source: it was exactly like that in S2 '13
    foreach_active(Player, playerPtr)
    {
        if (playerPtr->velocity.y >= 0 && ((1 << RSDK.GetEntityID(playerPtr)) & self->playerBits)) {
            int32 val  = minVal((playerPtr->position.x - self->position.x + 0x1C0000) >> 17, 28);
            int32 val2 = maxVal(val, 0);

            int32 val3 = val2;
            if ((self->direction & 1))
                val3 = 28 - val2;
            if (val3 >= 8) {
                if (self->animator.frameID == 3)
                    RSDK.SetSpriteAnimation(Springboard->aniFrames, 0, &self->animator, true, 0);
                if (self->animator.frameID == 2) {
                    int32 anim = playerPtr->animator.animationID;
                    if (anim == ANI_WALK || (anim > ANI_AIRWALK && anim <= ANI_DASH))
                        playerPtr->storedAnim = playerPtr->animator.animationID;
                    else
                        playerPtr->storedAnim = ANI_WALK;
                    playerPtr->state          = Player_State_Air;
                    playerPtr->onGround       = false;
                    playerPtr->tileCollisions = true;
                    RSDK.SetSpriteAnimation(playerPtr->aniFrames, ANI_SPRINGCS, &playerPtr->animator, true, 1);
                    playerPtr->groundVel   = playerPtr->velocity.x;
                    int32 pos                = minVal(2 * val3 - 16, 39);
                    playerPtr->velocity.y  = Springboard->springPower[pos] - playerPtr->gravityStrength - self->force;
                    playerPtr->jumpAbility = 0;
                    RSDK.PlaySfx(Springboard->sfxSpring, false, 255);
                }
            }
        }
    }

    foreach_active(Player, player)
    {
        int32 playerID   = RSDK.GetEntityID(player);
        int32 playerVel  = player->groundVel;
        int32 playerVelX = player->velocity.x;
        bool32 bounced    = false;
        int32 val        = clampVal((player->position.x - self->position.x + 0x1C0000) >> 17, 0, 27);

        if (!self->direction) {
            int32 hitboxTop = 0;

            if (self->animator.frameID <= 2) {
                hitboxTop = Springboard->heightsFlat[val];
            }
            else if (self->animator.frameID == 3) {
                hitboxTop = Springboard->heightsReady[val];
            }

            Hitbox hitbox;
            hitbox.left   = -28;
            hitbox.top    = -hitboxTop;
            hitbox.right  = 28;
            hitbox.bottom = 8;

            uint8 collision = 0;
            if (!((1 << playerID) & self->playerBits))
                bounced = collision = Player_CheckCollisionBox(player, self, &hitbox);
            else
                bounced = collision = Player_CheckCollisionPlatform(player, self, &hitbox);

            bounced = collision == C_TOP;
            switch (collision) {
                case 2: player->groundVel = playerVel; player->velocity.x = playerVelX;
                case 0:
                case 3:
                case 4:
                    if (player->velocity.y >= 0 && ((1 << playerID) & self->playerBits)) {
                        Hitbox *playerHitbox = Player_GetHitbox(player);
                        player->position.y   = self->position.y - (playerHitbox->bottom << 16) - (hitboxTop << 16);
                        if (!bounced)
                            bounced = player->position.x > self->position.x;
                    }
                    break;
                case 1:
#if RETRO_USE_PLUS 
                    if (player->state == Player_State_MightyHammerDrop)
                        player->state = Player_State_Air;
#endif
                    break;
                default: break;
            }
            player->flailing = false;
        }
        else if (self->direction == FLIP_X) {
            int32 pos       = abs(val - 27);
            int32 hitboxTop = 0;

            if (self->animator.frameID <= 2) {
                hitboxTop = Springboard->heightsFlat[pos];
            }
            else if (self->animator.frameID == 3) {
                hitboxTop = Springboard->heightsReady[pos];
            }

            Hitbox hitbox;
            hitbox.left   = -28;
            hitbox.top    = -hitboxTop;
            hitbox.right  = 28;
            hitbox.bottom = 8;

            uint8 collision = 0;
            if (!((1 << playerID) & self->playerBits))
                collision = Player_CheckCollisionBox(player, self, &hitbox);
            else
                 collision = Player_CheckCollisionPlatform(player, self, &hitbox);
            bounced = collision == C_TOP;
            switch (collision) {
                case 0:
                case 2:
                case 4: break;
                case 1:
#if RETRO_USE_PLUS 
                    if (player->state == Player_State_MightyHammerDrop) {
                        player->state = Player_State_Air;
                    }
#endif
                    break;
                case 3:
                    player->groundVel  = playerVel;
                    player->velocity.x = playerVelX;
                    break;
                default: break;
            }

            if (player->velocity.y >= 0 && ((1 << playerID) & self->playerBits)) {
                Hitbox *playerHitbox = Player_GetHitbox(player);
                player->position.y   = self->position.y - (playerHitbox->bottom << 16) - (hitboxTop << 16);
                if (!bounced)
                    bounced = player->position.x < self->position.x;
            }
            player->flailing = false;
        }

        if (bounced) {
            self->playerBits |= (1 << playerID);
            if (val >= 8 && !self->bounceDelay)
                self->bounceDelay = 6;
        }
        else {
            self->playerBits &= ~(1 << playerID);
        }
    }
}

void Springboard_LateUpdate(void) {}

void Springboard_StaticUpdate(void) {}

void Springboard_Draw(void)
{
    RSDK_THIS(Springboard);
    RSDK.DrawSprite(&self->animator, NULL, false);
}

void Springboard_Create(void *data)
{
    RSDK_THIS(Springboard);
    self->visible   = true;
    self->drawOrder = Zone->drawOrderLow;
    self->drawFX |= FX_FLIP;
    self->active        = ACTIVE_BOUNDS;
    self->updateRange.x = 0x400000;
    self->updateRange.y = 0x400000;
    if (!SceneInfo->inEditor) {
        self->force = (self->force + 8) << 15;
        RSDK.SetSpriteAnimation(Springboard->aniFrames, 0, &self->animator, true, 3);
    }
}

void Springboard_StageLoad(void)
{
    Springboard->aniFrames = RSDK.LoadSpriteAnimation("CPZ/Springboard.bin", SCOPE_STAGE);
    Springboard->sfxSpring = RSDK.GetSfx("Global/Spring.wav");
    DEBUGMODE_ADD_OBJ(Springboard);
}

void Springboard_DebugSpawn(void)
{
    RSDK_THIS(DebugMode);
    CREATE_ENTITY(Springboard, NULL, self->position.x, self->position.y);
}
void Springboard_DebugDraw(void)
{
    RSDK.SetSpriteAnimation(Springboard->aniFrames, 1, &DebugMode->animator, true, 0);
    RSDK.DrawSprite(&DebugMode->animator, NULL, false);
}

#if RETRO_INCLUDE_EDITOR
void Springboard_EditorDraw(void)
{
    RSDK_THIS(Springboard);
    RSDK.SetSpriteAnimation(Springboard->aniFrames, 0, &self->animator, false, 3);

    Springboard_Draw();
}

void Springboard_EditorLoad(void)
{
    Springboard->aniFrames = RSDK.LoadSpriteAnimation("CPZ/Springboard.bin", SCOPE_STAGE);

    RSDK_ACTIVE_VAR(Springboard, direction);
    RSDK_ENUM_VAR("No Flip", FLIP_NONE);
    RSDK_ENUM_VAR("Flip X", FLIP_X);
}
#endif

void Springboard_Serialize(void)
{
    RSDK_EDITABLE_VAR(Springboard, VAR_UINT8, direction);
    RSDK_EDITABLE_VAR(Springboard, VAR_ENUM, force);
}
