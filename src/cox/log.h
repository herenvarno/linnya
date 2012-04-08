/*
 * log.h
 * This file is part of linnya
 *
 * Copyright (C) 2011 - Edward Yang
 *
 * linnya is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * linnya is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linnya. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * SECTION:		log
 * @short_description:	record logs
 * @stability:	stable
 * @include:	cox.h
 *
 * This module is a module to record all logs into a particular file. which allow
 * developers to look for bugs.
 */

#ifndef LY_LOG_H
#define LY_LOG_H

#include "gla.h"
#include "msg.h"

void	ly_log_init		();
void	ly_log_fina		();

void	ly_log_put		(const char *format, ...);
void	ly_log_clear	();

#endif
