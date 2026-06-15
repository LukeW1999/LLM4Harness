#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable input value */
    /* item_size is guaranteed non‑zero by the validity predicate */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state for immutability checks */
    struct aws_array_list old = list;
    size_t old_length = list.length;
    struct store_byte_from_buffer old_byte;
    bool old_byte_saved = false;
    if (list.data != NULL && index < old_length) {
        save_byte_from_array((const uint8_t *)list.data + (index * list.item_size),
                             list.item_size,
                             &old_byte);
        old_byte_saved = true;
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. General post‑condition: the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* 7. Fields that never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        /* The element at the given index must now equal the input value */
        assert_bytes_match((const uint8_t *)list.data + (index * list.item_size),
                           (const uint8_t *)val,
                           list.item_size);

        /* Length updates only when index is beyond the previous length */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }
    } else {
        /* On failure the list must be unchanged */
        assert(list.length == old_length);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);

        if (old_byte_saved) {
            assert_byte_from_buffer_matches((const uint8_t *)list.data + (index * list.item_size),
                                            &old_byte);
        }
    }

    /* 8. Clean up */
    free(val);
}
