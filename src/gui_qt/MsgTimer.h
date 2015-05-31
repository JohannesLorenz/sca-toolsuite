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

#ifndef MSGTIMER_H
#define MSGTIMER_H

#include <QObject>
#include <QTimer>
#include <QQueue>

class MsgTimer : public QObject
{
	Q_OBJECT
	QQueue<QString> queue;
	QTimer timer;
	bool timer_running = false;
	constexpr static int timeout_msec = 4000;
private slots:
	void on_timeout();
public:
	explicit MsgTimer(QObject *parent = 0);
	void add_message(const char* msg);
signals:
	void on_message_changed(const char* new_msg);
};

#endif // MSGTIMER_H
