#include "SonicMania.h"

ObjectMegaOctus *MegaOctus;

void MegaOctus_Update(void)
{
    RSDK_THIS(MegaOctus);
    StateMachine_Run(self->state);
}

void MegaOctus_LateUpdate(void) {}

void MegaOctus_StaticUpdate(void)
{
    foreach_active(MegaOctus, boss)
    {
        if (boss->type == MEGAOCTUS_ARM)
            RSDK.AddDrawListRef(Zone->drawOrderLow, RSDK.GetEntityID(boss));
    }
}

void MegaOctus_Draw(void)
{
    RSDK_THIS(MegaOctus);
    StateMachine_Run(self->stateDraw);
}

void MegaOctus_Create(void *data)
{
    RSDK_THIS(MegaOctus);

    self->drawFX = FX_FLIP;
    if (!SceneInfo->inEditor) {
        if (globals->gameMode < MODE_TIMEATTACK) {
            self->active = ACTIVE_BOUNDS;
            if (data)
                self->type = voidToInt(data);
            switch (self->type) {
                case MEGAOCTUS_BODY:
                    self->visible   = false;
                    self->drawOrder = Zone->drawOrderLow;
                    RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 0, &self->animator1, true, 0);
                    RSDK.SetSpriteAnimation(MegaOctus->eggmanFrames, 1, &MegaOctus->animator3, true, 0);
                    RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 1, &MegaOctus->animator1, true, 5);
                    RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 2, &MegaOctus->animator2, true, 5);
                    RSDK.SetSpriteAnimation(MegaOctus->hatchFrames, 0, &MegaOctus->animator4, true, 0);
                    RSDK.SetSpriteAnimation(MegaOctus->hatchFrames, 1, &MegaOctus->animator5, true, 0);
                    self->hitbox.left   = -33;
                    self->hitbox.top    = -37;
                    self->hitbox.right  = 33;
                    self->hitbox.bottom = 6;
                    self->origin.x      = self->position.x + 0x800000;
                    self->origin.y      = self->position.y;
                    self->updateRange.x = 0x800000;
                    self->updateRange.y = 0x800000;
                    MegaOctus->bossPtr  = (Entity *)self;
                    MegaOctus->field_8C   = 0;
                    self->state         = MegaOctus_State_SetupBounds;
                    self->stateDraw     = MegaOctus_StateDraw_Body;
                    break;
                case MEGAOCTUS_UNUSED1: break;
                case MEGAOCTUS_HARPOON:
                    self->active        = ACTIVE_NORMAL;
                    self->visible       = true;
                    self->drawOrder     = Zone->drawOrderLow;
                    self->hitbox.left   = -8;
                    self->hitbox.top    = -8;
                    self->hitbox.right  = 8;
                    self->hitbox.bottom = 8;
                    self->origin.x      = self->position.x;
                    self->origin.y      = self->position.y;
                    self->updateRange.x = 0x800000;
                    self->updateRange.y = 0x800000;
                    self->angle         = 0;
                    self->rotation      = 0;
                    RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 3, &self->animator1, true, 1);
                    break;
                case MEGAOCTUS_GUN:
                    self->active        = ACTIVE_NORMAL;
                    self->visible       = true;
                    self->updateRange.x = 0x800000;
                    self->updateRange.y = 0x800000;
                    self->drawOrder     = Zone->drawOrderLow + 1;
                    self->hitbox.left   = -8;
                    self->hitbox.top    = -8;
                    self->hitbox.right  = 8;
                    self->hitbox.bottom = 8;
                    self->field_6C      = self->position.y - 0x700000;
                    self->origin.x      = self->position.x;
                    self->origin.y      = self->position.y;
                    self->field_70      = 256;
                    self->shotCount     = 3;
                    RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 3, &self->animator1, true, 1);
                    RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 4, &self->animator2, true, 3);
                    self->velocity.y = -0x10000;
                    self->state      = MegaOctus_StateGun_Unknown1;
                    self->stateDraw  = MegaOctus_StateDraw_Gun;
                    break;
                case MEGAOCTUS_ORB:
                    self->active        = ACTIVE_NORMAL;
                    self->visible       = true;
                    self->updateRange.x = 0x800000;
                    self->updateRange.y = 0x800000;
                    self->drawOrder     = Zone->drawOrderLow + 1;
                    self->hitbox.left   = -16;
                    self->hitbox.top    = -16;
                    self->hitbox.right  = 16;
                    self->hitbox.bottom = 16;
                    self->field_6C      = self->position.y - 0x1000000;
                    self->origin.x      = self->position.x;
                    self->origin.y      = self->position.y;
                    RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 3, &self->animator1, true, 1);
                    RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 5, &self->animator2, true, 0);
                    self->velocity.y = -0x18000;
                    self->state      = MegaOctus_StateOrb_Wait;
                    self->stateDraw  = MegaOctus_StateDraw_Orb;
                    break;
                case MEGAOCTUS_ARM:
                    self->active        = ACTIVE_NORMAL;
                    self->visible       = true;
                    self->updateRange.x = 0x800000;
                    self->drawOrder     = Zone->drawOrderLow + 1;
                    self->hitbox.left   = -8;
                    self->hitbox.top    = -8;
                    self->hitbox.right  = 8;
                    self->hitbox.bottom = 8;
                    self->origin.x      = self->position.x;
                    self->origin.y      = self->position.y;
                    self->updateRange.y = 0x800000;
                    self->angle         = 0;
                    self->rotation      = 0;
                    RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 3, &self->animator1, true, 1);
                    self->state     = MegaOctus_StateArm_Unknown1;
                    self->stateDraw = MegaOctus_StateDraw_Arm_Unknown1;
                    break;
                case MEGAOCTUS_LASER:
                    self->active        = ACTIVE_NORMAL;
                    self->visible       = true;
                    self->drawOrder     = Zone->drawOrderLow;
                    self->hitbox.left   = -16;
                    self->hitbox.top    = -1;
                    self->hitbox.right  = 16;
                    self->hitbox.bottom = 1;
                    self->updateRange.x = 0x800000;
                    self->updateRange.y = 0x800000;
                    RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 7, &self->animator1, true, 0);
                    self->state     = MegaOctus_State_Laser;
                    self->stateDraw = MegaOctus_StateDraw_Laser;
                    break;
                case MEGAOCTUS_ORBSHOT:
                    self->active        = ACTIVE_NORMAL;
                    self->visible       = true;
                    self->drawOrder     = Zone->drawOrderLow;
                    self->hitbox.left   = -3;
                    self->hitbox.top    = -3;
                    self->hitbox.right  = 3;
                    self->hitbox.bottom = 3;
                    self->updateRange.x = 0x400000;
                    self->updateRange.y = 0x400000;
                    RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 6, &self->animator1, true, 0);
                    self->state     = MegaOctus_State_Shot;
                    self->stateDraw = MegaOctus_StateDraw_OrbShot;
                    break;
                case MEGAOCTUS_UNUSED8: break;
                case MEGAOCTUS_LASERFIRE:
                    self->active        = ACTIVE_NORMAL;
                    self->visible       = true;
                    self->drawOrder     = Zone->drawOrderLow;
                    self->hitbox.left   = -8;
                    self->hitbox.top    = -4;
                    self->hitbox.right  = 8;
                    self->hitbox.bottom = 0;
                    self->updateRange.x = 0x800000;
                    self->updateRange.y = 0x800000;
                    RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 8, &self->animator1, true, 0);
                    self->state     = MegaOctus_State_LaserFire;
                    self->stateDraw = MegaOctus_StateDraw_Laser;
                    break;
                default: return;
            }
        }
        else {
            MegaOctus->bossPtr = NULL;
            destroyEntity(self);
        }
    }
}

