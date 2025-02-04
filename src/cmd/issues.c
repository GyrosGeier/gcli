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

#include <config.h>

#include <gcli/cmd/cmd.h>
#include <gcli/cmd/cmdconfig.h>
#include <gcli/cmd/comment.h>
#include <gcli/cmd/editor.h>
#include <gcli/cmd/table.h>

#include <gcli/comments.h>
#include <gcli/issues.h>

#include <stdlib.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

static void
usage(void)
{
	fprintf(stderr, "usage: gcli issues create [-o owner -r repo] [-y] title...\n");
	fprintf(stderr, "       gcli issues [-o owner -r repo] [-a] [-n number] [-A author] [-L label] [-s]\n");
	fprintf(stderr, "       gcli issues [-o owner -r repo] -i issue actions...\n");
	fprintf(stderr, "OPTIONS:\n");
	fprintf(stderr, "  -o owner           The repository owner\n");
	fprintf(stderr, "  -r repo            The repository name\n");
	fprintf(stderr, "  -y                 Do not ask for confirmation.\n");
	fprintf(stderr, "  -A author          Only print issues by the given author\n");
	fprintf(stderr, "  -L label           Filter issues by the given label\n");
	fprintf(stderr, "  -a                 Fetch everything including closed issues \n");
	fprintf(stderr, "  -s                 Print (sort) in reverse order\n");
	fprintf(stderr, "  -n number          Number of issues to fetch (-1 = everything)\n");
	fprintf(stderr, "  -i issue           ID of issue to perform actions on\n");
	fprintf(stderr, "ACTIONS:\n");
	fprintf(stderr, "  all                Display both status and and op\n");
	fprintf(stderr, "  status             Display status information\n");
	fprintf(stderr, "  op                 Display original post\n");
	fprintf(stderr, "  comments           Display comments\n");
	fprintf(stderr, "  close              Close the issue\n");
	fprintf(stderr, "  reopen             Reopen a closed issue\n");
	fprintf(stderr, "  assign <user>      Assign the issue to the given user\n");
	fprintf(stderr, "  labels ...         Add or remove labels:\n");
	fprintf(stderr, "                        add <name>\n");
	fprintf(stderr, "                        remove <name>\n");
	fprintf(stderr, "  milestone <id>     Assign this issue to the given milestone\n");
	fprintf(stderr, "  milestone -d       Clear the assigned milestone of the given issue\n");
	fprintf(stderr, "  notes              Alias for comments\n");
	fprintf(stderr, "\n");
	version();
	copyright();
}

void
gcli_print_issues(enum gcli_output_flags const flags,
                  gcli_issue_list const *const list, int const max)
{
	int n, pruned = 0;
	gcli_tbl table;
	gcli_tblcoldef cols[] = {
		{ .name = "NUMBER", .type = GCLI_TBLCOLTYPE_INT, .flags = GCLI_TBLCOL_JUSTIFYR },
		{ .name = "NOTES",  .type = GCLI_TBLCOLTYPE_INT, .flags = GCLI_TBLCOL_JUSTIFYR },
		{ .name = "STATE",  .type = GCLI_TBLCOLTYPE_SV,  .flags = GCLI_TBLCOL_STATECOLOURED },
		{ .name = "TITLE",  .type = GCLI_TBLCOLTYPE_SV,  .flags = 0 },
	};

	if (list->issues_size == 0) {
		puts("No issues");
		return;
	}

	table = gcli_tbl_begin(cols, ARRAY_SIZE(cols));
	if (!table)
		errx(1, "could not init table printer");

	/* Determine the correct number of items to print */
	if (max < 0 || (size_t)(max) > list->issues_size)
		n = list->issues_size;
	else
		n = max;

	/* Iterate depending on the output order */
	if (flags & OUTPUT_SORTED) {
		for (int i = 0; i < n; ++i) {
			if (!list->issues[n - 1 - 1].is_pr) {
				gcli_tbl_add_row(table,
				                 list->issues[n - i - 1].number,
				                 list->issues[n - i - 1].comments,
				                 list->issues[n - i - 1].state,
				                 list->issues[n - i - 1].title);
			} else {
				pruned++;
			}
		}
	} else {
		for (int i = 0; i < n; ++i) {
			if (!list->issues[i].is_pr) {
				gcli_tbl_add_row(table,
				                 list->issues[i].number,
				                 list->issues[i].comments,
				                 list->issues[i].state,
				                 list->issues[i].title);
			} else {
				pruned++;
			}
		}
	}

	/* Dump the table */
	gcli_tbl_end(table);

	/* Inform the user that we pruned pull requests from the output */
	if (pruned && sn_getverbosity() != VERBOSITY_QUIET)
		fprintf(stderr, "info: %d pull requests pruned\n", pruned);
}

