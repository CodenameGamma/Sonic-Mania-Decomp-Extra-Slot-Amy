#include "SonicMania.h"

ObjectScarab *Scarab;

void Scarab_Update(void)
{
    RSDK_THIS(Scarab);
    RSDK.ProcessAnimation(&self->animator1);
    RSDK.ProcessAnimation(&self->animator2);
    RSDK.ProcessAnimation(&self->animator3);
    self->moveOffset.x = -self->position.x;
    self->moveOffset.y = -self->position.y;

    StateMachine_Run(self->state);

    self->moveOffset.x += self->position.x;
    self->moveOffset.y += self->position.y;
    Scarab_CheckPlayerCollisions();
    Scarab_HandleChildMove();
    Scarab_HandlePlayerGrab();
    if (self->state != Scarab_State_Setup) {
        if (!RSDK.CheckOnScreen(self, NULL) && !RSDK.CheckPosOnScreen(&self->startPos, &self->updateRange)) {
            int32 x                = -self->position.x;
            int32 y                = -self->position.y;
            self->direction    = self->startDir;
            self->position     = self->startPos;
            self->moveOffset.x = self->position.x + x;
            self->moveOffset.y = self->position.y + y;
            Scarab_HandleChildMove();
            Scarab_Create(NULL);
        }
    }
}

void Scarab_LateUpdate(void) {}

void Scarab_StaticUpdate(void)
{
    foreach_active(Scarab, scarab) { RSDK.AddDrawListRef(scarab->drawOrderHigh, RSDK.GetEntityID(scarab)); }
}

void Scarab_Draw(void)
{
    RSDK_THIS(Scarab);
    if (SceneInfo->currentDrawGroup == self->drawOrderHigh) {
        RSDK.DrawSprite(&self->animator3, NULL, false);
    }
    else {
        RSDK.DrawSprite(&self->animator2, NULL, false);
        RSDK.DrawSprite(&self->animator1, NULL, false);
    }
}

void Scarab_Create(void *data)
{
    RSDK_THIS(Scarab);
    self->visible   = true;
    self->drawOrder = Zone->drawOrderLow;

    if (!self->planeFilter) {
        self->drawOrderLow  = Zone->drawOrderLow;
        self->drawOrderHigh = Zone->playerDrawHigh;
    }
    if ((uint8)(self->planeFilter - 1) & 2) {
        self->drawOrderLow  = Zone->drawOrderHigh;
        self->drawOrderHigh = Zone->playerDrawHigh;
    }
    else {
        self->drawOrderLow  = Zone->drawOrderLow;
        self->drawOrderHigh = Zone->playerDrawLow;
    }

    self->drawFX        = FX_FLIP;
    self->active        = ACTIVE_BOUNDS;
    self->updateRange.x = 0x800000;
    self->updateRange.y = 0x800000;
    if (!self->amplitude)
        self->amplitude = 100;
    self->startPos = self->position;
    self->startDir = self->direction;
    self->state    = Scarab_State_Setup;
}

void Scarab_StageLoad(void)
{
    if (RSDK.CheckStageFolder("MMZ"))
        Scarab->aniFrames = RSDK.LoadSpriteAnimation("MMZ/Scarab.bin", SCOPE_STAGE);
    Scarab->hitboxBadnik.left   = -16;
    Scarab->hitboxBadnik.top    = -14;
    Scarab->hitboxBadnik.right  = 14;
    Scarab->hitboxBadnik.bottom = 6;
    Scarab->hitboxGrab.left     = -48;
    Scarab->hitboxGrab.top      = -14;
    Scarab->hitboxGrab.right    = -17;
    Scarab->hitboxGrab.bottom   = 6;
    Scarab->active              = ACTIVE_ALWAYS;
    DEBUGMODE_ADD_OBJ(Scarab);
}

void Scarab_DebugSpawn(void)
{
    RSDK_THIS(Scarab);
    EntityScarab *scarab = CREATE_ENTITY(Scarab, NULL, self->position.x, self->position.y);
    scarab->direction    = self->direction;
    scarab->startDir     = self->direction;
}