void MegaOctus_StageLoad(void)
{
    MegaOctus->active       = ACTIVE_ALWAYS;
    MegaOctus->bossPtr      = NULL;
    MegaOctus->aniFrames    = RSDK.LoadSpriteAnimation("OOZ/MegaOctus.bin", SCOPE_STAGE);
    MegaOctus->eggmanFrames = RSDK.LoadSpriteAnimation("Eggman/EggmanOOZ.bin", SCOPE_STAGE);
    MegaOctus->hatchFrames  = RSDK.LoadSpriteAnimation("OOZ/Hatch.bin", SCOPE_STAGE);

    MegaOctus->hitbox.left    = -16;
    MegaOctus->hitbox.top     = 22;
    MegaOctus->hitbox.right   = 16;
    MegaOctus->hitbox.bottom  = 30;
    MegaOctus->spawnHarpoon   = false;
    MegaOctus->orbHealth[0]   = 4;
    MegaOctus->orbHealth[1]   = 4;
    MegaOctus->field_D4       = false;
    MegaOctus->sfxBossHit     = RSDK.GetSfx("Stage/BossHit.wav");
    MegaOctus->sfxExplosion   = RSDK.GetSfx("Stage/Explosion2.wav");
    MegaOctus->sfxLaser       = RSDK.GetSfx("OOZ/OOZLaser.wav");
    MegaOctus->sfxBullet      = RSDK.GetSfx("OOZ/OOZBullet.wav");
    MegaOctus->sfxHarpoon     = RSDK.GetSfx("OOZ/Harpoon.wav");
    MegaOctus->sfxSurface     = RSDK.GetSfx("OOZ/OOZSurface.wav");
    MegaOctus->sfxLaserSplash = RSDK.GetSfx("OOZ/LaserSplash.wav");
}

void MegaOctus_CheckPlayerCollisions_Body(void)
{
    RSDK_THIS(MegaOctus);

    if (self->invincibilityTimer)
        self->invincibilityTimer--;

    foreach_active(Player, player)
    {
        int32 playerRadius     = 0x100000;
        EntityShield *shield = RSDK_GET_ENTITY(Player->playerCount + RSDK.GetEntityID(player), Shield);
        if (shield->objectID == Shield->objectID && shield->state == Shield_State_Insta)
            playerRadius = 0x160000;

        if (RSDK.CheckObjectCollisionTouchCircle(self, 0x300000, player, playerRadius)) {
            int32 angle = RSDK.ATan2(player->position.x - self->position.x, player->position.y - self->position.y);

            player->velocity.x += 80 * RSDK.Cos256(angle);
            if (self->invincibilityTimer || !Player_CheckBossHit(player, self)) {
                if (80 * RSDK.Sin256(angle) >= 0)
                    player->velocity.y -= 80 * RSDK.Sin256(angle);
                else
                    player->velocity.y -= -80 * RSDK.Sin256(angle);
            }
            else {
                if (self->health)
                    self->health--;
                if (self->health) {
                    RSDK.PlaySfx(MegaOctus->sfxBossHit, false, 255);
                    self->invincibilityTimer = 30;
                    MegaOctus->spawnHarpoon    = true;
                }
                else {
                    SceneInfo->timeEnabled = false;
                    Player_GiveScore(RSDK_GET_ENTITY(SLOT_PLAYER1, Player), 1000);
                    RSDK.PlaySfx(MegaOctus->sfxExplosion, false, 255);
                    self->invincibilityTimer = 120;
                    RSDK.SetSpriteAnimation(MegaOctus->eggmanFrames, 5, &MegaOctus->animator3, true, 0);
                    MegaOctus->field_D4 = true;
                    self->state       = MegaOctus_State_Destroyed;
                }
            }
        }
    }
}

void MegaOctus_HandleEggmanAnim(void)
{
    RSDK_THIS(MegaOctus);

    switch (MegaOctus->animator3.animationID) {
        case 0:
        case 1:
            if (MegaOctus->animator3.frameID >= MegaOctus->animator3.frameCount - 1) {
                if (self->invincibilityTimer) {
                    RSDK.SetSpriteAnimation(MegaOctus->eggmanFrames, 3, &MegaOctus->animator3, true, 0);
                }
                else {
                    bool32 laugh = false;
                    foreach_active(Player, player)
                    {
                        if (player->state == Player_State_Hit || player->state == Player_State_Die)
                            laugh = true;
                    }

                    if (laugh)
                        RSDK.SetSpriteAnimation(MegaOctus->eggmanFrames, 4, &MegaOctus->animator3, true, 0);
                }
            }
            break;
        case 2:
        case 4:
            if (self->invincibilityTimer) {
                RSDK.SetSpriteAnimation(MegaOctus->eggmanFrames, 3, &MegaOctus->animator3, true, 0);
            }
            else {
                bool32 laugh = false;
                foreach_active(Player, player)
                {
                    if (player->state == Player_State_Hit || player->state == Player_State_Die)
                        laugh = true;
                }

                if (MegaOctus->animator3.frameID >= MegaOctus->animator3.frameCount - 1) {
                    if (!laugh) {
                        uint8 anim = self->state == MegaOctus_State_Unknown4 ? 2 : 0;
                        RSDK.SetSpriteAnimation(MegaOctus->eggmanFrames, anim, &MegaOctus->animator3, true, 0);
                    }
                    else {
                        RSDK.SetSpriteAnimation(MegaOctus->eggmanFrames, 4, &MegaOctus->animator3, true, 8);
                    }
                }
            }
            break;
        case 3:
            if (!self->invincibilityTimer) {
                if (MegaOctus->animator3.frameID >= MegaOctus->animator3.frameCount - 1) {
                    uint8 anim = self->state == MegaOctus_State_Unknown4 ? 2 : 0;
                    RSDK.SetSpriteAnimation(MegaOctus->eggmanFrames, anim, &MegaOctus->animator3, true, 0);
                }
            }
            break;
        default: break;
    }
}

void MegaOctus_Explode(void)
{
    RSDK_THIS(MegaOctus);
    if (!(Zone->timer % 3)) {
        RSDK.PlaySfx(MegaOctus->sfxExplosion, false, 255);

        if (Zone->timer & 4) {
            int32 x = RSDK.Rand(-48, 48) << 16;
            int32 y = RSDK.Rand(-48, 48) << 16;
            EntityExplosion *explosion =
                CREATE_ENTITY(Explosion, intToVoid((RSDK.Rand(0, 256) > 192) + 2), self->position.x + x, self->position.y + y);
            explosion->drawOrder = Zone->drawOrderHigh + 2;
        }
    }
}

