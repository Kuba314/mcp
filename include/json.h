

enum json_type {
    JSON_BOOLEAN,
    JSON_STRING,
    JSON_NUMBER,
    JSON_FLOATING,
    JSON_LIST,
    JSON_OBJECT,
};

typedef struct {
    const char *str;
    size_t length;
} json_string_t;

struct json_list_node {
    struct json_list_node *next;
    json_value_t value;
};
struct json_object_node {
    struct json_object_node *next;
    json_string_t key;
    json_value_t value;
};

typedef struct {
    struct json_list_node *head;
    size_t length;
} json_list_t;

typedef struct {
    struct json_object_node *head;
    size_t length;
} json_object_t;

typedef struct {
    enum json_type type;
    union {
        bool boolean;
        json_string_t string;
        int64_t number;
        double floating;
        json_list_t list;
        json_object_t object;
    };
} json_value_t;

json_value_t *json_parse(const char *str, size_t length);
void json_free(json_value_t *json);

#define JSON_LIST_FOREACH(json_list, json_iter)                                \
    for(struct json_list_node *json_iter = json_list->head;                    \
        json_iter->next != NULL; json_iter = json_iter->next)

#define JSON_OBJECT_FOREACH(json_object, json_iter)                            \
    for(struct json_object_node *json_iter = json_object->head;                \
        json_iter->next != NULL; json_iter = json_iter->next)
