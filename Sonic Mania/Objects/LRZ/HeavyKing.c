// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: HeavyKing Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectHeavyKing *HeavyKing;

void HeavyKing_Update(void)
{
    RSDK_THIS(HeavyKing);

    RSDK.ProcessAnimation(&self->animator1);
    RSDK.ProcessAnimation(&self->animator2);

    if (self->invincibilityTimer > 0)
        self->invincibilityTimer--;

    StateMachine_Run(self->state);
}

void HeavyKing_LateUpdate(void) {}

void HeavyKing_StaticUpdate(void) {}

void HeavyKing_Draw(void)
{
    RSDK_THIS(HeavyKing);

    if (self->invincibilityTimer & 1) {
        RSDK.CopyPalette(2, 16, 0, 16, 16);
        RSDK.CopyPalette(2, 192, 0, 192, 16);
        RSDK.DrawSprite(&self->animator1, NULL, false);
        RSDK.DrawSprite(&self->animator2, NULL, false);
        RSDK.DrawSprite(&self->animator3, NULL, false);
        RSDK.CopyPalette(1, 16, 0, 16, 16);
        RSDK.CopyPalette(1, 192, 0, 192, 16);
    }
    else {
        RSDK.DrawSprite(&self->animator1, NULL, false);
        RSDK.DrawSprite(&self->animator2, NULL, false);
        RSDK.DrawSprite(&self->animator3, NULL, false);
    }
}

void HeavyKing_Create(void *data)
{
    RSDK_THIS(HeavyKing);

    if (!SceneInfo->inEditor) {
        if (globals->gameMode == MODE_TIMEATTACK) {
            destroyEntity(self);
        }
        else {
            self->drawOrder     = Zone->drawOrderLow;
            self->drawFX        = FX_FLIP;
            self->active        = ACTIVE_BOUNDS;
            self->updateRange.x = 0xC00000;
            self->updateRange.y = 0x800000;
            self->health        = 8;
            self->state         = HeavyKing_State_SetupArena;
            RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 7, &self->animator1, true, 0);
            RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 16, &self->animator2, true, 0);
        }
    }
}

void HeavyKing_StageLoad(void)
{
    HeavyKing->aniFrames      = RSDK.LoadSpriteAnimation("LRZ3/HeavyKing.bin", SCOPE_STAGE);
    HeavyKing->cutsceneFrames = RSDK.LoadSpriteAnimation("Players/KnuxCutsceneHPZ.bin", SCOPE_STAGE);
    HeavyKing->attackPatternPos = 0;
    HeavyKing->sfxHit         = RSDK.GetSfx("Stage/BossHit.wav");
    HeavyKing->sfxImpact2     = RSDK.GetSfx("Stage/Impact2.wav");
    HeavyKing->sfxImpact5     = RSDK.GetSfx("Stage/Impact5.wav");
    HeavyKing->sfxExplosion   = RSDK.GetSfx("Stage/Explosion2.wav");
    HeavyKing->sfxFreeze      = RSDK.GetSfx("PSZ/Freeze.wav");
    HeavyKing->sfxCharge      = RSDK.GetSfx("LRZ/KingCharge.wav");
    HeavyKing->sfxImpact6     = RSDK.GetSfx("Stage/Impact6.wav");
    HeavyKing->sfxRodPlant    = RSDK.GetSfx("LRZ/RodPlant.wav");
    HeavyKing->sfxRodShine    = RSDK.GetSfx("LRZ/RodShine.wav");
    HeavyKing->sfxElecOn      = RSDK.GetSfx("LRZ/ElecOn.wav");
    HeavyKing->sfxTwinCharge  = RSDK.GetSfx("LRZ/TwinCharge.wav");

    HeavyKing->hitbox1.left   = -24;
    HeavyKing->hitbox1.top    = 40;
    HeavyKing->hitbox1.right  = 24;
    HeavyKing->hitbox1.bottom = 44;

    HeavyKing->hitbox2.left   = -20;
    HeavyKing->hitbox2.top    = 20;
    HeavyKing->hitbox2.right  = 20;
    HeavyKing->hitbox2.bottom = 16;

    HeavyKing->hitbox3.left   = -32;
    HeavyKing->hitbox3.top    = -40;
    HeavyKing->hitbox3.right  = 32;
    HeavyKing->hitbox3.bottom = 40;
}

void HeavyKing_CheckPlayerCollisions(void)
{
    RSDK_THIS(HeavyKing);

    foreach_active(Player, player)
    {
        if (!self->invincibilityTimer && Player_CheckBadnikTouch(player, self, &HeavyKing->hitbox2) && Player_CheckBossHit(player, self)) {
            HeavyKing_Hit();
        }
    }
}

void HeavyKing_CheckPlayerCollisions_Charging(void)
{
    RSDK_THIS(HeavyKing);

    foreach_active(Player, player)
    {
        if (!self->invincibilityTimer && Player_CheckCollisionTouch(player, self, &HeavyKing->hitbox3)) {
            Player_CheckElementalHit(player, self, SHIELD_LIGHTNING);
        }
    }
}

