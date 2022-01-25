// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: SizeLaser Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectSizeLaser *SizeLaser;

void SizeLaser_Update(void)
{
    RSDK_THIS(SizeLaser);
    self->animator1.frameID = (Zone->timer >> 2) & 1;
    self->animator2.frameID = (Zone->timer >> 2) & 1;
    StateMachine_Run(self->state);
}

void SizeLaser_LateUpdate(void) {}

void SizeLaser_StaticUpdate(void)
{
    foreach_active(Player, player)
    {
        int32 playerID = RSDK.GetEntityID(player);

        SizeLaser->playerPositions[playerID].x = player->position.x;
        SizeLaser->playerPositions[playerID].y = player->position.y;

        if (player->scale.x > 0x200) {
            Hitbox *playerHitbox = RSDK.GetHitbox(&player->animator, 0);
            if (!playerHitbox)
                playerHitbox = &defaultHitbox;

            SizeLaser->playerOuterBox[playerID].left   = (player->scale.x * playerHitbox->left) >> 9;
            SizeLaser->playerOuterBox[playerID].top    = (player->scale.x * playerHitbox->top) >> 9;
            SizeLaser->playerOuterBox[playerID].right  = (player->scale.x * playerHitbox->right) >> 9;
            SizeLaser->playerOuterBox[playerID].bottom = (player->scale.x * playerHitbox->bottom) >> 9;

            playerHitbox = RSDK.GetHitbox(&player->animator, 1);
            if (!playerHitbox)
                playerHitbox = &defaultHitbox;
            SizeLaser->playerInnerBox[playerID].left   = (player->scale.x * playerHitbox->left) >> 9;
            SizeLaser->playerInnerBox[playerID].top    = (player->scale.x * playerHitbox->top) >> 9;
            SizeLaser->playerInnerBox[playerID].right  = (player->scale.x * playerHitbox->right) >> 9;
            SizeLaser->playerInnerBox[playerID].bottom = (player->scale.x * playerHitbox->bottom) >> 9;
            player->outerbox                           = &SizeLaser->playerOuterBox[playerID];
            player->innerbox                           = &SizeLaser->playerInnerBox[playerID];
        }
    }
}

void SizeLaser_Draw(void)
{
    RSDK_THIS(SizeLaser);
    RSDK.DrawSprite(&self->animator1, NULL, false);
    if (self->state == SizeLaser_Unknown10)
        RSDK.DrawSprite(&self->animator2, &self->storedPos, false);
}

void SizeLaser_Create(void *data)
{
    RSDK_THIS(SizeLaser);

    self->drawFX = FX_FLIP;
    if (!SceneInfo->inEditor) {
        int32 type          = voidToInt(data);
        self->visible       = true;
        self->updateRange.x = 0x800000;
        self->updateRange.y = 0x800000;
        if (!type) {
            self->active    = ACTIVE_BOUNDS;
            self->drawOrder = Zone->drawOrderLow + 1;

            self->state = SizeLaser_Unknown8;
            switch (self->orientation) {
                case 0:
                    self->direction = FLIP_NONE;
                    RSDK.SetSpriteAnimation(SizeLaser->aniFrames, 2 * self->type, &self->animator1, true, 0);
                    break;
                case 1:
                    self->direction = FLIP_NONE;
                    RSDK.SetSpriteAnimation(SizeLaser->aniFrames, 2 * self->type + 1, &self->animator1, true, 0);
                    break;
                case 2:
                    self->direction = FLIP_X;
                    RSDK.SetSpriteAnimation(SizeLaser->aniFrames, 2 * self->type + 1, &self->animator1, true, 0);
                    break;
            }
        }
        else {
            self->active      = ACTIVE_NORMAL;
            self->drawOrder   = Zone->drawOrderLow;
            self->orientation = (type - 1) >> 2;
            self->type        = (type - 1) & 1;

            self->state = SizeLaser_Unknown9;
            switch (self->orientation) {
                default:
                case 0:
                    self->velocity.y = 0x40000;
                    RSDK.SetSpriteAnimation(SizeLaser->aniFrames, 2 * (self->type + 2), &self->animator1, true, 0);
                    RSDK.SetSpriteAnimation(SizeLaser->aniFrames, 2 * (self->type + 4), &self->animator2, true, 0);
                    break;
                case 1:
                    self->velocity.x = 0x40000;
                    RSDK.SetSpriteAnimation(SizeLaser->aniFrames, 2 * self->type + 5, &self->animator1, true, 0);
                    RSDK.SetSpriteAnimation(SizeLaser->aniFrames, 2 * self->type + 9, &self->animator2, true, 0);
                    break;
                case 2:
                    self->direction  = FLIP_X;
                    self->velocity.x = -0x40000;
                    RSDK.SetSpriteAnimation(SizeLaser->aniFrames, 2 * self->type + 5, &self->animator1, true, 0);
                    RSDK.SetSpriteAnimation(SizeLaser->aniFrames, 2 * self->type + 9, &self->animator2, true, 0);
                    break;
            }
        }
    }
}