void MegaOctus_HandleDirectionChange(void)
{
    RSDK_THIS(MegaOctus);
    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);

    if (abs(player1->position.x - self->position.x) < 0x200000) {
        if (self->orbID > 10) {
            self->orbID--;
        }
        else if (self->orbID < 10)
            self->orbID++;
    }
    else {
        if (player1->position.x < self->position.x) {
            if (self->orbID)
                self->orbID--;
        }
        else if (self->orbID < 20)
            self->orbID++;
    }

    self->direction            = self->orbID < 10;
    MegaOctus->animator1.frameID = self->orbID >> 1;
    MegaOctus->animator2.frameID = self->orbID >> 1;
    MegaOctus->field_8C          = MegaOctus->field_90[MegaOctus->animator1.frameID];
}

void MegaOctus_State_SetupBounds(void)
{
    RSDK_THIS(MegaOctus);
    if (++self->timer >= 2) {
        self->timer               = 0;
        Zone->playerBoundActiveR[0] = true;
        Zone->cameraBoundsR[0]     = (self->position.x >> 16) + 448;
        Zone->playerBoundActiveB[0] = false;
        Zone->cameraBoundsB[0]     = (self->position.y >> 16) + 96;
        Zone->deathBoundary[0]      = Zone->cameraBoundsB[0] << 16;
#if RETRO_USE_PLUS
        Zone->cameraBoundsT[0] = Zone->cameraBoundsB[0] - 384;
#endif
        self->position.y += 0xC00000;
        self->active  = ACTIVE_NORMAL;
        self->visible = true;
        self->state   = MegaOctus_State_SetupArena;
    }
}

void MegaOctus_State_SetupArena(void)
{
    RSDK_THIS(MegaOctus);
    Zone->playerBoundActiveL[0] = true;
    Zone->cameraBoundsL[0]     = ScreenInfo->position.x;

    if (RSDK_GET_ENTITY(SLOT_PLAYER1, Player)->position.x > self->origin.x) {
        RSDK.GetSceneLayer(Zone->fgLow)->drawLayer[0] = 2;
        Zone->playerBoundActiveL[0]                   = true;
        Zone->cameraBoundsL[0]                       = (self->position.x >> 16) - 192;
        Music_TransitionTrack(TRACK_EGGMAN1, 0.0075);
        OOZSetup->flags      = false;
        self->velocity.y   = -0x40000;
        self->health       = 8;
        self->timer        = 60;
        EntityMegaOctus *arm = CREATE_ENTITY(MegaOctus, intToVoid(MEGAOCTUS_ARM), self->position.x + 0x800000, self->origin.y + 0x400000);
        arm->direction       = self->direction;
        arm->angle           = 128;

        arm            = CREATE_ENTITY(MegaOctus, intToVoid(MEGAOCTUS_ARM), self->position.x - 0x800000, self->origin.y + 0x400000);
        arm->direction = self->direction;
        arm->angle     = 128;

        self->state = MegaOctus_State_Unknown1;
    }
}

void MegaOctus_State_Unknown1(void) {}

void MegaOctus_State_Unknown2(void)
{
    RSDK_THIS(MegaOctus);

    self->position.y += self->velocity.y;
    self->velocity.y += RETRO_USE_PLUS ? 0x3800 : 0x6000;
    if (self->velocity.y >= 0x10000) {
        self->state      = MegaOctus_State_Unknown3;
        self->velocity.y = 0;
    }
    MegaOctus_CheckPlayerCollisions_Body();
}

void MegaOctus_State_Unknown3(void)
{
    RSDK_THIS(MegaOctus);

    RSDK.ProcessAnimation(&MegaOctus->animator5);
    RSDK.ProcessAnimation(&MegaOctus->animator3);
    MegaOctus_HandleEggmanAnim();
    MegaOctus_HandleDirectionChange();
    MegaOctus_CheckPlayerCollisions_Body();

    if (--self->timer <= 0) {
        self->timer = 60;
        RSDK.SetSpriteAnimation(MegaOctus->hatchFrames, 2, &MegaOctus->animator5, true, 0);
        if (MegaOctus->animator3.animationID < 2)
            RSDK.SetSpriteAnimation(MegaOctus->eggmanFrames, 2, &MegaOctus->animator3, true, 0);
        self->state = MegaOctus_State_Unknown4;
    }
}

void MegaOctus_State_Unknown4(void)
{
    RSDK_THIS(MegaOctus);

    RSDK.ProcessAnimation(&MegaOctus->animator5);
    RSDK.ProcessAnimation(&MegaOctus->animator3);
    MegaOctus_HandleEggmanAnim();
    MegaOctus->field_84 += 0x4800;
    MegaOctus->field_80 += MegaOctus->field_84;
    MegaOctus_HandleDirectionChange();
    MegaOctus_CheckPlayerCollisions_Body();

    if (--self->timer <= 0)
        self->state = MegaOctus_State_Unknown5;
}

void MegaOctus_State_Unknown5(void)
{
    RSDK_THIS(MegaOctus);

    self->position.y += self->velocity.y;
    self->velocity.y += RETRO_USE_PLUS ? 0x3800 : 0x6000;

    if (self->position.y >= self->origin.y + 0xC00000) {
        self->position.y = self->origin.y + 0xC00000;
        self->state      = MegaOctus_State_SpawnWeapons;
        if (MegaOctus->spawnHarpoon)
            self->timer = 480;
        else
            self->timer = 240;
    }
    MegaOctus_CheckPlayerCollisions_Body();
}

void MegaOctus_State_SpawnWeapons(void)
{
    RSDK_THIS(MegaOctus);

    if (RSDK_GET_ENTITY(SLOT_PLAYER1, Player)->position.x <= self->origin.x)
        self->position.x = self->origin.x - 0x800000;
    else
        self->position.x = self->origin.x + 0x800000;

    --self->timer;
    if (self->timer == 240) {
        MegaOctus->spawnHarpoon  = false;
        EntityMegaOctus *harpoon = CREATE_ENTITY(MegaOctus, intToVoid(MEGAOCTUS_HARPOON), self->position.x, self->origin.y + 0x300000);
        harpoon->direction       = self->direction;
        RSDK.PlaySfx(MegaOctus->sfxHarpoon, false, 255);
        if (RSDK_GET_ENTITY(SLOT_PLAYER1, Player)->position.x <= self->position.x) {
            harpoon->origin.x -= 0x800000;
            harpoon->state     = MegaOctus_State_HarpoonLeft;
            harpoon->stateDraw = MegaOctus_StateDraw_HarpoonLeft;
        }
        else {
            harpoon->origin.x += 0x800000;
            harpoon->state     = MegaOctus_State_HarpoonRight;
            harpoon->stateDraw = MegaOctus_StateDraw_HarpoonRight;
        }
    }
    else if (self->timer <= 0) {
        self->timer                                                                                                  = 480;
        CREATE_ENTITY(MegaOctus, intToVoid(MEGAOCTUS_GUN), self->position.x, self->origin.y + 0x400000)->direction = self->direction;
        self->state                                                                                                  = MegaOctus_State_SpawnOrbs;
    }
    MegaOctus_CheckPlayerCollisions_Body();
}