void HeavyKing_Hit(void)
{
    RSDK_THIS(HeavyKing);

    if (--self->health <= 0) {
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 6, &self->animator1, false, 0);
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 15, &self->animator2, false, 0);
        self->state      = HeavyKing_State_Destroyed;
        self->velocity.y = -0x40000;
        self->onGround   = false;
        self->timer      = 0;
        RSDK.StopSfx(HeavyKing->sfxTwinCharge);
        foreach_active(KingAttack, attack) { destroyEntity(attack); }
        SceneInfo->timeEnabled = false;
        Player_GiveScore(RSDK_GET_ENTITY(SLOT_PLAYER1, Player), 1000);
    }
    else {
        if (self->animator1.animationID == 5) {
            self->invincibilityTimer = 48;
        }
        else {
            self->invincibilityTimer = 96;
            self->stateStore     = self->state;
            self->animator1Store = self->animator1;
            self->animator2Store = self->animator2;
            RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 6, &self->animator1, false, 0);
            RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 15, &self->animator2, false, 0);
            self->state = HeavyKing_State_HitRecoil;
        }
        RSDK.PlaySfx(HeavyKing->sfxHit, false, 255);
    }
}

void HeavyKing_HandleClawMovement(void)
{
    RSDK_THIS(HeavyKing);

    EntityKingClaw *claw = (EntityKingClaw *)self->claw;

    if (claw) {
        self->position.x = claw->position.x;
        self->position.y = claw->position.y;
        if (self->direction == FLIP_NONE)
            self->position.x += 0x300000;
        else
            self->position.x -= 0x300000;
        self->position.y += 0x9C0000;
    }
}

void HeavyKing_HandleAnimators(void)
{
    RSDK_THIS(HeavyKing);

    self->animator1.speed = 0;
    self->animator2.speed = 0;
    if (self->velocity.y >= 0) {
        if (self->velocity.y <= 0x10000) {
            self->animator1.frameID = 7;
            self->animator2.frameID = 7;
        }
        else {
            self->animator1.frameID = 8;
            self->animator2.frameID = 8;
        }
    }
    else if (self->velocity.y >= -0x10000) {
        self->animator1.frameID = 6;
        self->animator2.frameID = 6;
    }
    else {
        self->animator1.frameID = 5;
        self->animator2.frameID = 5;
    }
}

void HeavyKing_Unknown6(void)
{
    RSDK_THIS(HeavyKing);

    EntityPlayer *player = Player_GetNearestPlayerX();

    int bounds = HeavyKing->boundsL;
    if (player->position.x >= bounds) {
        bounds = player->position.x;
        if (player->position.x > HeavyKing->boundsR)
            bounds = HeavyKing->boundsR;
    }

    int angle = RSDK.ATan2((bounds - self->position.x) >> 16, (player->position.y - self->position.y) >> 16);
    int rot   = 2 * angle;

    if (abs(rot) >= abs(rot - 0x200)) {
        if (abs(rot - 0x200) < abs(rot + 0x200)) {
            self->rotation += ((rot - 0x200) >> 5);
        }
        else {
            self->rotation += ((rot + 0x200) >> 5);
        }
    }
    else {
        if (abs(rot) < abs(rot + 0x200)) {
            self->rotation += (rot >> 5);
        }
        else {
            self->rotation += ((rot + 0x200) >> 5);
        }
    }
    self->rotation &= 0x1FF;

    self->velocity.x += 8 * RSDK.Cos512(self->rotation);
    int angle2         = (self->position.x - HeavyKing->boundsM) >> 16;
    self->velocity.x = clampVal(self->velocity.x, -0x20000, 0x20000);
    self->position.x += self->velocity.x;
    self->originPos.y = HeavyKing->value10 + 96 * abs(angle2 * angle2);
    self->direction   = self->position.x <= player->position.x;
}

void HeavyKing_FindTargetEmerald(void)
{
    RSDK_THIS(HeavyKing);

    EntityHPZEmerald *emeraldPtr = NULL;
    int emeraldDist              = 0x7FFFFFFF;

    if (self->onGround) {
        if (self->direction == FLIP_X) {
            foreach_active(HPZEmerald, emerald)
            {
                if (emerald->type != HPZEMERALD_MASTER) {
                    int dist = emerald->position.x - self->position.x;
                    if (dist < emeraldDist && dist >= 0 && self->targetEmerald != (Entity*)emerald) {
                        emeraldDist = dist;
                        emeraldPtr  = emerald;
                    }
                }
            }
        }
        else {
            foreach_active(HPZEmerald, emerald)
            {
                if (emerald->type != HPZEMERALD_MASTER) {
                    int dist = self->position.x - emerald->position.x;
                    if (dist < emeraldDist && dist >= 0 && self->targetEmerald != (Entity *)emerald) {
                        emeraldDist = dist;
                        emeraldPtr  = emerald;
                    }
                }
            }
        }

        if (emeraldPtr)
            self->targetEmerald = (Entity *)emeraldPtr;
        else
            self->direction ^= FLIP_X;
    }
    else {
        foreach_active(HPZEmerald, emerald)
        {
            if (emerald->type != HPZEMERALD_MASTER) {
                int dist = abs(self->position.x - emerald->position.x);
                if (dist < emeraldDist) {
                    emeraldDist = dist;
                    emeraldPtr  = emerald;
                }
            }
        }

        if (emeraldPtr)
            self->targetEmerald = (Entity *)emeraldPtr;
    }
}

