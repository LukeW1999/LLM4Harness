#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

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
        if (list.data != NULL && old_data != NULL) {
            /* clear does not modify the underlying buffer */
            assert(memcmp(list.data, old_data, old_current_size) == 0);
        }
    } else {
        size_t expected_len = old_length - n;
        assert(list.length == expected_len);

        if (list.data != NULL && old_data != NULL) {
            size_t moved_bytes = expected_len * old_item_size;
            /* the remaining items must be shifted to the front */
            assert(memcmp(list.data,
                          old_data + n * old_item_size,
                          moved_bytes) == 0);
            /* bytes beyond the new length are not required to be unchanged
               (they may be overwritten with debug fill in DEBUG_BUILD) */
        }
    }

    /* clean up */
    free(old_data);
    return 0;
}
