#ifndef OBJ_UIPOPOVER_H
#define OBJ_UIPOPOVER_H

#include "SonicMania.h"

#if RETRO_USE_PLUS

#define UIPopover_OptionCount (4)

typedef enum {
    POPOVER_WATCH     = 7,
    POPOVER_CHALLENGE = 8,
    POPOVER_DELETE    = 9,
} PopoverOptions;

// Object Class
struct ObjectUIPopover {
    RSDK_OBJECT
    EntityUIPopover *activeEntity;
    EntityUIControl *storedControl;
    StateMachine(storedControlState);
};

// Entity Class
struct EntityUIPopover {
    RSDK_ENTITY
    StateMachine(state);
    int32 timer;
    int32 closeDelay;
    TextInfo unusedTextInfo;
    int32 buttonCount;
    Vector2 size;
    Vector2 drawPos; //unused, it's here cuz its based off UIDialog
    EntityUIControl *parent;
    Entity* storedEntity;
    uint8 frameIDs[UIPopover_OptionCount];
    StateMachine(callbacks[UIPopover_OptionCount]);
    bool32 closeOnSelect[UIPopover_OptionCount];
    EntityUIButton *buttons[UIPopover_OptionCount];
    StateMachine(closeCB);
    bool32 playEventSfx; // unused, so just a guess based on what this is in UIDialog, though theres about a 50/50 chance its right to some extent
    bool32 useAltColour; // unused, so just a guess based on what this is in UIDialog, though theres about a 50/50 chance its right to some extent
    uint8 triangleFlag;
};

// Object Struct
extern ObjectUIPopover *UIPopover;

// Standard Entity Events
void UIPopover_Update(void);
void UIPopover_LateUpdate(void);
void UIPopover_StaticUpdate(void);
void UIPopover_Draw(void);
void UIPopover_Create(void* data);
void UIPopover_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void UIPopover_EditorDraw(void);
void UIPopover_EditorLoad(void);
#endif
void UIPopover_Serialize(void);

// Extra Entity Functions
EntityUIPopover *UIPopover_CreatePopover(void);
void UIPopover_AddButton(EntityUIPopover *popover, uint8 frameID, void (*callback)(void), bool32 closeOnSelect);
void UIPopover_Setup(EntityUIPopover *popover, int32 posX, int32 posY);
void UIPopover_DrawSprites(void);
void UIPopover_SetupButtonPositions(void);
void UIPopover_Close(void);
bool32 UIPopover_BackPressCB(void);
void UIPopover_ButtonActionCB(void);
void UIPopover_State_Appear(void);
void UIPopover_State_Idle(void);
void UIPopover_State_Close(void);

#endif

#endif //!OBJ_UIPOPOVER_H
