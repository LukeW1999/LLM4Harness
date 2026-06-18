#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = aws_mem_acquire(allocator, list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 3. Choose a nondeterministic index */
    size_t index = 0;
    if (list.item_size > 0) {
        __CPROVER_assume(index <= SIZE_MAX / list.item_size);
    }

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        if (list.item_size > 0) {
            assert(memcmp((const uint8_t *)list.data + (index * list.item_size),
                          (const uint8_t *)val,
                          list.item_size) == 0);
        }

        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            assert(list.length == index + 1);
        }

        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    if (val) {
        aws_mem_release(allocator, val);
    }
}
