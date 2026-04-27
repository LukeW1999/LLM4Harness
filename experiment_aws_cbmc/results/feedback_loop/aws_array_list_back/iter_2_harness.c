#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void aws_array_list_back_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_array_list list;
    uint8_t val[MAX_ITEM_SIZE];

    /* 2. Bound the structure (limits CBMC state space) */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* 3. Allocate pointer members */
    ensure_array_list_has_allocated_data_member(&list);

    /* 4. Assume validity precondition */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 5. Save old state (for checking immutability) */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* 6. Assume function-specific preconditions */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* 7. Call function under test */
    int result = aws_array_list_back(&list, val);

    /* 8. Assert postconditions (both branches) */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length > 0);
        size_t last_item_offset = list.item_size * (list.length - 1);
        assert_bytes_match(val, (uint8_t *)list.data + last_item_offset, list.item_size);
    } else {
        assert(result == AWS_OP_ERR);
        assert(list.length == 0);
    }
    assert(aws_array_list_is_valid(&list));  // invariant always holds
    assert_array_list_equivalence(&list, &old, &old_byte);  // if read-only
}
