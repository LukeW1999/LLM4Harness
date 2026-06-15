#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_INITIAL_ITEM_ALLOCATION 8U
#define MAX_ITEM_SIZE               8U   /* reduced bound for CBMC loop unwinding */

void aws_array_list_push_back_harness(void) {
    /* 1. Declare and initialise the array list */
    struct aws_array_list list;
    list.alloc = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable input buffer of size list.item_size */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        for (size_t i = 0; i < list.item_size; ++i) {
            val[i] = nondet_uint8_t();
        }
    }

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* 4. Call function under test */
    int result = aws_array_list_push_back(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* validity invariant */
        assert(aws_array_list_is_valid(&list));

        /* allocator and item size never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* length must increase by exactly one */
        assert(list.length == old.length + 1);

        /* the newly stored element must equal the input value */
        if (list.item_size > 0) {
            assert_bytes_match(
                (uint8_t *)list.data + old.length * list.item_size,
                val,
                list.item_size);
        }
    } else {
        /* on failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* validity invariant must still hold */
        assert(aws_array_list_is_valid(&list));
    }

    /* 6. Clean up */
    free(val);
}
