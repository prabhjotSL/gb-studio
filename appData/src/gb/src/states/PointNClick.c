// clang-format off
#pragma bank 5
// clang-format on

#include "states/PointNClick.h"
#include "Scroll.h"
#include "Input.h"
#include "Collision.h"
#include "Actor.h"
#include "Trigger.h"
#include "GameTime.h"
#include "ScriptRunner.h"
#include "Camera.h"
#include "rand.h"

#define POINT_N_CLICK_CAMERA_DEADZONE 24

UBYTE last_hit_trigger = MAX_TRIGGERS;

void Start_PointNClick() {
  camera_offset.x = 0;
  camera_offset.y = 0;
  camera_deadzone.x = POINT_N_CLICK_CAMERA_DEADZONE;
  camera_deadzone.y = POINT_N_CLICK_CAMERA_DEADZONE;

  player.sprite_type = SPRITE_STATIC;
  player.dir.x = 0;
  player.dir.y = 1;
  player.collisionsEnabled = FALSE;
  player.rerender = TRUE;
}

void Update_PointNClick() {
  UBYTE tile_x, tile_y, hit_actor, hit_trigger, hover_actor, hover_trigger;
  WORD dir_x, dir_y;

  tile_x = player.pos.x >> 3;
  tile_y = player.pos.y >> 3;

  player.moving = FALSE;

  dir_x = 0;
  dir_y = 0;

  // Move
  if (INPUT_LEFT) {
    dir_x = -1;
  } else if (INPUT_RIGHT) {
    dir_x = 1;
  }
  if (INPUT_UP) {
    dir_y = -1;
  } else if (INPUT_DOWN) {
    dir_y = 1;
  }

  if (dir_x != 0 || dir_y != 0) {
    PlayerSetMovement(dir_x, dir_y);
  }

  // Find trigger or actor under player cursor
  hit_trigger = TriggerAtTile(tile_x, tile_y - 1);
  hit_actor = ActorAtTile(tile_x, tile_y, TRUE);

  hover_trigger = (hit_trigger != MAX_TRIGGERS) && (hit_trigger != last_hit_trigger) &&
                  (triggers[hit_trigger].events_ptr.bank != 0);
  hover_actor = (hit_actor != NO_ACTOR_COLLISON) && (hit_actor != 0) &&
                (actors[hit_actor].events_ptr.bank != 0);

  // Set player cursor to second frame on hover
  if ((hover_trigger || hover_actor) && player.frames_len != 1) {
    player.frame = 1;
    player.rerender = TRUE;
  } else {
    player.frame = 0;
    player.rerender = TRUE;
  }

  if (INPUT_A_PRESSED) {
    PlayerStopMovement();

    if (hover_actor) {
      // Run actor's interact script
      ActorRunScript(hit_actor);
    } else if (hover_trigger) {
      // Run trigger script
      TriggerRunScript(hit_trigger);
    }
  }
}
