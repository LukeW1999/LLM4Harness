#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_erase_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    list.alloc = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    /* Ensure the list can contain at least one element so that success is reachable */
    __CPROVER_assume(list.length > 0);

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(old.data, old.current_size, &old_byte);

    /* 3. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: index must be within bounds */
        assert(index < old.length);
        /* Length and size decrease by one element */
        assert(list.length == old.length - 1);
        assert(list.current_size == old.current_size - old.item_size);
        /* Fields that must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        /* Data content equivalence (except for the erased element) */
        assert_array_list_equivalence(&list, &old, &old_byte);
    } else {
        /* Failure: index out of bounds */
        assert(index >= old.length);
        /* List must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        /* Underlying buffer bytes are unchanged */
        assert_byte_from_buffer_matches(list.data, &old_byte);
    }

    /* 6. Invariant: list remains valid */
    assert(aws_array_list_is_valid(&list));
}