void MegaOctus_State_SpawnOrbs(void)
{
    RSDK_THIS(MegaOctus);
    MegaOctus_HandleDirectionChange();
    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    if (player1->position.x <= self->origin.x)
        self->position.x = self->origin.x - 0x800000;
    else
        self->position.x = self->origin.x + 0x800000;

    MegaOctus_CheckPlayerCollisions_Body();

    if (--self->timer <= 0) {
        EntityMegaOctus *arm = CREATE_ENTITY(MegaOctus, intToVoid(MEGAOCTUS_ARM), self->position.x + 0x800000, self->origin.y + 0x400000);
        arm->direction       = self->direction;
        arm->angle           = 128;

        arm            = CREATE_ENTITY(MegaOctus, intToVoid(MEGAOCTUS_ARM), self->position.x - 0x800000, self->origin.y + 0x400000);
        arm->direction = self->direction;
        arm->angle     = 128;

        if (MegaOctus->orbHealth[0] > 0) {
            EntityMegaOctus *orb = CREATE_ENTITY(MegaOctus, intToVoid(MEGAOCTUS_ORB), self->position.x + 0x380000, self->origin.y + 0x780000);
            orb->direction       = self->direction;
            orb->health          = MegaOctus->orbHealth[0];
            orb->orbID           = 0;
            if (self->position.x > self->origin.x)
                orb->velocity.x = 0;
            else
                orb->velocity.x = 136;
        }
        if (MegaOctus->orbHealth[1] > 0) {
            EntityMegaOctus *orb = CREATE_ENTITY(MegaOctus, intToVoid(MEGAOCTUS_ORB), self->position.x - 0x380000, self->origin.y + 0x780000);
            orb->direction       = self->direction;
            orb->health          = MegaOctus->orbHealth[1];
            orb->orbID           = 1;
            orb->angle           = 256;
            if (self->position.x > self->origin.x)
                orb->velocity.x = -136;
            else
                orb->velocity.x = 0;
        }
        self->timer = 120;
        self->state = MegaOctus_State_Unknown1;
    }
}

void MegaOctus_State_Destroyed(void)
{
    RSDK_THIS(MegaOctus);

    RSDK.ProcessAnimation(&MegaOctus->animator5);
    RSDK.ProcessAnimation(&MegaOctus->animator3);

    if (MegaOctus->field_84 > 0) {
        MegaOctus->field_84 += 0x4800;
        MegaOctus->field_80 += MegaOctus->field_84;
    }

    MegaOctus_Explode();
    if (--self->invincibilityTimer <= 0) {
        if (!MegaOctus->field_84)
            RSDK.SetSpriteAnimation(MegaOctus->hatchFrames, 2, &MegaOctus->animator5, true, 0);
        Music_TransitionTrack(TRACK_STAGE, 0.0125);
        self->timer = 0;
        self->state = MegaOctus_State_Finish;
    }
}

void MegaOctus_State_Finish(void)
{
    RSDK_THIS(MegaOctus);
    RSDK.ProcessAnimation(&MegaOctus->animator5);
    RSDK.ProcessAnimation(&MegaOctus->animator3);
    MegaOctus->field_84 += 0x4800;
    MegaOctus->field_80 += MegaOctus->field_84;
    self->position.y += 0x10000;

    if (!(Zone->timer & 7)) {
        int32 x                      = self->position.x + (RSDK.Rand(-48, 48) << 16);
        int32 y                      = self->position.y + (RSDK.Rand(-48, 48) << 16);
        EntityExplosion *explosion = CREATE_ENTITY(Explosion, intToVoid(3), x, y);
        explosion->drawOrder       = self->drawOrder;
    }

    if (++self->timer > 120) {
        bool32 flag = true;
        foreach_active(MegaOctus, boss)
        {
            if (boss->type == MEGAOCTUS_ARM) {
                flag = false;
                foreach_break;
            }
        }

        if (flag) {
            Zone->cameraBoundsR[0] += 424;
            destroyEntity(self);
        }
    }
}

void MegaOctus_StateDraw_Body(void)
{
    RSDK_THIS(MegaOctus);
    Vector2 drawPos;
    drawPos.x = self->position.x;
    drawPos.y = self->position.y;

    int32 val = abs(MegaOctus->field_8C) / 96;
    if (self->invincibilityTimer & 1) {
        RSDK.SetPaletteEntry(0, 128, 0xE0E0E0);
    }
    drawPos.x = self->position.x;
    drawPos.y -= 0x320000;
    RSDK.DrawSprite(&MegaOctus->animator5, &drawPos, false);

    Vector2 pos;
    pos   = drawPos;
    pos.y = drawPos.y + MegaOctus->field_80;
    RSDK.DrawSprite(&MegaOctus->animator3, &pos, false);

    self->animator1.frameID = 0;
    RSDK.DrawSprite(&self->animator1, 0, false);
    RSDK.DrawSprite(&MegaOctus->animator4, &drawPos, false);

    if (MegaOctus->field_8C < 0) {
        self->drawFX |= FX_SCALE;
        self->scale.y = 0x200;
        self->scale.x = 0x200 - (val << 9 >> 15);
    }
    drawPos = self->position;
    drawPos.x += val - 0x10000 + MegaOctus->field_8C + 16 * (val - 0x10000);
    self->animator1.frameID = 1;
    RSDK.DrawSprite(&self->animator1, &drawPos, false);

    drawPos.x += val * (MegaOctus->field_8C >> 18);
    self->animator1.frameID = 2;
    RSDK.DrawSprite(&self->animator1, &drawPos, false);

    drawPos.x -= val * (MegaOctus->field_8C >> 18);
    self->drawFX &= ~FX_SCALE;
    if (MegaOctus->field_8C > 0) {
        self->drawFX |= FX_SCALE;
        self->scale.y = 0x200;
        self->scale.x = 0x200 - (val << 9 >> 15);
    }

    drawPos.x += 34 * (0x10000 - val);
    self->animator1.frameID = 1;
    RSDK.DrawSprite(&self->animator1, &drawPos, false);

    drawPos.x += val * (MegaOctus->field_8C >> 18);
    self->animator1.frameID = 2;
    RSDK.DrawSprite(&self->animator1, &drawPos, false);

    drawPos.x -= val * (MegaOctus->field_8C >> 18);
    self->drawFX = FX_FLIP;
    RSDK.DrawSprite(&MegaOctus->animator2, NULL, false);
    RSDK.DrawSprite(&MegaOctus->animator1, NULL, false);

    self->animator1.frameID = 6;
    RSDK.DrawSprite(&self->animator1, NULL, false);
    RSDK.SetPaletteEntry(0, 128, 0x0000);
}

void MegaOctus_CheckPlayerCollisions_Unknown(void)
{
    RSDK_THIS(MegaOctus);
    self->position.x = 0x3400 * RSDK.Sin512(self->angle) + self->origin.x;
    self->position.y = 0x3400 * RSDK.Cos512(self->angle) + self->origin.y;

    foreach_active(Player, player)
    {
        if (Player_CheckCollisionTouch(player, self, &self->hitbox)) {
#if RETRO_USE_PLUS
            if (!Player_CheckMightyUnspin(0x300, player, 2, &player->uncurlTimer))
#endif
                Player_CheckHit(player, self);
        }
    }
}

void MegaOctus_State_HarpoonLeft(void)
{
    RSDK_THIS(MegaOctus);

    self->angle = (self->angle + 3) & 0x1FF;
    self->shotCount += 12;
    MegaOctus_CheckPlayerCollisions_Unknown();

    if (self->angle == 1)
        destroyEntity(self);
}

