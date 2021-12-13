/*
 * Copyright 2021 Nico Sonack <nsonack@outlook.com>
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

#include <ghcli/config.h>
#include <ghcli/json_util.h>
#include <ghcli/github/comments.h>

#include <pdjson/pdjson.h>

static void
github_parse_comment(json_stream *input, ghcli_comment *it)
{
    if (json_next(input) != JSON_OBJECT)
        errx(1, "Expected Comment Object");

    enum json_type key_type;
    while ((key_type = json_next(input)) == JSON_STRING) {
        size_t          len        = 0;
        const char     *key        = json_get_string(input, &len);
        enum json_type  value_type = 0;

        if (strncmp("created_at", key, len) == 0)
            it->date = get_string(input);
        else if (strncmp("body", key, len) == 0)
            it->body = get_string(input);
        else if (strncmp("user", key, len) == 0)
            it->author = get_user(input);
        else {
            value_type = json_next(input);

            switch (value_type) {
            case JSON_ARRAY:
                json_skip_until(input, JSON_ARRAY_END);
                break;
            case JSON_OBJECT:
                json_skip_until(input, JSON_OBJECT_END);
                break;
            default:
                break;
            }
        }
    }
}

void
github_perform_submit_comment(
    ghcli_submit_comment_opts  opts,
    ghcli_fetch_buffer        *out)
{
    char *post_fields = sn_asprintf(
        "{ \"body\": \""SV_FMT"\" }",
        SV_ARGS(opts.message));
    char *url         = sn_asprintf(
        "%s/repos/%s/%s/issues/%d/comments",
        ghcli_config_get_apibase(),
        opts.owner, opts.repo, opts.issue);

    ghcli_fetch_with_method("POST", url, post_fields, NULL, out);
    free(post_fields);
    free(url);
}

static int
github_get_comments(const char *url, ghcli_comment **comments)
{
    int                count       = 0;
    json_stream        stream      = {0};
    ghcli_fetch_buffer json_buffer = {0};

    ghcli_fetch(url, NULL, &json_buffer);
    json_open_buffer(&stream, json_buffer.data, json_buffer.length);
    json_set_streaming(&stream, true);

    enum json_type next_token = json_next(&stream);

    while ((next_token = json_peek(&stream)) != JSON_ARRAY_END) {
        if (next_token != JSON_OBJECT)
            errx(1, "Unexpected non-object in comment list");

        *comments = realloc(*comments, (count + 1) * sizeof(ghcli_comment));
        ghcli_comment *it = &(*comments)[count];
        github_parse_comment(&stream, it);
        count += 1;
    }

    json_close(&stream);
    free(json_buffer.data);

    return count;
}

int
github_get_issue_comments(
    const char     *owner,
    const char     *repo,
    int             issue,
    ghcli_comment **out)
{
    const char *url = sn_asprintf(
        "%s/repos/%s/%s/issues/%d/comments",
        ghcli_config_get_apibase(),
        owner, repo, issue);
    int n = github_get_comments(url, out);
    free((void *)url);
    return n;
}
