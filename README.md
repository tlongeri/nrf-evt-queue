# Event Queue for the nRF SDK v12.3.0

## Example Usage

```c
#include <stdbool.h>
#include <stdint.h>

#include "nrf_evt_queue.h"
#include "nrf_soc.h"

nrf_evt_queue_evt_t my_evt;

void event_callback(void)
{
    /* Do stuff */
}

void some_function(void)
{
    /* Do stuff */

    /* Cancel event for some reason */
    uint32_t err_code = nrf_evt_queue_remove(&my_evt);
    if (NRF_SUCCESS == err_code)
    {
        /* Event unqueued */
    }
    else /* NRF_ERROR_INVALID_STATE */
    {
        /* Event was not queued */
    }
}

void some_interrupt(void)
{
    /* Queue event */
    uint32_t err_code = nrf_evt_queue_put(&my_evt, event_callback);
    if (NRF_SUCCESS == err_code)
    {
        /* Event queued, do stuff */
    }
    else /* NRF_ERROR_INVALID_STATE */
    {
        /* Event was already queued, handle */
    }
}

void main(void)
{
    /* Setup stuff */

    while (true)
    {
        nrf_evt_queue_execute(); /* Execute all queued events */
        sd_app_evt_wait();
    }
}
```

## Why use this over the SDK's scheduler?

The reasons I had for writing and using this instead of using the SDK's app_scheduler module are:

- Allows removing events.
- Do not have to worry about running out of space in the event queue, since it uses a linked list with statically allocated elements.

On the flipside:

- Each event can only be queued once. If you need to queue an event multiple times, you might want to keep a counter.
  This was rather intentional, though, since I find that I rarely need an event to be queued more than once, and if I try to queue it a second time it is often because something went wrong.
- Does not allow passing opaque pointers like app_scheduler does. I just never needed this, although it could be easily added into the event structure.

## Background

I wrote this because I had an existing project that used the nRF SDK, and not being able to remove events was becoming too much of a hassle. If you are starting a new project from scratch, you might want to consider a more complete RTOS alternative (Mynewt actually has a similar API, if I remember correctly).

I have only used and tested this with nRF SDK v12.3.0, but it might work for other versions. It only depends on the SDK for asserts, critical regions and error codes.
