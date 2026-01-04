/*
    dishwasher_controller
    Copyright (C) 2026 Jim Pudar

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "qpn.h"
#include <Arduino.h>

#include "bsp.h"
#include "debug.h"
#include "qm_generated/dishwasher_sm.h"

static Dishwasher dishwasher_inst;

// The global pointer to the Active Object from include/active_objects.h
QActive *const AO_Dishwasher = &dishwasher_inst.super;

static QEvt l_dishwasherQSto[10];

QActiveCB const Q_ROM QF_active[] = {
    {(QActive *)0,  (QEvt *)0,        0U                     },
    {AO_Dishwasher, l_dishwasherQSto, Q_DIM(l_dishwasherQSto)}
};

void setup()
{
    DEBUG_INIT();
    DEBUG_PRINTLN("QF_init");
    QF_init(Q_DIM(QF_active));

    DEBUG_PRINTLN("QActive_ctor Dishwasher");
    QActive_ctor(&dishwasher_inst.super, Q_STATE_CAST(&Dishwasher_initial));

    BSP_init();
}

void loop()
{
    QF_run(); // run the QF-nano framework
}