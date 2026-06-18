#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 5
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 64
#endif

void aws_array_list_get_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare the array list and allocate its internal buffer */
    struct aws_array_list list;
    ensure_array_list_has_allocated_data_member(&list, allocator);
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a writable destination buffer for the value */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = (uint8_t *)malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 3. Choose a nondeterministic index */
    size_t index = __CPROVER_nondet_size_t();
    if (list.item_size > 0) {
        __CPROVER_assume(index <= list.length + 5);
    }

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* 5. Call the function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* 6. Global validity invariant must hold */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* The value copied must match the source buffer */
        for (size_t i = 0; i < list.item_size; ++i) {
            assert(((uint8_t *)list.data)[index * list.item_size + i] == ((uint8_t *)val)[i]);
        }

        /* Length and other fields must remain unchanged */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* Clean up */
    free(val);
}
