#include "RSDK/Core/RetroEngine.hpp"

#if RETRO_REV02
RSDK::ObjectDevOutput *RSDK::DevOutput;

void RSDK::DevOutput_Update()
{
    EntityDevOutput *entity = (EntityDevOutput *)sceneInfo.entity;

    switch (entity->type) {
        case 0:
            if (entity->id <= 0)
                entity->type = 1;
            else
                entity->id--;
            break;
        case 1:
            if (entity->position.y >= 0)
                entity->type = 2;
            else
                entity->position.y += 2;
            break;
        case 2:
            if (entity->id >= 120)
                entity->type = 3;
            else
                entity->id++;
            break;
        case 3:
            entity->position.y -= 2;
            if (-entity->position.y > entity->yOffset)
                ResetEntityPtr(entity, 0, NULL);
            break;
        default: break;
    }
}

void RSDK::DevOutput_LateUpdate() {}
void RSDK::DevOutput_StaticUpdate() {}

void RSDK::DevOutput_Draw()
{
    EntityDevOutput *entity = (EntityDevOutput *)sceneInfo.entity;
    DrawRectangle(0, 0, currentScreen->size.x, entity->position.y + entity->yOffset, 128, 255, INK_NONE, true);
    DrawDevText(entity->message, 8, entity->position.y + 8, 0, 0xF0F0F0);
}

void RSDK::DevOutput_Create(void *source)
{
    EntityDevOutput *entity = (EntityDevOutput *)sceneInfo.entity;
    entity->active          = ACTIVE_ALWAYS;
    entity->visible         = true;
    entity->isPermanent     = true;
    entity->drawOrder       = 15;
    strncpy(entity->message, (char *)source, 0x3F4);
    entity->id         = 180 * GetEntityCount(RSDK::DevOutput->objectID, 0);
    entity->yOffset    = DevOutput_GetStringYOffset(entity->message);
    entity->position.y = -entity->yOffset;
}

void RSDK::DevOutput_StageLoad() {}

int RSDK::DevOutput_GetStringYOffset(char *string)
{
    char cur = *string;
    int v2   = 0;
    int v4   = 0;

    if (!*string)
        return 24;

    do {
        bool32 isNewLine = cur == '\n';
        cur              = *++string;
        v4               = v2 + 1;
        if (!isNewLine)
            v4 = v2;
        v2 = v4;
    } while (cur);
    if (v4 >= 1)
        return 8 * v4 + 16;
    else
        return 24;
}

void RSDK::DevOutput_EditorDraw() {}
void RSDK::DevOutput_EditorLoad() {}
void RSDK::DevOutput_Serialize() {}
#endif
