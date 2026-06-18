#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

void aws_array_list_erase_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    size_t index = nondet_size_t();

    /* Save full old data contents (bounded) */
    uint8_t old_data[MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE];
    size_t old_data_len = old.length * old.item_size;
    __CPROVER_assume(old_data_len <= sizeof(old_data));
    memcpy(old_data, old.data, old_data_len);

    int result = aws_array_list_erase(&list, index);

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Fields that never change */
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* index must be strictly less than old.length for success */
        assert(index < old.length);
        assert(list.length == old.length - 1);
        assert(list.data == old.data);

        /* Elements before index are unchanged */
        if (index > 0) {
            assert_bytes_match(list.data, old_data, index * old.item_size);
        }

        /* Elements after index are shifted left by one */
        if (index < old.length - 1) {
            size_t trailing_bytes = (old.length - index - 1) * old.item_size;
            assert_bytes_match((uint8_t *)list.data + index * old.item_size,
                               old_data + (index + 1) * old.item_size,
                               trailing_bytes);
        }
    } else {
        /* Failure: index out of bounds, list completely unchanged */
        assert(index >= old.length);
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert_bytes_match(list.data, old_data, old_data_len);
    }
}
