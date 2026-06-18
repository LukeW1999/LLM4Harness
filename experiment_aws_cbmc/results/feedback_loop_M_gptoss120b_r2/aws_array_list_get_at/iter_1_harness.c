#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save a copy of the original state */
    struct aws_array_list old = list;

    /* Save a byte from the original data buffer for immutability checks */
    struct store_byte_from_buffer old_data_byte;
    if (old.data != NULL && old.current_size > 0) {
        save_byte_from_array(old.data, old.current_size, &old_data_byte);
    }

    /* 2. Nondeterministic but bounded inputs */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_BUFFER_SIZE);

    /* Allocate a readable buffer for the value to be copied */
    uint8_t val_buf[MAX_ITEM_SIZE];
    const void *val = val_buf; /* readable for list.item_size bytes */

    /* 3. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 4. Global invariant: the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* 5. Fields that never change */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* Length is increased to index+1 if index was beyond the old length */
        size_t expected_length = old.length;
        if (index >= old.length) {
            expected_length = index + 1;
        }
        assert(list.length == expected_length);

        /* The bytes at the targeted index now match the input value */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val_buf,
                           list.item_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.data != NULL && old.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_data_byte);
        }
    }
}
