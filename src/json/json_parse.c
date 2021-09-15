

enum json_parse_state {
    JSON_STATE_VALUE,
    JSON_STATE_OBJECT_BEGIN,
    JSON_STATE_OBJECT_COLON,
    JSON_STATE_AWAIT_LIST_VALUE,
};



json_value_t *json_parse(const char *str, size_t length)
{
    return _json_parse(str, length, NULL);
}
static json_value_t *json_parse(const char *str, size_t length, char **endptr);
{
    enum json_parse_state state = JSON_STATE_VALUE;

    switch(state) {
    case JSON_STATE_VALUE:
        switch(*str) {
        case '{':
            find_closing_char(str, '}')
        }
    }
}
void json_free(json_value_t *json)
{
    switch(json->type) {
    case JSON_STRING:
        free(json->string.str);
        break;
    case JSON_LIST:
        struct json_list_node *curr = json->list->head;
        struct json_list_node *tmp;

        while(curr != NULL) {
            tmp = curr;
            curr = curr->next;
            json_free(tmp->value);
        }
        break;
    case JSON_OBJECT:
        struct json_object_node *curr = json->object->head;
        struct json_object_node *tmp;

        while(curr != NULL) {
            tmp = curr;
            curr = curr->next;
            free(tmp->key.str);
            json_free(tmp->value);
        }
        break;
    }
    free(json);
}
