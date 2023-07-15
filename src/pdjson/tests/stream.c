/* This tool prints out the token stream for standard input. It's primarily
 * intended to help write tests.
 */
#include <stdio.h>
#include "../pdjson.h"

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

int
main(void)
{
    json_stream s[1];
    json_open_stream(s, stdin);
    json_set_streaming(s, 1);
    puts("struct expect seq[] = {");
    for (bool first = true;;) {
        enum json_type type = json_next(s);
        const char *value = 0;
        switch (type) {
            case JSON_NULL:
                value = "null";
                break;
            case JSON_TRUE:
                value = "true";
                break;
            case JSON_FALSE:
                value = "false";
                break;
            case JSON_NUMBER:
                value = json_get_string(s, 0);
                break;
            case JSON_STRING:
                value = json_get_string(s, 0);
                break;
            case JSON_ARRAY:
            case JSON_OBJECT:
            case JSON_OBJECT_END:
            case JSON_ARRAY_END:
            case JSON_ERROR:
            case JSON_DONE:
                break;
        }
        if (value)
            printf("    {JSON_%s, \"%s\"},\n", json_typename[type], value);
        else
            printf("    {JSON_%s},\n", json_typename[type]);
        if (type == JSON_ERROR)
            break;
        if (type == JSON_DONE) {
            if (first)
                break;
            json_reset(s);
            first = true;
        } else
            first = false;
    }
    puts("};");
    json_close(s);
}