void MegaOctus_State_HarpoonRight(void)
{
    RSDK_THIS(MegaOctus);

    self->angle = (self->angle - 3) & 0x1FF;
    self->shotCount -= 12;
    MegaOctus_CheckPlayerCollisions_Unknown();

    if (self->angle == 2)
        destroyEntity(self);
}

void MegaOctus_StateDraw_HarpoonLeft(void)
{
    RSDK_THIS(MegaOctus);

    self->animator1.frameID = 1;
    int32 angle                 = (self->angle - 108) & 0x1FF;
    for (int32 i = 0; i < 9; ++i) {
        self->position.x = 0x3400 * RSDK.Sin512(angle) + self->origin.x;
        self->position.y = 0x3400 * RSDK.Cos512(angle) + self->origin.y;
        RSDK.DrawSprite(&self->animator1, NULL, false);
        angle = (angle + 12) & 0x1FF;
    }

    self->drawFX |= FX_ROTATE;
    self->direction         = FLIP_NONE;
    self->rotation          = (-256 - self->angle) & 0x1FF;
    self->animator1.frameID = 0;
    RSDK.DrawSprite(&self->animator1, NULL, false);
    self->drawFX &= ~FX_ROTATE;
}

void MegaOctus_StateDraw_HarpoonRight(void)
{
    RSDK_THIS(MegaOctus);

    self->animator1.frameID = 1;
    int32 angle                 = (self->angle + 108) & 0x1FF;
    for (int32 i = 0; i < 9; ++i) {
        self->position.x = 0x3400 * RSDK.Sin512(angle) + self->origin.x;
        self->position.y = 0x3400 * RSDK.Cos512(angle) + self->origin.y;
        RSDK.DrawSprite(&self->animator1, NULL, false);
        angle = (angle - 12) & 0x1FF;
    }

    self->drawFX |= FX_ROTATE;
    self->direction         = FLIP_X;
    self->rotation          = (-256 - self->angle) & 0x1FF;
    self->animator1.frameID = 0;
    RSDK.DrawSprite(&self->animator1, NULL, false);
    self->drawFX &= ~FX_ROTATE;
}

void MegaOctus_CheckPlayerCollisions_Gun(void)
{
    RSDK_THIS(MegaOctus);

    Vector2 pos;
    pos.x = (RSDK.Cos512(self->angle) << 10) + self->position.x;
    pos.y = (RSDK.Sin512(self->angle) << 8) + self->position.y;

    foreach_active(Player, player)
    {
        if (Player_CheckCollisionTouch(player, &pos, &self->hitbox)) {
#if RETRO_USE_PLUS
            if (!Player_CheckMightyUnspin(0x300, player, 2, &player->uncurlTimer))
#endif
            Player_CheckHit(player, self);
        }
    }
}

void MegaOctus_StateGun_Unknown1(void)
{
    RSDK_THIS(MegaOctus);
    self->position.y += self->velocity.y;
    self->angle = (self->angle + 6) & 0x1FF;
    RSDK.ProcessAnimation(&self->animator2);
    self->direction = RSDK_GET_ENTITY(SLOT_PLAYER1, Player)->position.x >= self->position.x;
    if (self->position.y <= self->field_6C) {
        self->shotCount = RETRO_USE_PLUS ? 2 : 3;
        self->timer     = 128;
        self->field_70  = 256;
        self->state     = MegaOctus_StateGun_Unknown2;
    }
    MegaOctus_CheckPlayerCollisions_Gun();
}

void MegaOctus_StateGun_Unknown2(void)
{
    RSDK_THIS(MegaOctus);
    self->angle = (self->angle + 6) & 0x1FF;
    RSDK.ProcessAnimation(&self->animator2);
    self->direction = RSDK_GET_ENTITY(SLOT_PLAYER1, Player)->position.x >= self->position.x;
    if (--self->timer <= 0) {
        if (self->shotCount <= 0) {
            self->velocity.y = 0x10000;
            self->state      = MegaOctus_StateGun_Unknown4;
        }
        else {
            int32 id = 0;
            while (id == self->field_70) id = RSDK.Rand(0, 4);

            self->field_70 = id;
            self->field_6C = MegaOctus->field_BC[id] + self->origin.y;
            self->state    = MegaOctus_StateGun_FireLaser;
            if (self->field_6C < self->position.y)
                self->velocity.y = -0x8000;
            else
                self->velocity.y = 0x8000;
        }
    }
    MegaOctus_CheckPlayerCollisions_Gun();
}

void MegaOctus_StateGun_FireLaser(void)
{
    RSDK_THIS(MegaOctus);
    self->position.y += self->velocity.y;
    self->angle = (self->angle + 6) & 0x1FF;
    RSDK.ProcessAnimation(&self->animator2);
    self->direction = RSDK_GET_ENTITY(SLOT_PLAYER1, Player)->position.x >= self->position.x;

    if ((self->velocity.y >= 0 && self->position.y < self->field_6C) || (self->velocity.y < 0 && self->position.y > self->field_6C)) {
        // TODO: cleanup
    }
    else {
        self->position.y = self->field_6C;
        RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 4, &self->animator2, true, 0);
        RSDK.PlaySfx(MegaOctus->sfxLaser, false, 255);

        --self->shotCount;
        self->timer          = 40;
        int32 x                  = (RSDK.Cos512(self->angle) << 10) + self->position.x;
        int32 y                  = (RSDK.Sin512(self->angle) << 9) + self->position.y;
        EntityMegaOctus *child = CREATE_ENTITY(MegaOctus, intToVoid(MEGAOCTUS_LASER), x, y);
        child->direction       = self->direction;
        if (self->direction)
            child->velocity.x = 0x40000;
        else
            child->velocity.x = -0x40000;
        child->position.x += child->velocity.x;
        child->parent = (Entity *)self;
        child->timer  = 11;
        self->state = MegaOctus_StateGun_Unknown2;
    }
    MegaOctus_CheckPlayerCollisions_Gun();
}

void MegaOctus_StateGun_Unknown4(void)
{
    RSDK_THIS(MegaOctus);
    self->angle = (self->angle + 6) & 0x1FF;
    RSDK.ProcessAnimation(&self->animator2);
    self->direction = RSDK_GET_ENTITY(SLOT_PLAYER1, Player)->position.x >= self->position.x;
    self->position.y += self->velocity.y;
    MegaOctus_CheckPlayerCollisions_Gun();

    if (self->position.y > self->origin.y)
        destroyEntity(self);
}
void MegaOctus_StateDraw_Gun(void)
{
    RSDK_THIS(MegaOctus);
    Vector2 drawPos;

    int32 angle = self->angle;
    int32 y     = self->position.y + 0x780000;

    for (int32 i = 0; i < 8; ++i) {
        drawPos.x = (RSDK.Cos512(angle) << 10) + self->position.x;
        drawPos.y = (RSDK.Sin512(angle) << 8) + y;
        RSDK.DrawSprite(&self->animator1, &drawPos, false);
        angle = (angle + 64) & 0x1FF;
        y -= 0xF0000;
    }

    drawPos.x = (RSDK.Cos512(angle) << 10) + self->position.x;
    drawPos.y = (RSDK.Sin512(angle) << 8) + y;
    RSDK.DrawSprite(&self->animator2, &drawPos, false);
}

