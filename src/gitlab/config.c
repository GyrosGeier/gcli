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

#include <gcli/config.h>
#include <gcli/gitlab/config.h>

#include <sn/sn.h>

static sn_sv
gitlab_default_account_name(gcli_ctx *ctx)
{
	sn_sv section_name;

	/* Use default override account */
	section_name = gcli_config_get_override_default_account(ctx);

	/* If not manually overridden */
	if (sn_sv_null(section_name)) {
		section_name = gcli_config_find_by_key(
			ctx,
			SV("defaults"),
			"gitlab-default-account");

		/* Welp, no luck here */
		if (sn_sv_null(section_name))
			warnx("Config file does not name a default GitLab account name.");
	}

	return section_name;
}

char *
gitlab_get_apibase(gcli_ctx *ctx)
{
	sn_sv account = gitlab_default_account_name(ctx);
	if (sn_sv_null(account))
		goto default_val;

	sn_sv api_base = gcli_config_find_by_key(ctx, account, "apibase");
	if (sn_sv_null(api_base))
		goto default_val;

	return sn_sv_to_cstr(api_base);

default_val:
	return "https://gitlab.com/api/v4";
}

char *
gitlab_get_authheader(gcli_ctx *ctx)
{
	sn_sv const account = gitlab_default_account_name(ctx);
	if (sn_sv_null(account))
		return NULL;

	sn_sv const token = gcli_config_find_by_key(ctx, account, "token");
	if (sn_sv_null(token))
		errx(1, "Missing GitLab token");
	return sn_asprintf("PRIVATE-TOKEN: "SV_FMT, SV_ARGS(token));
}

int
gitlab_get_account(gcli_ctx *ctx, sn_sv *out)
{
	sn_sv const section = gitlab_default_account_name(ctx);
	if (sn_sv_null(section))
		return gcli_error(ctx, "no default gitlab account");

	sn_sv const account = gcli_config_find_by_key(ctx, section, "account");
	if (sn_sv_null(account))
		return gcli_error(ctx, "missing account name in default gitlab account");

	*out = account;

	return 0;
}
