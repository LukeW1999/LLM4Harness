#ifndef AWS_COMMON_HASH_TABLE_H
#define AWS_COMMON_HASH_TABLE_H

#include <aws/common/common.h>

#include <stddef.h>

AWS_PUSH_SANE_WARNING_LEVEL

enum {
    AWS_COMMON_HASH_TABLE_ITER_CONTINUE = (1 << 0),
    AWS_COMMON_HASH_TABLE_ITER_DELETE = (1 << 1),
    AWS_COMMON_HASH_TABLE_ITER_ERROR = (1 << 2),
};

struct hash_table_state; 
struct aws_hash_table {
    struct hash_table_state *p_impl;
};

struct aws_hash_element {
    const void *key;
    void *value;
};

enum aws_hash_iter_status {
    AWS_HASH_ITER_STATUS_DONE,
    AWS_HASH_ITER_STATUS_DELETE_CALLED,
    AWS_HASH_ITER_STATUS_READY_FOR_USE,
};

struct aws_hash_iter {
    const struct aws_hash_table *map;
    struct aws_hash_element element;
    size_t slot;
    size_t limit;
    enum aws_hash_iter_status status;
    
    int unused_0;
    void *unused_1;
    void *unused_2;
};

typedef uint64_t(aws_hash_fn)(const void *key);

typedef bool(aws_hash_callback_eq_fn)(const void *a, const void *b);

typedef void(aws_hash_callback_destroy_fn)(void *key_or_value);

AWS_EXTERN_C_BEGIN

AWS_COMMON_API
int aws_hash_table_init(
    struct aws_hash_table *map,
    struct aws_allocator *alloc,
    size_t size,
    aws_hash_fn *hash_fn,
    aws_hash_callback_eq_fn *equals_fn,
    aws_hash_callback_destroy_fn *destroy_key_fn,
    aws_hash_callback_destroy_fn *destroy_value_fn);

AWS_COMMON_API
void aws_hash_table_clean_up(struct aws_hash_table *map);

AWS_COMMON_API
void aws_hash_table_swap(struct aws_hash_table *AWS_RESTRICT a, struct aws_hash_table *AWS_RESTRICT b);

AWS_COMMON_API
void aws_hash_table_move(struct aws_hash_table *AWS_RESTRICT to, struct aws_hash_table *AWS_RESTRICT from);

AWS_COMMON_API
size_t aws_hash_table_get_entry_count(const struct aws_hash_table *map);

AWS_COMMON_API
struct aws_hash_iter aws_hash_iter_begin(const struct aws_hash_table *map);

AWS_COMMON_API
bool aws_hash_iter_done(const struct aws_hash_iter *iter);

AWS_COMMON_API
void aws_hash_iter_next(struct aws_hash_iter *iter);

AWS_COMMON_API
void aws_hash_iter_delete(struct aws_hash_iter *iter, bool destroy_contents);

AWS_COMMON_API
int aws_hash_table_find(const struct aws_hash_table *map, const void *key, struct aws_hash_element **p_elem);

AWS_COMMON_API
int aws_hash_table_create(
    struct aws_hash_table *map,
    const void *key,
    struct aws_hash_element **p_elem,
    int *was_created);

AWS_COMMON_API
int aws_hash_table_put(struct aws_hash_table *map, const void *key, void *value, int *was_created);

AWS_COMMON_API
int aws_hash_table_remove(
    struct aws_hash_table *map,
    const void *key,
    struct aws_hash_element *p_value,
    int *was_present);

AWS_COMMON_API
int aws_hash_table_remove_element(struct aws_hash_table *map, struct aws_hash_element *p_value);

AWS_COMMON_API
int aws_hash_table_foreach(
    struct aws_hash_table *map,
    int (*callback)(void *context, struct aws_hash_element *p_element),
    void *context);

AWS_COMMON_API
bool aws_hash_table_eq(
    const struct aws_hash_table *a,
    const struct aws_hash_table *b,
    aws_hash_callback_eq_fn *value_eq);

AWS_COMMON_API
void aws_hash_table_clear(struct aws_hash_table *map);

AWS_COMMON_API
uint64_t aws_hash_c_string(const void *item);

AWS_COMMON_API
uint64_t aws_hash_string(const void *item);

AWS_COMMON_API
uint64_t aws_hash_byte_cursor_ptr(const void *item);

AWS_COMMON_API
uint64_t aws_hash_ptr(const void *item);

AWS_COMMON_API
uint64_t aws_hash_combine(uint64_t item1, uint64_t item2);

AWS_COMMON_API
bool aws_hash_callback_c_str_eq(const void *a, const void *b);

AWS_COMMON_API
bool aws_hash_callback_string_eq(const void *a, const void *b);

AWS_COMMON_API
void aws_hash_callback_string_destroy(void *a);

AWS_COMMON_API
bool aws_ptr_eq(const void *a, const void *b);

AWS_COMMON_API
bool aws_hash_table_is_valid(const struct aws_hash_table *map);

AWS_COMMON_API
bool aws_hash_iter_is_valid(const struct aws_hash_iter *iter);

AWS_COMMON_API uint64_t aws_hash_uint64_t_by_identity(const void *item);

AWS_COMMON_API bool aws_hash_compare_uint64_t_eq(const void *a, const void *b);

AWS_EXTERN_C_END
AWS_POP_SANE_WARNING_LEVEL

#endif
