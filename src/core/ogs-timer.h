/*
 * Copyright (C) 2019 by Sukchan Lee <acetcom@gmail.com>
 *
 * This file is part of Open5GS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef OGS_TIMER_H
#define OGS_TIMER_H

#if !defined(OGS_CORE_INSIDE)
#error "Only <ogs-core.h> can be included directly."
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ogs_timer_mgr_s ogs_timer_mgr_t;
typedef struct ogs_timer_s ogs_timer_t;;

ogs_timer_mgr_t *ogs_timer_mgr_create(void);
void ogs_timer_mgr_destroy(ogs_timer_mgr_t *manager);

ogs_timer_t *ogs_timer_add(
        ogs_timer_mgr_t *manager, void (*cb)(void *data), void *data);
void ogs_timer_delete(ogs_timer_t *timer);

void ogs_timer_start(ogs_timer_t *timer, ogs_time_t duration);
void ogs_timer_stop(ogs_timer_t *timer);

ogs_time_t ogs_timer_mgr_next(ogs_timer_mgr_t *manager);
void ogs_timer_mgr_expire(ogs_timer_mgr_t *manager);

#ifdef __cplusplus
}
#endif

#endif /* OGS_TIMER_H */
