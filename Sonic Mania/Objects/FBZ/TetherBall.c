#include "SonicMania.h"

ObjectTetherBall *TetherBall;

void TetherBall_Update(void)
{
    RSDK_THIS(TetherBall);
    StateMachine_Run(self->state);
}

void TetherBall_LateUpdate(void) {}

void TetherBall_StaticUpdate(void) {}

void TetherBall_Draw(void)
{
    RSDK_THIS(TetherBall);
    Vector2 drawPos;

    int32 i = 0x200;
    for (; i < self->size; i += 0x400) {
        TetherBall->animator.frameID = 2;
        drawPos                      = self->position;
        drawPos.x += i * RSDK.Cos1024(self->angle);
        drawPos.y += i * RSDK.Sin1024(self->angle);
        RSDK.DrawSprite(&TetherBall->animator, &drawPos, false);
    }

    TetherBall->animator.frameID = 3;
    drawPos                      = self->position;
    drawPos.x += (i + 0x200) * RSDK.Cos1024(self->angle);
    drawPos.y += (i + 0x200) * RSDK.Sin1024(self->angle);
    RSDK.DrawSprite(&TetherBall->animator, &drawPos, false);

    self->posUnknown           = drawPos;
    TetherBall->animator.frameID = self->type >> 1;
    self->drawFX               = FX_FLIP;
    RSDK.DrawSprite(&TetherBall->animator, NULL, false);

    self->drawFX = FX_NONE;
}

void TetherBall_Create(void *data)
{
    RSDK_THIS(TetherBall);
    switch (self->type) {
        case 0:
        case 2: self->direction = FLIP_NONE; break;
        case 1: self->direction = FLIP_Y; break;
        case 3: self->direction = FLIP_X; break;
        default: break;
    }

    if (!SceneInfo->inEditor) {
        self->active            = ACTIVE_BOUNDS;
        self->size              = (self->chainCount << 10) + 0x200;
        self->updateRange.x     = 0x1000000;
        self->updateRange.y     = 0x1000000;
        self->visible           = true;
        self->drawOrder         = Zone->drawOrderLow;
        TetherBall->hitbox.top    = -40;
        TetherBall->hitbox.left   = -40;
        TetherBall->hitbox.right  = 40;
        TetherBall->hitbox.bottom = 40;
        self->angle             = self->angleStart;
        if (self->angleStart > self->angleEnd) {
            int start   = self->angleStart;
            self->angleStart = self->angleEnd;
            self->angleEnd   = start;
        }
        self->state = TetherBall_Unknown1;
    }
}

void TetherBall_StageLoad(void)
{
    TetherBall->aniFrames = RSDK.LoadSpriteAnimation("FBZ/TetherBall.bin", SCOPE_STAGE);
    RSDK.SetSpriteAnimation(TetherBall->aniFrames, 0, &TetherBall->animator, true, 0);
    TetherBall->sfxWhack = RSDK.GetSfx("Stage/Whack.wav");
}