void HeavyKing_Unknown8(int frame)
{
    RSDK_THIS(HeavyKing);

    RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 3, &self->animator1, true, frame);
    RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 12, &self->animator2, true, frame);
    if (RSDK.GetEntityCount(KingAttack->objectID, true)) {
        HeavyKing_FindTargetEmerald();
        self->velocity.x   = (self->targetEmerald->position.x - self->position.x) / 40;
        EntityPlayer *player = Player_GetNearestPlayer();

        foreach_active(KingAttack, attack)
        {
            if (attack->state == KingAttack_State2_Unknown2) {
                int angle          = RSDK.ATan2((player->position.x - attack->position.x) >> 16, (player->position.y - attack->position.y) >> 16);
                attack->field_70.x = 0x600 * RSDK.Cos256(angle);
                attack->field_70.y = 0x600 * RSDK.Sin256(angle);
                attack->drawOrder  = Zone->drawOrderLow;
                attack->state      = KingAttack_State_Unknown;
                RSDK.PlaySfx(KingAttack->sfxElecPulse, false, 255);
                foreach_break;
            }
        }
    }
    else {
        self->field_70 = 0;
        if (self->position.x - HeavyKing->boundsM > 0)
            self->velocity.x = -0x30000;
        else
            self->velocity.x = 0x30000;
    }
    self->velocity.y = -0x60000;
    self->onGround   = false;
    self->state      = HeavyKing_State_Unknown1;
}

void HeavyKing_Unknown9(void)
{
    RSDK_THIS(HeavyKing);

    HeavyKing_FindTargetEmerald();
    self->velocity.x = 0;
    self->velocity.y = 0;
    self->onGround   = false;
    self->state      = HeavyKing_State_Unknown13;
    RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 3, &self->animator1, false, 6);
    RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 12, &self->animator2, false, 6);
}

void HeavyKing_CreateSpinRay(void)
{
    RSDK_THIS(HeavyKing);

    EntityFXSpinRay *ray = NULL;
    if (self->direction) {
        ray             = CREATE_ENTITY(FXSpinRay, NULL, self->position.x - 0x240000, self->position.y - 0x300000);
        ray->offsetPos.x = -0x240000;
    }
    else {
        ray             = CREATE_ENTITY(FXSpinRay, NULL, self->position.x + 0x240000, self->position.y - 0x300000);
        ray->offsetPos.x = 0x240000;
    }
    ray->parent     = (Entity *)self;
    ray->offsetPos.y = -0x300000;
}

void HeavyKing_CreateExpandRing(void)
{
    RSDK_THIS(HeavyKing);

    EntityFXExpandRing *expandRing = CREATE_ENTITY(FXExpandRing, NULL, self->position.x + 0x240000, self->position.y - 0x300000);
    expandRing->parent             = (Entity *)self;
    expandRing->drawFX             = FX_FLIP;
    expandRing->posOffset.x        = 0x240000;
    expandRing->posOffset.y        = -0x300000;
}

void HeavyKing_CreateAttack(void)
{
    RSDK_THIS(HeavyKing);

    EntityKingAttack *attack = CREATE_ENTITY(KingAttack, intToVoid(0), self->position.x, HeavyKing->value9);
    attack->parent           = (Entity *)self;
    if (self->direction) {
        attack->position.x -= 0xC00000;
        attack->velocity.x = 0x100000;
    }
    else {
        attack->position.x += 0xC00000;
        attack->velocity.x = -0x100000;
    }
}

void HeavyKing_CreateAttackCircle(void)
{
    RSDK_THIS(HeavyKing);

    for (int angle = 0; angle < 1020; angle += 170) {
        EntityKingAttack *attack = CREATE_ENTITY(KingAttack, intToVoid(2), self->position.x, self->position.y);
        attack->angle            = angle;
        attack->parent           = (Entity *)self;
    }
}

void HeavyKing_State_SetupArena(void)
{
    RSDK_THIS(HeavyKing);

    if (++self->timer >= 8) {
        self->timer = 0;
        foreach_active(KingClaw, claw) { self->claw = (Entity *)claw; }
        EntityKingClaw *kingClaw = (EntityKingClaw *)self->claw;

        if (kingClaw) {
            HeavyKing->boundsL          = kingClaw->position.x - 0xA00000;
            HeavyKing->boundsM          = kingClaw->position.x;
            HeavyKing->boundsR          = kingClaw->position.x + 0x600000;
            Zone->playerBoundActiveL[0] = true;
            Zone->cameraBoundsL[0]     = (kingClaw->position.x >> 16) - 320;
            Zone->playerBoundActiveR[0] = true;
            Zone->cameraBoundsR[0]     = (kingClaw->position.x >> 16) + 320;
            Zone->cameraBoundsT[0]     = (kingClaw->position.y >> 16);
            HeavyKing->value9           = kingClaw->position.y + 0x2400000;
            self->active              = ACTIVE_NORMAL;
            HeavyKing_HandleClawMovement();
            self->state = HeavyKing_State_HandleCutsceneSetup;
        }
    }
}

