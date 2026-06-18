#include <aws/common/array_list.h>
#include <string.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_harness() {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    list.alloc = allocator;
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate buffer for the retrieved value */
    uint8_t *out = NULL;
    if (list.item_size > 0) {
        out = aws_mem_acquire(allocator, list.item_size);
        __CPROVER_assume(out != NULL);
    }

    /* 3. Choose a nondeterministic index (avoid overflow) */
    size_t idx = nondet_size_t();
    __CPROVER_assume(list.item_size == 0 || idx <= SIZE_MAX / list.item_size);

    /* 4. Save old state before the call */
    struct aws_array_list old = list;

    uint8_t *old_data_copy = NULL;
    if (old.data && old.current_size > 0) {
        old_data_copy = aws_mem_acquire(allocator, old.current_size);
        __CPROVER_assume(old_data_copy != NULL);
        memcpy(old_data_copy, old.data, old.current_size);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_get_at(&list, out, idx);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* retrieved value must match stored data */
        if (list.item_size > 0) {
            for (size_t i = 0; i < list.item_size; ++i) {
                uint8_t expected = ((uint8_t *)list.data)[idx * list.item_size + i];
                assert(out[i] == expected);
            }
        }
    } else {
        /* on failure the whole list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.data && old.current_size > 0) {
            assert(memcmp(list.data, old_data_copy, old.current_size) == 0);
        }
    }

    /* 7. Invariant must hold irrespective of the outcome */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    if (out) {
        aws_mem_release(allocator, out);
    }
    if (old_data_copy) {
        aws_mem_release(allocator, old_data_copy);
    }
}