void SizeLaser_StageLoad(void)
{
    SizeLaser->aniFrames = RSDK.LoadSpriteAnimation("MMZ/SizeLaser.bin", SCOPE_STAGE);
    switch (globals->playerID & 0xFF) {
        case ID_SONIC: SizeLaser->sonicFrames = RSDK.LoadSpriteAnimation("Players/ChibiSonic.bin", SCOPE_STAGE); break;
        case ID_TAILS:
            SizeLaser->tailsFrames = RSDK.LoadSpriteAnimation("Players/ChibiTails.bin", SCOPE_STAGE);
            SizeLaser->tailFrames  = RSDK.LoadSpriteAnimation("Players/CTailSprite.bin", SCOPE_STAGE);
            break;
        case ID_KNUCKLES: SizeLaser->knuxFrames = RSDK.LoadSpriteAnimation("Players/ChibiKnux.bin", SCOPE_STAGE); break;
#if RETRO_USE_PLUS
        case ID_MIGHTY: SizeLaser->mightyFrames = RSDK.LoadSpriteAnimation("Players/ChibiMighty.bin", SCOPE_STAGE); break;
        case ID_RAY: SizeLaser->rayFrames = RSDK.LoadSpriteAnimation("Players/ChibiRay.bin", SCOPE_STAGE); break;
#endif
        default: break;
    }

    switch ((globals->playerID >> 8) & 0xFF) {
        case ID_SONIC: SizeLaser->sonicFrames = RSDK.LoadSpriteAnimation("Players/ChibiSonic.bin", SCOPE_STAGE); break;
        case ID_TAILS:
            SizeLaser->tailsFrames = RSDK.LoadSpriteAnimation("Players/ChibiTails.bin", SCOPE_STAGE);
            SizeLaser->tailFrames  = RSDK.LoadSpriteAnimation("Players/CTailSprite.bin", SCOPE_STAGE);
            break;
        case ID_KNUCKLES: SizeLaser->knuxFrames = RSDK.LoadSpriteAnimation("Players/ChibiKnux.bin", SCOPE_STAGE); break;
#if RETRO_USE_PLUS
        case ID_MIGHTY: SizeLaser->mightyFrames = RSDK.LoadSpriteAnimation("Players/ChibiMighty.bin", SCOPE_STAGE); break;
        case ID_RAY: SizeLaser->rayFrames = RSDK.LoadSpriteAnimation("Players/ChibiRay.bin", SCOPE_STAGE); break;
#endif
        default: break;
    }

    SizeLaser->hitbox.left   = -4;
    SizeLaser->hitbox.top    = -4;
    SizeLaser->hitbox.right  = 4;
    SizeLaser->hitbox.bottom = 4;
    SizeLaser->sfxShrink2    = RSDK.GetSfx("MMZ/Shrink2.wav");
    SizeLaser->sfxGrow2      = RSDK.GetSfx("MMZ/Grow2.wav");

    Soundboard_LoadSFX("MMZ/SizeLaser.wav", true, SizeLaser_SizeChangeSFXCheck, NULL);
}