void
gcli_issue_print_summary(gcli_issue const *const it)
{
	gcli_dict dict;

	dict = gcli_dict_begin();

	gcli_dict_add(dict, "NAME", 0, 0, "%d", it->number);
	gcli_dict_add(dict, "TITLE", 0, 0, SV_FMT, SV_ARGS(it->title));
	gcli_dict_add(dict, "CREATED", 0, 0, SV_FMT, SV_ARGS(it->created_at));
	gcli_dict_add(dict, "AUTHOR",  GCLI_TBLCOL_BOLD, 0,
	              SV_FMT, SV_ARGS(it->author));
	gcli_dict_add(dict, "STATE", GCLI_TBLCOL_STATECOLOURED, 0,
	              SV_FMT, SV_ARGS(it->state));
	gcli_dict_add(dict, "COMMENTS", 0, 0, "%d", it->comments);
	gcli_dict_add(dict, "LOCKED", 0, 0, "%s", sn_bool_yesno(it->locked));

	if (it->milestone.length)
		gcli_dict_add(dict, "MILESTONE", 0, 0, SV_FMT, SV_ARGS(it->milestone));

	if (it->labels_size) {
		gcli_dict_add_sv_list(dict, "LABELS", it->labels, it->labels_size);
	} else {
		gcli_dict_add(dict, "LABELS", 0, 0, "none");
	}

	if (it->assignees_size) {
		gcli_dict_add_sv_list(dict, "ASSIGNEES",
		                      it->assignees, it->assignees_size);
	} else {
		gcli_dict_add(dict, "ASSIGNEES", 0, 0, "none");
	}

	/* Dump the dictionary */
	gcli_dict_end(dict);

}

void
gcli_issue_print_op(gcli_issue const *const it)
{
	if (it->body.length && it->body.data)
		pretty_print(it->body.data, 4, 80, stdout);
}

static void
issue_init_user_file(gcli_ctx *ctx, FILE *stream, void *_opts)
{
	(void) ctx;
	gcli_submit_issue_options *opts = _opts;
	fprintf(
		stream,
		"! ISSUE TITLE : "SV_FMT"\n"
		"! Enter issue description above.\n"
		"! All lines starting with '!' will be discarded.\n",
		SV_ARGS(opts->title));
}

static sn_sv
gcli_issue_get_user_message(gcli_submit_issue_options *opts)
{
	return gcli_editor_get_user_message(g_clictx, issue_init_user_file, opts);
}

static int
create_issue(gcli_submit_issue_options opts, int always_yes)
{
	int rc;

	opts.body = gcli_issue_get_user_message(&opts);

	printf("The following issue will be created:\n"
	       "\n"
	       "TITLE   : "SV_FMT"\n"
	       "OWNER   : %s\n"
	       "REPO    : %s\n"
	       "MESSAGE :\n"SV_FMT"\n",
	       SV_ARGS(opts.title),
	       opts.owner, opts.repo,
	       SV_ARGS(opts.body));

	putchar('\n');

	if (!always_yes) {
		if (!sn_yesno("Do you want to continue?"))
			errx(1, "Submission aborted.");
	}

	rc = gcli_issue_submit(g_clictx, opts);

	free(opts.body.data);
	free(opts.body.data);

	return rc;
}

