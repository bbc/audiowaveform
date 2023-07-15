#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../pdjson.h"

#if _WIN32
#  define C_RED(s)   s
#  define C_GREEN(s) s
#  define C_BOLD(s)  s
#else
#  define C_RED(s)   "\033[31;1m" s "\033[0m"
#  define C_GREEN(s) "\033[32;1m" s "\033[0m"
#  define C_BOLD(s)  "\033[1m"    s "\033[0m"
#endif

struct expect {
    enum json_type type;
    const char *str;
};

#define countof(a) (sizeof(a) / sizeof(*a))

#define TEST_IMPL(name, stream, sep) \
    do { \
        int r = test(name, stream, sep, seq, countof(seq), str, sizeof(str) - 1); \
        if (r) \
            count_pass++; \
        else \
            count_fail++; \
    } while (0)

#define TEST(name)             TEST_IMPL(name, false, '\0')
#define TEST_STREAM(name, sep) TEST_IMPL(name, true, sep)

const char json_typename[][16] = {
    [JSON_ERROR]      = "ERROR",
    [JSON_DONE]       = "DONE",
    [JSON_OBJECT]     = "OBJECT",
    [JSON_OBJECT_END] = "OBJECT_END",
    [JSON_ARRAY]      = "ARRAY",
    [JSON_ARRAY_END]  = "ARRAY_END",
    [JSON_STRING]     = "STRING",
    [JSON_NUMBER]     = "NUMBER",
    [JSON_TRUE]       = "TRUE",
    [JSON_FALSE]      = "FALSE",
    [JSON_NULL]       = "NULL",
};

static int
has_value(enum json_type type)
{
    return type == JSON_STRING || type == JSON_NUMBER;
}

static int
test(const char *name,
     int stream,
     char sep,
     struct expect *seq,
     size_t seqlen,
     const char *buf,
     size_t len)
{
    int success = 1;
    struct json_stream json[1];
    enum json_type expect, actual;
    const char *expect_str, *actual_str;

    json_open_buffer(json, buf, len);
    json_set_streaming(json, stream);
    for (size_t i = 0; success && i < seqlen; i++) {
        expect = seq[i].type;
        actual = json_next(json);
        actual_str = has_value(actual) ? json_get_string(json, 0) : "";
        expect_str = seq[i].str ? seq[i].str : "";

        if (actual != expect)
            success = 0;
        else if (seq[i].str && !!strcmp(expect_str, actual_str))
            success = 0;
        else if (stream && actual == JSON_DONE) {
            if (sep != '\0') {
                int c = '\0';
                while (json_isspace(c = json_source_peek(json))) {
                    json_source_get(json);
                    if (c == sep)
                        break;
                }
                if (c != sep && c != EOF)
                    success = 0;
            }
            json_reset(json);
        }
    }

    if (success) {
        printf(C_GREEN("PASS") " %s\n", name);
    } else {
        printf(C_RED("FAIL") " %s: "
               "expect " C_BOLD("%s") " %s / "
               "actual " C_BOLD("%s") " %s\n",
               name,
               json_typename[expect], expect_str,
               json_typename[actual], actual_str);
    }
    json_close(json);
    return success;
}

