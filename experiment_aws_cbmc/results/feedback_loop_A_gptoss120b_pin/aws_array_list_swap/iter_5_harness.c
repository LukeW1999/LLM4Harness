#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_swap_harness() {
    /* 1. Declare and bound data structure */
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.length <= list.current_size / list.item_size);

    /* 2. Initialise the list contents with nondet bytes (already nondet) */
    size_t data_bytes = list.length * list.item_size;
    /* No explicit initialization needed; memory is nondet */

    /* 3. Save old state */
    struct aws_array_list old = list;
    uint8_t *old_bytes = NULL;
    if (data_bytes > 0) {
        old_bytes = malloc(data_bytes);
        __CPROVER_assume(old_bytes != NULL);
        memcpy(old_bytes, list.data, data_bytes);
    }

    /* 4. Nondeterministic indices respecting preconditions */
    size_t a;
    size_t b;
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 5. Call function under test and assume success */
    int rv = aws_array_list_swap(&list, a, b);
    __CPROVER_assume(rv == AWS_OP_SUCCESS);

    /* 6. Postconditions */

    /* Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* Content changes */
    if (a == b) {
        if (data_bytes > 0) {
            assert(memcmp(list.data, old_bytes, data_bytes) == 0);
        }
    } else {
        uint8_t *data = (uint8_t *)list.data;
        assert(memcmp(data + a * list.item_size,
                      old_bytes + b * list.item_size,
                      list.item_size) == 0);
        assert(memcmp(data + b * list.item_size,
                      old_bytes + a * list.item_size,
                      list.item_size) == 0);
        for (size_t i = 0; i < list.length; ++i) {
            if (i != a && i != b) {
                assert(memcmp(data + i * list.item_size,
                              old_bytes + i * list.item_size,
                              list.item_size) == 0);
            }
        }
    }

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(old_bytes);
}
