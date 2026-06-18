#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate output buffer (val) */
    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 3. Save old state of the list */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes = {0};
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_bytes);
    }

    /* 4. Save old contents of val for the failure case */
    void *old_val = NULL;
    if (list.item_size > 0) {
        old_val = malloc(list.item_size);
        __CPROVER_assume(old_val != NULL);
        memcpy(old_val, val, list.item_size);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* list must remain unchanged */
        assert_array_list_equivalence(&list, &old, &old_bytes);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);

        /* val must contain the last element */
        if (list.length > 0 && list.item_size > 0) {
            size_t last_offset = list.item_size * (list.length - 1);
            assert_bytes_match(
                (uint8_t *)list.data + last_offset,
                val,
                list.item_size);
        }
    } else {
        /* on failure the list must be unchanged */
        assert_array_list_equivalence(&list, &old, &old_bytes);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);

        /* val must be unchanged */
        if (list.item_size > 0) {
            assert_bytes_match(val, old_val, list.item_size);
        }
    }

    /* 7. Invariant: list remains valid */
    assert(aws_array_list_is_valid(&list));
}
