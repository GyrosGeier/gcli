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

#ifndef GITHUB_PULLS_H
#define GITHUB_PULLS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gcli/curl.h>
#include <gcli/pulls.h>

int github_get_pulls(gcli_ctx *ctx, char const *owner, char const *reponame,
                     gcli_pull_fetch_details const *details, int max,
                     gcli_pull_list *out);

int github_print_pull_diff(gcli_ctx *ctx, FILE *stream, char const *owner,
                           char const *reponame, gcli_id pr_number);

int github_pull_get_checks(gcli_ctx *ctx, char const *owner, char const *repo,
                           gcli_id pr_number, gcli_pull_checks_list *out);

int github_pull_merge(gcli_ctx *ctx, char const *owner, char const *reponame,
                      gcli_id pr_number, enum gcli_merge_flags flags);

int github_pull_reopen(gcli_ctx *ctx, char const *owner, char const *reponame,
                       gcli_id pr_number);

int github_pull_close(gcli_ctx *ctx, char const *owner, char const *reponame,
                      gcli_id pr_number);

int github_perform_submit_pull(gcli_ctx *ctx, gcli_submit_pull_options opts);

int github_get_pull_commits(gcli_ctx *ctx, char const *owner, char const *repo,
                            gcli_id pr_number, gcli_commit_list *out);

int github_get_pull(gcli_ctx *ctx, char const *owner, char const *repo,
                    gcli_id pr_number, gcli_pull *out);

sn_sv github_pull_try_derive_head(void);

int github_pull_add_reviewer(gcli_ctx *ctx, char const *owner, char const *repo,
                             gcli_id pr_number, char const *username);

#endif /* GITHUB_PULLS_H */
