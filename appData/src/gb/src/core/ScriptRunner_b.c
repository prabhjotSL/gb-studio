// clang-format off
#pragma bank=4
// clang-format on

#include "ScriptRunner.h"
#include "MusicManager.h"
#include "FadeManager.h"
#include "BankData.h"
#include "BankManager.h"
#include "DataManager.h"
#include "UI.h"
#include "Math.h"
#include "Actor.h"
#include "GameTime.h"
#include "Core_Main.h"
#include "Collision.h"
#include "Input.h"
#include <rand.h>

#define RAM_START_PTR 0xA000
#define RAM_START_VARS_PTR 0xA0FF

UINT8 scriptrunner_bank = 4;

UBYTE *RAMPtr;
UINT16 actor_move_dest_x = 0;
UINT16 actor_move_dest_y = 0;
BYTE actor_move_dir_x = 0;
BYTE actor_move_dir_y = 0;
BYTE actor_move_speed = 1;

const SCRIPT_CMD script_cmds[] = {
    {Script_End_b, 0},                 // 0x00
    {Script_Text_b, 2},                // 0x01
    {Script_Goto_b, 2},                // 0x02
    {Script_IfFlag_b, 4},              // 0x03
    {Script_Noop_b, 0},                // 0x04
    {Script_SetFlag_b, 2},             // 0x05
    {Script_ClearFlag_b, 2},           // 0x06
    {Script_ActorSetDir_b, 1},         // 0x07
    {Script_ActorActivate_b, 1},       // 0x08
    {Script_CameraMoveTo_b, 3},        // 0x09
    {Script_CameraLock_b, 1},          // 0x0A
    {Script_Wait_b, 1},                // 0x0B
    {Script_FadeOut_b, 1},             // 0x0C
    {Script_FadeIn_b, 1},              // 0x0D
    {Script_LoadScene_b, 6},           // 0x0E
    {Script_ActorSetPos_b, 2},         // 0x0F
    {Script_ActorMoveTo_b, 2},         // 0x10
    {Script_ShowSprites_b, 0},         // 0x11
    {Script_HideSprites_b, 0},         // 0x12
    {Script_PlayerSetSprite_b, 1},     // 0x13
    {Script_ActorShow_b, 0},           // 0x14
    {Script_ActorHide_b, 0},           // 0x15
    {Script_ActorSetEmote_b, 1},       // 0x16
    {Script_CameraShake_b, 1},         // 0x17
    {Script_Noop_b, 0},                // 0x18
    {Script_ShowOverlay_b, 3},         // 0x19
    {Script_HideOverlay_b, 0},         // 0x1A
    {Script_Noop_b, 0},                // 0x1B
    {Script_OverlayMoveTo_b, 3},       // 0x1C
    {Script_AwaitInput_b, 1},          // 0x1D
    {Script_MusicPlay_b, 2},           // 0x1E
    {Script_MusicStop_b, 0},           // 0x1F
    {Script_ResetVariables_b, 0},      // 0x20
    {Script_NextFrame_b, 0},           // 0x21
    {Script_IncFlag_b, 2},             // 0x22
    {Script_DecFlag_b, 2},             // 0x23
    {Script_SetFlagValue_b, 3},        // 0x24
    {Script_IfValue_b, 6},             // 0x25
    {Script_IfInput_b, 3},             // 0x26
    {Script_Choice_b, 4},              // 0x27
    {Script_ActorPush_b, 1},           // 0x28
    {Script_IfActorPos_b, 4},          // 0x29
    {Script_LoadData_b, 0},            // 0x2A
    {Script_SaveData_b, 0},            // 0x2B
    {Script_ClearData_b, 0},           // 0x2C
    {Script_IfSavedData_b, 2},         // 0x2D
    {Script_IfActorDirection_b, 3},    // 0x2E
    {Script_SetFlagRandomValue_b, 4},  // 0x2F
    {Script_ActorGetPos_b, 0},         // 0x30
    {Script_ActorSetPosToVal_b, 0},    // 0x31
    {Script_ActorMoveToVal_b, 0},      // 0x32
    {Script_ActorMoveRel_b, 4},        // 0x33
    {Script_ActorSetPosRel_b, 4},      // 0x34
    {Script_MathAdd_b, 3},             // 0x35
    {Script_MathSub_b, 3},             // 0x36
    {Script_MathMul_b, 3},             // 0x37
    {Script_MathDiv_b, 3},             // 0x38
    {Script_MathMod_b, 3},             // 0x39
    {Script_MathAddVal_b, 0},          // 0x3A
    {Script_MathSubVal_b, 0},          // 0x3B
    {Script_MathMulVal_b, 0},          // 0x3C
    {Script_MathDivVal_b, 0},          // 0x3D
    {Script_MathModVal_b, 0},          // 0x3E
    {Script_CopyVal_b, 0},             // 0x3F
    {Script_IfValueCompare_b, 3},      // 0x40
    {Script_LoadVectors_b, 4},         // 0x41
    {Script_ActorSetMoveSpeed_b, 1},   // 0x42
    {Script_ActorSetAnimSpeed_b, 1},   // 0x43
    {Script_TextSetAnimSpeed_b, 3},    // 0x44
    {Script_ScenePushState_b, 0},      // 0x45
    {Script_ScenePopState_b, 1},       // 0x46
    {Script_ActorInvoke_b, 0},         // 0x47
    {Script_StackPush_b, 0},           // 0x48
    {Script_StackPop_b, 0},            // 0x49
    {Script_SceneResetStack_b, 0},     // 0x4A
    {Script_ScenePopAllState_b, 1},    // 0x4B
    {Script_SetInputScript_b, 4},      // 0x4C
    {Script_RemoveInputScript_b, 1},   // 0x4D
    {Script_ActorSetFrame_b, 1},       // 0x4E
    {Script_ActorSetFlip_b, 1},        // 0x4F
    {Script_TextMulti_b, 1},           // 0x50
    {Script_ActorSetFrameToVal_b, 2},  // 0x51
    {Script_VariableAddFlags_b, 3},    // 0x52
    {Script_VariableClearFlags_b, 3},  // 0x53
    {Script_SoundStartTone_b, 2},      // 0x54
    {Script_SoundStopTone_b, 0},       // 0x55
    {Script_SoundPlayBeep_b, 1},       // 0x56
    {Script_SoundPlayCrash_b, 0},      // 0x57
    {Script_SetTimerScript_b, 4},      // 0x58
    {Script_ResetTimer_b, 0},          // 0x59
    {Script_RemoveTimerScript_b, 0},   // 0x5A
    {Script_TextWithAvatar_b, 3},      // 0x5B
    {Script_TextMenu_b, 6},            // 0x5C
    {Script_ActorSetCollisions_b, 1}   // 0x5D
};

