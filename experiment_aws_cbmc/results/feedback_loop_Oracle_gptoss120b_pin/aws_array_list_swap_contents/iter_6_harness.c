#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

static size_t nondet_size_t(void) {
    return __CPROVER_nondet_size_t();
}

void aws_array_list_swap_contents_harness(void) {
    /* use a valid allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* nondet parameters */
    size_t item_sz = nondet_size_t();
    __CPROVER_assume(item_sz > 0);
    __CPROVER_assume(item_sz <= MAX_ITEM_SIZE);

    size_t cap_from = nondet_size_t();
    __CPROVER_assume(cap_from <= MAX_INITIAL_ITEM_ALLOCATION);
    size_t cap_to = nondet_size_t();
    __CPROVER_assume(cap_to <= MAX_INITIAL_ITEM_ALLOCATION);

    /* allocate data buffers */
    uint8_t *data_from = NULL;
    uint8_t *data_to   = NULL;
    if (cap_from > 0) {
        data_from = malloc(cap_from * item_sz);
        __CPROVER_assume(data_from != NULL);
    }
    if (cap_to > 0) {
        data_to = malloc(cap_to * item_sz);
        __CPROVER_assume(data_to != NULL);
    }

    /* nondet lengths (number of items) */
    size_t len_from = nondet_size_t();
    __CPROVER_assume(len_from <= cap_from);
    size_t len_to = nondet_size_t();
    __CPROVER_assume(len_to <= cap_to);

    struct aws_array_list from;
    from.alloc        = allocator;
    from.item_size    = item_sz;
    from.capacity     = cap_from;
    from.length       = len_from;
    from.current_size = len_from * item_sz;
    from.data         = data_from;

    struct aws_array_list to;
    to.alloc        = allocator;
    to.item_size    = item_sz;
    to.capacity     = cap_to;
    to.length       = len_to;
    to.current_size = len_to * item_sz;
    to.data         = data_to;

    /* capture old state */
    struct aws_array_list old_from = from;
    struct aws_array_list old_to   = to;

    /* capture data buffer contents (frame condition) */
    size_t max_bytes = MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE;
    uint8_t *buf_from = malloc(max_bytes);
    uint8_t *buf_to   = malloc(max_bytes);
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

    /* call the function under test */
    aws_array_list_swap_contents(&from, &to);

    /* post‑conditions */
    assert(from.alloc == old_to.alloc);
    assert(to.alloc   == old_from.alloc);

    assert(from.item_size == old_to.item_size);
    assert(to.item_size   == old_from.item_size);

    assert(aws_array_list_length(&from) == aws_array_list_length(&old_to));
    assert(aws_array_list_length(&to)   == aws_array_list_length(&old_from));

    assert(from.current_size == old_to.current_size);
    assert(to.current_size   == old_from.current_size);

    assert(from.data == old_to.data);
    assert(to.data   == old_from.data);

    assert(aws_array_list_capacity(&from) == aws_array_list_capacity(&old_to));
    assert(aws_array_list_capacity(&to)   == aws_array_list_capacity(&old_from));

    /* frame condition: underlying data buffers unchanged */
    if (old_from.data != NULL && old_from.current_size <= max_bytes) {
        for (size_t i = 0; i < old_from.current_size; ++i) {
            assert(((uint8_t *)old_from.data)[i] == buf_from[i]);
        }
    }
    if (old_to.data != NULL && old_to.current_size <= max_bytes) {
        for (size_t i = 0; i < old_to.current_size; ++i) {
            assert(((uint8_t *)old_to.data)[i] == buf_to[i]);
        }
    }

    /* validity predicates must still hold */
    assert(aws_array_list_is_valid(&from));
    assert(aws_array_list_is_valid(&to));

    free(buf_from);
    free(buf_to);
    free(data_from);
    free(data_to);
}