bool32 SizeLaser_SizeChangeSFXCheck(void)
{
    int32 cnt = 0;
    foreach_active(SizeLaser, entity)
    {
        if (entity->activeScreens)
            ++cnt;
    }
    return cnt > 0;
}

void SizeLaser_SetP2State(EntityPlayer *player, bool32 chibiFlag)
{
    if (chibiFlag) {
        switch (player->characterID) {
            case ID_TAILS:
                player->aniFrames  = SizeLaser->tailsFrames;
                player->tailFrames = SizeLaser->tailFrames;
                player->isChibi    = chibiFlag;
                break;
            case ID_KNUCKLES:
                player->aniFrames  = SizeLaser->knuxFrames;
                player->tailFrames = -1;
                player->isChibi    = chibiFlag;
                break;
#if RETRO_USE_PLUS
            case ID_MIGHTY:
                player->aniFrames  = SizeLaser->mightyFrames;
                player->tailFrames = -1;
                player->isChibi    = chibiFlag;
                break;
            case ID_RAY:
                player->aniFrames  = SizeLaser->rayFrames;
                player->tailFrames = -1;
                player->isChibi    = chibiFlag;
                break;
#endif
            default:
                player->aniFrames  = SizeLaser->sonicFrames;
                player->tailFrames = -1;
                player->isChibi    = chibiFlag;
                break;
        }
    }
    else {
        switch (player->characterID) {
            case ID_TAILS:
                player->aniFrames  = Player->tailsFrames;
                player->tailFrames = Player->tailsTailsFrames;
                player->isChibi    = false;
                break;
            case ID_KNUCKLES:
                player->aniFrames  = Player->knuxFrames;
                player->tailFrames = -1;
                player->isChibi    = chibiFlag;
                break;
#if RETRO_USE_PLUS
            case ID_MIGHTY:
                player->aniFrames  = Player->mightyFrames;
                player->tailFrames = -1;
                player->isChibi    = chibiFlag;
                break;
            case ID_RAY:
                player->aniFrames  = Player->rayFrames;
                player->tailFrames = -1;
                player->isChibi    = chibiFlag;
                break;
#endif
            default:
                player->aniFrames  = Player->sonicFrames;
                player->tailFrames = -1;
                player->isChibi    = chibiFlag;
                break;
        }
    }
}

void SizeLaser_P2JumpInResize(void)
{
    EntityPlayer *self    = RSDK_GET_ENTITY(SceneInfo->entitySlot, Player);
    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);

    self->position.x = player1->position.x;
    self->position.y = player1->position.y;

    self->position.x += self->abilityValues[0];
    self->position.y += self->abilityValues[1];
    if (self->abilityValues[0] <= 0) {
        if (self->abilityValues[0] < 0) {
            self->abilityValues[0] -= (self->abilityValues[0] >> 4);
            if (self->abilityValues[0] > 0)
                self->abilityValues[0] = 0;
        }
    }
    else {
        self->abilityValues[0] -= (self->abilityValues[0] >> 4);
        if (self->abilityValues[0] < 0)
            self->abilityValues[0] = 0;
    }

    if (self->abilityValues[1] <= 0) {
        if (self->abilityValues[1] < 0) {
            self->abilityValues[1] -= (self->abilityValues[0] >> 4);
            if (self->abilityValues[1] > 0)
                self->abilityValues[1] = 0;
        }
    }
    else {
        self->abilityValues[1] -= (self->abilityValues[0] >> 4);
        if (self->abilityValues[1] < 0)
            self->abilityValues[1] = 0;
    }

    if (player1->state == SizeLaser_P2JumpInShrink) {
        if (player1->state == SizeLaser_P2JumpInGrow || !player1->isChibi) {
            self->state       = SizeLaser_P2JumpInGrow;
            self->interaction = false;
        }
    }
    else if (player1->isChibi) {
        self->state       = SizeLaser_P2JumpInShrink;
        self->interaction = false;
    }
}

