#ifndef SCHEDULER_H
#define SCHEDULER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*scheduler_evt_handler_t)(void);

typedef struct scheduler_evt_s scheduler_evt_t;

struct scheduler_evt_s {
    scheduler_evt_handler_t handler;
    scheduler_evt_t * next;
};

bool scheduler_is_queued(scheduler_evt_t * p_evt);
uint32_t scheduler_put(scheduler_evt_t * p_evt, scheduler_evt_handler_t callback);
uint32_t scheduler_remove(scheduler_evt_t * p_evt);

void scheduler_execute(void);

#ifdef __cplusplus
}
#endif

#endif // SCHEDULER_H