static int
subcommand_issue_create(int argc, char *argv[])
{
	int ch;
	gcli_submit_issue_options opts = {0};
	int always_yes = 0;

	const struct option options[] = {
		{ .name    = "owner",
		  .has_arg = required_argument,
		  .flag    = NULL,
		  .val     = 'o' },
		{ .name    = "repo",
		  .has_arg = required_argument,
		  .flag    = NULL,
		  .val     = 'r' },
		{ .name    = "yes",
		  .has_arg = no_argument,
		  .flag    = NULL,
		  .val     = 'y' },
		{0},
	};

	while ((ch = getopt_long(argc, argv, "o:r:", options, NULL)) != -1) {
		switch (ch) {
		case 'o':
			opts.owner = optarg;
			break;
		case 'r':
			opts.repo = optarg;
			break;
		case 'y':
			always_yes = 1;
			break;
		default:
			usage();
			return EXIT_FAILURE;
		}
	}

	argc -= optind;
	argv += optind;

	check_owner_and_repo(&opts.owner, &opts.repo);

	if (argc != 1) {
		fprintf(stderr, "error: Expected one argument for issue title\n");
		usage();
		return EXIT_FAILURE;
	}

	opts.title = SV(argv[0]);

	if (create_issue(opts, always_yes) < 0)
		errx(1, "error: failed to submit issue: %s", gcli_get_error(g_clictx));

	return EXIT_SUCCESS;
}

static inline int handle_issues_actions(int argc, char *argv[],
                                        char const *const owner,
                                        char const *const repo,
                                        int const issue);

int
subcommand_issues(int argc, char *argv[])
{
	gcli_issue_list list = {0};
	char const *owner = NULL;
	char const *repo = NULL;
	char *endptr = NULL;
	int ch = 0, issue_id = -1, n = 30;
	gcli_issue_fetch_details details = {0};
	enum gcli_output_flags flags = 0;

	/* detect whether we wanna create an issue */
	if (argc > 1 && (strcmp(argv[1], "create") == 0)) {
		shift(&argc, &argv);
		return subcommand_issue_create(argc, argv);
	}

	const struct option options[] = {
		{ .name    = "all",
		  .has_arg = no_argument,
		  .flag    = NULL,
		  .val     = 'a' },
		{ .name    = "sorted",
		  .has_arg = no_argument,
		  .flag    = NULL,
		  .val     = 's' },
		{ .name    = "repo",
		  .has_arg = required_argument,
		  .flag    = NULL,
		  .val     = 'r' },
		{ .name    = "owner",
		  .has_arg = required_argument,
		  .flag    = NULL,
		  .val     = 'o' },
		{ .name    = "id",
		  .has_arg = required_argument,
		  .flag    = NULL,
		  .val     = 'i' },
		{ .name    = "count",
		  .has_arg = required_argument,
		  .flag    = NULL,
		  .val     = 'n' },
		{ .name    = "author",
		  .has_arg = required_argument,
		  .flag    = NULL,
		  .val     = 'A', },
		{ .name    = "label",
		  .has_arg = required_argument,
		  .flag    = NULL,
		  .val     = 'L',
		},
		{0},
	};

	/* parse options */
	while ((ch = getopt_long(argc, argv, "+sn:o:r:i:aA:L:", options, NULL)) != -1) {
		switch (ch) {
		case 'o':
			owner = optarg;
			break;
		case 'r':
			repo = optarg;
			break;
		case 'i': {
			issue_id = strtol(optarg, &endptr, 10);
			if (endptr != (optarg + strlen(optarg)))
				err(1, "error: cannot parse issue number");

			if (issue_id < 0)
				errx(1, "error: issue number is out of range");
		} break;
		case 'n': {
			n = strtol(optarg, &endptr, 10);
			if (endptr != (optarg + strlen(optarg)))
				err(1, "error: cannot parse issue count");

			if (n < -1)
				errx(1, "error: issue count is out of range");

			if (n == 0)
				errx(1, "error: issue count must not be zero");
		} break;
		case 'a':
			details.all = true;
			break;
		case 's':
			flags |= OUTPUT_SORTED;
			break;
		case 'A': {
			details.author = optarg;
		} break;
		case 'L': {
			details.label = optarg;
		} break;
		case '?':
		default:
			usage();
			return EXIT_FAILURE;
		}
	}

	argc -= optind;
	argv += optind;


	check_owner_and_repo(&owner, &repo);

	/* No issue number was given, so list all open issues */
	if (issue_id < 0) {
		if (gcli_get_issues(g_clictx, owner, repo, &details, n, &list) < 0)
			errx(1, "error: could not get issues: %s", gcli_get_error(g_clictx));

		gcli_print_issues(flags, &list, n);

		gcli_issues_free(&list);
		return EXIT_SUCCESS;
	}

	/* require -a to not be set */
	if (details.all) {
		fprintf(stderr, "error: -a cannot be combined with operations on an issue\n");
		usage();
		return EXIT_FAILURE;
	}

	/* Handle all the actions */
	return handle_issues_actions(argc, argv, owner, repo, issue_id);
}