void HeavyKing_State_HandleCutsceneSetup(void)
{
    RSDK_THIS(HeavyKing);
    EntityKingClaw *claw = (EntityKingClaw *)self->claw;

    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    if (player1->position.x > self->position.x - 0x1800000) {
        CutsceneSeq_LockPlayerControl(player1);
        player1->stateInput       = StateMachine_None;
        player1->jumpPress        = false;
        player1->jumpHold         = false;
        player1->right            = true;
        player1->pushing          = false;
        player1->state            = Player_State_Ground;
#if RETRO_USE_PLUS
        Player->disableP2KeyCheck = true;
#endif
        if (player1->velocity.x > 0x20000) {
            player1->groundVel  = 0x20000;
            player1->velocity.x = 0x20000;
        }
    }

    if (player1->position.x > self->position.x - 0xE00000) {
        player1->groundVel  = 0;
        player1->velocity.x = 0;
        player1->right      = false;
        player1->state      = Player_State_None;
        RSDK.SetSpriteAnimation(player1->aniFrames, ANI_LOOKUP, &player1->animator, false, 0);

        EntityPlayer *player2 = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
        if (player2->objectID == Player->objectID) {
            player2->groundVel  = 0;
            player2->velocity.x = 0;
            player2->direction  = FLIP_NONE;
            CutsceneSeq_LockPlayerControl(player2);
            player2->stateInput = StateMachine_None;
            player2->up         = true;
        }
        Music_TransitionTrack(TRACK_HBHMISCHIEF, 0.0125);
        claw->state     = KingClaw_Unknown2;
        self->visible = true;
        self->state   = HeavyKing_State_CutsceneUnknown1;
        Camera_ShakeScreen(0, 6, 6);
        RSDK.PlaySfx(HeavyKing->sfxImpact6, false, 255);
    }
}

void HeavyKing_State_CutsceneUnknown1(void)
{
    RSDK_THIS(HeavyKing);

    ++self->timer;
    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    if (player1->animator.frameID == 5)
        player1->animator.speed = 0;
    if (self->timer == 30) {
        self->timer = 0;
        foreach_all(HPZEmerald, emerald)
        {
            emerald->active = ACTIVE_NORMAL;
            if (emerald->type == HPZEMERALD_MASTER) {
                self->masterEmerald = (Entity *)emerald;
                Camera_SetupLerp(2, 0, emerald->position.x - 0x500000, emerald->position.y - 0x800000, 3);
            }
        }
        self->state = HeavyKing_State_CutsceneUnknown2;
    }
    HeavyKing_HandleClawMovement();
}

void HeavyKing_State_CutsceneUnknown2(void)
{
    RSDK_THIS(HeavyKing);

    if (++self->timer == 180) {
        self->timer        = 0;
        EntityCamera *camera = RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);
        Camera_SetupLerp(3, 0, camera->position.x, camera->startLerpPos.y, 4);
        self->state = HeavyKing_State_CutsceneUnknown3;

        EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
#if RETRO_USE_PLUS
        if (player1->characterID == ID_KNUCKLES)
#endif
            RSDK.SetSpriteAnimation(player1->aniFrames, ANI_IDLE, &player1->animator, false, 0);
    }
    HeavyKing_HandleClawMovement();
}

void HeavyKing_State_CutsceneUnknown3(void)
{
    RSDK_THIS(HeavyKing);

    if (!RSDK_GET_ENTITY(SLOT_CAMERA1, Camera)->state) {
        EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
#if RETRO_USE_PLUS
        if (player1->characterID == ID_KNUCKLES) {
#endif
            RSDK.SetSpriteAnimation(HeavyKing->cutsceneFrames, 0, &player1->animator, false, 0);
            CREATE_ENTITY(ThoughtBubble, NULL, player1->position.x - 0x300000, player1->position.y - 0x500000);
#if RETRO_USE_PLUS
        }
#endif
        self->state = HeavyKing_State_CutsceneUnknown4;
    }
    HeavyKing_HandleClawMovement();
}

void HeavyKing_State_CutsceneUnknown4(void)
{
    RSDK_THIS(HeavyKing);
    EntityKingClaw *claw = (EntityKingClaw *)self->claw;

    HeavyKing_HandleClawMovement();
    if (++self->timer == 180) {
        claw->state = KingClaw_Unknown3;
    }
    if (self->timer == 240) {
        self->timer = 0;
        Camera_ShakeScreen(0, 6, 6);
        RSDK.PlaySfx(HeavyKing->sfxImpact6, false, 255);
        EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
#if RETRO_USE_PLUS
        if (player1->characterID == ID_KNUCKLES)
#endif
            RSDK.SetSpriteAnimation(HeavyKing->cutsceneFrames, 1, &player1->animator, false, 0);
        self->state = HeavyKing_State_CutsceneUnknown5;
    }
}

void HeavyKing_State_CutsceneUnknown5(void)
{
    RSDK_THIS(HeavyKing);

    HeavyKing_HandleClawMovement();

    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
#if RETRO_USE_PLUS
    if (player1->characterID == ID_KNUCKLES) {
#endif
        if (player1->animator.frameID == player1->animator.frameCount - 1) {
            RSDK.SetSpriteAnimation(HeavyKing->cutsceneFrames, 2, &player1->animator, false, 0);
            self->state = HeavyKing_State_CutsceneUnknown6;
        }
#if RETRO_USE_PLUS
    }
    else {
        self->state = HeavyKing_State_CutsceneUnknown6;
    }
#endif
}

