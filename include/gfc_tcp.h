/*
** gfc
**
** Copyright (C) 2019 doublegsoft.open
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*!
** @file gfc_net.h
**
** @author Christian Gann <guo.guo.gan@gmail.com>
**
** @date 2019-03-21
**
** @since 1.0
*/

#ifndef __GFC_TCP_H__
#define __GFC_TCP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "gfc_type.h"

GFC_API int
gfc_tcp_sock();

GFC_API int
gfc_tcp_connect(int sock, const char* hostname, int port);

#ifdef __cplusplus
}
#endif

#endif // __GFC_TCP_H__
