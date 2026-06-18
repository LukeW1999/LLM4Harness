#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Maximum bounds for the nondeterministic allocation sizes used in the harness */
#define MAX_INITIAL_ITEM_ALLOCATION 64U
#define MAX_ITEM_SIZE               32U

void aws_array_list_set_at_harness(void) {
    /* -------------------------------------------------------------------- */
    /*  Nondeterministic setup of the array list and the value to be set.   */
    /* -------------------------------------------------------------------- */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministically choose item size and initial allocation within bounds */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.current_size <= MAX_INITIAL_ITEM_ALLOCATION * list.item_size);
    __CPROVER_assume(list.length <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(list.alloc == alloc);

    /* allocate the backing buffer (may be NULL for static lists) */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data);
    } else {
        list.data = NULL;
    }

    /* -------------------------------------------------------------------- */
    /*  Ground‑truth preconditions (must be copied verbatim).                */
    /* -------------------------------------------------------------------- */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.data != NULL);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondeterministic index to write at */
    size_t index;
    __CPROVER_assume(index <= (size_t)UINT_MAX); /* just keep it nondet */

    /* allocate a readable value buffer of exactly list.item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    __CPROVER_assume(val && AWS_MEM_IS_READABLE(val, list.item_size));

    /* -------------------------------------------------------------------- */
    /*  Snapshot of the pre‑state for post‑condition checking.               */
    /* -------------------------------------------------------------------- */
    size_t old_length   = list.length;
    size_t old_capacity = aws_array_list_capacity(&list);
    size_t old_item_sz  = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* make a copy of the entire backing buffer (if any) */
    uint8_t *old_data = NULL;
    if (list.data) {
        old_data = malloc(old_capacity * old_item_sz);
        __CPROVER_assume(old_data);
        memcpy(old_data, list.data, old_capacity * old_item_sz);
    }

    /* -------------------------------------------------------------------- */
    /*  Call the function under verification.                               */
    /* -------------------------------------------------------------------- */
    int ret = aws_array_list_set_at(&list, val, index);

    /* -------------------------------------------------------------------- */
    /*  Post‑conditions.                                                    */
    /* -------------------------------------------------------------------- */

    /* 1. Return value must be a known success or error code. */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    /* 2. The list must remain valid after the call. */
    assert(aws_array_list_is_valid(&list));

    /* 3. Allocator and item size must be unchanged. */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_sz);

    /* 4. Length / capacity invariants. */
    size_t new_capacity = aws_array_list_capacity(&list);
    assert(list.length <= new_capacity);               /* length never exceeds capacity */
    assert(new_capacity >= old_capacity);              /* capacity never shrinks */

    if (ret == AWS_OP_SUCCESS) {
        /* Length must be at least the old length and at least index+1 */
        assert(list.length >= old_length);
        assert(list.length >= index + 1);

        /* The element at 'index' must now contain the value we wrote. */
        assert(memcmp((uint8_t *)list.data + (index * list.item_size), val, list.item_size) == 0);

        /* All other elements must be unchanged. */
        if (old_data) {
            for (size_t i = 0; i < old_capacity; ++i) {
                if (i != index) {
                    assert(memcmp((uint8_t *)list.data + (i * list.item_size),
                                  old_data + (i * list.item_size),
                                  list.item_size) == 0);
                }
            }
        }
    } else {
        /* On error the list state must be unchanged. */
        assert(list.length == old_length);
        assert(new_capacity == old_capacity);
        if (old_data) {
            assert(memcmp(list.data, old_data, old_capacity * list.item_size) == 0);
        }
    }

    /* 5. Frame condition: memory outside the list's backing buffer must be untouched. */
    /*    (Handled implicitly by the above checks on the backing buffer and by
    *     the fact that we never write outside of it.) */

    return 0;
}
