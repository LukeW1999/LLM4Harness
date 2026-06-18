#include <aws/common/array_list.h>
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
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer storage;
    if (list.data != NULL && list.length > 0 && list.item_size > 0) {
        save_byte_from_array(
            (const uint8_t *)list.data,
            list.length * list.item_size,
            &storage);
    }

    /* 3. Nondeterministic n */
    size_t n = nondet_size_t();

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Postconditions */
    if (n >= old.length) {
        /* List cleared */
        assert(list.length == 0);
        assert(list.data == old.data);
    } else if (n == 0) {
        /* No change */
        assert(list.length == old.length);
        assert(list.data == old.data);
    } else {
        /* n > 0 && n < old.length */
        assert(list.length == old.length - n);
        assert(list.data == old.data);
        /* Verify that the remaining elements are unchanged */
        assert_array_list_equivalence(&list, &old, &storage);
    }

    /* 6. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
