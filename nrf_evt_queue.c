#include <string.h>

#include "app_util_platform.h"
#include "nrf_error.h"

#include "scheduler.h"

scheduler_evt_t * p_evt_queue_head = NULL;

/**
 * @ret A pointer to the predecessor, or NULL if it was not found
 * 
 * @warning Should be called within a critical section
 * @warning p_evt_queue_head should have been checked before calling this, and not be NULL or p_evt
 *          itself
 */
static scheduler_evt_t * find_predecessor(scheduler_evt_t * p_evt)
{
    ASSERT(p_evt_queue_head != NULL && p_evt_queue_head != p_evt);
    scheduler_evt_t * p_iterating_evt = p_evt_queue_head;
    while (NULL != p_iterating_evt->next && p_evt != p_iterating_evt->next)
    {
        p_iterating_evt = p_iterating_evt->next;
    }

    scheduler_evt_t * ret_val;
    if (NULL == p_iterating_evt->next)
    {
        ret_val = NULL;
    }
    else
    {
        ret_val = p_iterating_evt;
    }
    return ret_val;
}

static scheduler_evt_t * get_last(void)
{
    scheduler_evt_t * p_iterating_evt = p_evt_queue_head;

    if (NULL != p_iterating_evt)
    {
        while (NULL != p_iterating_evt->next)
        {
            p_iterating_evt = p_iterating_evt->next;
        }
    }
    return p_iterating_evt;
}

bool scheduler_is_queued(scheduler_evt_t * p_evt)
{
    uint8_t interrupt_state;
    app_util_critical_region_enter(&interrupt_state);
    scheduler_evt_t * p_iterating_evt = p_evt_queue_head;
    while (NULL != p_iterating_evt && p_evt != p_iterating_evt)
    {
        p_iterating_evt = p_iterating_evt->next;
    }
    app_util_critical_region_exit(interrupt_state);
    return p_iterating_evt == p_evt;
}

uint32_t scheduler_put(scheduler_evt_t * p_evt, scheduler_evt_handler_t callback)
{
    ASSERT(NULL != callback);

    uint32_t ret_val;
    uint8_t interrupt_state;
    app_util_critical_region_enter(&interrupt_state);
    if (NULL == p_evt_queue_head)
    {
        p_evt->next = NULL;
        p_evt->handler = callback;
        p_evt_queue_head = p_evt;
        ret_val = NRF_SUCCESS;
    }
    else if (p_evt == p_evt_queue_head)
    {
        ret_val = NRF_ERROR_INVALID_STATE;
    }
    else
    {
        scheduler_evt_t * p_predecessor = find_predecessor(p_evt);
        if (NULL == p_predecessor) // Was not found already queued
        {
            scheduler_evt_t * p_last = get_last();
            ASSERT(NULL != p_last);
            p_evt->next = NULL;
            p_evt->handler = callback;
            p_last->next = p_evt;
            ret_val = NRF_SUCCESS;
        }
        else // Was already queued
        {
            ret_val = NRF_ERROR_INVALID_STATE;
        }
    }
    app_util_critical_region_exit(interrupt_state);
    return ret_val;
}


uint32_t scheduler_remove(scheduler_evt_t * p_evt)
{
    uint32_t ret_val;
    uint8_t interrupt_state;
    app_util_critical_region_enter(&interrupt_state);
    if (NULL == p_evt_queue_head)
    {
        ret_val = NRF_ERROR_INVALID_STATE;
    }
    else if (p_evt == p_evt_queue_head)
    {
        p_evt_queue_head = p_evt_queue_head->next;
        ret_val = NRF_SUCCESS;
    }
    else
    {
        scheduler_evt_t * p_predecessor = find_predecessor(p_evt);
        if (NULL == p_predecessor) // Was not found already queued
        {
            ret_val = NRF_ERROR_INVALID_STATE;
        }
        else // Was already queued
        {
            p_predecessor->next = p_evt->next;
            ret_val = NRF_SUCCESS;
        }
    }

    app_util_critical_region_exit(interrupt_state);
    return ret_val;
}


/**
 * @warning Call only from main context
 */
void scheduler_execute(void)
{
    uint8_t interrupt_state;
    app_util_critical_region_enter(&interrupt_state);
    while (p_evt_queue_head != NULL)
    {
        scheduler_evt_handler_t handler = p_evt_queue_head->handler;
        p_evt_queue_head = p_evt_queue_head->next;
        app_util_critical_region_exit(interrupt_state);
        
        handler();

        app_util_critical_region_enter(&interrupt_state);
    }
    app_util_critical_region_exit(interrupt_state);    
}