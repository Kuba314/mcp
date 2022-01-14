#include "json.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "debug.h"

static void skip_whitespace(char **ptr)
{
    while(**ptr && isspace(**ptr)) {
        (*ptr)++;
    }
}

static json_value_t *_json_parse_number(const char *str, char **endptr)
{
    enum
    {
        STATE_SIGN,
        STATE_DIGIT,
        STATE_DECIMAL_DOT,
        STATE_DECIMAL,
        STATE_EXPONENT_SIGN,
        STATE_EXPONENT_EMPTY,
        STATE_EXPONENT,
    } state = STATE_SIGN;

    const char *const beginning = str;
    bool running = true;
    bool is_floating = false;
    while(running) {
        switch(state) {
        case STATE_SIGN:
            if(*str == '+' || *str == '-') {
                str++;
            }
            state = STATE_DIGIT;
            break;
        case STATE_DIGIT:
            if(*str == '.') {
                state = STATE_DECIMAL_DOT;
                is_floating = true;
                str++;
            } else if(*str == 'e' || *str == 'E') {
                state = STATE_EXPONENT_SIGN;
                is_floating = true;
                str++;
            } else if(isdigit(*str)) {
                str++;
            } else {
                running = false;
            }
            break;
        case STATE_DECIMAL_DOT:
            if(isdigit(*str)) {
                state = STATE_DECIMAL;
                str++;
            } else {
                running = false;
            }
            break;
        case STATE_DECIMAL:
            if(*str == 'e' || *str == 'E') {
                state = STATE_EXPONENT_SIGN;
                str++;
            } else if(isdigit(*str)) {
                str++;
            } else {
                running = false;
            }
            break;
        case STATE_EXPONENT_SIGN:
            if(*str == '+' || *str == '-') {
                str++;
            }
            state = STATE_EXPONENT_EMPTY;
            break;
        case STATE_EXPONENT_EMPTY:
        case STATE_EXPONENT:
            if(isdigit(*str)) {
                state = STATE_EXPONENT;
                str++;
            } else {
                running = false;
            }
        }
    }
    *endptr = (char *) str;
    switch(state) {
    case STATE_SIGN:
    case STATE_DECIMAL_DOT:
    case STATE_EXPONENT_SIGN:
    case STATE_EXPONENT_EMPTY:
        error("json", "number ended in a wrong state (%d)", state);
        return NULL;
    default: break;
    }

    json_value_t *ret = malloc(sizeof(json_value_t));
    if(ret == NULL) {
        alloc_error();
        return NULL;
    }
    char *tmp_endptr = NULL;
    if(is_floating) {
        ret->type = JSON_FLOATING;
        ret->floating = strtod(beginning, &tmp_endptr);
    } else {
        ret->type = JSON_NUMBER;
        ret->number = strtoll(beginning, &tmp_endptr, 10);
    }

    if(tmp_endptr != str) {
        free(ret);
        error("json", "c strtoxx function didn't agree (%p != %p)", tmp_endptr, str);
        return NULL;
    }
    return ret;
}
static json_value_t *_json_parse_string(const char *str, char **endptr)
{
    skip_whitespace(&str);

    int offset = find_closing_quote(str);
    if(offset == -1) {
        return NULL;
    }
    json_value_t *ret = malloc(sizeof(json_value_t));
    if(ret == NULL) {
        alloc_error();
        return NULL;
    }
    ret->type = JSON_STRING;
    ret->string.str = malloc(offset);
    if(ret->string.str == NULL) {
        alloc_error();
        free(ret);
        return NULL;
    }
    strncpy((char *) ret->string.str, str + 1, offset - 1);
    ret->string.length = offset - 1;
    return ret;
}
static json_value_t *_json_parse_object(const char *str, char **endptr)
{
    str++;
    json_object_t *obj = malloc(sizeof(json_object_t));
    if(obj == NULL) {
        alloc_error();
        return NULL;
    }
    while(true) {
        if(isspace(*str)) {
            continue;
        } else if(*str ==)
    }
}
static json_value_t *_json_parse_list(const char *str, char **endptr)
{
}
static json_value_t *_json_parse_keyword(const char *str, char **endptr)
{
    enum json_type type;
    bool value;
    size_t kw_length = 0;
    if(strcmp(str, "true") == 0) {
        type = JSON_BOOLEAN;
        value = true;
        kw_length = 4;
    } else if(strcmp(str, "false") == 0) {
        type = JSON_BOOLEAN;
        value = false;
        kw_length = 5;
    } else if(strcmp(str, "null") == 0) {
        type = JSON_NULL;
        kw_length = 4;
    } else {
        return NULL;
    }

    json_value_t *ret = malloc(sizeof(json_value_t));
    if(ret == NULL) {
        alloc_error();
        return NULL;
    }
    ret->type = type;
    if(type == JSON_BOOLEAN) {
        ret->boolean = value;
    }
    *endptr = (char *) str + kw_length;
    return ret;
}

json_value_t *json_parse(const char *str, char **endptr)
{
    skip_whitespace(&str);

    if(isdigit(*str)) {
        return _json_parse_number(str, endptr);
    } else if(*str == '"') {
        return _json_parse_string(str, endptr);
    } else if(*str == '{') {
        return _json_parse_object(str, endptr);
    } else if(*str == '[') {
        return _json_parse_list(str, endptr);
    } else {
        return _json_parse_keyword(str, endptr);
    }
}
void json_free(json_value_t *json)
{
    switch(json->type) {
    case JSON_STRING: {
        free((char *) json->string.str);
        break;
    }
    case JSON_LIST: {
        struct json_list_node *curr = json->list->head;
        struct json_list_node *tmp;

        while(curr != NULL) {
            tmp = curr;
            curr = curr->next;
            json_free(&tmp->value);
        }
        break;
    }
    case JSON_OBJECT: {
        struct json_object_node *curr = json->object->head;
        struct json_object_node *tmp;

        while(curr != NULL) {
            tmp = curr;
            curr = curr->next;
            free((char *) tmp->key.str);
            json_free(&tmp->value);
        }
        break;
    }
    default: break;
    }
    free(json);
}
