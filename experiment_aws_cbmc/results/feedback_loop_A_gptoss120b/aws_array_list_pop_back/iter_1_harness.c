#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Prepare a readable input value */
    size_t item_sz = list.item_size;               /* item_size > 0 by validity */
    uint8_t *val = malloc(item_sz);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_sz; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* 4. Choose a nondeterministic index (bounded to keep state finite) */
    size_t index = nondet_size_t();
    __CPROVER_assume(index < (MAX_INITIAL_ITEM_ALLOCATION * 2));

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Global invariant: the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* ----- Success path ----- */

        /* Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* Length may grow to index+1 if index was beyond the old length */
        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            assert(list.length == index + 1);
        }

        /* The element at the given index now matches the input value */
        assert(list.data != NULL);
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);
    } else {
        /* ----- Failure path ----- */

        /* The list must be unchanged on error */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* Clean up */
    free(val);
}
