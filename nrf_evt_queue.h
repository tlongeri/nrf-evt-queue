#ifndef NRF_EVT_QUEUE_H
#define NRF_EVT_QUEUE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*nrf_evt_queue_evt_handler_t)(void);

typedef struct nrf_evt_queue_evt_s nrf_evt_queue_evt_t;

struct nrf_evt_queue_evt_s {
    nrf_evt_queue_evt_handler_t handler;
    nrf_evt_queue_evt_t * next;
};

/** @brief Return true if a given event is currently queued for execution */
bool nrf_evt_queue_is_queued(nrf_evt_queue_evt_t * p_evt);

/**
 * @brief Queue event
 * 
 * @retval NRF_SUCCESS             Event successfully queued
 * @retval NRF_ERROR_INVALID_STATE Event already queued
 */
uint32_t nrf_evt_queue_put(nrf_evt_queue_evt_t * p_evt, nrf_evt_queue_evt_handler_t callback);

/**
 * @brief Unqueue event
 * 
 * @retval NRF_SUCCESS Event successfully unqueued
 */
uint32_t nrf_evt_queue_remove(nrf_evt_queue_evt_t * p_evt);

/** @brief Execute all queued events */
void nrf_evt_queue_execute(void);

#ifdef __cplusplus
}
#endif

#endif // NRF_EVT_QUEUE_H