void TetherBall_Unknown1(void)
{
    RSDK_THIS(TetherBall);
    int32 storeX       = self->position.x;
    int32 storeY       = self->position.y;
    self->position = self->posUnknown;
    self->rotation = self->angle << 16;
    foreach_active(Player, player)
    {
        if (!player->sidekick) {
            if (Player_CheckCollisionTouch(player, self, &TetherBall->hitbox)) {
                switch ((self->angle >> 8) & 3) {
                    case 0:
                        if (!player->onGround && player->groundedStore) {
                            if (player->rotation > 112 && player->rotation < 144) {
                                self->field_70        = 8 * player->velocity.y / self->chainCount;
                                self->playerPtr       = player;
                                player->state           = Player_State_None;
                                player->nextGroundState = StateMachine_None;
                                player->nextAirState    = StateMachine_None;
                                self->state           = TetherBall_Unknown2;
                            }
                        }
                        break;
                    case 1:
                        if (!player->onGround && player->groundedStore) {
                            if (player->rotation > 240 && player->rotation < 272) {
                                self->field_70        = 8 * -player->velocity.x / self->chainCount;
                                self->playerPtr       = player;
                                player->state           = Player_State_None;
                                player->nextGroundState = StateMachine_None;
                                player->nextAirState    = StateMachine_None;
                                self->state           = TetherBall_Unknown2;
                            }
                        }
                        break;
                    case 2:
                        if (!player->onGround && player->groundedStore) {
                            if (player->rotation > 368 && player->rotation < 400) {
                                self->field_70        = 8 * -player->velocity.y / self->chainCount;
                                self->playerPtr       = player;
                                player->state           = Player_State_None;
                                player->nextGroundState = StateMachine_None;
                                player->nextAirState    = StateMachine_None;
                                self->state           = TetherBall_Unknown2;
                            }
                        }
                        break;
                    case 3:
                        if (!player->onGround && player->groundedStore) {
                            if (player->rotation < 16 || player->rotation > 496) {
                                self->field_70        = 8 * player->velocity.x / self->chainCount;
                                self->playerPtr       = player;
                                player->state           = Player_State_None;
                                player->nextGroundState = StateMachine_None;
                                player->nextAirState    = StateMachine_None;
                                self->state           = TetherBall_Unknown2;
                            }
                        }
                        break;
                }
            }
        }
    }
    self->position.x = storeX;
    self->position.y = storeY;
}
void TetherBall_Unknown2(void)
{
    RSDK_THIS(TetherBall);
    self->rotation += self->field_70;
    self->angle = self->rotation >> 16;
    if ((uint32)(self->angle - 0x101) > 0x1FE)
        self->field_70 += 0x1800;
    else
        self->field_70 -= 0x1800;

    if (self->field_70 >= 0) {
        if (self->angle >= self->angleEnd) {
            self->angle    = self->angleEnd;
            self->rotation = self->angleEnd << 16;
            self->state    = TetherBall_Unknown3;
            RSDK.PlaySfx(TetherBall->sfxWhack, 0, 255);
        }
    }
    else {
        if (self->angle <= self->angleStart) {
            self->angle    = self->angleStart;
            self->rotation = self->angleStart << 16;
            self->state    = TetherBall_Unknown3;
            RSDK.PlaySfx(TetherBall->sfxWhack, 0, 255);
        }
    }

    EntityPlayer *player = self->playerPtr;
    if (player) {
        if (Player_CheckValidState(player)) {
            Hitbox *playerHitbox = Player_GetHitbox(player);
            player->angle        = ((self->angle >> 2) + 64);
            player->rotation     = 2 * player->angle;
            player->position.x   = self->position.x;
            player->position.y   = self->position.y;
            player->position.x += (self->size + ((playerHitbox->bottom + 24) << 6)) * RSDK.Cos1024(self->angle);
            player->position.y += (self->size + ((playerHitbox->bottom + 24) << 6)) * RSDK.Sin1024(self->angle);
            player->onGround        = true;
            player->nextGroundState = StateMachine_None;
            player->nextAirState    = StateMachine_None;
            player->groundVel       = (self->field_70 * self->chainCount) >> 3;
            if (self->state == TetherBall_Unknown2) {
                if (player->jumpPress) {
                    self->playerPtr = NULL;
                    Player_StartJump(player);
                }
            }
            else {
                int32 vel = 0;
                if (player->groundVel <= 0)
                    vel = -0x60000;
                else
                    vel = 0x60000;
                switch (self->angle >> 8) {
                    case 0:
                        player->collisionMode = CMODE_RWALL;
                        player->position.y += vel;
                        break;
                    case 1:
                        player->collisionMode = CMODE_ROOF;
                        player->position.x -= vel;
                        break;
                    case 2:
                        player->collisionMode = CMODE_LWALL;
                        player->position.y -= vel;
                        break;
                    case 3:
                        player->collisionMode = CMODE_FLOOR;
                        player->position.x += vel;
                        break;
                }
                self->playerPtr = NULL;
                self->timer     = 0;
                if (player->animator.animationID == ANI_JUMP)
                    player->state = Player_State_Roll;
                else
                    player->state = Player_State_Ground;
            }
        }
        else {
            self->playerPtr = NULL;
        }
    }
}
void TetherBall_Unknown3(void)
{
    RSDK_THIS(TetherBall);
    if (++self->timer == 8) {
        self->timer = 0;
        self->state = TetherBall_Unknown1;
    }
}

#if RETRO_INCLUDE_EDITOR
void TetherBall_EditorDraw(void)
{
    RSDK_THIS(TetherBall);

    switch (self->type) {
        case TETHERBALL_0:
        case TETHERBALL_2: self->direction = FLIP_NONE; break;
        case TETHERBALL_1: self->direction = FLIP_Y; break;
        case TETHERBALL_3: self->direction = FLIP_X; break;
        default: break;
    }

    self->size              = (self->chainCount << 10) + 0x200;
    self->updateRange.x     = 0x1000000;
    self->updateRange.y     = 0x1000000;

    TetherBall_Draw();
}

void TetherBall_EditorLoad(void)
{
    TetherBall->aniFrames = RSDK.LoadSpriteAnimation("FBZ/TetherBall.bin", SCOPE_STAGE);
    RSDK.SetSpriteAnimation(TetherBall->aniFrames, 0, &TetherBall->animator, true, 0);
}
#endif

void TetherBall_Serialize(void)
{
    RSDK_EDITABLE_VAR(TetherBall, VAR_UINT8, type);
    RSDK_EDITABLE_VAR(TetherBall, VAR_UINT8, chainCount);
    RSDK_EDITABLE_VAR(TetherBall, VAR_ENUM, angleStart);
    RSDK_EDITABLE_VAR(TetherBall, VAR_ENUM, angleEnd);
}
