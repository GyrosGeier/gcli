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

#include <gcli/pgen.h>

#include <assert.h>

static void
pregen_array_parser(struct objparser *p, struct objentry *it)
{
    fprintf(outfile,
            "static void\n"
            "parse_%s_%s_array(struct json_stream *stream, %s *out)\n",
            p->name, it->name, p->returntype);
    fprintf(outfile, "{\n");
    fprintf(outfile, "\tif (json_peek(stream) == JSON_NULL) {\n");
    fprintf(outfile, "\t\tjson_next(stream);\n");
    fprintf(outfile, "\t\tout->%s = NULL;\n", it->name);
    fprintf(outfile, "\t\tout->%s_size = 0;\n", it->name);
    fprintf(outfile, "\t\treturn;\n");
    fprintf(outfile, "\t}\n\n");

    fprintf(outfile, "\tif (json_next(stream) != JSON_ARRAY)\n");
    fprintf(outfile, "\t\terrx(1, \"Expected array for %s array in %s\");\n\n",
            it->name, p->name);

    fprintf(outfile, "\twhile (json_peek(stream) != JSON_ARRAY_END) {\n");
    fprintf(outfile, "\t\tout->%s = realloc(out->%s, sizeof(*out->%s) * (out->%s_size + 1));\n",
            it->name, it->name, it->name, it->name);
    fprintf(outfile, "\t\t%s(stream, &out->%s[out->%s_size++]);\n",
            it->parser, it->name, it->name);
    fprintf(outfile, "\t}\n\n");

    fprintf(outfile, "\tassert(json_next(stream) == JSON_ARRAY_END);\n");
    fprintf(outfile, "}\n\n");
}

static void
objparser_pregen_array_parsers(struct objparser *p)
{
    for (struct objentry *it = p->entries; it; it = it->next) {
        if (it->kind == OBJENTRY_ARRAY)
            pregen_array_parser(p, it);
    }
}

static void
objparser_dump_entries(struct objparser *p)
{
    fprintf(outfile, "\twhile ((key_type = json_next(stream)) == JSON_STRING) {\n");
    fprintf(outfile, "\t\tsize_t len;\n");
    fprintf(outfile, "\t\tkey = json_get_string(stream, &len);\n");

    for (struct objentry *it = p->entries; it; it = it->next)
    {
        fprintf(outfile, "\t\tif (strncmp(\"%s\", key, len) == 0)\n", it->jsonname);

        if (it->kind == OBJENTRY_SIMPLE) {

            if (it->parser)
                fprintf(outfile, "\t\t\t%s(stream, &out->%s);\n", it->parser, it->name);
            else
                fprintf(outfile, "\t\t\tout->%s = get_%s(stream);\n", it->name, it->type);

        } else if (it->kind == OBJENTRY_ARRAY) {
            fprintf(outfile, "\t\t\tparse_%s_%s_array(stream, out);\n",
                    p->name, it->name);
        }
        fprintf(outfile, "\t\telse ");
    }

    fprintf(outfile, "\n\t\t\tSKIP_OBJECT_VALUE(stream);\n");

    fprintf(outfile, "\t}\n");
}

static void
objparser_dump_select(struct objparser *p)
{
    fprintf(outfile, "\twhile ((key_type = json_next(stream)) == JSON_STRING) {\n");
    fprintf(outfile, "\t\tsize_t len;\n");
    fprintf(outfile, "\t\tkey = json_get_string(stream, &len);\n");
    fprintf(outfile, "\t\tif (strncmp(\"%s\", key, len) == 0)\n", p->select.fieldname);
    fprintf(outfile, "\t\t\t*out = get_%s(stream);\n", p->select.fieldtype);
    fprintf(outfile, "\t\telse ");
    fprintf(outfile, "\n\t\t\tSKIP_OBJECT_VALUE(stream);\n");
    fprintf(outfile, "\t}\n");
}

void
objparser_dump_c(struct objparser *p)
{
    objparser_pregen_array_parsers(p);

    fprintf(outfile,
            "void\n"
            "parse_%s(struct json_stream *stream, %s *out)\n",
            p->name, p->returntype);
    fprintf(outfile, "{\n");
    fprintf(outfile, "\tenum json_type key_type;\n");
    fprintf(outfile, "\tconst char *key;\n\n");
    fprintf(outfile, "\tjson_next(stream);\n\n");

    switch (p->kind) {
    case OBJPARSER_ENTRIES: objparser_dump_entries(p); break;
    case OBJPARSER_SELECT: objparser_dump_select(p); break;
    default: assert(0 && "unreached");
    }

    fprintf(outfile, "\tif (key_type != JSON_OBJECT_END)\n");
    fprintf(outfile, "\t\terrx(1, \"unexpected object key type\");\n");
    fprintf(outfile, "}\n\n");
}

void
include_dump_c(const char *file)
{
    fprintf(outfile, "#include <%s>\n", file);
}

void
header_dump_c(void)
{
    fprintf(outfile, "#include <pdjson/pdjson.h>\n");
    fprintf(outfile, "#include <assert.h>\n");
    fprintf(outfile, "#include <stdlib.h>\n");
}
