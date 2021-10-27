#include "SonicMania.h"

ObjectProjectile *Projectile;

void Projectile_Update(void)
{
    RSDK_THIS(Projectile);
    StateMachine_Run(entity->state);
    if (entity->drawFX & FX_ROTATE)
        entity->rotation = (entity->rotation + entity->rotationSpeed) & 0x1FF;
    RSDK.ProcessAnimation(&entity->animator);
}

void Projectile_LateUpdate(void) {}

void Projectile_StaticUpdate(void) {}

void Projectile_Draw(void)
{
    RSDK_THIS(Projectile);
    RSDK.DrawSprite(&entity->animator, NULL, false);
}

void Projectile_Create(void *data)
{
    RSDK_THIS(Projectile);
    entity->active  = ACTIVE_NORMAL;
    entity->visible = true;
    entity->state   = (Type_StateMachine)data;
}

void Projectile_StageLoad(void) {}

void Projectile_CheckPlayerCollisions(void)
{
    RSDK_THIS(Projectile);

    foreach_active(Player, player)
    {
        if (Player_CheckCollisionTouch(player, entity, &entity->hitbox)) {
            switch (entity->type) {
                case 0: break;
                case 1:
                    if (entity->isProjectile)
                        Player_CheckProjectileHit(player, entity);
                    else
                        Player_CheckElementalHit(player, entity, SHIELD_FIRE);
                    break;
                case 2:
                    if (entity->isProjectile)
                        Player_CheckProjectileHit(player, entity);
                    else
                        Player_CheckElementalHit(player, entity, SHIELD_LIGHTNING);
                    break;
                case 3: break;
                case 5: break;
                case 6: break;
                case 4:
                case 7: {
                    int32 anim = player->playerAnimator.animationID;
                    if (entity->isProjectile
#if RETRO_USE_PLUS
                        || (player->characterID == ID_MIGHTY
                            && (anim == ANI_CROUCH || anim == ANI_JUMP || anim == ANI_SPINDASH || anim == ANI_DROPDASH))
#endif
                    ) {
                        if (Player_CheckProjectileHit(player, entity)) {
                            entity->gravityStrength = 0x3800;
                            entity->state           = Projectile_State_MoveGravity;
                        }
                    }
                    else {
                        Player_CheckHit(player, entity);
                    }
                    break;
                }
                default: break;
            }
        }
    }
}

void Projectile_State_Move(void)
{
    RSDK_THIS(Projectile);
    entity->position.x += entity->velocity.x;
    entity->position.y += entity->velocity.y;
    if (entity->hurtDelay <= 0)
        Projectile_CheckPlayerCollisions();
    else
        entity->hurtDelay--;

    if (entity->timer <= 0) {
        if (!RSDK.CheckOnScreen(entity, NULL))
            destroyEntity(entity);
    }
    else {
        if (!entity->timer)
            destroyEntity(entity);
    }
}

void Projectile_State_MoveGravity(void)
{
    RSDK_THIS(Projectile);
    entity->position.x += entity->velocity.x;
    entity->position.y += entity->velocity.y;
    entity->velocity.y += entity->gravityStrength;
    if (entity->hurtDelay <= 0)
        Projectile_CheckPlayerCollisions();
    else
        entity->hurtDelay--;

    if (entity->timer <= 0) {
        if (!RSDK.CheckOnScreen(entity, NULL))
            destroyEntity(entity);
    }
    else {
        if (!entity->timer)
            destroyEntity(entity);
    }
}

#if RETRO_INCLUDE_EDITOR
void Projectile_EditorDraw(void) {}

void Projectile_EditorLoad(void) {}
#endif

void Projectile_Serialize(void) {}
