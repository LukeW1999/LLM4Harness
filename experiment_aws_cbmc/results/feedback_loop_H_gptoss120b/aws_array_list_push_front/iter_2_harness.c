#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/assertions.h"

void aws_array_list_push_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a nondeterministic input value */
    __CPROVER_assume(list.item_size > 0);
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Save old state before the call */
    struct aws_array_list old = list;
    uint8_t *old_data = NULL;
    if (old.data != NULL && old.length > 0) {
        old_data = malloc(old.length * old.item_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, old.data, old.length * old.item_size);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Unconditional sanity check (ensures at least one reachable assert) */
    assert(aws_array_list_is_valid(&list));

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length must increase by one */
        assert(list.length == old.length + 1);

        /* allocator and item_size must stay the same */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* The first element must equal the input value */
        for (size_t i = 0; i < list.item_size; ++i) {
            assert(((uint8_t *)list.data)[i] == ((uint8_t *)val)[i]);
        }

        /* If there were previous elements, they must be shifted right by one */
        if (old.length > 0) {
            for (size_t i = 0; i < old.length * old.item_size; ++i) {
                assert(((uint8_t *)list.data)[list.item_size + i] == old_data[i]);
            }
        }
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.length > 0) {
            for (size_t i = 0; i < old.length * old.item_size; ++i) {
                assert(((uint8_t *)list.data)[i] == old_data[i]);
            }
        }
    }

    /* 7. Fields that never change regardless of outcome */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* Clean up */
    free(val);
    free(old_data);
}