void SizeLaser_P2JumpInGrow(void)
{
    EntityPlayer *self  = RSDK_GET_ENTITY(SceneInfo->entitySlot, Player);
    StateMachine(state) = self->abilityPtrs[0];
    StateMachine_Run(state);

    if (self->scale.x >= 0x200) {
        self->drawFX &= ~FX_SCALE;
        self->isChibi     = false;
        self->scale.x     = 0x200;
        self->scale.y     = 0x200;
        self->interaction = true;
        self->state       = Player_State_Air;
        Player_ChangePhysicsState(self);
        if (self->characterID == ID_TAILS)
            self->cameraOffset = 0;
        else
            self->cameraOffset = 0x50000;
    }
    else {
        self->state = SizeLaser_P2JumpInGrow;
        self->scale.x += 6;
        self->scale.y = self->scale.x;
    }
}

void SizeLaser_P2JumpInShrink(void)
{
    EntityPlayer *self  = RSDK_GET_ENTITY(SceneInfo->entitySlot, Player);
    StateMachine(state) = self->abilityPtrs[0];
    StateMachine_Run(state);

    if (self->scale.x <= 0x140) {
        switch (self->characterID) {
            case ID_TAILS:
                self->aniFrames  = SizeLaser->tailsFrames;
                self->tailFrames = SizeLaser->tailFrames;
                break;
            case ID_KNUCKLES:
                self->aniFrames  = SizeLaser->knuxFrames;
                self->tailFrames = -1;
                break;
#if RETRO_USE_PLUS
            case ID_MIGHTY:
                self->aniFrames  = SizeLaser->mightyFrames;
                self->tailFrames = -1;
                break;
            case ID_RAY:
                self->aniFrames  = SizeLaser->rayFrames;
                self->tailFrames = -1;
                break;
#endif
            default:
                self->aniFrames  = SizeLaser->sonicFrames;
                self->tailFrames = -1;
                break;
        }
        self->cameraOffset = 0x40000;
        RSDK.SetSpriteAnimation(self->aniFrames, ANI_HURT, &self->animator, false, 0);
        self->isChibi = true;
        self->drawFX &= ~FX_SCALE;
        self->scale.x     = 0x200;
        self->scale.y     = 0x200;
        self->interaction = true;
        self->state       = Player_State_Air;
        Player_ChangePhysicsState(self);
    }
    else {
        self->state = SizeLaser_P2JumpInShrink;
        self->scale.x -= 6;
        self->scale.y = self->scale.x;
    }
}

void SizeLaser_PlayerState_Grow(void)
{
    EntityPlayer *self = RSDK_GET_ENTITY(SceneInfo->entitySlot, Player);

    StateMachine(state) = self->abilityPtrs[0];
    StateMachine_Run(state);

    if (self->scale.x >= 0x300) {
        self->scale.x     = 0x300;
        self->scale.y     = 0x300;
        self->interaction = true;
        self->state       = Player_State_Air;
        Player_ChangePhysicsState(self);
    }
    else {
        self->state = SizeLaser_PlayerState_Grow;
        self->scale.x += 8;
        self->scale.y = self->scale.x;
    }
}

