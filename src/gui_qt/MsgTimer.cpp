/*************************************************************************/
/* sca toolsuite - a toolsuite to simulate cellular automata.            */
/* Copyright (C) 2011-2015                                               */
/* Johannes Lorenz                                                       */
/* https://github.com/JohannesLorenz/sca-toolsuite                       */
/*                                                                       */
/* This program is free software; you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation; either version 3 of the License, or (at */
/* your option) any later version.                                       */
/* This program is distributed in the hope that it will be useful, but   */
/* WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      */
/* General Public License for more details.                              */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program; if not, write to the Free Software           */
/* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA  */
/*************************************************************************/

#include <QDebug>
#include "MsgTimer.h"

void MsgTimer::on_timeout()
{
	if(!queue.empty())
	{
		on_message_changed(queue.front().toAscii().data());
		queue.pop_front();
		timer.start(timeout_msec);
	}
}

MsgTimer::MsgTimer(QObject *parent) :
	QObject(parent)
{
	connect(&timer, SIGNAL(timeout()), this, SLOT(on_timeout()));
}

void MsgTimer::add_message(const char *msg)
{
	//queue.append(msg);
	if(timer_running)
	{
		queue.push_back(msg);
	}
	else // queue must be empty, bypass it
	{
		on_message_changed(msg);
		timer.start(timeout_msec);
	}
}
