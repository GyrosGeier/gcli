/*
 * Copyright 2022 Nico Sonack <nsonack@herrhotzenplotz.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <gcli/status.h>
#include <gcli/forges.h>

int
gcli_get_notifications(gcli_ctx *ctx, int const max,
                       gcli_notification_list *const out)
{
	return gcli_forge(ctx)->get_notifications(ctx, max, out);
}

void
gcli_free_notifications(gcli_notification_list *list)
{
	for (size_t i = 0; i < list->notifications_size; ++i) {
		free(list->notifications[i].id);
		free(list->notifications[i].title);
		free(list->notifications[i].reason);
		free(list->notifications[i].date);
		free(list->notifications[i].type);
		free(list->notifications[i].repository);
	}

	free(list->notifications);
	list->notifications = NULL;
	list->notifications_size = 0;
}

int
gcli_notification_mark_as_read(gcli_ctx *ctx, char const *id)
{
	return gcli_forge(ctx)->notification_mark_as_read(ctx, id);
}