void HeavyKing_State_CutsceneUnknown6(void)
{
    RSDK_THIS(HeavyKing);
    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityCamera *camera  = RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);

    HeavyKing_HandleClawMovement();
    if (player1->onGround)
        player1->groundVel = 0;

    switch (++self->timer) {
        case 30:
#if RETRO_USE_PLUS
            if (player1->characterID == ID_KNUCKLES)
#endif
                RSDK.SetSpriteAnimation(HeavyKing->cutsceneFrames, 4, &player1->animator, false, 0);
            break;
        case 90:
            RSDK.PlaySfx(Player->sfxJump, false, 255);
            RSDK.SetSpriteAnimation(player1->aniFrames, ANI_JUMP, &player1->animator, false, 0);
            player1->velocity.x  = 0x2C000;
            player1->velocity.y  = -0x50000;
            player1->jumpAbility = 0;
            player1->onGround    = false;
            player1->state       = Player_State_Air;
            break;
        case 124:
            RSDK.PlaySfx(Player->sfxJump, false, 255);
            RSDK.SetSpriteAnimation(player1->aniFrames, ANI_JUMP, &player1->animator, false, 0);
            player1->velocity.x  = -0x1C000;
            player1->velocity.y  = -0x66000;
            player1->jumpAbility = 0;
            player1->onGround    = false;
            player1->state       = Player_State_Air;
            Camera_SetupLerp(3, 0, camera->startLerpPos.x, camera->startLerpPos.y + 0x200000, 4);
            break;
        default:
            if (self->timer > 144 && player1->onGround) {
#if RETRO_USE_PLUS
                if (player1->characterID == ID_KNUCKLES)
#endif
                    RSDK.SetSpriteAnimation(HeavyKing->cutsceneFrames, 3, &player1->animator, false, 0);
#if RETRO_USE_PLUS
                else
                    RSDK.SetSpriteAnimation(player1->aniFrames, ANI_IDLE, &player1->animator, false, 0);
#endif
                player1->position.y -= player1->cameraOffset;
                player1->state           = Player_State_None;
                player1->nextGroundState = StateMachine_None;
                self->timer            = 0;
                self->state            = HeavyKing_State_CutsceneUnknown7;
            }
            break;
    }
}

void HeavyKing_State_CutsceneUnknown7(void)
{
    RSDK_THIS(HeavyKing);
    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);

    HeavyKing_HandleClawMovement();
    if (++self->timer == 90) {
        self->timer  = 0;
        player1->down  = true;
        player1->state = Player_State_Ground;
        self->state  = HeavyKing_State_CutsceneUnknown8;
    }
}

void HeavyKing_State_CutsceneUnknown8(void)
{
    RSDK_THIS(HeavyKing);
    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);

    HeavyKing_HandleClawMovement();
    self->timer++;
    player1->jumpPress = self->timer > 24 && !(self->timer & 7);
    if (self->timer == 56) {
        self->timer = 0;
        player1->down = false;
        self->state = HeavyKing_State_CutsceneUnknown9;
        Music_TransitionTrack(TRACK_EGGMAN2, 0.0125);
    }
}

void HeavyKing_State_CutsceneUnknown9(void)
{
    RSDK_THIS(HeavyKing);
    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);

    HeavyKing_HandleClawMovement();
    if (++self->timer == 2) {
        player1->jumpPress   = true;
        player1->jumpHold    = true;
        player1->scrollDelay = 160;
        if (self->masterEmerald)
            Camera_SetupLerp(2, 0, self->masterEmerald->position.x, self->masterEmerald->position.y + 0x300000, 4);
    }
    else {
        player1->jumpPress = false;
    }

    if (self->timer == 14) {
        self->timer      = 0;
        self->velocity.y = -0x20000;
        self->velocity.x = 0x10000;
        self->state      = HeavyKing_State_CutsceneUnknown10;
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 6, &self->animator1, false, 0);
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 15, &self->animator2, false, 0);
        player1->velocity.x = -0x24000;
        player1->velocity.y = -0x20000;

        EntityKingClaw *claw = (EntityKingClaw *)self->claw;
        claw->state          = KingClaw_Unknown5;
        claw->timer          = 128;
        if (claw->masterEmerald)
            claw->masterEmerald->onGround = false;
        RSDK.PlaySfx(HeavyKing->sfxImpact5, false, 255);
    }
}

void HeavyKing_State_CutsceneUnknown10(void)
{
    RSDK_THIS(HeavyKing);

    self->velocity.y += 0x3800;
    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;

    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    if (player1->onGround)
        player1->groundVel = 0;

    if (++self->timer == 24) {
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 3, &self->animator1, false, 6);
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 12, &self->animator2, false, 6);
    }

    if (self->onGround) {
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 4, &self->animator1, false, 0);
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 13, &self->animator2, false, 0);
        HeavyKing_Unknown8(0);
