#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/proof_allocators.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_erase_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Non-deterministic index */
    size_t index = nondet_size_t();

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length - 1);
        if (index == 0) {
            assert(list.data != old.data || list.length == 0); // If length is 0, data can be anything
        } else if (index == old.length - 1) {
            assert(list.data != old.data || list.length == 0); // If length is 0, data can be anything
        } else {
            uint8_t *item_ptr = (uint8_t *)list.data + (index * list.item_size);
            uint8_t *next_item_ptr = item_ptr + list.item_size;
            size_t trailing_items = (old.length - index) - 1;
            size_t trailing_bytes = trailing_items * list.item_size;
            assert(AWS_MEM_IS_READABLE(item_ptr, trailing_bytes));
            assert(AWS_MEM_IS_READABLE(next_item_ptr, trailing_bytes));
            assert(memcmp(item_ptr, next_item_ptr, trailing_bytes) == 0);
        }
    } else {
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    }

    /* 6. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 7. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
