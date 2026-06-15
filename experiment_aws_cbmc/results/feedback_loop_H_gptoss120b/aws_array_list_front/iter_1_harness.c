#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer for the value to set */
    size_t item_sz = list.item_size;
    uint8_t *val = malloc(item_sz);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_sz));

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length handling */
        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            assert(list.length == index + 1);
        }

        /* the stored value must be copied to the target location */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);

        /* fields that must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* on failure the list must remain unchanged */
        assert_array_list_equivalence(&list, &old, &old_byte);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 7. Invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
