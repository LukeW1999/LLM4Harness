#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer */
    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    /* 3. Choose a nondeterministic index (bounded to keep state space reasonable) */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= (MAX_INITIAL_ITEM_ALLOCATION * 2));

    /* 4. Save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (list.data && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_bytes);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The value at the given index must now equal the source buffer */
        if (list.data && list.item_size > 0) {
            uint8_t *dest = (uint8_t *)list.data + (index * list.item_size);
            assert_bytes_match(dest, val, list.item_size);
        }

        /* Length may increase if we wrote past the previous end */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Fields that must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        if (list.data && old.data && list.current_size > 0) {
            assert_array_list_equivalence(&list, &old, &old_bytes);
        }
    }

    /* 7. The list must always remain valid */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
}
