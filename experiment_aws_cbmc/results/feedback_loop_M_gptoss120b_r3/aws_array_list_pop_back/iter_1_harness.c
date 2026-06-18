#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/math.h>

#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

void aws_array_list_erase_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                                MAX_INITIAL_ITEM_ALLOCATION,
                                                MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data = {0};
    if (list.data && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data);
    }

    /* 3. Nondeterministic index, bounded */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Postcondition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length must decrease by one */
        assert(list.length == old.length - 1);

        /* fields that must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* validity invariant */
        assert(aws_array_list_is_valid(&list));

        /* optional: data equivalence except for removed element */
        assert_array_list_equivalence(&list, &old, &old_data);
    } else {
        /* on failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* data buffer unchanged */
        if (list.data && list.current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_data);
        }

        /* validity invariant */
        assert(aws_array_list_is_valid(&list));
    }
}
