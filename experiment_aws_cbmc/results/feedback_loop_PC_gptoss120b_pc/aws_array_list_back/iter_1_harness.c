#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate output buffer (val) */
    size_t item_size = list.item_size;
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Save old state before the call */
    struct aws_array_list old = list;

    /* Save original bytes of val */
    struct store_byte_from_buffer val_old;
    save_byte_from_array((uint8_t *)val, item_size, &val_old);

    /* If the list is non‑empty, save the last element */
    struct store_byte_from_buffer elem_old;
    bool list_nonempty = list.length > 0;
    if (list_nonempty) {
        size_t last_offset = item_size * (list.length - 1);
        save_byte_from_array((uint8_t *)list.data + last_offset,
                             item_size,
                             &elem_old);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success path: list must have been non‑empty */
        assert(list_nonempty);

        /* The output buffer must contain the copied last element */
        assert_byte_from_buffer_matches((uint8_t *)val, &elem_old);

        /* The array list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    } else {
        /* Failure path: list unchanged and val unchanged */
        assert(!list_nonempty);
        assert_byte_from_buffer_matches((uint8_t *)val, &val_old);

        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    }

    /* 6. Invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));

    free(val);
}