void MegaOctus_CheckPlayerCollisions_Orb(void)
{
    RSDK_THIS(MegaOctus);

    Vector2 pos;
    pos.x = (RSDK.Cos512(self->angle) << 10) + self->position.x;
    pos.y = (RSDK.Sin512(self->angle) << 8) + self->position.y;

    if (self->invincibilityTimer) {
        self->invincibilityTimer--;
    }
    else {
        foreach_active(Player, player)
        {
            if (Player_CheckBadnikTouch(player, &pos, &self->hitbox) && Player_CheckBossHit(player, self)) {
                --self->health;
                --MegaOctus->orbHealth[self->orbID];
                if (self->health) {
                    RSDK.PlaySfx(MegaOctus->sfxBossHit, false, 255);
                    self->invincibilityTimer = 30;
                }
                else {
                    RSDK.PlaySfx(MegaOctus->sfxExplosion, false, 255);
                    self->invincibilityTimer = 60;
                    self->state              = MegaOctus_StateOrb_Destroyed;
                }
            }
        }
    }
}

void MegaOctus_StateOrb_Wait(void)
{
    RSDK_THIS(MegaOctus);
    self->position.y += self->velocity.y;

    self->angle = (self->angle + 6) & 0x1FF;
    RSDK.ProcessAnimation(&self->animator2);
    if (MegaOctus->field_D4) {
        self->invincibilityTimer = 60;
        self->state              = MegaOctus_StateOrb_Destroyed;
    }
    if (self->position.y <= self->field_6C) {
        self->timer = 60;
        self->state = MegaOctus_StateOrb_FireShot;
    }
    MegaOctus_CheckPlayerCollisions_Orb();
}

void MegaOctus_StateOrb_FireShot(void)
{
    RSDK_THIS(MegaOctus);
    self->angle = (self->angle + 6) & 0x1FF;
    RSDK.ProcessAnimation(&self->animator2);

    if (MegaOctus->field_D4) {
        self->invincibilityTimer = 60;
        self->state              = MegaOctus_StateOrb_Destroyed;
    }
#if RETRO_USE_PLUS
    if (self->shotCount > 0)
        self->position.x += self->velocity.x * RSDK.Sin1024(++self->field_68);
#endif
    if (--self->timer <= 0) {
        ++self->shotCount;

        int32 x = (RSDK.Cos512(self->angle) << 10) + self->position.x;
        int32 y = (RSDK.Sin512(self->angle) << 9) + self->position.y;

        int32 angle = 0;
#if RETRO_USE_PLUS
        if (MegaOctus->bossPtr->position.x <= x)
            angle = -3 * self->shotCount;
        else
            angle = 3 * self->shotCount;
        angle *= 4;

        EntityMegaOctus *shot = CREATE_ENTITY(MegaOctus, intToVoid(MEGAOCTUS_ORBSHOT), x, y);
        shot->velocity.x      = 0x300 * RSDK.Sin256(angle);
        shot->velocity.y      = 0x300 * RSDK.Cos256(angle);
#else
        EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
        angle                 = RSDK.ATan2(player1->position.x - x, player1->position.y - y);

        EntityMegaOctus *shot = CREATE_ENTITY(MegaOctus, intToVoid(MEGAOCTUS_ORBSHOT), x, y);
        shot->velocity.x      = 0x300 * RSDK.Cos256(angle);
        shot->velocity.y      = 0x300 * RSDK.Sin256(angle);
#endif

        RSDK.PlaySfx(MegaOctus->sfxBullet, false, 255);
        if (self->shotCount >= 4) {
            self->velocity.y = 0x10000;
            self->state      = MegaOctus_StateOrb_Idle;
        }
        else {
            self->timer = 60;
        }
    }
    MegaOctus_CheckPlayerCollisions_Orb();
}

void MegaOctus_StateOrb_Idle(void)
{
    RSDK_THIS(MegaOctus);

    self->angle = (self->angle + 6) & 0x1FF;
    RSDK.ProcessAnimation(&self->animator2);
    if (MegaOctus->field_D4) {
        self->invincibilityTimer = 60;
        self->state              = MegaOctus_StateOrb_Destroyed;
    }
    self->position.y += self->velocity.y;
    MegaOctus_CheckPlayerCollisions_Orb();

    if (self->position.y > self->origin.y)
        destroyEntity(self);
}

void MegaOctus_StateOrb_Destroyed(void)
{
    RSDK_THIS(MegaOctus);

    if (!(Zone->timer % 3)) {
        RSDK.PlaySfx(MegaOctus->sfxExplosion, false, 255);

        if (Zone->timer & 4) {
            int32 x                      = self->position.x + (RSDK.Cos512(self->angle) << 10) + (RSDK.Rand(-16, 16) << 16);
            int32 y                      = self->position.y + (RSDK.Sin512(self->angle) << 9) + (RSDK.Rand(-16, 16) << 16);
            EntityExplosion *explosion = CREATE_ENTITY(Explosion, intToVoid((RSDK.Rand(0, 256) > 192) + 2), x, y);
            explosion->drawOrder       = Zone->drawOrderHigh + 2;
        }
    }

    if (--self->invincibilityTimer <= 0) {
        int32 angle = self->angle;
        int32 y     = self->position.y + 0xF80000;

        EntityDebris *debris = NULL;
        for (int32 i = 0; i < 16; ++i) {
            debris =
                CREATE_ENTITY(Debris, Debris_State_FallAndFlicker, self->position.x + (RSDK.Cos512(angle) << 10), y + (RSDK.Sin512(angle) << 8));
            RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 3, &debris->animator, true, 1);
            debris->velocity.x    = RSDK.Rand(-6, 6) << 15;
            debris->velocity.y    = RSDK.Rand(-10, -6) << 15;
            debris->gravity       = 0x3800;
            debris->drawOrder     = Zone->drawOrderLow + 1;
            debris->updateRange.x = 0x400000;
            debris->updateRange.y = 0x400000;
            angle                 = (angle + 64) & 0x1FF;
            y -= 0xF0000;
        }

        debris = CREATE_ENTITY(Debris, Debris_State_FallAndFlicker, self->position.x + (RSDK.Cos512(angle) << 10), y + (RSDK.Sin512(angle) << 8));
        RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 9, &debris->animator, true, 0);
        debris->velocity.x    = -0x20000;
        debris->velocity.y    = -0x40000;
        debris->gravity       = 0x3800;
        debris->drawOrder     = Zone->drawOrderLow + 1;
        debris->updateRange.x = 0x400000;
        debris->updateRange.y = 0x400000;

        debris = CREATE_ENTITY(Debris, Debris_State_FallAndFlicker, self->position.x + (RSDK.Cos512(angle) << 10), y + (RSDK.Sin512(angle) << 8));
        RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 9, &debris->animator, true, 1);
        debris->velocity.x    = 0x20000;
        debris->velocity.y    = -0x40000;
        debris->gravity       = 0x3800;
        debris->drawOrder     = Zone->drawOrderLow + 1;
        debris->updateRange.x = 0x400000;
        debris->updateRange.y = 0x400000;

        debris = CREATE_ENTITY(Debris, Debris_State_FallAndFlicker, self->position.x + (RSDK.Cos512(angle) << 10), y + (RSDK.Sin512(angle) << 8));
        RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 9, &debris->animator, true, 2);
        debris->velocity.x    = -0x10000;
        debris->velocity.y    = -0x20000;
        debris->gravity       = 0x3800;
        debris->drawOrder     = Zone->drawOrderLow + 1;
        debris->updateRange.x = 0x400000;
        debris->updateRange.y = 0x400000;

        debris = CREATE_ENTITY(Debris, Debris_State_FallAndFlicker, self->position.x + (RSDK.Cos512(angle) << 10), y + (RSDK.Sin512(angle) << 8));
        RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 9, &debris->animator, true, 3);
        debris->velocity.x    = 0x10000;
        debris->velocity.y    = -0x20000;
        debris->gravity       = 0x3800;
        debris->drawOrder     = Zone->drawOrderLow + 1;
        debris->updateRange.x = 0x400000;
        debris->updateRange.y = 0x400000;

        destroyEntity(self);
    }
}

