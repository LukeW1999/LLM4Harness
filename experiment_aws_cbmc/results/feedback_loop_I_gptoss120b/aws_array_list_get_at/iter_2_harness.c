#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_get_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    list.alloc = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a writable output buffer */
    void *out = malloc(list.item_size);
    __CPROVER_assume(out != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(out, list.item_size));

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_elem;
    bool old_elem_saved = false;
    if (list.data != NULL && index < list.length) {
        save_byte_from_array(
            (const uint8_t *)list.data + (index * list.item_size),
            list.item_size,
            &old_elem);
        old_elem_saved = true;
    }
    struct store_byte_from_buffer old_out;
    save_byte_from_array((const uint8_t *)out, list.item_size, &old_out);

    /* 5. Call the function under test */
    int result = aws_array_list_get_at(&list, out, index);

    /* 6. General post‑condition: the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* 7. Frame conditions: fields that never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.data == old.data);

    if (result == AWS_OP_SUCCESS) {
        /* The output buffer must now contain the element at the given index */
        assert_bytes_match(
            (const uint8_t *)list.data + (index * list.item_size),
            (const uint8_t *)out,
            list.item_size);
    } else {
        /* On failure the output buffer must be unchanged */
        assert_byte_from_buffer_matches((const uint8_t *)out, &old_out);
        /* The element in the list (if it existed) must be unchanged */
        if (old_elem_saved) {
            assert_byte_from_buffer_matches(
                (const uint8_t *)list.data + (index * list.item_size),
                &old_elem);
        }
    }

    /* 8. Clean up */
    free(out);
}