UBYTE ScriptUpdate_MoveActor() {
  // Actor reached destination
  if (actors[script_actor].pos.x == actor_move_dest_x &&
      actors[script_actor].pos.y == actor_move_dest_y) {
    return TRUE;
  }
  // Actor not at horizontal destination
  if (actors[script_actor].pos.x != actor_move_dest_x) {
    if (Lt16(actors[script_actor].pos.x, actor_move_dest_x)) {
      actors[script_actor].pos.x += actor_move_speed;
    } else if (Gt16(actors[script_actor].pos.x, actor_move_dest_x)) {
      actors[script_actor].pos.x -= actor_move_speed;
    }
  } else {
    // Actor not at vertical destination
    if (Lt16(actors[script_actor].pos.y, actor_move_dest_y)) {
      actors[script_actor].pos.y += actor_move_speed;
    } else if (Gt16(actors[script_actor].pos.y, actor_move_dest_y)) {
      actors[script_actor].pos.y -= actor_move_speed;
    }
  }
  return FALSE;
}

UBYTE ScriptUpdate_AwaitFade() { return !IsFading(); }

UBYTE ScriptUpdate_AwaitUIClosed() { return UIIsClosed(); }

/*
 * Command: Noop
 * ----------------------------
 * Perform no action, jump ahead by size of command.
 * Useful for skipping unimplemented commands.
 */
void Script_Noop_b() { script_ptr += 1 + script_cmd_args_len; }

/*
 * Command: End
 * ----------------------------
 * Stop current script from running and reset script pointer.
 */
void Script_End_b() {
  script_ptr_bank = 0;
  script_ptr = 0;
}

/*
 * Command: Text
 * ----------------------------
 * Display a line of dialogue.
 *
 *   arg0: High 8 bits for string index
 *   arg1: Low 8 bits for string index
 */
void Script_Text_b() {
  script_ptr += 1 + script_cmd_args_len;
  UIShowText((script_cmd_args[0] * 256) + script_cmd_args[1]);
  script_update_fn = ScriptUpdate_AwaitUIClosed;
}

/*
 * Command: Goto
 * ----------------------------
 * Jump to new script pointer position.
 *
 *   arg0: High 8 bits for new pointer
 *   arg1: Low 8 bits for new pointer
 */
void Script_Goto_b() {
  script_ptr = script_start_ptr + (script_cmd_args[0] * 256) + script_cmd_args[1];
}

/*
 * Command: IfFlag
 * ----------------------------
 * Jump to new script pointer position if specified flag is not false.
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: High 8 bits for new pointer
 *   arg3: Low 8 bits for new pointer
 */
void Script_IfFlag_b() {
  if (script_variables[(script_cmd_args[0] * 256) +
                       script_cmd_args[1]]) {  // True path, jump to position specified by ptr
    script_ptr = script_start_ptr + (script_cmd_args[2] * 256) + script_cmd_args[3];
  } else {  // False path, skip to next command
    script_ptr += 1 + script_cmd_args_len;
  }
}

/*
 * Command: IfValue
 * ----------------------------
 * Jump to new script pointer position if specified flag is true when compared using operator to
 * comparator.
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Operator
 *   arg3: Comparator
 *   arg4: High 8 bits for new pointer
 *   arg5: Low 8 bits for new pointer
 */
void Script_IfValue_b() {
  UBYTE value, match;
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  value = script_variables[ptr];

  switch (script_cmd_args[2]) {
    case OPERATOR_EQ:
      match = value == script_cmd_args[3];
      break;
    case OPERATOR_LT:
      match = value < script_cmd_args[3];
      break;
    case OPERATOR_LTE:
      match = value <= script_cmd_args[3];
      break;
    case OPERATOR_GT:
      match = value > script_cmd_args[3];
      break;
    case OPERATOR_GTE:
      match = value >= script_cmd_args[3];
      break;
    case OPERATOR_NE:
      match = value != script_cmd_args[3];
      break;
    default:
      match = FALSE;
  }

  if (match) {  // True path, jump to position specified by ptr
    script_ptr = script_start_ptr + (script_cmd_args[4] * 256) + script_cmd_args[5];
  } else {  // False path, skip to next command
    script_ptr += 1 + script_cmd_args_len;
  }
}

/*
 * Command: SetFlag
 * ----------------------------
 * Set specified flag to be true.
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 */
void Script_SetFlag_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  script_variables[ptr] = TRUE;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ClearFlag
 * ----------------------------
 * Set specified flag to be false.
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 */
void Script_ClearFlag_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  script_variables[ptr] = FALSE;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ActorSetDir
 * ----------------------------
 * Set active actor facing direction.
 *
 *   arg0: Direction for active actor to face
 */
