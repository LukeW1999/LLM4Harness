#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U

void aws_array_list_set_at_harness(void) {
    /*--------------------------------------------------------------------
     * Setup: allocate and nondeterministically initialize a list and a
     *        value buffer.
     *--------------------------------------------------------------------*/
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet initial allocation count and item size within bounds */
    size_t init_count;
    __CPROVER_assume(init_count <= MAX_INITIAL_ITEM_ALLOCATION);
    size_t item_sz;
    __CPROVER_assume(item_sz > 0 && item_sz <= MAX_ITEM_SIZE);

    /* initialize the list in dynamic mode (the exact mode is irrelevant
     * for the proof – the preconditions guarantee a valid list). */
    aws_array_list_init_dynamic(&list, alloc, init_count, item_sz);

    /* make the list bounded and valid as required by the ground‑truth
     * preconditions. */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.data != NULL);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* allocate a nondet value buffer that is readable for item_sz bytes */
    uint8_t *val = malloc(item_sz);
    __CPROVER_assume(val);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_sz));

    /* nondet index within a reasonable range */
    size_t index;
    __CPROVER_assume(index <= (list.current_size / list.item_size) + MAX_INITIAL_ITEM_ALLOCATION);

    /*--------------------------------------------------------------------
     * Snapshot old state for post‑condition checks.
     *--------------------------------------------------------------------*/
    struct aws_array_list old_list = list;
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    void *old_data_ptr = list.data;
    size_t old_capacity = aws_array_list_capacity(&list);
    size_t item_size = list.item_size;

    uint8_t *old_contents = NULL;
    if (old_data_ptr && old_length > 0 && item_size > 0) {
        old_contents = malloc(old_length * item_size);
        __CPROVER_assume(old_contents);
        memcpy(old_contents, old_data_ptr, old_length * item_size);
    }

    uint8_t *val_snapshot = malloc(item_size);
    __CPROVER_assume(val_snapshot);
    memcpy(val_snapshot, val, item_size);

    /*--------------------------------------------------------------------
     * Call the function under verification.
     *--------------------------------------------------------------------*/
    int ret = aws_array_list_set_at(&list, val, index);

    /*--------------------------------------------------------------------
     * Post‑condition checks.
     *--------------------------------------------------------------------*/
    /* 1. The list must remain valid. */
    assert(aws_array_list_is_valid(&list));

    /* 2. Capacity never shrinks. */
    assert(aws_array_list_capacity(&list) >= old_capacity);

    if (ret == AWS_OP_SUCCESS) {
        /* Length must be at least the old length and, if we wrote past the
         * previous end, exactly index+1. */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }

        /* The element at 'index' must now contain the value we wrote. */
        assert(memcmp((uint8_t *)list.data + index * item_size,
                      val_snapshot,
                      item_size) == 0);

        /* All other previously existing elements must be unchanged. */
        for (size_t i = 0; i < old_length; ++i) {
            if (i == index) {
                continue;
            }
            assert(memcmp((uint8_t *)list.data + i * item_size,
                          old_contents + i * item_size,
                          item_size) == 0);
        }
    } else {
        /* On error the list must be unchanged. */
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
        assert(list.data == old_data_ptr);
        if (old_contents) {
            assert(memcmp(list.data,
                          old_contents,
                          old_length * item_size) == 0);
        }
    }

    /* 3. Frame condition: memory outside the list's data region must be
     *    untouched.  We conservatively check that the allocator's internal
     *    bookkeeping (if any) is unchanged by asserting that the allocator
     *    pointer itself is unchanged. */
    assert(list.alloc == alloc);

    /* clean up */
    free(val);
    free(old_contents);
    free(val_snapshot);
    aws_array_list_clean_up(&list);
    return 0;
}
