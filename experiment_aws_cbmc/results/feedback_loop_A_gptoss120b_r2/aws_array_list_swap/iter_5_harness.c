#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_swap_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    struct aws_allocator *allocator = aws_default_allocator();
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Additional precondition: item size must be non‑zero and length > 0 */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.length > 0);

    /* 3. Nondeterministic indices within bounds */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Save old state */
    struct aws_array_list old = list;

    /*   Save a copy of the underlying data buffer */
    size_t total_bytes = list.length * list.item_size;
    uint8_t *old_data = (uint8_t *)aws_mem_acquire(allocator, total_bytes);
    __CPROVER_assume(old_data != NULL);
    for (size_t i = 0; i < total_bytes; ++i) {
        old_data[i] = ((uint8_t *)list.data)[i];
    }

    /* 5. Call the function under test and require success */
    int rv = aws_array_list_swap(&list, a, b);
    assert(rv == AWS_OP_SUCCESS);

    /* 6. Assert fields that must not change */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 7. Assert that the elements at a and b are swapped and all others unchanged */
    for (size_t i = 0; i < list.length; ++i) {
        uint8_t *new_elem = (uint8_t *)list.data + i * list.item_size;
        uint8_t *expected_elem;
        if (i == a) {
            expected_elem = old_data + b * list.item_size;
        } else if (i == b) {
            expected_elem = old_data + a * list.item_size;
        } else {
            expected_elem = old_data + i * list.item_size;
        }
        assert_bytes_match(new_elem, expected_elem, list.item_size);
    }

    /* 8. The list must remain valid */
    assert(aws_array_list_is_valid(&list));

    aws_mem_release(allocator, old_data);
}