void Scarab_DebugDraw(void)
{
    RSDK.SetSpriteAnimation(Scarab->aniFrames, 4, &DebugMode->animator, true, 0);
    RSDK.DrawSprite(&DebugMode->animator, NULL, false);

    RSDK.SetSpriteAnimation(Scarab->aniFrames, 0, &DebugMode->animator, true, 0);
    RSDK.DrawSprite(&DebugMode->animator, NULL, false);

    RSDK.SetSpriteAnimation(Scarab->aniFrames, 2, &DebugMode->animator, true, 0);
    RSDK.DrawSprite(&DebugMode->animator, NULL, false);
}

void Scarab_CheckPlayerCollisions(void)
{
    RSDK_THIS(Scarab);

    foreach_active(Player, player)
    {
        if (self->planeFilter <= 0 || player->collisionPlane == ((uint8)(self->planeFilter - 1) & 1)) {
            int32 playerID = RSDK.GetEntityID(player);
            if (Player_CheckBadnikTouch(player, self, &Scarab->hitboxBadnik)) {
                Scarab_HandlePlayerRelease();
                Player_CheckBadnikBreak(self, player, true);
            }

            if (!self->childCount) {
                if (Player_CheckCollisionTouch(player, self, &Scarab->hitboxGrab)) {
                    if (!((1 << playerID) & self->grabbedPlayers) && !self->playerTimers[playerID]) {
                        self->grabbedPlayers |= (1 << playerID);
                        RSDK.SetSpriteAnimation(player->aniFrames, ANI_JUMP, &player->animator, false, 0);
                        player->groundVel       = 0;
                        player->onGround        = false;
                        player->tileCollisions  = false;
                        player->state           = Player_State_None;
                        player->nextAirState    = StateMachine_None;
                        player->nextGroundState = StateMachine_None;
                        self->isPermanent     = true;
                    }
                }
            }
        }
    }
}

void Scarab_HandleChildMove(void)
{
    RSDK_THIS(Scarab);

    int32 slot = SceneInfo->entitySlot + 1;
    for (int32 c = 0; c < self->childCount; ++c) {
        EntityItemBox *child = RSDK_GET_ENTITY(slot + c, ItemBox);
        child->position.x += self->moveOffset.x;
        child->position.y += self->moveOffset.y;
        if (child->objectID == ItemBox->objectID)
            child->moveOffset = self->moveOffset;
    }
}

void Scarab_HandlePlayerGrab(void)
{
    RSDK_THIS(Scarab);

    foreach_active(Player, player)
    {
        int32 playerID = RSDK.GetEntityID(player);
        if ((1 << playerID) & self->grabbedPlayers) {
            if (self->playerTimers[playerID] < 60 && player->interaction && player->state == Player_State_None) {
                player->position.x = self->position.x + ((2 * (self->direction != FLIP_NONE) - 1) << 21);
                player->position.y = self->position.y - 0xA0000;
                if (self->state == Scarab_State_Move)
                    player->velocity.x = 0x6000 * self->field_DB;
                else
                    player->velocity.x = 0;
                player->velocity.y = 0;
                ++self->playerTimers[playerID];
            }
            else {
                self->grabbedPlayers &= ~(1 << playerID);
                self->playerTimers[playerID] = -10;
                if (player->state != Player_State_FlyIn && player->state != Player_State_JumpIn) {
                    player->tileCollisions = true;
                    player->state          = Player_State_Air;
                    player->velocity.x     = 0xA0000 * (2 * (self->direction != FLIP_NONE) - 1);
                    player->velocity.y     = 0;
                }
                self->isPermanent = false;
            }
        }
        else {
            if (self->playerTimers[playerID] < 0)
                self->playerTimers[playerID]++;
        }
    }
}

void Scarab_HandlePlayerRelease(void)
{
    RSDK_THIS(Scarab);

    foreach_active(Player, player)
    {
        int32 playerID = RSDK.GetEntityID(player);

        if ((1 << playerID) & self->grabbedPlayers) {
            self->grabbedPlayers &= ~(1 << playerID);
            self->playerTimers[playerID] = -10;
            if (player->state != Player_State_FlyIn && player->state != Player_State_JumpIn) {
                player->tileCollisions = true;
                player->state          = Player_State_Air;
                player->velocity.x     = 0xA0000 * (2 * (self->direction != FLIP_NONE) - 1);
                player->velocity.y     = 0;
            }
            self->isPermanent = false;
        }
    }
}

