#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
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

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Prepare a readable input value */
    size_t item_sz = list.item_size;
    /* item_sz is guaranteed non‑zero by the validity predicate */
    uint8_t *val = malloc(item_sz);
    __CPROVER_assume(val != NULL);
    /* make the buffer nondeterministically initialized */
    for (size_t i = 0; i < item_sz; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* 4. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* the element at the given index must now equal the input value */
        assert(memcmp((uint8_t *)list.data + (index * list.item_size),
                      val,
                      list.item_size) == 0);

        /* length is increased only when writing past the previous end */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* validity invariant must hold */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* on failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        /* validity invariant must still hold */
        assert(aws_array_list_is_valid(&list));
    }

    /* 7. Clean up */
    free(val);
}