void SizeLaser_Unknown3(void)
{
    RSDK_THIS(SizeLaser);
    int32 entityX = self->position.x;
    int32 entityY = self->position.y;

    int32 extendX1[2];
    int32 extendY1[2];
    int32 extendX2[2];
    int32 extendY2[2];

    switch (self->orientation) {
        case 0:
            extendX1[0] = entityX - 0x80000;
            extendX1[1] = entityX + 0x80000;

            extendY1[0] = entityY - 0x200000;
            extendY1[1] = entityY - 0x200000;

            extendX2[0] = entityX - 0x80000;
            extendX2[1] = entityX + 0x80000;

            extendY2[0] = entityY + (self->extend << 16);
            extendY2[1] = entityY + (self->extend << 16);
            break;

        case 1:
            extendX1[0] = entityX - 0x200000;
            extendX1[1] = entityX - 0x200000;

            extendY1[0] = entityY - 0x80000;
            extendY1[1] = entityY + 0x80000;

            extendX2[0] = entityX + (self->extend << 16);
            extendX2[1] = entityX + (self->extend << 16);

            extendY2[1] = entityY + 0x80000;
            extendY2[0] = entityY - 0x80000;
            break;

        case 2:
            extendX1[0] = entityX - 0x200000;
            extendX1[1] = entityX;

            extendY1[0] = entityY - 0x80000;
            extendY1[1] = entityY + 0x80000;

            extendX2[0] = entityX + 0x200000;
            extendX2[1] = entityX - (self->extend << 16);

            extendY2[0] = entityY + 0x80000;
            extendY2[1] = entityY - 0x80000;
            break;
    }

    foreach_active(Player, player)
    {
        int32 playerID = RSDK.GetEntityID(player);
        if (MathHelpers_Unknown12(SizeLaser->playerPositions[playerID].x, SizeLaser->playerPositions[playerID].y, player->position.x,
                                  player->position.y, extendX1[0], extendY1[0], extendX2[0], extendY2[0])
            || MathHelpers_Unknown12(SizeLaser->playerPositions[playerID].x, SizeLaser->playerPositions[playerID].y, player->position.x,
                                     player->position.y, extendX1[1], extendY1[1], extendX2[1], extendY2[1])) {

            if (self->type) {
                if (player->state == SizeLaser_P2JumpInShrink || player->state == SizeLaser_P2JumpInGrow || !player->isChibi) {
                    if (player->state != SizeLaser_PlayerState_Grow && player->scale.x > 0x400) {
                        player->onGround        = false;
                        player->interaction     = false;
                        player->velocity.y      = -0x40000;
                        player->nextAirState    = StateMachine_None;
                        player->nextGroundState = StateMachine_None;
                        if (!player->direction)
                            player->velocity.x = -0x20000;
                        else
                            player->velocity.x = 0x20000;
                        player->drawFX |= FX_SCALE;
                        player->scale.x        = 0x200;
                        player->scale.y        = 0x200;
                        player->tileCollisions = true;
                        RSDK.SetSpriteAnimation(player->aniFrames, ANI_HURT, &player->animator, false, 0);
                        player->abilityPtrs[0] = Player_State_Hit;
                        player->state          = SizeLaser_PlayerState_Grow;
                    }
                }
                else {
                    player->onGround        = false;
                    player->interaction     = false;
                    player->velocity.y      = -0x40000;
                    player->nextAirState    = StateMachine_None;
                    player->nextGroundState = StateMachine_None;
                    if (!player->direction)
                        player->velocity.x = -0x20000;
                    else
                        player->velocity.x = 0x20000;
                    player->drawFX |= FX_SCALE;
                    player->scale.x = 0x140;
                    player->scale.y = 0x140;
                    switch (player->characterID) {
                        case ID_TAILS:
                            player->aniFrames  = Player->tailsFrames;
                            player->tailFrames = Player->tailsTailsFrames;
                            break;
                        case ID_KNUCKLES:
                            player->aniFrames  = Player->knuxFrames;
                            player->tailFrames = -1;
                            break;
#if RETRO_USE_PLUS
                        case ID_MIGHTY:
                            player->aniFrames  = Player->mightyFrames;
                            player->tailFrames = -1;
                            break;
                        case ID_RAY:
                            player->aniFrames  = Player->rayFrames;
                            player->tailFrames = -1;
                            break;
#endif
                        default:
                            if (player->superState == SUPERSTATE_SUPER)
                                player->aniFrames = Player->superFrames;
                            else
                                player->aniFrames = Player->sonicFrames;
                            player->tailFrames = -1;
                            break;
                    }
                    RSDK.SetSpriteAnimation(player->aniFrames, ANI_HURT, &player->animator, false, 0);
                    RSDK.PlaySfx(SizeLaser->sfxGrow2, false, 255);
                    player->tileCollisions = true;
                    player->abilityPtrs[0] = Player_State_Hit;
                    player->state          = SizeLaser_P2JumpInGrow;
                }
            }
            else if (player->state != SizeLaser_P2JumpInShrink && player->state != SizeLaser_P2JumpInGrow && !player->isChibi) {
                player->onGround        = false;
                player->interaction     = false;
                player->velocity.y      = -0x40000;
                player->nextAirState    = StateMachine_None;
                player->nextGroundState = StateMachine_None;
                if (!player->direction)
                    player->velocity.x = -0x20000;
                else
                    player->velocity.x = 0x20000;
                player->drawFX |= FX_SCALE;
                player->scale.x = 0x200;
                player->scale.y = 0x200;
                RSDK.SetSpriteAnimation(player->aniFrames, ANI_HURT, &player->animator, false, 0);
                RSDK.PlaySfx(SizeLaser->sfxShrink2, 0, 255);
                player->tileCollisions = true;
                player->abilityPtrs[0] = Player_State_Hit;
                player->state          = SizeLaser_P2JumpInShrink;
            }
        }
    }
}