static inline void
ensure_issue(char const *const owner, char const *const repo,
             int const issue_id,
             int *const have_fetched_issue, gcli_issue *const issue)
{
	if (*have_fetched_issue)
		return;

	if (gcli_get_issue(g_clictx, owner, repo, issue_id, issue) < 0)
		errx(1, "error: failed to retrieve issue data: %s",
		     gcli_get_error(g_clictx));

	*have_fetched_issue = 1;
}

static inline void
handle_issue_labels_action(int *argc, char ***argv,
                           char const *const owner,
                           char const *const repo,
                           int const issue_id)
{
	char const **add_labels = NULL;
	size_t add_labels_size = 0;
	char const **remove_labels = NULL;
	size_t remove_labels_size = 0;
	int rc = 0;

	if (argc == 0) {
		fprintf(stderr, "error: expected label operations\n");
		usage();
		exit(EXIT_FAILURE);
	}

	parse_labels_options(argc, argv, &add_labels, &add_labels_size,
	                     &remove_labels, &remove_labels_size);

	/* actually go about deleting and adding the labels */
	if (add_labels_size) {
		rc = gcli_issue_add_labels(g_clictx, owner, repo, issue_id,
		                           add_labels, add_labels_size);

		if (rc < 0) {
			errx(1, "error: failed to add labels: %s",
			     gcli_get_error(g_clictx));
		}
	}

	if (remove_labels_size) {
		rc = gcli_issue_remove_labels(g_clictx, owner, repo, issue_id,
		                              remove_labels, remove_labels_size);

		if (rc < 0) {
			errx(1, "error: failed to remove labels: %s",
			     gcli_get_error(g_clictx));
		}
	}

	free(add_labels);
	free(remove_labels);
}

