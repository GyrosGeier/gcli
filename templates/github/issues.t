include "gcli/issues.h";
include "gcli/labels.h";

include "templates/github/labels.h";

parser github_issue_milestone is
object of gcli_issue with
	("title" => milestone as sv);

parser github_issue is
object of gcli_issue with
	("title"        => title as sv,
	 "state"        => state as sv,
	 "body"         => body as sv,
	 "created_at"   => created_at as sv,
	 "number"       => number as id,
	 "comments"     => comments as int,
	 "user"         => author as user_sv,
	 "locked"       => locked as bool,
	 "labels"       => labels as array of github_label
	                   use parse_github_label_text,
	 "assignees"    => assignees as array of sn_sv use parse_user,
	 "pull_request" => is_pr as github_is_pr,
	 "milestone"    => use parse_github_issue_milestone);

parser github_issues is array of gcli_issue use parse_github_issue;
