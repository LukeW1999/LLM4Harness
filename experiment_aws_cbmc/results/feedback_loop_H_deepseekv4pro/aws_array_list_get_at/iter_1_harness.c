#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministic val pointer */
    size_t item_size = list.item_size;
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

    /* Non-deterministic index */
    size_t index = nondet_size_t();

    /* 2. Save old state BEFORE calling */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* 3. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: value at index should match val */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data != NULL);
        /* Check that the element at index was written */
        assert_bytes_match((uint8_t *)list.data + (index * item_size), val, item_size);
        /* Length should be at least index + 1 */
        assert(list.length >= index + 1);
    } else {
        /* Failure: struct should be unchanged */
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    /* current_size may change (dynamic mode), but data pointer should remain valid if it was valid */
    if (old.current_size > 0) {
        assert(list.data == old.data);
    }

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    free(val);
}