static inline void
handle_issue_milestone_action(int *argc, char ***argv,
                              char const *const owner,
                              char const *const repo,
                              int const issue_id)
{
	char const *milestone_str;
	char *endptr;
	int milestone, rc;

	/* Set the milestone for the issue
	 *
	 * Check that the user provided a milestone id */
	if (!argc) {
		fprintf(stderr, "error: missing milestone id\n");
		usage();
		exit(EXIT_FAILURE);
	}

	/* Fetch the milestone from the argument vector */
	milestone_str = shift(argc, argv);

	/* Check if the milestone_str is -d indicating that we should
	 * clear the milestone */
	if (strcmp(milestone_str, "-d") == 0) {
		rc = gcli_issue_clear_milestone(g_clictx, owner, repo, issue_id);
		if (rc < 0) {
			errx(1, "error: could not clear milestone of issue #%d: %s",
			     issue_id, gcli_get_error(g_clictx));
		}
		return;
	}

	/* It is a milestone ID. Parse it. */
	milestone = strtoul(milestone_str, &endptr, 10);

	/* Check successful for parse */
	if (endptr != milestone_str + strlen(milestone_str)) {
		fprintf(stderr, "error: could not parse milestone id\n");
		usage();
		exit(EXIT_FAILURE);
	}

	/* Pass it to the dispatch */
	if (gcli_issue_set_milestone(g_clictx, owner, repo, issue_id, milestone) < 0)
		errx(1, "error: could not assign milestone: %s",
		     gcli_get_error(g_clictx));
}

static inline int
handle_issues_actions(int argc, char *argv[],
                      char const *const owner,
                      char const *const repo,
                      int const issue_id)
{
	int have_fetched_issue = 0;
	gcli_issue issue = {0};

	/* Check if the user missed out on supplying actions */
	if (argc == 0) {
		fprintf(stderr, "error: no actions supplied\n");
		usage();
		exit(EXIT_FAILURE);
	}

	/* execute all operations on the given issue */
	while (argc > 0) {
		char const *operation = shift(&argc, &argv);

		if (strcmp("all", operation) == 0) {
			/* Make sure we have fetched the issue data */
			ensure_issue(owner, repo, issue_id, &have_fetched_issue, &issue);

			gcli_issue_print_summary(&issue);

			puts("\nORIGINAL POST\n");
			gcli_issue_print_op(&issue);

		} else if (strcmp("comments", operation) == 0 ||
		           strcmp("notes", operation) == 0) {
			/* Doesn't require fetching the issue data */
			if (gcli_issue_comments(owner, repo, issue_id) < 0)
				errx(1, "error: failed to fetch issue comments: %s",
				     gcli_get_error(g_clictx));

		} else if (strcmp("op", operation) == 0) {
			/* Make sure we have fetched the issue data */
			ensure_issue(owner, repo, issue_id, &have_fetched_issue, &issue);

			gcli_issue_print_op(&issue);

		} else if (strcmp("status", operation) == 0) {
			/* Make sure we have fetched the issue data */
			ensure_issue(owner, repo, issue_id, &have_fetched_issue, &issue);

			gcli_issue_print_summary(&issue);

		} else if (strcmp("close", operation) == 0) {

			if (gcli_issue_close(g_clictx, owner, repo, issue_id) < 0)
				errx(1, "error: failed to close issue: %s",
				     gcli_get_error(g_clictx));

		} else if (strcmp("reopen", operation) == 0) {

			if (gcli_issue_reopen(g_clictx, owner, repo, issue_id) < 0)
				errx(1, "error: failed to reopen issue: %s",
				     gcli_get_error(g_clictx));

		} else if (strcmp("assign", operation) == 0) {

			char const *assignee = shift(&argc, &argv);
			if (gcli_issue_assign(g_clictx, owner, repo, issue_id, assignee) < 0)
				errx(1, "error: failed to assign issue: %s",
				     gcli_get_error(g_clictx));

		} else if (strcmp("labels", operation) == 0) {

			handle_issue_labels_action(
				&argc, &argv, owner, repo, issue_id);

		} else if (strcmp("milestone", operation) == 0) {

			handle_issue_milestone_action(
				&argc, &argv, owner, repo, issue_id);

		} else {
			fprintf(stderr, "error: unknown operation %s\n", operation);
			usage();
			return EXIT_FAILURE;
		}
	}

	if (have_fetched_issue)
		gcli_issue_free(&issue);

	return EXIT_SUCCESS;
}
