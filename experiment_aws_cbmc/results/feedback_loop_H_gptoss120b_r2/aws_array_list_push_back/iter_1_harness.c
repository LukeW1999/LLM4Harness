#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

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
    uint8_t *old_data = NULL;
    if (list.current_size > 0) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data);
        memcpy(old_data, list.data, list.current_size);
    }

    /* 3. Prepare a readable input value */
    uint8_t *val = NULL;
    __CPROVER_assume(list.item_size > 0);
    val = malloc(list.item_size);
    __CPROVER_assume(val);
    for (size_t i = 0; i < list.item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* 4. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The element at the given index now matches the input value */
        assert(list.data != NULL);
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);

        /* Length is increased to index+1 only if it was previously shorter */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Fields that must stay the same */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);

        /* Capacity may grow but never shrink */
        assert(list.current_size >= old.current_size);
    } else {
        /* On failure the whole structure must remain unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);

        /* And the underlying buffer contents must be unchanged */
        if (old_data && list.current_size > 0) {
            assert_bytes_match((uint8_t *)list.data, old_data, list.current_size);
        }
    }

    /* 7. The validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Clean up auxiliary allocations */
    free(val);
    free(old_data);
}
