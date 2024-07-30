/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "lib_fsm.h"
#include "stdlib.h"
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#ifdef LIB_MALLOC
#define LIB_FSM_MALLOC(x) LIB_MALLOC(x)
#else
#define LIB_FSM_MALLOC(x) malloc(x)
#endif

#ifdef LIB_FREE
#define LIB_FSM_FREE(x) LIB_FREE(x)
#else
#define LIB_FSM_FREE(x) free(x)
#endif

#ifdef LIB_INIT_EMPTY
#define LIB_FSM_INIT_EMPTY LIB_INIT_EMPTY
#else
#define LIB_FSM_INIT_EMPTY 1
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static lib_fsm_transition_t *
lib_fsm_get_transtition(lib_fsm_t *fsm, lib_fsm_event_t *event, void *param);
static void lib_fsm_goto_error_state(lib_fsm_t *fsm, lib_fsm_event_t *event,
				     void *action_param);

void lib_fsm_init(lib_fsm_t *fsm, lib_fsm_state_t *init_state,
		  lib_fsm_state_t *error_state)
{
	if (fsm == NULL) {
		return;
	}
	fsm->current = init_state;
	fsm->prev = NULL;
	fsm->error = error_state;
}

void lib_fsm_state_init_static(lib_fsm_state_t *state, char *name,
			       lib_fsm_action_fn entry_fn,
			       lib_fsm_action_fn exit_fn)
{
	if (state == NULL) {
		return;
	}
	state->name = name;
	state->entry_fn = entry_fn;
	state->exit_fn = exit_fn;
	state->num = 0;
}

void lib_fsm_reg_transition(lib_fsm_state_t *state, lib_fsm_state_t *next,
			    int32_t event, void *param,
			    lib_fsm_guard_fn guard_fn,
			    lib_fsm_action_fn action_fn)
{
	if (state == NULL || next == NULL) {
		return;
	}
	lib_fsm_transition_t *transition = NULL;
	transition = LIB_FSM_MALLOC(sizeof(lib_fsm_transition_t));
	if (transition == NULL) {
		return;
	}
#if LIB_FSM_INIT_EMPTY
	memset(transition, 0, sizeof(lib_fsm_transition_t));
#endif
	transition->event = event;
	transition->param = param;
	transition->guard_fn = guard_fn;
	transition->action_fn = action_fn;
	transition->state = next;
	if (state->transitions == NULL) {
		state->transitions = transition;
	} else {
		lib_fsm_transition_t *t = state->transitions;
		while (t->next != NULL) {
			t = t->next;
		}
		t->next = transition;
	}
	++state->num;
}

void lib_fsm_reg_transition_static(lib_fsm_state_t *state,
				   lib_fsm_state_t *next,
				   lib_fsm_transition_t *transition)
{
	if (state == NULL || next == NULL || transition == NULL) {
		return;
	}

	if (state->transitions == NULL) {
		state->transitions = transition;
	} else {
		lib_fsm_transition_t *t = state->transitions;
		while (t->next != NULL) {
			t = t->next;
		}
		t->next = transition;
	}
	++state->num;
}

static lib_fsm_transition_t *
lib_fsm_get_transtition(lib_fsm_t *fsm, lib_fsm_event_t *event, void *param)
{
	lib_fsm_state_t *current = fsm->current;
	volatile uint16_t num = current->num;
	lib_fsm_transition_t *transition = current->transitions;
	while (transition != NULL || num != 0) {
		--num;
		if (transition->event != event->type) {
			transition = transition->next;
			continue;
		}
		if (transition->guard_fn != NULL) {
			if (transition->guard_fn(event, transition->param,
						 param) == false) {
				transition = transition->next;
				continue;
			}
		}
		return transition;
	}

	return NULL;
}

static void lib_fsm_goto_error_state(lib_fsm_t *fsm, lib_fsm_event_t *event,
				     void *action_param)
{
	fsm->prev = fsm->current;
	fsm->current = fsm->error;
	if (fsm->current != NULL) {
		if (fsm->current->entry_fn != NULL) {
			fsm->current->entry_fn(event, action_param);
		}
	}
}

lib_fsm_status lib_fsm_push_event(lib_fsm_t *fsm, lib_fsm_event_t *event,
				  void *condition_param, void *action_param)
{
	if (fsm == NULL || event == NULL) {
		return LIB_FSM_ERROR;
	}
	lib_fsm_state_t *current_state = fsm->current;
	if (current_state == NULL) {
		lib_fsm_goto_error_state(fsm, event, action_param);
		return LIB_FSM_ERROR;
	}

	lib_fsm_transition_t *transition =
		lib_fsm_get_transtition(fsm, event, condition_param);
	if (transition == NULL) {
		return LIB_FSM_NO_CHANGE;
	}

	lib_fsm_state_t *next_state = transition->state;
	if (next_state == NULL) {
		lib_fsm_goto_error_state(fsm, event, action_param);
		return LIB_FSM_ERROR;
	}

	if (next_state != current_state && current_state->exit_fn) {
		current_state->exit_fn(event, action_param);
	}

	if (transition->action_fn) {
		transition->action_fn(event, action_param);
	}

	if (next_state != current_state && current_state->entry_fn) {
		next_state->entry_fn(event, action_param);
	}

	fsm->prev = fsm->current;
	fsm->current = next_state;

	return LIB_FSM_STATE_CHANGED;
}

inline lib_fsm_state_t *lib_fsm_get_current_state(lib_fsm_t *fsm)
{
	if (fsm == NULL) {
		return NULL;
	}
	return fsm->current;
}

inline lib_fsm_state_t *lib_fsm_get_prev_state(lib_fsm_t *fsm)
{
	if (fsm == NULL) {
		return NULL;
	}
	return fsm->prev;
}

inline char *lib_fsm_get_name(lib_fsm_state_t *state)
{
	if (state == NULL) {
		return NULL;
	}
	return state->name;
}
