#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Prepare a nondeterministic input value */
    uint8_t val_buf[MAX_ITEM_SIZE];
    for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
        val_buf[i] = nondet_uint8_t();
    }
    const void *val = val_buf;

    /* 4. Nondeterministic index, bounded */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Global invariant: list must remain valid */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* 6a. Data at the specified index must match the input value */
        assert_bytes_match((uint8_t *)list.data + index * list.item_size,
                           val_buf,
                           list.item_size);

        /* 6b. Length updates only when index is beyond the previous length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* 6c. Fields that never change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* 6d. Current size cannot shrink */
        assert(list.current_size >= old.current_size);
    } else {
        /* 7a. On failure the list must be unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);

        /* 7b. Data buffer must be unchanged (if any) */
        if (old.current_size > 0) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old.data,
                               old.current_size);
        }
    }
}
