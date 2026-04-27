#ifndef AWS_COMMON_ARRAY_LIST_H
#define AWS_COMMON_ARRAY_LIST_H

#include <aws/common/common.h>
#include <aws/common/math.h>

#include <stdlib.h>

AWS_PUSH_SANE_WARNING_LEVEL

enum { AWS_ARRAY_LIST_DEBUG_FILL = 0xDD };

struct aws_array_list {
    struct aws_allocator *alloc;
    size_t current_size;
    size_t length;
    size_t item_size;
    void *data;
};

typedef int(aws_array_list_comparator_fn)(const void *a, const void *b);

AWS_EXTERN_C_BEGIN

AWS_STATIC_IMPL
int aws_array_list_init_dynamic(
    struct aws_array_list *AWS_RESTRICT list,
    struct aws_allocator *alloc,
    size_t initial_item_allocation,
    size_t item_size);

AWS_STATIC_IMPL
void aws_array_list_init_static(
    struct aws_array_list *AWS_RESTRICT list,
    void *raw_array,
    size_t item_count,
    size_t item_size);

AWS_STATIC_IMPL
void aws_array_list_init_static_from_initialized(
    struct aws_array_list *AWS_RESTRICT list,
    void *raw_array,
    size_t item_count,
    size_t item_size);

AWS_STATIC_IMPL
bool aws_array_list_is_valid(const struct aws_array_list *AWS_RESTRICT list);

AWS_STATIC_IMPL
void aws_array_list_clean_up(struct aws_array_list *AWS_RESTRICT list);

AWS_STATIC_IMPL
void aws_array_list_clean_up_secure(struct aws_array_list *AWS_RESTRICT list);

AWS_STATIC_IMPL
int aws_array_list_push_back(struct aws_array_list *AWS_RESTRICT list, const void *val);

AWS_STATIC_IMPL
int aws_array_list_front(const struct aws_array_list *AWS_RESTRICT list, void *val);

AWS_STATIC_IMPL
int aws_array_list_push_front(struct aws_array_list *AWS_RESTRICT list, const void *val);

AWS_STATIC_IMPL
int aws_array_list_pop_front(struct aws_array_list *AWS_RESTRICT list);

AWS_STATIC_IMPL
void aws_array_list_pop_front_n(struct aws_array_list *AWS_RESTRICT list, size_t n);

AWS_STATIC_IMPL
int aws_array_list_erase(struct aws_array_list *AWS_RESTRICT list, size_t index);

AWS_STATIC_IMPL
int aws_array_list_back(const struct aws_array_list *AWS_RESTRICT list, void *val);

AWS_STATIC_IMPL
int aws_array_list_pop_back(struct aws_array_list *AWS_RESTRICT list);

AWS_STATIC_IMPL
void aws_array_list_clear(struct aws_array_list *AWS_RESTRICT list);

AWS_COMMON_API
int aws_array_list_shrink_to_fit(struct aws_array_list *AWS_RESTRICT list);

AWS_COMMON_API
int aws_array_list_copy(const struct aws_array_list *AWS_RESTRICT from, struct aws_array_list *AWS_RESTRICT to);

AWS_STATIC_IMPL
void aws_array_list_swap_contents(
    struct aws_array_list *AWS_RESTRICT list_a,
    struct aws_array_list *AWS_RESTRICT list_b);

AWS_STATIC_IMPL
size_t aws_array_list_capacity(const struct aws_array_list *AWS_RESTRICT list);

AWS_STATIC_IMPL
size_t aws_array_list_length(const struct aws_array_list *AWS_RESTRICT list);

AWS_STATIC_IMPL
int aws_array_list_get_at(const struct aws_array_list *AWS_RESTRICT list, void *val, size_t index);

AWS_STATIC_IMPL
int aws_array_list_get_at_ptr(const struct aws_array_list *AWS_RESTRICT list, void **val, size_t index);

AWS_COMMON_API
int aws_array_list_ensure_capacity(struct aws_array_list *AWS_RESTRICT list, size_t index);

AWS_STATIC_IMPL
int aws_array_list_set_at(struct aws_array_list *AWS_RESTRICT list, const void *val, size_t index);

AWS_COMMON_API
void aws_array_list_swap(struct aws_array_list *AWS_RESTRICT list, size_t a, size_t b);

AWS_COMMON_API
void aws_array_list_sort(struct aws_array_list *AWS_RESTRICT list, aws_array_list_comparator_fn *compare_fn);

AWS_EXTERN_C_END
#ifndef AWS_NO_STATIC_IMPL
#    include <aws/common/array_list.inl>
#endif 

AWS_POP_SANE_WARNING_LEVEL

#endif