int
main(void)
{
    int count_pass = 0;
    int count_fail = 0;

    {
        const char str[] = "  1024\n";
        struct expect seq[] = {
            {JSON_NUMBER, "1024"},
            {JSON_DONE},
        };
        TEST("number");
    }

    {
        const char str[] = "  true \n";
        struct expect seq[] = {
            {JSON_TRUE},
            {JSON_DONE},
        };
        TEST("true");
    }

    {
        const char str[] = "\nfalse\r\n";
        struct expect seq[] = {
            {JSON_FALSE},
            {JSON_DONE},
        };
        TEST("false");
    }

    {
        const char str[] = "\tnull";
        struct expect seq[] = {
            {JSON_NULL},
            {JSON_DONE},
        };
        TEST("null");
    }

    {
        const char str[] = "\"foo\"";
        struct expect seq[] = {
            {JSON_STRING, "foo"},
            {JSON_DONE},
        };
        TEST("string");
    }

    {
        const char str[] = "\"Tim \\\"The Tool Man\\\" Taylor\"";
        struct expect seq[] = {
            {JSON_STRING, "Tim \"The Tool Man\" Taylor"},
            {JSON_DONE},
        };
        TEST("string quotes");
    }

    {
        const char str[] = "{\"abc\": -1}";
        struct expect seq[] = {
            {JSON_OBJECT},
            {JSON_STRING, "abc"},
            {JSON_NUMBER, "-1"},
            {JSON_OBJECT_END},
            {JSON_DONE},
        };
        TEST("object");
    }

    {
        const char str[] = "[1, \"two\", true, null]";
        struct expect seq[] = {
            {JSON_ARRAY},
            {JSON_NUMBER, "1"},
            {JSON_STRING, "two"},
            {JSON_TRUE},
            {JSON_NULL},
            {JSON_ARRAY_END},
            {JSON_DONE},
        };
        TEST("array");
    }

    {
        const char str[] = "1 10 100 2002";
        struct expect seq[] = {
            {JSON_NUMBER, "1"},
            {JSON_DONE},
            {JSON_NUMBER, "10"},
            {JSON_DONE},
            {JSON_NUMBER, "100"},
            {JSON_DONE},
            {JSON_NUMBER, "2002"},
            {JSON_DONE},
            {JSON_DONE},
        };
        TEST_STREAM("number stream", '\0');
    }

    {
        const char str[] = "{\"foo\": [1, 2, 3]}\n[]\n\"name\"";
        struct expect seq[] = {
            {JSON_OBJECT},
            {JSON_STRING, "foo"},
            {JSON_ARRAY},
            {JSON_NUMBER, "1"},
            {JSON_NUMBER, "2"},
            {JSON_NUMBER, "3"},
            {JSON_ARRAY_END},
            {JSON_OBJECT_END},
            {JSON_DONE},
            {JSON_ARRAY},
            {JSON_ARRAY_END},
            {JSON_DONE},
            {JSON_STRING, "name"},
            {JSON_DONE},
            {JSON_DONE},
        };
        TEST_STREAM("mixed stream", '\0');
    }

    {
        const char str[] = " \n";
        struct expect seq[] = {
            {JSON_DONE},
        };
        TEST_STREAM("empty stream", '\0');
    }

    {
        const char str[] = "1\n10 \n100 \n 2002";
        struct expect seq[] = {
            {JSON_NUMBER, "1"},
            {JSON_DONE},
            {JSON_NUMBER, "10"},
            {JSON_DONE},
            {JSON_NUMBER, "100"},
            {JSON_DONE},
            {JSON_NUMBER, "2002"},
            {JSON_DONE},
            {JSON_DONE},
        };
        TEST_STREAM("stream separation", '\n');
    }

    {
        const char str[] = "[1, 2, 3";
        struct expect seq[] = {
            {JSON_ARRAY},
            {JSON_NUMBER, "1"},
            {JSON_NUMBER, "2"},
            {JSON_NUMBER, "3"},
            {JSON_ERROR},
        };
        TEST("incomplete array");
    }

    {
        const char str[] = "\"\\u0068\\u0065\\u006c\\u006c\\u006F\"";
        struct expect seq[] = {
            {JSON_STRING, "hello"},
            {JSON_DONE},
        };
        TEST("\\uXXXX");
    }

    {
        /* This surrogate half must precede another half */
        const char str[] = "\"\\uD800\\u0065\"";
        struct expect seq[] = {
            {JSON_ERROR}
        };
        TEST("invalid surrogate pair");
    }

    {
        /* This surrogate half cannot be alone */
        const char str[] = "\"\\uDC00\"";
        struct expect seq[] = {
            {JSON_ERROR}
        };
        TEST("invalid surrogate half");
    }

    {
        /* Surrogate halves are in the wrong order */
        const char str[] = "\":\\uDc00\\uD800\"";
        struct expect seq[] = {
            {JSON_ERROR}
        };
        TEST("surrogate misorder");
    }

    {
        /* This is a valid encoding for U+10000 */
        const char str[] = "\":\\uD800\\uDC00\"";
        struct expect seq[] = {
            {JSON_STRING, ":\xf0\x90\x80\x80"}, /* UTF-8 for U+10000 */
            {JSON_DONE},
        };
        TEST("surrogate pair");
    }

    printf("%d pass, %d fail\n", count_pass, count_fail);
    exit(count_fail ? EXIT_FAILURE : EXIT_SUCCESS);
}