#if RETRO_USE_PLUS
        Player->disableP2KeyCheck = false;
#endif
        player1->stateInput       = Player_ProcessP1Input;

        EntityPlayer *player2 = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
        if (player2->objectID == Player->objectID) {
            CutsceneSeq_LockPlayerControl(player2);
            player2->stateInput = Player_ProcessP2Input_AI;
        }
    }
}

void HeavyKing_State_Unknown13(void)
{
    RSDK_THIS(HeavyKing);

    HeavyKing_HandleAnimators();

    if (self->onGround) {
        HeavyKing_Unknown8(4);
        if (!self->field_70) {
            EntityHPZEmerald *masterEmerald = (EntityHPZEmerald *)self->masterEmerald;
            if (abs(self->position.x - HeavyKing->boundsM) < 0x100000) {
                masterEmerald->solid = true;
                self->state       = HeavyKing_State_Unknown2;
            }
        }
    }
    else {
        EntityHPZEmerald *targetEmerald = (EntityHPZEmerald *)self->targetEmerald;
        if (targetEmerald)
            self->position.x += (targetEmerald->position.x - self->position.x) >> 3;
        self->velocity.y += 0x3800;
        self->position.y += self->velocity.y;
    }
    HeavyKing_CheckPlayerCollisions();
}

void HeavyKing_State_Unknown1(void)
{
    RSDK_THIS(HeavyKing);

    if (self->animator1.frameID >= 5) {
        HeavyKing_HandleAnimators();
        RSDK.ObjectTileCollision(self, Zone->fgLayers, CMODE_FLOOR, 0, 0, HeavyKing->hitbox1.bottom >> 15, true);
        if (!self->onGround) {
            int vel = self->velocity.y;
            self->velocity.y += 0x3800;
            if (self->velocity.y >= 0 && vel < 0)
                HeavyKing_FindTargetEmerald();

            self->position.x += self->velocity.x;

            EntityHPZEmerald *targetEmerald = (EntityHPZEmerald *)self->targetEmerald;
            if (self->velocity.y > 0) {
                if (self->velocity.x <= 0) {
                    if (self->velocity.x < 0) {
                        if (self->position.x < targetEmerald->position.x) {
                            self->velocity.x = 0;
                            self->position.x = targetEmerald->position.x;
                        }
                    }
                }
                else {
                    if (self->position.x > targetEmerald->position.x) {
                        self->velocity.x = 0;
                        self->position.x = targetEmerald->position.x;
                    }
                }
            }
            self->position.y += self->velocity.y;
        }
        else {
            EntityHPZEmerald *masterEmerald = (EntityHPZEmerald *)self->masterEmerald;
            HeavyKing_Unknown8(4);
            if (!self->field_70) {
                if (abs(self->position.x - HeavyKing->boundsM) >= 0x200000) {
                    self->direction = self->position.x < masterEmerald->position.x;
                }
                else {
                    masterEmerald->solid = true;
                    self->state       = HeavyKing_State_Unknown2;
                }
            }
        }
    }
    HeavyKing_CheckPlayerCollisions();
}

void HeavyKing_State_Unknown2(void)
{
    RSDK_THIS(HeavyKing);

    if (self->animator1.frameID >= 5) {
        HeavyKing_HandleAnimators();
        EntityHPZEmerald *masterEmerald = (EntityHPZEmerald *)self->masterEmerald;

        if (!self->onGround) {
            self->velocity.y += 0x3800;
            self->position.y += self->velocity.y;
            self->position.x += (masterEmerald->position.x - self->position.x) >> 2;
        }
        else {
            masterEmerald->solid = false;
            RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 4, &self->animator1, false, 0);
            RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 13, &self->animator2, false, 0);
            foreach_active(KingAttack, attack)
            {
                if (attack->state == KingAttack_State2_Unknown2)
                    destroyEntity(attack);
            }
            self->state = HeavyKing_State_Unknown3;
        }
    }
    HeavyKing_CheckPlayerCollisions();
}

void HeavyKing_State_Unknown3(void)
{
    RSDK_THIS(HeavyKing);

    if (self->animator1.frameID == self->animator1.frameCount - 1) {
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 1, &self->animator1, false, 0);
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 10, &self->animator2, false, 0);
        self->state = HeavyKing_State_Unknown4;
    }
    HeavyKing_CheckPlayerCollisions();
}

void HeavyKing_State_Unknown4(void)
{
    RSDK_THIS(HeavyKing);

    if (self->animator2.frameID == 7 && self->animator2.timer == 1)
        RSDK.PlaySfx(HeavyKing->sfxRodPlant, false, 255);
    if (self->animator1.frameID == 12) {
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 18, &self->animator3, false, 0);
        self->state    = HeavyKing_State_Unknown5;
        self->field_70 = (self->health >= 2) + 1;
    }
    HeavyKing_CheckPlayerCollisions();
}

