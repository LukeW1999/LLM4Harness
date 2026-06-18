#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <stdlib.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               16U

void aws_array_list_pop_front_n_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet initialization of the list structure */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* capture old state */
    size_t old_length       = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size    = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    uint8_t *old_data = NULL;
    if (list.data != NULL && old_current_size > 0) {
        /* ensure the original buffer is a fresh, readable region */
        __CPROVER_assume(__CPROVER_is_fresh(list.data, old_current_size));
        old_data = malloc(old_current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, old_current_size);
    }

    /* nondet n */
    size_t n;
    __CPROVER_assume(n <= MAX_INITIAL_ITEM_ALLOCATION * 2U);

    /* call the function under verification */
    aws_array_list_pop_front_n(&list, n);

    /* allocator, item size and allocated size must stay unchanged */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);
    assert(list.current_size == old_current_size);

    if (n >= old_length) {
        /* list should be cleared */
        assert(list.length == 0U);
        /* no further assumptions about buffer contents */
    } else {
        size_t expected_len = old_length - n;
        assert(list.length == expected_len);

        if (list.data != NULL && old_data != NULL) {
            size_t moved_bytes = expected_len * old_item_size;
            if (moved_bytes > 0) {
                /* the remaining items must be shifted to the front */
                assert(memcmp(list.data,
                              old_data + n * old_item_size,
                              moved_bytes) == 0);
            }
        }
    }

    free(old_data);
}
