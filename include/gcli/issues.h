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

#ifndef ISSUES_H
#define ISSUES_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sn/sn.h>
#include <gcli/gcli.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct gcli_issue gcli_issue;
typedef struct gcli_submit_issue_options gcli_submit_issue_options;
typedef struct gcli_issue_list gcli_issue_list;
typedef struct gcli_issue_fetch_details gcli_issue_fetch_details;

struct gcli_issue {
	int     number;
	sn_sv   title;
	sn_sv   created_at;
	sn_sv   author;
	sn_sv   state;
	int     comments;
	bool    locked;
	sn_sv   body;
	sn_sv  *labels;
	size_t  labels_size;
	sn_sv  *assignees;
	size_t  assignees_size;
	/* workaround for GitHub where PRs are also issues */
	int     is_pr;
	sn_sv   milestone;
};

struct gcli_submit_issue_options {
	char const *owner;
	char const *repo;
	sn_sv       title;
	sn_sv       body;
	bool        always_yes;
};

struct gcli_issue_list {
	gcli_issue *issues;
	size_t issues_size;
};

struct gcli_issue_fetch_details {
	bool all;                   /* disregard the issue state */
	char const *author;         /* A username who issued */
};

int gcli_get_issues(char const *owner,
                    char const *reponame,
                    gcli_issue_fetch_details const *details,
                    int max,
                    gcli_issue_list *out);

void gcli_issues_free(gcli_issue_list *);

void gcli_print_issues_table(enum gcli_output_flags flags,
                             gcli_issue_list const *list,
                             int max);

void gcli_get_issue(char const *owner,
                    char const *reponame,
                    int issue_number,
                    gcli_issue *out);

void gcli_issue_free(gcli_issue *it);

void gcli_issue_print_summary(gcli_issue const *it);
void gcli_issue_print_op(gcli_issue const *it);

int gcli_issue_close(char const *owner,
                     char const *repo,
                     int issue_number);

int gcli_issue_reopen(char const *owner,
                      char const *repo,
                      int issue_number);

int gcli_issue_submit(gcli_submit_issue_options);

int gcli_issue_assign(char const *owner,
                      char const *repo,
                      int issue_number,
                      char const *assignee);

int gcli_issue_add_labels(char const *owner,
                          char const *repo,
                          int issue_number,
                          char const *const labels[],
                          size_t labels_size);

int gcli_issue_remove_labels(char const *owner,
                             char const *repo,
                             int issue_number,
                             char const *const labels[],
                             size_t labels_size);

int gcli_issue_set_milestone(char const *owner,
                             char const *repo,
                             int issue,
                             int milestone);

int gcli_issue_clear_milestone(char const *owner,
                               char const *repo,
                               int issue);

#endif /* ISSUES_H */
