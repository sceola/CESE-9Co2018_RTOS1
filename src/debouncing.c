/*=============================================================================
 * Copyright (c) 2019, Sebastian Ceola <sebastian.ceola@gmail.com>
 * All rights reserved.
 * License: mit (see LICENSE.txt)
 * Date: 2019/06/06
 * Version: 0.0.1
 *===========================================================================*/
#include <board.h>
#include "debouncing.h"

void debouncer_init( debouncer_type* db, unsigned minpulse, int pin )
{
    db->minpulse = minpulse;
    db->pin      = pin;
    db->state    = ST_UNDEF;
	db->edge	 = false;
}

void debouncer_update( debouncer_type* db )
{
    bool pinval = Board_TEC_GetStatus(db->pin);

    db->elapsed++;
	db->edge = false;

    switch (db->state)
    {
        case ST_HIGH:
            if (!pinval)
            {
                db->state = ST_HI_TO_LO;
                db->elapsed = 0;
            }
            break;

        case ST_HI_TO_LO:
            if (pinval)
            {
                db->state = ST_HIGH;
            }
            else
            {
                if (db->elapsed >= db->minpulse)
				{
                    db->state = ST_LOW;
					db->edge  = true;
				}
            }
            break;

        case ST_LOW:
            if (pinval)
            {
                db->state = ST_LO_TO_HI;
                db->elapsed = 0;
            }
            break;

        case ST_LO_TO_HI:
            if (!pinval)
            {
                db->state = ST_LOW;
            }
            else
            {
                if (db->elapsed >= db->minpulse)
				{
                    db->state = ST_HIGH;
					db->edge  = true;
				}
            }
            break;

        case ST_UNDEF:
        default:
            if (pinval)
                db->state = ST_HIGH;
            else
                db->state = ST_LOW;
            break;
    }
}

int debouncer_is_hi( const debouncer_type* db )
{
    int ret;
    switch (db->state)
    {
        case ST_HIGH:
        case ST_HI_TO_LO:
            ret = 1;
            break;
        case ST_LOW:
        case ST_LO_TO_HI:
        default:
            ret = 0;
            break;
    }
    return ret;
}

bool debouncer_is_edge( const debouncer_type* db )
{
	return db->edge;
}
