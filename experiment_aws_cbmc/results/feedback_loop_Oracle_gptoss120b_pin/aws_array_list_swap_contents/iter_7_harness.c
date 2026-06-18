#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_ITEM_SIZE 256
#define MAX_INITIAL_ITEM_ALLOCATION 10

static size_t nondet_size_t(void) {
    return __CPROVER_nondet_size_t();
}

void aws_array_list_swap_contents_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    size_t item_sz = nondet_size_t();
    __CPROVER_assume(item_sz > 0);
    __CPROVER_assume(item_sz <= MAX_ITEM_SIZE);

    size_t cap_from = nondet_size_t();
    __CPROVER_assume(cap_from <= MAX_INITIAL_ITEM_ALLOCATION);
    size_t cap_to = nondet_size_t();
    __CPROVER_assume(cap_to <= MAX_INITIAL_ITEM_ALLOCATION);

    uint8_t *data_from = NULL;
    uint8_t *data_to   = NULL;
    if (cap_from > 0) {
        data_from = (uint8_t *)malloc(cap_from * item_sz);
        __CPROVER_assume(data_from != NULL);
    }
    if (cap_to > 0) {
        data_to = (uint8_t *)malloc(cap_to * item_sz);
        __CPROVER_assume(data_to != NULL);
    }

    size_t len_from = nondet_size_t();
    __CPROVER_assume(len_from <= cap_from);
    size_t len_to = nondet_size_t();
    __CPROVER_assume(len_to <= cap_to);

    struct aws_array_list from = {
        .alloc        = allocator,
        .item_size    = item_sz,
        .capacity     = cap_from,
        .length       = len_from,
        .current_size = len_from * item_sz,
        .data         = data_from
    };

    struct aws_array_list to = {
        .alloc        = allocator,
        .item_size    = item_sz,
        .capacity     = cap_to,
        .length       = len_to,
        .current_size = len_to * item_sz,
        .data         = data_to
    };

    struct aws_array_list old_from = from;
    struct aws_array_list old_to   = to;

    size_t max_bytes = MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE;
    uint8_t *buf_from = (uint8_t *)malloc(max_bytes);
    uint8_t *buf_to   = (uint8_t *)malloc(max_bytes);
    __CPROVER_assume(buf_from != NULL);
    __CPROVER_assume(buf_to   != NULL);

    if (old_from.data != NULL && old_from.current_size <= max_bytes) {
        for (size_t i = 0; i < old_from.current_size; ++i) {
            buf_from[i] = ((uint8_t *)old_from.data)[i];
        }
    }
    if (old_to.data != NULL && old_to.current_size <= max_bytes) {
        for (size_t i = 0; i < old_to.current_size; ++i) {
            buf_to[i] = ((uint8_t *)old_to.data)[i];
        }
    }

    aws_array_list_swap_contents(&from, &to);

    __CPROVER_assert(from.alloc == old_to.alloc, "allocator swapped");
    __CPROVER_assert(to.alloc   == old_from.alloc, "allocator swapped");

    __CPROVER_assert(from.item_size == old_to.item_size, "item_size swapped");
    __CPROVER_assert(to.item_size   == old_from.item_size, "item_size swapped");

    __CPROVER_assert(aws_array_list_length(&from) == aws_array_list_length(&old_to), "length swapped");
    __CPROVER_assert(aws_array_list_length(&to)   == aws_array_list_length(&old_from), "length swapped");

    __CPROVER_assert(from.current_size == old_to.current_size, "current_size swapped");
    __CPROVER_assert(to.current_size   == old_from.current_size, "current_size swapped");

    __CPROVER_assert(from.data == old_to.data, "data pointer swapped");
    __CPROVER_assert(to.data   == old_from.data, "data pointer swapped");

    __CPROVER_assert(aws_array_list_capacity(&from) == aws_array_list_capacity(&old_to), "capacity swapped");
    __CPROVER_assert(aws_array_list_capacity(&to)   == aws_array_list_capacity(&old_from), "capacity swapped");

    if (old_from.data != NULL && old_from.current_size <= max_bytes) {
        for (size_t i = 0; i < old_from.current_size; ++i) {
            __CPROVER_assert(((uint8_t *)old_from.data)[i] == buf_from[i], "from buffer unchanged");
        }
    }
    if (old_to.data != NULL && old_to.current_size <= max_bytes) {
        for (size_t i = 0; i < old_to.current_size; ++i) {
            __CPROVER_assert(((uint8_t *)old_to.data)[i] == buf_to[i], "to buffer unchanged");
        }
    }

    __CPROVER_assert(aws_array_list_is_valid(&from), "from list valid");
    __CPROVER_assert(aws_array_list_is_valid(&to),   "to list valid");

    free(buf_from);
    free(buf_to);
    free(data_from);
    free(data_to);
}