void MegaOctus_StateDraw_Orb(void)
{
    RSDK_THIS(MegaOctus);
    Vector2 drawPos;

    int32 angle = self->angle;
    int32 y     = self->position.y + 0xF80000;

    if (self->invincibilityTimer & 1)
        RSDK.SetPaletteEntry(0, 128u, 0xE0E0E0);

    for (int32 i = 0; i < 16; ++i) {
        drawPos.x = (RSDK.Cos512(angle) << 10) + self->position.x;
        drawPos.y = (RSDK.Sin512(angle) << 8) + y;
        RSDK.DrawSprite(&self->animator1, &drawPos, false);
        angle = (angle + 64) & 0x1FF;
        y -= 0xF0000;
    }

    drawPos.x = (RSDK.Cos512(angle) << 10) + self->position.x;
    drawPos.y = (RSDK.Sin512(angle) << 8) + y;
    RSDK.DrawSprite(&self->animator2, &drawPos, false);

    RSDK.SetPaletteEntry(0, 128, 0x000000);
}

void MegaOctus_StateArm_Unknown1(void)
{
    RSDK_THIS(MegaOctus);
    self->angle += 4;
    self->position.y -= 0xC000;
    self->shotCount += 0xC000;
    self->position.x = 0x1400 * RSDK.Cos256(self->angle) + self->origin.x;
    if (self->shotCount >= 0x4B0000) {
        if (!self->field_6C) {
            self->field_6C     = 1;
            EntityMegaOctus *arm = CREATE_ENTITY(MegaOctus, intToVoid(MEGAOCTUS_ARM), self->position.x, self->position.y + 0x40000);
            arm->direction       = self->direction;
            arm->state           = MegaOctus_StateArm_Unknown5;
            arm->stateDraw       = MegaOctus_StateDraw_Arm_Unknown2;
        }
        if (self->shotCount >= 0x71A000) {
            self->timer = 30;
            self->state = MegaOctus_StateArm_Unknown2;
        }
    }
}

void MegaOctus_StateArm_Unknown5(void)
{
    RSDK_THIS(MegaOctus);
    self->angle -= 4;
    self->position.x += 0xC000;
    self->shotCount += 0xC000;

    self->position.y = (RSDK.Sin256(self->angle) << 12) + self->origin.y;
    if (self->shotCount >= 0x270000) {
        foreach_active(TilePlatform, platform)
        {
            if (RSDK.CheckObjectCollisionTouchBox(self, &self->hitbox, platform, &platform->hitbox)) {
                self->parent = (Entity *)platform;
#if RETRO_USE_PLUS
                self->tilePlatY = platform->position.y;
#endif
                RSDK.CopyTileLayer(Zone->fgLow, (platform->position.x >> 20) - 4, (platform->position.y >> 20) - 2, Zone->moveLayer, 10, 1, 8, 5);
            }
        }

        int32 slot = RSDK.GetEntityID(MegaOctus->bossPtr) + 4;

        EntityCollapsingPlatform *collapsingPlatform = RSDK_GET_ENTITY(slot, CollapsingPlatform);
        collapsingPlatform->collapseDelay            = 24;
        collapsingPlatform->playerPos.x              = self->position.x;

        collapsingPlatform                = RSDK_GET_ENTITY(slot + 1, CollapsingPlatform);
        collapsingPlatform->collapseDelay = 24;
        collapsingPlatform->playerPos.x   = self->position.x;

        self->timer = 30;
        self->state = MegaOctus_StateArm_Unknown2;
    }
}

void MegaOctus_StateArm_Unknown2(void)
{
    RSDK_THIS(MegaOctus);
    if (--self->timer <= 0) {
#if !RETRO_USE_PLUS
        foreach_active(Player, player)
        {
            if (abs(player->position.x - player->position.x) < 0x400000 && player->groundedStore) {
                player->state      = Player_State_Air;
                player->velocity.y = -0x10000;
                player->onGround   = false;
            }
        }
#endif

        self->state = MegaOctus_StateArm_Unknown3;
    }
}

void MegaOctus_StateArm_Unknown3(void)
{
    RSDK_THIS(MegaOctus);
    EntityTilePlatform *parent = (EntityTilePlatform *)self->parent;

    self->origin.y += 0x60000;
    self->position.y += 0x60000;
    if (parent) {
#if RETRO_USE_PLUS
        parent->state      = MegaOctus_TilePlatformState_Unknown;
        parent->velocity.y = 0x60000;
        if (parent->drawPos.y - self->tilePlatY > 0x480000)
            parent->stateCollide = Platform_CollisionState_None;
#else
        parent->drawPos.y += 0x60000;
        parent->centerPos.y += 0x60000;
#endif
    }

    if (++self->timer >= 32) {
        EntityMegaOctus *boss = (EntityMegaOctus *)MegaOctus->bossPtr;
        boss->velocity.y      = RETRO_USE_PLUS ? -0x8F400 : -0xB8000;
        boss->state           = MegaOctus_State_Unknown2;

        MegaOctus->field_80 = 0;
        MegaOctus->field_84 = 0;
        RSDK.SetSpriteAnimation(MegaOctus->eggmanFrames, 1, &MegaOctus->animator3, true, 0);
        RSDK.SetSpriteAnimation(MegaOctus->hatchFrames, 1, &MegaOctus->animator5, true, 0);
        RSDK.PlaySfx(MegaOctus->sfxSurface, false, 255);

        EntityMegaOctus *parent = (EntityMegaOctus *)self->parent;
        if (parent) {
            self->timer = 384;
            self->state = MegaOctus_StateArm_Unknown4;
#if RETRO_USE_PLUS
            parent->state      = StateMachine_None;
            parent->position.y = self->tilePlatY + (self->timer << 15);
#endif
        }
        else {
            destroyEntity(self);
        }
    }
}

void MegaOctus_StateArm_Unknown4(void)
{
    RSDK_THIS(MegaOctus);
    EntityTilePlatform *parent = (EntityTilePlatform *)self->parent;

#if RETRO_USE_PLUS
    if (parent) {
        parent->state         = MegaOctus_TilePlatformState_Unknown;
        parent->timer      = 4;
        parent->velocity.y    = -0x8000;

        if (parent->drawPos.y > self->tilePlatY)
            --self->timer;
        else
            self->timer = 0;

        if (parent->drawPos.y - self->tilePlatY < 0x480000)
            parent->stateCollide = Platform_CollisionState_Tiles;
    }
#else
    parent->drawPos.y -= 0x8000;
    parent->centerPos.y -= 0x8000;
    --self->timer;
#endif

    if (self->timer <= 0) {
#if RETRO_USE_PLUS
        if (parent) {
            parent->state      = StateMachine_None;
            parent->drawPos.y  = self->tilePlatY;
            parent->velocity.y = 0;
        }
#endif
        RSDK.CopyTileLayer(Zone->fgLow, (parent->position.x >> 20) - 4, (parent->position.y >> 20) - 2, Zone->moveLayer, 1, 1, 8, 5);
        destroyEntity(self);
    }
}