void HeavyKing_State_Unknown5(void)
{
    RSDK_THIS(HeavyKing);

    RSDK.ProcessAnimation(&self->animator3);
    ++self->timer;

    EntityHPZEmerald *masterEmerald = (EntityHPZEmerald *)self->masterEmerald;
    masterEmerald->alpha            = (RSDK.Sin256(8 * self->timer) >> 1) + 128;
    if (self->timer == 176) {
        self->timer        = 0;
        masterEmerald->alpha = 0;
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 2, &self->animator1, false, 0);
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 11, &self->animator2, false, 0);
        RSDK.SetSpriteAnimation(-1, 0, &self->animator3, false, 0);
        self->state = HeavyKing_State_Unknown6;
    }
    if (!(self->timer & 7))
        RSDK.PlaySfx(HeavyKing->sfxCharge, false, 0xFF);
    HeavyKing_CheckPlayerCollisions_Charging();
}

void HeavyKing_State_Unknown6(void)
{
    RSDK_THIS(HeavyKing);

    if (self->animator1.frameID == self->animator1.frameCount - 1) {
        RSDK.Rand(0, 3);
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 3, &self->animator1, false, 0);
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 12, &self->animator2, false, 0);
        self->state = HeavyKing_State_Unknown7;
    }
    HeavyKing_CheckPlayerCollisions();
}

void HeavyKing_State_Unknown7(void)
{
    RSDK_THIS(HeavyKing);

    if (self->animator1.frameID == 5) {
        self->position.y -= 0x60000;
        self->originPos.x = self->position.x;
        self->originPos.y = self->position.y;
        self->velocity.x  = 0;
        self->velocity.y  = -0x10000;
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 5, &self->animator1, false, 0);
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 14, &self->animator2, false, 0);
        self->targetEmerald = NULL;
        self->onGround      = false;
        self->state         = HeavyKing_State_Unknown8;
    }
    HeavyKing_CheckPlayerCollisions();
}

void HeavyKing_State_Unknown8(void)
{
    RSDK_THIS(HeavyKing);

    self->velocity.y -= 0x800;
    self->originPos.y += self->velocity.y;
    BadnikHelpers_Oscillate(self->originPos.y, 2, 11);
    if (++self->timer == 24) {
        HeavyKing->value10 = self->originPos.y;
        self->timer      = 0;
        self->timer      = 0;
        self->state      = HeavyKing_State_Unknown9;
    }
    HeavyKing_CheckPlayerCollisions();
}

void HeavyKing_State_Unknown9(void)
{
    RSDK_THIS(HeavyKing);

    HeavyKing_Unknown6();
    BadnikHelpers_Oscillate(self->originPos.y, 2, 11);
    if (++self->timer == 120) {
        self->timer    = 0;
        self->field_6C = HeavyKing->attackPattern[HeavyKing->attackPatternPos++];
        HeavyKing->attackPatternPos &= 0x1F;
        if (self->field_70 > 0)
            self->field_70--;

        switch (self->field_6C) {
            case 0:
                HeavyKing_CreateSpinRay();
                RSDK.PlaySfx(HeavyKing->sfxRodShine, false, 255);
                self->state = HeavyKing_State_Unknown10;
                break;
            case 1:
                HeavyKing_CreateExpandRing();
                RSDK.PlaySfx(HeavyKing->sfxElecOn, false, 255);
                self->state = HeavyKing_State_Unknown11;
                break;
            case 2:
                --self->field_70;
                CREATE_ENTITY(KingAttack, intToVoid(4), self->position.x, self->position.y)->parent = (Entity *)self;
                RSDK.PlaySfx(HeavyKing->sfxTwinCharge, false, 255);
                self->state = HeavyKing_State_Unknown12;
                break;
        }
    }
    HeavyKing_CheckPlayerCollisions();
}

void HeavyKing_State_Unknown10(void)
{
    RSDK_THIS(HeavyKing);

    BadnikHelpers_Oscillate(self->originPos.y, 2, 11);
    if (++self->timer == 60)
        HeavyKing_CreateAttack();
    if (self->timer == 120) {
        self->timer = 0;
        if (self->field_70 <= 0)
            HeavyKing_Unknown9();
        else
            self->state = HeavyKing_State_Unknown9;
    }
    HeavyKing_CheckPlayerCollisions();
}

void HeavyKing_State_Unknown11(void)
{
    RSDK_THIS(HeavyKing);

    HeavyKing_Unknown6();
    BadnikHelpers_Oscillate(self->originPos.y, 2, 11);
    if (++self->timer == 16)
        HeavyKing_CreateAttackCircle();
    if (self->timer == 128) {
        self->timer = 0;
        HeavyKing_Unknown9();
    }
    HeavyKing_CheckPlayerCollisions();
}

void HeavyKing_State_Unknown12(void)
{
    RSDK_THIS(HeavyKing);

    BadnikHelpers_Oscillate(self->originPos.y, 2, 11);
    if (!RSDK.GetEntityCount(KingAttack->objectID, true))
        HeavyKing_Unknown9();
    HeavyKing_CheckPlayerCollisions();
}

void HeavyKing_State_HitRecoil(void)
{
    RSDK_THIS(HeavyKing);

    if (self->invincibilityTimer == 48) {
        self->animator1 = self->animator1Store;
        self->animator2 = self->animator2Store;
        self->state     = self->stateStore;
    }
}