void Scarab_State_Setup(void)
{
    RSDK_THIS(Scarab);

    self->active   = ACTIVE_NORMAL;
    self->state    = Scarab_State_Move;
    self->timer2   = 0;
    self->timer    = 0;
    self->field_DB = 2 * (self->direction != FLIP_NONE) - 1;
    RSDK.SetSpriteAnimation(Scarab->aniFrames, 1, &self->animator1, true, 0);
    RSDK.SetSpriteAnimation(Scarab->aniFrames, 5, &self->animator2, true, 0);
    RSDK.SetSpriteAnimation(Scarab->aniFrames, 3, &self->animator3, true, 0);
    Scarab_State_Move();
}

void Scarab_State_Move(void)
{
    RSDK_THIS(Scarab);
    if (self->animator3.frameID == self->animator3.frameCount - 1)
        ++self->timer2;
    if (self->timer2 == 9) {
        RSDK.SetSpriteAnimation(Scarab->aniFrames, 0, &self->animator1, true, 0);
        RSDK.SetSpriteAnimation(Scarab->aniFrames, 4, &self->animator2, true, 0);
        RSDK.SetSpriteAnimation(Scarab->aniFrames, 2, &self->animator3, true, 0);
        self->timer = 45;
        self->state = Scarab_State_Wait;
    }

    int32 mult      = self->field_DB;
    int32 amplitude = self->amplitude << 16;
    int32 x         = self->position.x + 0x6000 * mult;
    int32 y         = self->position.y;

    self->position.x = x;
    if (abs(x - self->startPos.x) < amplitude && RSDK.ObjectTileGrip(self, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x60000, 2)) {
        self->position.x = x;
        //self->position.y = y;
    }
    else {
        self->position.y = y;
        self->position.x = self->startPos.x + amplitude * mult;
        self->field_DB   = -self->field_DB;
    }
    RSDK.ObjectTileGrip(self, Zone->fgLayers, CMODE_FLOOR, 0, 0, 0x60000, 4);
}

void Scarab_State_Wait(void)
{
    RSDK_THIS(Scarab);

    if (self->timer <= 0) {
        RSDK.SetSpriteAnimation(Scarab->aniFrames, 1, &self->animator1, true, 0);
        RSDK.SetSpriteAnimation(Scarab->aniFrames, 5, &self->animator2, true, 0);
        RSDK.SetSpriteAnimation(Scarab->aniFrames, 3, &self->animator3, true, 0);
        self->timer2 = 0;
        self->state  = Scarab_State_Move;
    }
    else {
        self->timer--;
    }
}

void Scarab_EditorDraw(void)
{
    RSDK_THIS(Scarab);
    RSDK.SetSpriteAnimation(Scarab->aniFrames, 4, &self->animator1, true, 0);
    RSDK.DrawSprite(&self->animator1, NULL, false);

    RSDK.SetSpriteAnimation(Scarab->aniFrames, 0, &self->animator2, true, 0);
    RSDK.DrawSprite(&self->animator2, NULL, false);

    RSDK.SetSpriteAnimation(Scarab->aniFrames, 2, &self->animator3, true, 0);
    RSDK.DrawSprite(&self->animator3, NULL, false);
}

void Scarab_EditorLoad(void)
{
    Scarab->aniFrames = RSDK.LoadSpriteAnimation("MMZ/Scarab.bin", SCOPE_STAGE);

    RSDK_ACTIVE_VAR(Scarab, planeFilter);
    RSDK_ENUM_VAR("No Filter", PLANEFILTER_NONE);
    RSDK_ENUM_VAR("Plane A", PLANEFILTER_A);
    RSDK_ENUM_VAR("Plane B", PLANEFILTER_B);
}

void Scarab_Serialize(void)
{
    RSDK_EDITABLE_VAR(Scarab, VAR_UINT8, direction);
    RSDK_EDITABLE_VAR(Scarab, VAR_INT32, amplitude);
    RSDK_EDITABLE_VAR(Scarab, VAR_UINT8, childCount);
    RSDK_EDITABLE_VAR(Scarab, VAR_ENUM, planeFilter);
}