void Script_ActorSetDir_b() {
  actors[script_actor].dir.x = script_cmd_args[0] == 2 ? -1 : script_cmd_args[0] == 4 ? 1 : 0;
  actors[script_actor].dir.y = script_cmd_args[0] == 8 ? -1 : script_cmd_args[0] == 1 ? 1 : 0;
  // SceneRenderActor(script_actor);
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ActorActivate
 * ----------------------------
 * Set active actor.
 *
 *   arg0: Actor index
 */
void Script_ActorActivate_b() {
  script_actor = script_cmd_args[0];
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: CameraMoveTo
 * ----------------------------
 * Move camera to specified locatopn.
 *
 *   arg0: Camera destination X (in Tiles)
 *   arg1: Camera destination Y (in Tiles)
 *   arg2: Camera settings
 */
void Script_CameraMoveTo_b() {
  /*
  camera_dest.x = script_cmd_args[0] << 3;
  camera_dest.y = 0; // @wtf-but-needed
  camera_dest.y = script_cmd_args[1] << 3;
  camera_settings = (UBYTE)script_cmd_args[2] & ~CAMERA_LOCK_FLAG;
  camera_speed = (UBYTE)script_cmd_args[2] & CAMERA_SPEED_MASK;
  script_ptr += 1 + script_cmd_args_len;
  script_action_complete = FALSE;
  */
}

/*
 * Command: CameraLock
 * ----------------------------
 * Move camera to specified locatopn.
 *
 *   arg0: Camera settings
 */
void Script_CameraLock_b() {
  /*
  UBYTE cam_x, cam_y;

  camera_settings = script_cmd_args[0] & ~CAMERA_LOCK_FLAG;
  camera_speed = (UBYTE)script_cmd_args[0] & CAMERA_SPEED_MASK;
  cam_x = ClampUBYTE(actors[0].pos.x, SCREEN_WIDTH_HALF, MUL_8(scene_width) - SCREEN_WIDTH_HALF);
  camera_dest.x = cam_x - SCREEN_WIDTH_HALF;
  cam_y = ClampUBYTE(actors[0].pos.y, SCREEN_HEIGHT_HALF, MUL_8(scene_height) - SCREEN_HEIGHT_HALF);
  camera_dest.y = cam_y - SCREEN_HEIGHT_HALF;

  script_ptr += 1 + script_cmd_args_len;
  script_action_complete = FALSE;
  */
}

/*
 * Command: Wait
 * ----------------------------
 * Wait for a specified number of frames before continuing script.
 *
 *   arg0: Frames to wait
 */
void Script_Wait_b() {
  wait_time = script_cmd_args[0];
  script_ptr += 1 + script_cmd_args_len;
  script_action_complete = FALSE;
}

/*
 * Command: FadeOut
 * ----------------------------
 * Fade screen to white.
 *
 *   arg0: Fade speed
 */
void Script_FadeOut_b() {
  FadeOut();
  FadeSetSpeed(script_cmd_args[0]);
  script_ptr += 1 + script_cmd_args_len;
  script_update_fn = ScriptUpdate_AwaitFade;
}

/*
 * Command: FadeIn
 * ----------------------------
 * Fade screen from white.
 *
 *   arg0: Fade speed
 */
void Script_FadeIn_b() {
  FadeIn();
  FadeSetSpeed(script_cmd_args[0]);
  script_ptr += 1 + script_cmd_args_len;
  script_update_fn = ScriptUpdate_AwaitFade;
}

/*
 * Command: LoadScene
 * ----------------------------
 * Load a new scene.
 *
 *   arg0: High 8 bits for stage index
 *   arg1: Low 8 bits for stage index
 *   arg2: Starting X Pos
 *   arg3: Starting Y Pos
 *   arg4: Starting direction
 *   arg5: Fade speed
 */
void Script_LoadScene_b() {
  UINT16 scene_next_index = (script_cmd_args[0] * 256) + script_cmd_args[1];
  // scene_next_index = (script_cmd_args[0] * 256) + script_cmd_args[1];
  // scene_index = scene_next_index + 1;

  map_next_pos.x = 0;  // @wtf-but-needed
  map_next_pos.x = (script_cmd_args[2] << 3);
  map_next_pos.y = 0;  // @wtf-but-needed
  map_next_pos.y = (script_cmd_args[3] << 3);
  map_next_dir.x = script_cmd_args[4] == 2 ? -1 : script_cmd_args[4] == 4 ? 1 : 0;
  map_next_dir.y = script_cmd_args[4] == 8 ? -1 : script_cmd_args[4] == 1 ? 1 : 0;

  SetState(scene_next_index);

  // stage_next_type = SCENE;
  // scene_loaded = FALSE;
  // script_action_complete = FALSE;

  FadeSetSpeed(script_cmd_args[5]);
  // FadeOut();

  // script_action_complete = FALSE;
  script_ptr += 1 + script_cmd_args_len;

  LOG("Script_LoadScene_b BBB script_update_fn=%d\n", script_update_fn);

  script_update_fn = ScriptUpdate_AwaitFade;

  LOG("Script_LoadScene_b CCC script_update_fn=%d\n", script_update_fn);
}

/*
 * Command: ActorSetPos
 * ----------------------------
 * Instantly position actor at new location.
 *
 *   arg0: New X Pos
 *   arg1: New Y Pos
 */
void Script_ActorSetPos_b() {
  actors[script_actor].pos.x = 0;  // @wtf-but-needed
  actors[script_actor].pos.x = (script_cmd_args[0] << 3) + 8;
  actors[script_actor].pos.y = 0;  // @wtf-but-needed
  actors[script_actor].pos.y = (script_cmd_args[1] << 3) + 8;
  /* @todo
  if (script_cmd_args[1] == 31)
  {
    actors[script_actor].pos.y = ACTOR_MAX_Y;
  }
  */
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ActorMoveTo
 * ----------------------------
 * Walk actor to new location.
 *
 *   arg0: New X Pos
 *   arg1: New Y Pos
 */
void Script_ActorMoveTo_b() {
  actor_move_dest_x = 0;  // @wtf-but-needed
  actor_move_dest_x = (script_cmd_args[0] * 8);
  actor_move_dest_y = 0;  // @wtf-but-needed
  actor_move_dest_y = (script_cmd_args[1] * 8);
  script_ptr += 1 + script_cmd_args_len;
  script_update_fn = ScriptUpdate_MoveActor;
}

/*
 * Command: ShowSprites
 * ----------------------------
 * Make all sprites visible
 */
void Script_ShowSprites_b() {
  SHOW_SPRITES;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: HideSprites
 * ----------------------------
 * Hide all sprites
 */
void Script_HideSprites_b() {
  HIDE_SPRITES;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ActorShow
 * ----------------------------
 * Unhide actor
 */
void Script_ActorShow_b() {
  actors[script_actor].enabled = TRUE;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ActorHide
 * ----------------------------
 * Hide actor
 */
void Script_ActorHide_b() {
  actors[script_actor].enabled = FALSE;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ActorSetCollisions
 * ----------------------------
 * Set collisions enabled flag for actor.
 *
 *   arg0: Enabled
 */
void Script_ActorSetCollisions_b() {
  actors[script_actor].collisionsEnabled = script_cmd_args[0];
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ActorSetEmote
 * ----------------------------
 * Display emote bubble over actor.
 *
 *   arg0: Emote Id
 */
void Script_ActorSetEmote_b() {
  /*
  script_ptr += 1 + script_cmd_args_len;
  SceneSetEmote(script_actor, script_cmd_args[0]);
  script_action_complete = FALSE;
  */
}

/*
 * Command: CameraShake
 * ----------------------------
 * Shake camera for earthquake effect.
 *
 *   arg0: Number of frames to shake for
 */
void Script_CameraShake_b() {
  /*
  shake_time = script_cmd_args[0];
  script_ptr += 1 + script_cmd_args_len;
  script_action_complete = FALSE;
  */
}

/*
 * Command: ShowOverlay
 * ----------------------------
 * Load image into window buffer and position.
 */
void Script_ShowOverlay_b() {
  UISetColor(script_cmd_args[0]);
  UISetPos(script_cmd_args[1] << 3, script_cmd_args[2] << 3);
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: HideOverlay
 * ----------------------------
 * Hide window buffer
 */
void Script_HideOverlay_b() {
  /*
  UISetPos(0, MENU_CLOSED_Y);
  script_ptr += 1 + script_cmd_args_len;
  */
}

/*
 * Command: OverlaySetPos
 * ----------------------------
 * Window buffer set position to X/Y
 */
void Script_OverlaySetPos_b() {
  UISetPos(script_cmd_args[0] << 3, script_cmd_args[1] << 3);
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: OverlaySetPos
 * ----------------------------
 * Window buffer move position to X/Y with speed
 */
void Script_OverlayMoveTo_b() {
  UIMoveTo(script_cmd_args[0] << 3, script_cmd_args[1] << 3, script_cmd_args[2]);
  script_ptr += 1 + script_cmd_args_len;
  script_action_complete = FALSE;
}

/*
 * Command: AwaitInput
 * ----------------------------
 * Pause script until joy overlaps bits with provided input
 */
void Script_AwaitInput_b() {
  await_input = script_cmd_args[0];
  script_ptr += 1 + script_cmd_args_len;
  script_action_complete = FALSE;
}

/*
 * Command: MusicPlay
 * ----------------------------
 * Play the music track with given index
 */
void Script_MusicPlay_b() {
  MusicPlay(script_cmd_args[0], script_cmd_args[1], scriptrunner_bank);
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: MusicStop
 * ----------------------------
 * Stop any playing music
 */
void Script_MusicStop_b() {
  MusicStop(scriptrunner_bank);
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ResetVariables
 * ----------------------------
 * Reset all variables back to false
 */
void Script_ResetVariables_b() {
  UWORD i;
  for (i = 0; i != NUM_VARIABLES; ++i) {
    script_variables[i] = FALSE;
  }
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: NextFrame
 * ----------------------------
 * Wait until next frame
 */
void Script_NextFrame_b() {
  script_ptr += 1 + script_cmd_args_len;
  script_await_next_frame = TRUE;
}

/*
 * Command: IncFlag
 * ----------------------------
 * Increase value stored in flag
 */
void Script_IncFlag_b() {
  UBYTE value;
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  value = script_variables[ptr];
  if (value != 255) {
    script_variables[ptr] = value + 1;
  }
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: DecFlag
 * ----------------------------
 * Decrease value stored in flag
 */
void Script_DecFlag_b() {
  UBYTE value;
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  value = script_variables[ptr];
  if (value != 0) {
    script_variables[ptr] = value - 1;
  }
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: SetFlagValue
 * ----------------------------
 * Set flag to specific value
 */
void Script_SetFlagValue_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  script_variables[ptr] = script_cmd_args[2];
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: IfInput
 * ----------------------------
 * Goto true path if joy mask overlaps
 */
void Script_IfInput_b() {
  /*
  UBYTE mask;
  mask = 0;
  mask = script_cmd_args[0];
  if ((joy & mask) != 0)
  { // True path, jump to position specified by ptr
    script_ptr = script_start_ptr + (script_cmd_args[1] * 256) + script_cmd_args[2];
  }
  else
  { // False path, skip to next command
    script_ptr += 1 + script_cmd_args_len;
  }
  */
}

/*
 * Command: Choice
 * ----------------------------
 * Display multiple choice input
 */
void Script_Choice_b() {
  script_ptr += 1 + script_cmd_args_len;
  UIShowChoice((script_cmd_args[0] * 256) + script_cmd_args[1],
               (script_cmd_args[2] * 256) + script_cmd_args[3]);
  script_update_fn = ScriptUpdate_AwaitUIClosed;
}

/*
 * Command: Menu
 * ----------------------------
 * Display multiple choice menu
 */
void Script_TextMenu_b() {
  script_ptr += 1 + script_cmd_args_len;
  UIShowMenu((script_cmd_args[0] * 256) + script_cmd_args[1],
             (script_cmd_args[2] * 256) + script_cmd_args[3], script_cmd_args[4],
             script_cmd_args[5]);
  script_update_fn = ScriptUpdate_AwaitUIClosed;
}

/*
 * Command: PlayerSetSprite
 * ----------------------------
 * Change sprite used by player
 */
void Script_PlayerSetSprite_b() {
  BANK_PTR sprite_bank_ptr;
  UBYTE *sprite_ptr;
  UBYTE sprite_index, sprite_frames, sprite_len;

  // Load Player Sprite
  sprite_index = script_cmd_args[0];
  ReadBankedBankPtr(DATA_PTRS_BANK, &sprite_bank_ptr, &sprite_bank_ptrs[sprite_index]);
  sprite_ptr = ((UBYTE *)bank_data_ptrs[sprite_bank_ptr.bank]) + sprite_bank_ptr.offset;
  sprite_frames = ReadBankedUBYTE(sprite_bank_ptr.bank, sprite_ptr);
  sprite_len = MUL_4(sprite_frames);
  SetBankedSpriteData(sprite_bank_ptr.bank, 0, sprite_len, sprite_ptr + 1);
  actors[0].sprite = 0;
  actors[0].frame = 0;
  actors[0].sprite_type = sprite_frames == 6 ? SPRITE_ACTOR_ANIMATED
                                             : sprite_frames == 3 ? SPRITE_ACTOR : SPRITE_STATIC;
  actors[0].frames_len = sprite_frames == 6 ? 2 : sprite_frames == 3 ? 1 : sprite_frames;
  // SceneRenderActor(0);

  // Keep new sprite when switching scene
  map_next_sprite = sprite_index;

  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ActorPush
 * ----------------------------
 * Push actor in direction player is facing
 */
void Script_ActorPush_b() {
  UINT16 dest_x, dest_y;
  UBYTE check_tile, end_tile, check_tile2;
  UBYTE check_dir = 0;

  if (script_cmd_args[0]) {
    if (actors[0].dir.x < 0) {
      dest_x = 0;
      check_dir = 1;
    } else if (actors[0].dir.x > 0) {
      dest_x = image_width;
      check_dir = 2;
    } else {
      dest_x = actors[script_actor].pos.x;
    }
    if (actors[0].dir.y < 0) {
      dest_y = 0;
      check_dir = 3;
    } else if (actors[0].dir.y > 0) {
      dest_y = image_height;
      check_dir = 4;
    } else {
      dest_y = actors[script_actor].pos.y;
    }
  } else {
    dest_x = actors[script_actor].pos.x + (actors[0].dir.x * 16);
    dest_y = actors[script_actor].pos.y + (actors[0].dir.y * 16);
  }

  switch (check_dir) {
    case 1:
      LOG("CHECK LEFT\n");
      end_tile = DIV_8(dest_x);
      check_tile = DIV_8(actors[script_actor].pos.x);
      check_tile2 = DIV_8(dest_y);
      while (check_tile != end_tile) {
        if (TileAt(check_tile - 1, check_tile2)) {
          dest_x = (check_tile)*8;
          break;
        }
        check_tile--;
      }
      break;
      break;
    case 2:
      LOG("CHECK RIGHT\n");
      end_tile = DIV_8(dest_x);
      check_tile = DIV_8(actors[script_actor].pos.x);
      check_tile2 = DIV_8(dest_y);
      while (check_tile != end_tile) {
        if (TileAt(check_tile + 2, check_tile2)) {
          LOG("CHECK RIGHT FOUND TILE AT [%d, %d]\n", check_tile - 1, check_tile2);
          dest_x = (check_tile)*8;
          break;
        }
        check_tile++;
      }
      break;
    case 3:
      LOG("CHECK UP\n");
      end_tile = DIV_8(dest_y);
      check_tile = DIV_8(actors[script_actor].pos.y);
      check_tile2 = DIV_8(dest_x);
      while (check_tile != end_tile) {
        if (TileAt(check_tile2, check_tile - 1)) {
          dest_y = (check_tile)*8;
          break;
        }
        check_tile--;
      }
      break;
    case 4:
      LOG("CHECK DOWN\n");
      end_tile = DIV_8(dest_y);
      check_tile = DIV_8(actors[script_actor].pos.y);
      check_tile2 = DIV_8(dest_x);
      while (check_tile != end_tile) {
        if (TileAt(check_tile2, check_tile + 1)) {
          dest_y = (check_tile)*8;
          break;
        }
        check_tile++;
      }
      break;
  }

  actor_move_settings |= ACTOR_MOVE_ENABLED;
  actor_move_settings &= ~ACTOR_NOCLIP;
  actor_move_dest_x = 0;  // @wtf-but-needed
  actor_move_dest_x = dest_x;
  actor_move_dest_y = 0;  // @wtf-but-needed
  actor_move_dest_y = dest_y;
  actor_move_dir_x = actors[0].dir.x;
  actor_move_dir_y = actors[0].dir.y;

  // script_ptr += 1 + script_cmd_args_len;
  // script_action_complete = FALSE;

  script_ptr += 1 + script_cmd_args_len;
  script_action_complete = FALSE;
  script_update_fn = ScriptUpdate_MoveActor;

  LOG("DEST_X =%u\n", dest_x);
  LOG("DEST_Y =%u\n", dest_y);
}

/*
 * Command: IfActorPos
 * ----------------------------
 * Jump to new script pointer position if specified actor is at desired position.
 *
 *   arg0: Actor index
 *   arg1: Actor X Pos
 *   arg2: Actor Y Pos
 *   arg3: High 8 bits for new pointer
 *   arg4: Low 8 bits for new pointer
 */
void Script_IfActorPos_b() {
  if (((script_cmd_args[0] << 3) + 8 == actors[script_actor].pos.x) &&
      ((script_cmd_args[1] << 3) + 8 ==
       actors[script_actor].pos.y)) {  // True path, jump to position specified by ptr
    script_ptr = script_start_ptr + (script_cmd_args[2] * 256) + script_cmd_args[3];
  } else {  // False path, skip to next command
    script_ptr += 1 + script_cmd_args_len;
  }
}

/*
 * Command: SaveData
 * ----------------------------
 * Store current scene, player position and direction, current sprite and variable values into RAM
 */
void Script_SaveData_b() {
  /*
  UWORD i;

  ENABLE_RAM;

  RAMPtr = (UBYTE *)RAM_START_PTR;
  RAMPtr[0] = TRUE; // Flag to determine if data has been stored

  // Save current scene
  RAMPtr[1] = scene_index;

  // Save player position
  RAMPtr[2] = actors[0].pos.x;
  RAMPtr[3] = actors[0].pos.y;
  if (actors[0].dir.x < 0)
  {
    RAMPtr[4] = 2;
  }
  else if (actors[0].dir.x > 0)
  {
    RAMPtr[4] = 4;
  }
  else if (actors[0].dir.y < 0)
  {
    RAMPtr[4] = 8;
  }
  else
  {
    RAMPtr[4] = 1;
  }

  // Save player sprite
  RAMPtr[5] = map_next_sprite;

  // Save variable values
  RAMPtr = (UBYTE *)RAM_START_VARS_PTR;
  for (i = 0; i < NUM_VARIABLES; i++)
  {
    RAMPtr[i] = script_variables[i];
  }

  DISABLE_RAM;

  script_ptr += 1 + script_cmd_args_len;
  */
}

/*
 * Command: LoadData
 * ----------------------------
 * Restore current scene, player position and direction, current sprite and variable values from RAM
 */
void Script_LoadData_b() {
  /*
  UWORD i;

  ENABLE_RAM;

  RAMPtr = (UBYTE *)RAM_START_PTR;
  if (*RAMPtr == TRUE)
  {
    // Set scene index
    RAMPtr++;
    scene_next_index = *RAMPtr;
    scene_index = scene_next_index + 1;

    // Position player
    RAMPtr++;
    map_next_pos.x = 0; // @wtf-but-needed
    map_next_pos.x = *RAMPtr;
    RAMPtr++;
    map_next_pos.y = 0; // @wtf-but-needed
    map_next_pos.y = *RAMPtr;
    RAMPtr++;
    map_next_dir.x = *RAMPtr == 2 ? -1 : *RAMPtr == 4 ? 1 : 0;
    map_next_dir.y = *RAMPtr == 8 ? -1 : *RAMPtr == 1 ? 1 : 0;

    // Load player sprite
    RAMPtr++;
    map_next_sprite = *RAMPtr;

    // Load variable values
    RAMPtr = (UBYTE *)RAM_START_VARS_PTR;
    for (i = 0; i < NUM_VARIABLES; i++)
    {
      script_variables[i] = RAMPtr[i];
    }

    // Switch to next scene
    stage_next_type = SCENE;
    scene_loaded = FALSE;
    FadeSetSpeed(2);
    FadeOut();

    script_action_complete = FALSE;
  }

  DISABLE_RAM;

  script_ptr += 1 + script_cmd_args_len;
  */
}

/*
 * Command: ClearData
 * ----------------------------
 * Clear current data in RAM
 */
void Script_ClearData_b() {
  ENABLE_RAM;
  RAMPtr = (UBYTE *)RAM_START_PTR;
  RAMPtr[0] = FALSE;
  DISABLE_RAM;

  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: IfSavedData
 * ----------------------------
 * Jump to new script pointer position if data is saved in RAM.
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 */
void Script_IfSavedData_b() {
  UBYTE jump;

#ifdef __EMSCRIPTEN__
  jump = 0;
#else
  ENABLE_RAM;
  RAMPtr = (UBYTE *)RAM_START_PTR;
  jump = 0;
  jump = *RAMPtr == TRUE;
  DISABLE_RAM;
#endif

  if (jump) {  // True path, jump to position specified by ptr
    script_ptr = script_start_ptr + (script_cmd_args[0] * 256) + script_cmd_args[1];
  } else {  // False path, skip to next command
    script_ptr += 1 + script_cmd_args_len;
  }
}

/*
 * Command: IfActorDirection
 * ----------------------------
 * Jump to new script pointer position if actor direction matches.
 *
 *   arg0: Actor index
 *   arg1: Direction for active actor to match
 *   arg2: High 8 bits for new pointer
 *   arg3: Low 8 bits for new pointer
 */
void Script_IfActorDirection_b() {
  if ((actors[script_actor].dir.x == 1 && script_cmd_args[0] == 4 ||
       actors[script_actor].dir.x == -1 && script_cmd_args[0] == 2) ||
      (actors[script_actor].dir.y == 1 && script_cmd_args[0] == 1 ||
       actors[script_actor].dir.y == -1 &&
           script_cmd_args[0] == 8)) {  // True path, jump to position specified by ptr
    script_ptr = script_start_ptr + (script_cmd_args[1] * 256) + script_cmd_args[2];
  } else {  // False path, skip to next command
    script_ptr += 1 + script_cmd_args_len;
  }
}

/*
 * Command: SetFlagRandomValue
 * ----------------------------
 * Set flag to random value
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Max value
 */
void Script_SetFlagRandomValue_b() {
  UBYTE rand_val;
  UBYTE modulo;
  UBYTE offset;
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  rand_val = rand();
  offset = script_cmd_args[2];
  modulo = script_cmd_args[3] + 1;
  script_variables[ptr] = offset + (rand_val % modulo);
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ActorGetPos
 * ----------------------------
 * Store Actor position in variables
 */
void Script_ActorGetPos_b() {
  script_variables[script_ptr_x] = actors[script_actor].pos.x - 8 >> 3;
  script_variables[script_ptr_y] = actors[script_actor].pos.y - 8 >> 3;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ActorSetPosToVal
 * ----------------------------
 * Set Actor position from variables
 */
void Script_ActorSetPosToVal_b() {
  /*
  actors[script_actor].pos.x = 0; // @wtf-but-needed
  actors[script_actor].pos.x = (script_variables[script_ptr_x] << 3) + 8;
  actors[script_actor].pos.y = 0; // @wtf-but-needed
  actors[script_actor].pos.y = (script_variables[script_ptr_y] << 3) + 8;
  if (script_variables[script_ptr_y] == 31)
  {
    actors[script_actor].pos.y = ACTOR_MAX_Y;
  }
  script_ptr += 1 + script_cmd_args_len;
  */
}

/*
 * Command: ActorMoveToVal
 * ----------------------------
 * Set Actor position from variables
 */
void Script_ActorMoveToVal_b() {
  /*
  actor_move_settings |= ACTOR_MOVE_ENABLED;
  actor_move_settings |= ACTOR_NOCLIP;
  actor_move_dest_x = 0; // @wtf-but-needed
  actor_move_dest_x = (script_variables[script_ptr_x] << 3) + 8;
  actor_move_dest_y = 0; // @wtf-but-needed
  actor_move_dest_y = (script_variables[script_ptr_y] << 3) + 8;
  if (script_variables[script_ptr_y] == 31)
  {
    actor_move_dest_y = ACTOR_MAX_Y;
  }
  script_ptr += 1 + script_cmd_args_len;
  script_action_complete = FALSE;
  */
}

/*
 * Command: ActorMoveRel
 * ----------------------------
 * Walk actor to relative location.
 *
 *   arg0: Offset X Pos
 *   arg1: Offset Y Pos
 */
void Script_ActorMoveRel_b() {
  /*
  actor_move_settings |= ACTOR_MOVE_ENABLED;
  actor_move_settings |= ACTOR_NOCLIP;
  actor_move_dest_x = 0; // @wtf-but-needed
  actor_move_dest_x = actors[script_actor].pos.x;
  if (script_cmd_args[0] != 0)
  {
    if (script_cmd_args[1] == 1)
    {
      actor_move_dest_x = actor_move_dest_x - (script_cmd_args[0] << 3);
      // If destination wrapped past left edge set to min X
      if (actor_move_dest_x > actors[script_actor].pos.x)
      {
        actor_move_dest_x = ACTOR_MIN_X;
      }
      else if (actor_move_dest_x < ACTOR_MIN_X)
      {
        actor_move_dest_x = ACTOR_MIN_X;
      }
    }
    else
    {
      actor_move_dest_x = actor_move_dest_x + (script_cmd_args[0] << 3);
      // If destination wrapped past right edge set to max X
      if (actor_move_dest_x < actors[script_actor].pos.x)
      {
        actor_move_dest_x = ACTOR_MAX_X;
      }
    }
  }

  actor_move_dest_y = 0; // @wtf-but-needed
  actor_move_dest_y = actors[script_actor].pos.y;
  if (script_cmd_args[2] != 0)
  {
    if (script_cmd_args[3] == 1)
    {
      actor_move_dest_y = actor_move_dest_y - (script_cmd_args[2] << 3);
      // If destination wrapped past top edge set to min Y
      if (actor_move_dest_y > actors[script_actor].pos.y)
      {
        actor_move_dest_y = ACTOR_MIN_Y;
      }
      else if (actor_move_dest_y < ACTOR_MIN_Y)
      {
        actor_move_dest_y = ACTOR_MIN_Y;
      }
    }
    else
    {
      actor_move_dest_y = actor_move_dest_y + (script_cmd_args[2] << 3);
      // If destination wrapped past bottom edge set to max Y
      if (actor_move_dest_y < actors[script_actor].pos.y)
      {
        actor_move_dest_y = ACTOR_MAX_Y;
      }
    }
  }

  script_ptr += 1 + script_cmd_args_len;
  script_action_complete = FALSE;
  */
}

/*
 * Command: ActorSetPosRel
 * ----------------------------
 * Instantly position actor at relative location.
 *
 *   arg0: Offset X Pos
 *   arg1: Offset Y Pos
 */
void Script_ActorSetPosRel_b() {
  if (script_cmd_args[0] != 0) {
    if (script_cmd_args[1]) {
      actors[script_actor].pos.x = actors[script_actor].pos.x - (script_cmd_args[0] << 3);
    } else {
      actors[script_actor].pos.x = actors[script_actor].pos.x + (script_cmd_args[0] << 3);
    }
  }

  if (script_cmd_args[2] != 0) {
    if (script_cmd_args[3]) {
      actors[script_actor].pos.y = actors[script_actor].pos.y - (script_cmd_args[2] << 3);
    } else {
      actors[script_actor].pos.y = actors[script_actor].pos.y + (script_cmd_args[2] << 3);
    }
  }

  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: MathAdd
 * ----------------------------
 * Add value to flag
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Value
 */
void Script_MathAdd_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  UBYTE a = script_variables[ptr];
  UBYTE b = script_cmd_args[2];
  script_variables[ptr] = a + b;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: MathSub
 * ----------------------------
 * Subtract value from flag
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Value
 */
void Script_MathSub_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  UBYTE a = script_variables[ptr];
  UBYTE b = script_cmd_args[2];
  script_variables[ptr] = a - b;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: MathMul
 * ----------------------------
 * Multiply flag by value
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Value
 */
void Script_MathMul_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  UBYTE a = script_variables[ptr];
  UBYTE b = script_cmd_args[2];
  script_variables[ptr] = a * b;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: MathDiv
 * ----------------------------
 * Divide flag by value
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Value
 */
void Script_MathDiv_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  UBYTE a = script_variables[ptr];
  UBYTE b = script_cmd_args[2];
  script_variables[ptr] = a / b;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: MathMod
 * ----------------------------
 * Flag modulo by value
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Value
 */
void Script_MathMod_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  UBYTE a = script_variables[ptr];
  UBYTE b = script_cmd_args[2];
  script_variables[ptr] = a % b;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: MathAddVal
 * ----------------------------
 * Add value from flag to flag
 */
void Script_MathAddVal_b() {
  UBYTE a = script_variables[script_ptr_x];
  UBYTE b = script_variables[script_ptr_y];
  script_variables[script_ptr_x] = a + b;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: MathSubVal
 * ----------------------------
 * Subtract value from flag to flag
 */
void Script_MathSubVal_b() {
  UBYTE a = script_variables[script_ptr_x];
  UBYTE b = script_variables[script_ptr_y];
  script_variables[script_ptr_x] = a - b;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: MathMulVal
 * ----------------------------
 * Multiply value from flag to flag
 */
void Script_MathMulVal_b() {
  UBYTE a = script_variables[script_ptr_x];
  UBYTE b = script_variables[script_ptr_y];
  script_variables[script_ptr_x] = a * b;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: MathDiv
 * ----------------------------
 * Divide value from flag to flag
 */
void Script_MathDivVal_b() {
  UBYTE a = script_variables[script_ptr_x];
  UBYTE b = script_variables[script_ptr_y];
  script_variables[script_ptr_x] = a / b;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: MathModVal
 * ----------------------------
 * Modulo value from flag to flag
 */
void Script_MathModVal_b() {
  UBYTE a = script_variables[script_ptr_x];
  UBYTE b = script_variables[script_ptr_y];
  script_variables[script_ptr_x] = a % b;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: CopyVal
 * ----------------------------
 * Copy value from flag to flag
 */
void Script_CopyVal_b() {
  UBYTE value = script_variables[script_ptr_y];
  script_variables[script_ptr_x] = value;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: IfValue
 * ----------------------------
 * Jump to new script pointer position if specified flag is true when compared using operator to
 * comparator.
 *
 *   arg0: Operator
 *   arg1: High 8 bits for new pointer
 *   arg2: Low 8 bits for new pointer
 */
void Script_IfValueCompare_b() {
  UBYTE match;
  UBYTE a = script_variables[script_ptr_x];
  UBYTE b = script_variables[script_ptr_y];

  switch (script_cmd_args[0]) {
    case OPERATOR_EQ:
      match = a == b;
      break;
    case OPERATOR_LT:
      match = a < b;
      break;
    case OPERATOR_LTE:
      match = a <= b;
      break;
    case OPERATOR_GT:
      match = a > b;
      break;
    case OPERATOR_GTE:
      match = a >= b;
      break;
    case OPERATOR_NE:
      match = a != b;
      break;
    default:
      match = FALSE;
  }

  if (match) {  // True path, jump to position specified by ptr
    script_ptr = script_start_ptr + (script_cmd_args[1] * 256) + script_cmd_args[2];
  } else {  // False path, skip to next command
    script_ptr += 1 + script_cmd_args_len;
  }
}

/*
 * Command: LoadVectors
 * ----------------------------
 * Loads a vector pair
 *
 *   arg0: High 8 bits for first pointer
 *   arg1: Low 8 bits for first pointer
 *   arg2: High 8 bits for second pointer
 *   arg3: Low 8 bits for second pointer
 */
void Script_LoadVectors_b() {
  script_ptr_x = (script_cmd_args[0] * 256) + script_cmd_args[1];
  script_ptr_y = (script_cmd_args[2] * 256) + script_cmd_args[3];
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ActorSetMoveSpeed
 * ----------------------------
 * Set active actor movement speed.
 *
 *   arg0: Movement speed to use
 */
void Script_ActorSetMoveSpeed_b() {
  actors[script_actor].move_speed = script_cmd_args[0];
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ActorSetAnimSpeed
 * ----------------------------
 * Set active actor animation speed.
 *
 *   arg0: Animation speed to use
 */
void Script_ActorSetAnimSpeed_b() {
  actors[script_actor].anim_speed = script_cmd_args[0];
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: TextSetAnimSpeed
 * ----------------------------
 * Set global text animation speed.
 *
 *   arg0: Animation speed to use
 *   arg1: Animation speed to use fading out
 */
void Script_TextSetAnimSpeed_b() {
  /*
  text_in_speed = script_cmd_args[0];
  text_out_speed = script_cmd_args[1];
  text_draw_speed = script_cmd_args[2];
  script_ptr += 1 + script_cmd_args_len;
  */
}

/*
 * Command: ActorInvoke
 * ----------------------------
 * Invoke Actor script
 */
void Script_ActorInvoke_b() {
  Script_StackPush_b();
  ScriptStart(&actors[script_actor].events_ptr);
}

/*
 * Command: StackPush
 * ----------------------------
 * Push the current script pointer to the stack
 */
void Script_StackPush_b() {
  script_stack[script_stack_ptr] = script_ptr;
  script_start_stack[script_stack_ptr] = script_start_ptr;
  script_stack[script_stack_ptr] += 1 + script_cmd_args_len;
  script_stack_ptr++;
}

/*
 * Command: StackPop
 * ----------------------------
 * Pop the script pointer from the stack
 */
void Script_StackPop_b() {
  script_stack_ptr--;
  script_ptr = script_stack[script_stack_ptr];
  script_start_ptr = script_start_stack[script_stack_ptr];
}

/*
 * Command: ScenePushState
 * ----------------------------
 * Stores the state of the current scene
 */
void Script_ScenePushState_b() {
  /*
  if (scene_stack_ptr < MAX_SCENE_STATES)
  {
    scene_stack[scene_stack_ptr].scene_index = scene_index;
    scene_stack[scene_stack_ptr].player_dir.x = actors[0].dir.x;
    scene_stack[scene_stack_ptr].player_dir.y = actors[0].dir.y;
    scene_stack[scene_stack_ptr].player_pos.x = 0; // @wtf-but-needed
    scene_stack[scene_stack_ptr].player_pos.x = actors[0].pos.x >> 3;
    scene_stack[scene_stack_ptr].player_pos.y = 0; // @wtf-but-needed
    scene_stack[scene_stack_ptr].player_pos.y = actors[0].pos.y >> 3;
    scene_stack_ptr++;
  }

  script_ptr += 1 + script_cmd_args_len;
  */
}

/*
 * Command: ScenePopState
 * ----------------------------
 * Restores the saved scene state
 *
 *   arg0: Fade speed
 */
void Script_ScenePopState_b() {
  /*
  if (scene_stack_ptr)
  {
    scene_stack_ptr--;

    scene_next_index = scene_stack[scene_stack_ptr].scene_index;
    scene_index = scene_next_index + 1;

    map_next_pos.x = 0; // @wtf-but-needed
    map_next_pos.x = scene_stack[scene_stack_ptr].player_pos.x << 3;
    map_next_pos.y = 0; // @wtf-but-needed
    map_next_pos.y = scene_stack[scene_stack_ptr].player_pos.y << 3;
    map_next_dir.x = scene_stack[scene_stack_ptr].player_dir.x;
    map_next_dir.y = scene_stack[scene_stack_ptr].player_dir.y;

    stage_next_type = SCENE;
    scene_loaded = FALSE;
    script_action_complete = FALSE;
    FadeSetSpeed(script_cmd_args[0]);
    FadeOut();
    script_ptr += 1 + script_cmd_args_len;

    return;
  }

  script_action_complete = TRUE;
  script_ptr += 1 + script_cmd_args_len;
  */
}

/*
 * Command: SceneResetStack
 * ----------------------------
 * Clear all saved scene state
 */
void Script_SceneResetStack_b() {
  /*
  scene_stack_ptr = 0;
  script_ptr += 1 + script_cmd_args_len;
  */
}

/*
 * Command: ScenePopAllState
 * ----------------------------
 * Restores the first saved scene state
 */
void Script_ScenePopAllState_b() {
  /*
  if (scene_stack_ptr)
  {
    scene_stack_ptr = 0;

    scene_next_index = scene_stack[scene_stack_ptr].scene_index;
    scene_index = scene_next_index + 1;

    map_next_pos.x = 0; // @wtf-but-needed
    map_next_pos.x = scene_stack[scene_stack_ptr].player_pos.x << 3;
    map_next_pos.y = 0; // @wtf-but-needed
    map_next_pos.y = scene_stack[scene_stack_ptr].player_pos.y << 3;
    map_next_dir.x = scene_stack[scene_stack_ptr].player_dir.x;
    map_next_dir.y = scene_stack[scene_stack_ptr].player_dir.y;

    stage_next_type = SCENE;
    scene_loaded = FALSE;
    script_action_complete = FALSE;
    FadeSetSpeed(script_cmd_args[0]);
    FadeOut();
    script_ptr += 1 + script_cmd_args_len;

    return;
  }

  script_action_complete = TRUE;
  script_ptr += 1 + script_cmd_args_len;
  */
}

/*
 * Command: SetInputScript
 * ----------------------------
 * Attach script to button press
 */
void Script_SetInputScript_b() {
  UBYTE input, index;

  input = script_cmd_args[0];

  index = 0;
  while (!(input & 1) && input != 0) {
    index += 1;
    input = input >> 1;
  }

  input_script_ptrs[index].bank = script_cmd_args[1];
  input_script_ptrs[index].offset = (script_cmd_args[2] * 256) + script_cmd_args[3];

  script_action_complete = TRUE;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: RemoveInputScript
 * ----------------------------
 * Remove script from button press
 */
void Script_RemoveInputScript_b() {
  UBYTE input, index;

  input = script_cmd_args[0];

  index = 0;
  for (index = 0; index != 8; ++index) {
    if (input & 1) {
      input_script_ptrs[index].bank = 0;
    }
    input = input >> 1;
  }

  script_action_complete = TRUE;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ActorSetFrame
 * ----------------------------
 * Set animation frame of current actor
 */
void Script_ActorSetFrame_b() {
  actors[script_actor].frame = 0;
  actors[script_actor].flip = 0;
  actors[script_actor].frame = script_cmd_args[0] % actors[script_actor].frames_len;
  // SceneRenderActor(script_actor);
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ActorSetFrameToVal
 * ----------------------------
 * Set animation frame of current actor using variable
 */
void Script_ActorSetFrameToVal_b() {
  actors[script_actor].frame = 0;
  actors[script_actor].flip = 0;
  actors[script_actor].frame = script_variables[(script_cmd_args[0] * 256) + script_cmd_args[1]] %
                               actors[script_actor].frames_len;
  // SceneRenderActor(script_actor);
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: ActorSetFlip
 * ----------------------------
 * Set flip state of current actor
 */
void Script_ActorSetFlip_b() {
  actors[script_actor].flip = 0;
  actors[script_actor].flip = script_cmd_args[0];
  // SceneRenderActor(script_actor);
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: TextMulti
 * ----------------------------
 * Set multi text box mode
 * mode - 0 (store text close speed and set to instant)
 *      - 1 (store open speed and set to instant)
 *      - 2 (restore close speed)
 *      - 3 (restore open and close speed)
 */
void Script_TextMulti_b() {
  UBYTE mode;
  mode = script_cmd_args[0];

  LOG("Script_TextMulti_b\n");

  if (mode == 0) {
    tmp_text_out_speed = text_out_speed;
    text_out_speed = 0;
  } else if (mode == 1) {
    tmp_text_in_speed = text_in_speed;
    text_in_speed = 0;
  } else if (mode == 2) {
    text_out_speed = tmp_text_out_speed;
  } else if (mode == 3) {
    text_in_speed = tmp_text_in_speed;
    text_out_speed = tmp_text_out_speed;
  }

  LOG("Script_TextMulti_b DONE\n");

  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: VariableAddFlags
 * ----------------------------
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Value
 */
void Script_VariableAddFlags_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  UBYTE a = script_variables[ptr];
  UBYTE b = script_cmd_args[2];
  script_variables[ptr] = a | b;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: VariableClearFlags
 * ----------------------------
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Value
 */
void Script_VariableClearFlags_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  UBYTE a = script_variables[ptr];
  UBYTE b = script_cmd_args[2];
  script_variables[ptr] = a & ~b;
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: SoundStartTone
 * ----------------------------
 */
void Script_SoundStartTone_b() {
  UWORD tone = (script_cmd_args[0] * 256) + script_cmd_args[1];

  // enable sound
  NR52_REG = 0x80;

  // play tone on channel 1
  NR10_REG = 0x00;
  NR11_REG = (0x00 << 6) | 0x01;
  NR12_REG = (0x0F << 4) | 0x00;
  NR13_REG = (tone & 0x00FF);
  NR14_REG = 0x80 | ((tone & 0x0700) >> 8);

  // enable volume
  NR50_REG = 0x77;

  // enable channel 1
  NR51_REG |= 0x11;

  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: SoundStopTone
 * ----------------------------
 */
void Script_SoundStopTone_b() {
  // stop tone on channel 1
  NR12_REG = 0x00;

  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: SoundPlayBeep
 * ----------------------------
 */
void Script_SoundPlayBeep_b() {
  UBYTE pitch = script_cmd_args[0];

  // enable sound
  NR52_REG = 0x80;

  // play beep sound on channel 4
  NR41_REG = 0x01;
  NR42_REG = (0x0F << 4);
  NR43_REG = 0x20 | 0x08 | pitch;
  NR44_REG = 0x80 | 0x40;

  // enable volume
  NR50_REG = 0x77;

  // enable channel 4
  NR51_REG |= 0x88;

  // no delay
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: SoundPlayCrash
 * ----------------------------
 */
void Script_SoundPlayCrash_b() {
  // enable sound
  NR52_REG = 0x80;

  // play crash sound on channel 4
  NR41_REG = 0x01;
  NR42_REG = (0x0F << 4) | 0x02;
  NR43_REG = 0x13;
  NR44_REG = 0x80;

  // enable volume
  NR50_REG = 0x77;

  // enable channel 4
  NR51_REG |= 0x88;

  // no delay
  script_ptr += 1 + script_cmd_args_len;
}

/*
 * Command: SetTimerScript
 * ----------------------------
 * Attach script to timer
 */
void Script_SetTimerScript_b() {
  /*
  timer_script_duration = script_cmd_args[0];
  timer_script_time = script_cmd_args[0];
  timer_script_ptr.bank = script_cmd_args[1];
  timer_script_ptr.offset = (script_cmd_args[2] * 256) + script_cmd_args[3];

  script_action_complete = TRUE;
  script_ptr += 1 + script_cmd_args_len;
  */
}

/*
 * Command: ResetTimer
 * ----------------------------
 * Reset the countdown timer
 */
void Script_ResetTimer_b() {
  /*
  timer_script_time = timer_script_duration;
  script_ptr += 1 + script_cmd_args_len;
  */
}

/*
 * Command: RemoveTimerScript
 * ----------------------------
 * Disable timer script
 */
void Script_RemoveTimerScript_b() {
  /*
  timer_script_duration = 0;
  script_ptr += 1 + script_cmd_args_len;
  */
}

/*
 * Command: Text with Avatar
 * ----------------------------
 * Display a line of dialogue with a 16x16 avatar on the left
 *
 *   arg0: High 8 bits for string index
 *   arg1: Low 8 bits for string index
 *   arg2: Spritesheet to use as the dialogue avatar
 */
void Script_TextWithAvatar_b() {
  script_ptr += 1 + script_cmd_args_len;
  UIShowText((script_cmd_args[0] * 256) + script_cmd_args[1]);
  UIShowAvatar(script_cmd_args[2]);
  script_update_fn = ScriptUpdate_AwaitUIClosed;
}

UBYTE ScriptLastFnComplete_b() {
  UBYTE fading = IsFading();

  /*
    if (last_fn == Script_ScenePopState_b) {
      return TRUE;
    }

    if (last_fn == Script_ScenePopAllState_b) {
      return TRUE;
    }

    if (last_fn == Script_LoadData_b && !fading) {
      return TRUE;
    }
  */
  /*
  if (last_fn == Script_ActorSetEmote_b && !SceneIsEmoting())
  {
    return TRUE;
  }
  */
  /*


    if (last_fn == Script_OverlayMoveTo_b && UIAtDest()) {
      return TRUE;
    }

    if (last_fn == Script_AwaitInput_b && AwaitInputPressed()) {
      return TRUE;
    }
  */
  /*
  if (last_fn == Script_CameraMoveTo_b && SceneCameraAtDest())
  {
    camera_settings = camera_settings & ~CAMERA_TRANSITION_FLAG;
    return TRUE;
  }

  if (last_fn == Script_CameraLock_b && SceneCameraAtDest())
  {
    camera_settings = CAMERA_LOCK_FLAG;
    return TRUE;
  }
  */

  return FALSE;
}