void MegaOctus_StateDraw_Arm_Unknown1(void)
{
    RSDK_THIS(MegaOctus);
    int32 pos = 0xA0000;

    Vector2 drawPos;
    drawPos.x = self->position.x;
    drawPos.y = self->position.y;

    uint8 angle = self->angle;
    for (int32 i = self->shotCount; i > 0; i -= 0x6000) {
        pos += 0x6000;
        if (pos >= 0x60000) {
            if (i < 0x400000) {
                if ((SceneInfo->currentDrawGroup == Zone->drawOrderLow + 1 && angle < 0x80)
                    || (SceneInfo->currentDrawGroup == Zone->drawOrderLow && angle >= 0x80))
                    RSDK.DrawSprite(&self->animator1, &drawPos, false);
            }
            angle -= 32;
            drawPos.y += 0x60000;
            drawPos.x = 0x1400 * RSDK.Cos256(angle) + self->origin.x;
            pos -= 0x60000;
        }
    }
}

void MegaOctus_StateDraw_Arm_Unknown2(void)
{
    RSDK_THIS(MegaOctus);
    int32 pos = 0xA0000;

    Vector2 drawPos;
    drawPos.x = self->position.x;
    drawPos.y = self->position.y;
    int32 angle = self->angle;

    uint8 angle2 = angle - 64;
    int32 count   = (self->shotCount - 1) / 0x6000 + 1;
    for (int32 i = 0; i < count; ++i) {
        pos += 0x6000;
        if (pos >= 0x60000) {
            if ((SceneInfo->currentDrawGroup == Zone->drawOrderLow + 1 && angle2 < 0x80)
                || (SceneInfo->currentDrawGroup == Zone->drawOrderLow && angle2 >= 0x80))
                RSDK.DrawSprite(&self->animator1, &drawPos, false);
            angle2 += 32;
            drawPos.x -= 0x60000;
            angle += 32;
            drawPos.y = (RSDK.Sin256(angle) << 12) + self->origin.y;
            pos -= 0x60000;
        }
    }
}

void MegaOctus_State_Laser(void)
{
    RSDK_THIS(MegaOctus);
    RSDK.ProcessAnimation(&self->animator1);
    self->position.x += self->velocity.x;

    if (self->timer > 0) {
        self->timer--;
        self->position.y = (RSDK.Sin512(self->parent->angle) << 9) + self->parent->position.y;
    }

    foreach_active(Player, player)
    {
        if (Player_CheckCollisionTouch(player, self, &self->hitbox))
            Player_CheckHit(player, self);
    }

    if (self->activeScreens == 1) {
        if (RSDK.ObjectTileGrip(self, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0, 8)) {
            if (self->shotCount != self->position.x >> 20) {
                self->shotCount = self->position.x >> 20;
                RSDK.PlaySfx(MegaOctus->sfxLaserSplash, false, 255);
                CREATE_ENTITY(MegaOctus, intToVoid(MEGAOCTUS_LASERFIRE), (self->position.x & 0xFFF00000) + 0x80000, self->position.y);
            }
        }
    }

    if (!RSDK.CheckOnScreen(self, &self->updateRange))
        destroyEntity(self);
}

void MegaOctus_State_LaserFire(void)
{
    RSDK_THIS(MegaOctus);
    RSDK.ProcessAnimation(&self->animator1);

    foreach_active(Player, player)
    {
        if (Player_CheckCollisionTouch(player, self, &self->hitbox))
            Player_CheckHit(player, self);
    }

    if (self->animator1.frameID == self->animator1.frameCount - 1)
        destroyEntity(self);
}

void MegaOctus_StateDraw_Laser(void)
{
    RSDK_THIS(MegaOctus);
    RSDK.DrawSprite(&self->animator1, NULL, false);
}

void MegaOctus_State_Shot(void)
{
    RSDK_THIS(MegaOctus);
    if (MegaOctus->field_D4 || !RSDK.CheckOnScreen(self, &self->updateRange)) {
        destroyEntity(self);
    }
    else {
        RSDK.ProcessAnimation(&self->animator1);
        self->position.x += self->velocity.x;
        self->position.y += self->velocity.y;

        foreach_active(Player, player)
        {
            if (Player_CheckCollisionTouch(player, self, &self->hitbox))
                Player_CheckProjectileHit(player, self);
        }
    }
}

void MegaOctus_StateDraw_OrbShot(void)
{
    RSDK_THIS(MegaOctus);
    RSDK.DrawSprite(&self->animator1, NULL, false);
}

#if RETRO_USE_PLUS
void MegaOctus_TilePlatformState_Unknown(void)
{
    EntityTilePlatform *self = RSDK_GET_ENTITY(SceneInfo->entitySlot, TilePlatform);

    if (self->velocity.y <= 0x10000) {
        foreach_active(Player, player)
        {
            if ((1 << player->playerID) & self->stoodPlayers) {
                if (player->state != OOZSetup_PlayerState_OilPool) {
                    player->state = Player_State_Ground;
                    OOZSetup->activePlayers &= ~(1 << player->playerID);
                }
            }
        }
    }
    else {
        foreach_active(Player, player)
        {
            if ((1 << player->playerID) & self->stoodPlayers) {
                if (player->state != OOZSetup_PlayerState_OilPool)
                    player->velocity.y = self->velocity.y >> 1;
                player->onGround = false;
                self->stoodPlayers &= ~(1 << player->playerID);
            }
        }
    }

    self->drawPos.y += self->velocity.y;
}
#endif

#if RETRO_INCLUDE_EDITOR
void MegaOctus_EditorDraw(void)
{
    RSDK_THIS(MegaOctus);

    RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 0, &self->animator1, false, 0);
    RSDK.SetSpriteAnimation(MegaOctus->eggmanFrames, 1, &MegaOctus->animator3, false, 0);
    RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 1, &MegaOctus->animator1, false, 5);
    RSDK.SetSpriteAnimation(MegaOctus->aniFrames, 2, &MegaOctus->animator2, false, 5);
    RSDK.SetSpriteAnimation(MegaOctus->hatchFrames, 0, &MegaOctus->animator4, false, 0);
    RSDK.SetSpriteAnimation(MegaOctus->hatchFrames, 1, &MegaOctus->animator5, false, 0);

    MegaOctus_StateDraw_Body();
}

void MegaOctus_EditorLoad(void)
{
    MegaOctus->aniFrames    = RSDK.LoadSpriteAnimation("OOZ/MegaOctus.bin", SCOPE_STAGE);
    MegaOctus->eggmanFrames = RSDK.LoadSpriteAnimation("Eggman/EggmanOOZ.bin", SCOPE_STAGE);
    MegaOctus->hatchFrames  = RSDK.LoadSpriteAnimation("OOZ/Hatch.bin", SCOPE_STAGE);
}
#endif

void MegaOctus_Serialize(void) {}
