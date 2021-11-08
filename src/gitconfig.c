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

#include <ghcli/gitconfig.h>
#include <ghcli/config.h>
#include <sn/sn.h>

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>

static const char *
find_file_in_dotgit(const char *fname)
{
    DIR           *curr_dir    = NULL;
    struct dirent *ent;
    char          *curr_dir_path;
    char          *dotgit      = NULL;
    char          *config_path = NULL;

    curr_dir_path = getcwd(NULL, 128);
    if (!curr_dir_path)
        err(1, "getcwd");

    /* Here we are trying to traverse upwards through the directory
     * tree, searching for a directory called .git.
     * Starting point is ".".*/
    do {
        curr_dir = opendir(curr_dir_path);
        if (!curr_dir)
            err(1, "opendir");

        while ((ent = readdir(curr_dir))) {
            if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
                continue;

            if (strcmp(".git", ent->d_name) == 0) {
                size_t len = strlen(curr_dir_path);
                dotgit = malloc(len + strlen(ent->d_name) + 2);
                memcpy(dotgit, curr_dir_path, len);
                dotgit[len] = '/';
                memcpy(dotgit + len + 1, ent->d_name, strlen(ent->d_name));

                dotgit[len + 1 + strlen(ent->d_name)] = 0;

                break;
            }
        }

        if (!dotgit) {
            size_t len = strlen(curr_dir_path);
            char *tmp = malloc(len + sizeof("/.."));

            memcpy(tmp, curr_dir_path, len);
            memcpy(tmp + len, "/..", sizeof("/.."));

            free(curr_dir_path);

            curr_dir_path = realpath(tmp, NULL);
            if (!curr_dir_path)
                err(1, "realpath at %s", tmp);

            free(tmp);

            if (strcmp("/", curr_dir_path) == 0) {
                free(curr_dir_path);
                closedir(curr_dir);
                errx(1, "Not a git repository");
            }
        }


        closedir(curr_dir);
    } while (dotgit == NULL);

    free(curr_dir_path);

    curr_dir = opendir(dotgit);
    if (!curr_dir)
        err(1, "opendir");

    while ((ent = readdir(curr_dir))) {
        if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
            continue;

        // We found the config file, put together it's path and return that
        if (strcmp(fname, ent->d_name) == 0) {
            int len = strlen(dotgit);

            config_path = malloc(len + 1 + sizeof(fname));

            memcpy(config_path, dotgit, len);
            config_path[len] = '/';

            memcpy(config_path + len + 1, fname, strlen(fname) + 1);

            closedir(curr_dir);
            free(dotgit);

            return config_path;
        }
    }

    errx(1, ".git without a config file");
    return NULL;
}

const char *
ghcli_find_gitconfig(void)
{
    return find_file_in_dotgit("config");
}

sn_sv
ghcli_gitconfig_get_current_branch(void)
{
    const char *HEAD;
    void       *mmap_pointer;
    sn_sv       buffer;
    char        prefix[] = "ref: refs/heads/";

    HEAD = find_file_in_dotgit("HEAD");

    if (!HEAD)
        return SV_NULL;

    int len = sn_mmap_file(HEAD, &mmap_pointer);
    if (len < 0)
        err(1, "mmap");

    buffer = sn_sv_from_parts(mmap_pointer, len);

    if (sn_sv_has_prefix(buffer, prefix)) {
        buffer.data   += sizeof(prefix) - 1;
        buffer.length -= sizeof(prefix) - 1;

        return sn_sv_trim(buffer);
    } else {
        munmap(mmap_pointer, len);
        return SV_NULL;
    }
}

static bool
gitconfig_find_url_entry(sn_sv buffer, sn_sv *out)
{
    while (buffer.length > 0) {
        buffer = sn_sv_trim_front(buffer);
        if (buffer.length == 0)
            return false;

        sn_sv line = sn_sv_chop_until(&buffer, '\n');
        if (sn_sv_has_prefix(line, "url")) {
            sn_sv_chop_until(&line, '=');
            *out = sn_sv_trim_front(line);

            return true;
        }
    }

    return false;
}

static char *gitconfig_org, *gitconfig_repo;
static bool  should_free_org_and_repo = 0;

static void
ghcli_gitconfig_atexit(void)
{
    if (should_free_org_and_repo) {
        /*
         * Prevent double free by setting to null */
        free(gitconfig_org);
        gitconfig_org = NULL;
        free(gitconfig_repo);
        gitconfig_repo = NULL;
    }
}

static bool
gitconfig_url_extract_github_data(sn_sv url, const char **org, const char **repo)
{
    sn_sv foo;

    foo         = sn_sv_chop_until(&url, '=');
    url.length -= 1;
    url.data   += 1;
    url         = sn_sv_trim_front(url);

    if (sn_sv_has_prefix(url, "https://")) {
        if (!sn_sv_has_prefix(url, "https://github.com/"))
            return false;

        url.data   += sizeof("https://github.com/") - 1;
        url.length -= sizeof("https://github.com/") - 1;
    } else {
        // SSH
        foo = sn_sv_chop_until(&url, '@');
        if (url.length == 0)
            return false;

        url.length -= 1;
        url.data   += 1;

        if (!sn_sv_has_prefix(url, "github.com"))
            return false;

        foo = sn_sv_chop_until(&url, ':');
        if (url.length == 0)
            return false;

        url.length -= 1;
        url.data   += 1;
    }

    foo = sn_sv_chop_until(&url, '/');
    if (url.length == 0)
        return false;

    *org = gitconfig_org = sn_strndup(foo.data, foo.length);

    url.length -= 1;
    url.data   += 1;

    *repo = gitconfig_repo = sn_strip_suffix(sn_strndup(url.data, url.length), ".git");

    should_free_org_and_repo = true;
    atexit(ghcli_gitconfig_atexit);

    return true;
}

void
ghcli_gitconfig_get_repo(const char **org, const char **repo)
{
    const char *path     = NULL;
    sn_sv       buffer   = {0};
    sn_sv       upstream = {0};

    if (gitconfig_org && gitconfig_repo) {
        *org  = gitconfig_org;
        *repo = gitconfig_repo;
    }

    if ((upstream = ghcli_config_get_upstream()).length != 0) {
        sn_sv org_sv   = sn_sv_chop_until(&upstream, '/');
        sn_sv repo_sv  = sn_sv_from_parts(upstream.data + 1, upstream.length - 1);

        *org  = gitconfig_org  = sn_sv_to_cstr(org_sv);
        *repo = gitconfig_repo = sn_sv_to_cstr(repo_sv);

        should_free_org_and_repo = true;
        atexit(ghcli_gitconfig_atexit);

        return;
    }

    path          = ghcli_find_gitconfig();
    buffer.length = sn_mmap_file(path, (void **)&buffer.data);

    while (buffer.length > 0) {
        buffer = sn_sv_trim_front(buffer);

        if (buffer.length == 0)
            break;

        // TODO: Git Config files support comments
        if (*buffer.data != '[')
            errx(1, "Invalid git config");

        sn_sv section_title = sn_sv_chop_until(&buffer, ']');
        section_title.length -= 1;
        section_title.data   += 1;

        buffer.length -= 2;
        buffer.data   += 2;

        sn_sv entry = sn_sv_chop_until(&buffer, '[');

        if (sn_sv_has_prefix(section_title, "remote")) {

            sn_sv url = {0};

            if (gitconfig_find_url_entry(entry, &url)
                && gitconfig_url_extract_github_data(url, org, repo))
                return;
        }
    }

    errx(1, "No GitHub remote found");
}
