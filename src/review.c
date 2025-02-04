/*
 * Copyright 2021, 2022 Nico Sonack <nsonack@herrhotzenplotz.de>
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

#include <gcli/curl.h>
#include <gcli/forges.h>
#include <gcli/github/config.h>
#include <gcli/json_util.h>
#include <gcli/review.h>

#include <pdjson/pdjson.h>

#include <limits.h>

void
gcli_review_reviews_free(gcli_pr_review_list *list)
{
	if (!list)
		return;

	for (size_t i = 0; i < list->reviews_size; ++i) {
		free(list->reviews[i].author);
		free(list->reviews[i].date);
		free(list->reviews[i].state);
		free(list->reviews[i].body);
		free(list->reviews[i].id);
	}

	free(list->reviews);

	list->reviews = NULL;
	list->reviews_size = 0;
}

void
gcli_review_comments_free(gcli_pr_review_comment *it, size_t const size)
{
	if (!it)
		return;

	for (size_t i = 0; i < size; ++i) {
		free(it[i].id);
		free(it[i].author);
		free(it[i].date);
		free(it[i].diff);
		free(it[i].path);
		free(it[i].body);
	}

	free(it);
}

int
gcli_review_get_reviews(gcli_ctx *ctx, char const *owner, char const *repo,
                        int const pr, gcli_pr_review_list *const out)
{
	return gcli_forge(ctx)->get_reviews(ctx, owner, repo, pr, out);
}