void SizeLaser_Unknown8(void)
{
    RSDK_THIS(SizeLaser);

    if (self->timer < 24 && !(self->timer & 1))
        CREATE_ENTITY(SizeLaser, intToVoid(self->type + 4 * self->orientation + 1), self->position.x, self->position.y);
    SizeLaser_Unknown3();
    self->timer = (self->timer + 1) & 0x1F;
}

void SizeLaser_Unknown9(void)
{
    RSDK_THIS(SizeLaser);
    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;

    bool32 collided = false;
    switch (self->orientation) {
        case 0: collided = RSDK.ObjectTileCollision(self, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x40000, false); break;
        case 1: collided = RSDK.ObjectTileCollision(self, Zone->fgLayers, CMODE_LWALL, 0, 0x40000, 0, false); break;
        case 2: collided = RSDK.ObjectTileCollision(self, Zone->fgLayers, CMODE_RWALL, 0, -0x40000, 0, false); break;
    }

    if (collided) {
        self->storedPos.x = self->position.x;
        self->storedPos.y = self->position.y;
        self->state       = SizeLaser_Unknown10;
    }
}

void SizeLaser_Unknown10(void)
{
    RSDK_THIS(SizeLaser);
    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;
    if (++self->timer == 4)
        destroyEntity(self);
}

#if RETRO_INCLUDE_EDITOR
void SizeLaser_EditorDraw(void)
{
    RSDK_THIS(SizeLaser);

    switch (self->orientation) {
        case 0:
            self->direction = FLIP_NONE;
            RSDK.SetSpriteAnimation(SizeLaser->aniFrames, 2 * self->type, &self->animator1, true, 0);
            break;
        case 1:
            self->direction = FLIP_NONE;
            RSDK.SetSpriteAnimation(SizeLaser->aniFrames, 2 * self->type + 1, &self->animator1, true, 0);
            break;
        case 2:
            self->direction = FLIP_X;
            RSDK.SetSpriteAnimation(SizeLaser->aniFrames, 2 * self->type + 1, &self->animator1, true, 0);
            break;
    }

    RSDK.DrawSprite(&self->animator1, NULL, false);
    RSDK.DrawSprite(&self->animator2, NULL, false);
}

void SizeLaser_EditorLoad(void) { SizeLaser->aniFrames = RSDK.LoadSpriteAnimation("MMZ/SizeLaser.bin", SCOPE_STAGE); }
#endif

void SizeLaser_Serialize(void)
{
    RSDK_EDITABLE_VAR(SizeLaser, VAR_UINT8, type);
    RSDK_EDITABLE_VAR(SizeLaser, VAR_UINT8, orientation);
    RSDK_EDITABLE_VAR(SizeLaser, VAR_ENUM, extend);
}
