/*
 * kmod - log infrastructure
 *
 * Copyright (C) 2012  ProFUSION embedded systems
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include "libkmod.h"
#include "kmod.h"

static bool log_use_syslog;
static int log_priority = LOG_ERR;

void log_open(bool use_syslog)
{
	log_use_syslog = use_syslog;

	if (log_use_syslog)
		openlog(binname, LOG_CONS, LOG_DAEMON);
}

void log_close(void)
{
	if (log_use_syslog)
		closelog();
}

void log_kmod(void *data, int priority, const char *file, int line,
	      const char *fn, const char *format, va_list args)
{
	const char *prioname = prio_to_str(priority);
	char *str;

	if (vasprintf(&str, format, args) < 0)
		return;

	if (log_use_syslog) {
#ifdef ENABLE_DEBUG
		syslog(priority, "%s: %s:%d %s() %s", prioname, file, line,
		       fn, str);
#else
		syslog(priority, "%s: %s", prioname, str);
#endif
	} else {
#ifdef ENABLE_DEBUG
		fprintf(stderr, "%s: %s: %s:%d %s() %s", binname, prioname,
			file, line, fn, str);
#else
		fprintf(stderr, "%s: %s: %s", binname, prioname, str);
#endif
	}

	free(str);
	(void)data;
}

void log_setup_kmod_log(struct kmod_ctx *ctx, int priority)
{
	log_priority = priority;

	kmod_set_log_priority(ctx, log_priority);
	kmod_set_log_fn(ctx, log_kmod, NULL);
}
