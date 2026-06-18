#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound data structure */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministically choose index and value */
    size_t index = nondet_size_t();
    /* Bound index to avoid unreasonable values */
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + 1);

    /* Allocate and initialize the value to be set */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 2. Save old state BEFORE calling */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(list.data, list.current_size, &old_byte);

    /* 3. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* On success, the element at index should be the value we set */
        assert(list.data != NULL);
        if (index < aws_array_list_length(&list)) {
            /* If index was within previous length, check that value was copied */
            assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                               (const uint8_t *)val, list.item_size);
        }
        /* If index was beyond current length, length should have grown */
        if (index >= old.length) {
            assert(list.length == index + 1);
        }
    } else {
        /* On failure, the list structure should remain unchanged */
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    /* current_size may grow but pointer may be reallocated — 
       if current_size didn't change, data pointer must not change either */
    if (list.current_size == old.current_size) {
        assert(list.data == old.data);
    }

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Cleanup */
    free(val);
}