void HeavyKing_State_Destroyed(void)
{
    RSDK_THIS(HeavyKing);

    if (!(Zone->timer & 7)) {
        RSDK.PlaySfx(HeavyKing->sfxExplosion, false, 255);
        if (!(Zone->timer & 0xF)) {
            int x = self->position.x + RSDK.Rand(-0x180000, 0x180000);
            int y = self->position.y + RSDK.Rand(-0x180000, 0x180000);
            CREATE_ENTITY(Explosion, intToVoid((RSDK.Rand(0, 256) > 192) + EXPLOSION_BOSS), x, y)->drawOrder = Zone->drawOrderHigh;
        }
    }

    RSDK.ObjectTileCollision(self, Zone->fgLayers, CMODE_FLOOR, 0, 0, HeavyKing->hitbox1.bottom >> 15, true);
    if (!self->onGround) {
        self->velocity.y += 0x3800;
        self->position.y += self->velocity.y;
    }
    if (++self->timer == 60)
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 17, &self->animator2, false, 0);

    if (self->timer == 72) {
        RSDK.PlaySfx(HeavyKing->sfxFreeze, false, 255);
        int centerX = self->position.x - 0x280000;
        if (self->direction == FLIP_NONE)
            centerX = self->position.x + 0x280000;

        for (int i = 0; i < 6; ++i) {
            int spawnY           = (self->position.y - 0x300000) + RSDK.Rand(-0x40000, 0x40000);
            int spawnX           = centerX + RSDK.Rand(-0x40000, 0x40000);
            EntityDebris *debris = CREATE_ENTITY(Debris, NULL, spawnX, spawnY);
            debris->state        = Debris_State_Fall;
            debris->gravity      = 0x3800;
            debris->velocity.x   = RSDK.Rand(0, 0x20000);
            if (debris->position.x < centerX)
                debris->velocity.x = -debris->velocity.x;
            debris->velocity.y = RSDK.Rand(-0x40000, -0x10000);
            debris->drawFX     = FX_FLIP;
            debris->direction  = i & 3;
            debris->drawOrder  = Zone->drawOrderHigh;
            debris->timer      = 120;
            RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 19, &debris->animator, true, RSDK.Rand(0, 3));
        }
    }
    if (self->timer == 120) {
        EntityKingClaw *claw             = (EntityKingClaw *)self->claw;
        self->animator1.speed = 0;
        self->animator2.speed = 0;
        self->timer                    = 0;
        self->state                    = HeavyKing_State_Escape;
        if (self->direction == FLIP_NONE)
            claw->position.x = self->position.x - 0x300000;
        else
            claw->position.x = self->position.x + 0x300000;
        claw->state       = KingClaw_Unknown6;
        claw->velocity.y  = 0x10000;
        claw->flag        = true;
        claw->targetY     = self->position.y - 0xA00000;
        claw->position.y  = claw->targetY - 0xE00000;
        claw->drawOrder   = Zone->drawOrderHigh;
        self->drawOrder = Zone->drawOrderHigh;
        KingClaw->active  = ACTIVE_NEVER;
    }
}

void HeavyKing_State_Escape(void)
{
    RSDK_THIS(HeavyKing);

    if (!(Zone->timer & 0x1F)) {
        int x                                                                    = self->position.x + RSDK.Rand(-0x180000, 0x180000);
        int y                                                                    = self->position.y + RSDK.Rand(-0x180000, 0x180000);
        CREATE_ENTITY(Explosion, intToVoid(EXPLOSION_BOSSPUFF), x, y)->drawOrder = Zone->drawOrderHigh;
    }

    if ((Zone->timer & 0xF) == 4) {
        int x = self->position.x - 0x2C0000;
        if (self->direction == FLIP_NONE)
            x = self->position.x + 0x2C0000;
        CREATE_ENTITY(Explosion, intToVoid(EXPLOSION_BOSSPUFF), x, self->position.y - 0x300000)->drawOrder = Zone->drawOrderHigh;
    }

    EntityKingClaw *claw = (EntityKingClaw *)self->claw;
    if (!claw->state) {
        claw->state = KingClaw_Unknown7;
        RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 7, &self->animator1, true, 0);
        RSDK.SetSpriteAnimation(-1, 0, &self->animator2, true, 0);
        self->state = HeavyKing_State_Finish;
    }
}

void HeavyKing_State_Finish(void)
{
    RSDK_THIS(HeavyKing);
    HeavyKing_HandleClawMovement();

    if (!RSDK.CheckOnScreen(self, &self->updateRange)) {
        Music_TransitionTrack(TRACK_STAGE, 0.0125);
        Zone->cameraBoundsR[0] += 1024;
        destroyEntity(self->claw);
        destroyEntity(self);
    }
}

#if RETRO_INCLUDE_EDITOR
void HeavyKing_EditorDraw(void)
{
    RSDK_THIS(HeavyKing);
    RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 7, &self->animator1, true, 0);
    RSDK.SetSpriteAnimation(HeavyKing->aniFrames, 16, &self->animator2, true, 0);

    HeavyKing_Draw();
}

void HeavyKing_EditorLoad(void) { HeavyKing->aniFrames = RSDK.LoadSpriteAnimation("LRZ3/HeavyKing.bin", SCOPE_STAGE); }
#endif

void HeavyKing_Serialize(void) {}
