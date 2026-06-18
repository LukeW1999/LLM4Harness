#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

#define MAX_ITEM_SIZE 256
#define MAX_INITIAL_ITEM_ALLOCATION 4

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic inputs */
    size_t index;
    size_t item_size = list.item_size;
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

    /* 3. Save old state BEFORE calling */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* 4. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* On success, the element at index should equal val */
        assert_bytes_match((uint8_t *)list.data + (index * item_size), val, item_size);
        /* length may have increased if index >= old length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
    } else {
        /* On failure, the list should be unchanged */
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    /* 6. Assert fields that must NOT change regardless of result */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    /* 7. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    free(val);
}
