#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifndef MAX_ITEM_SIZE
#    define MAX_ITEM_SIZE 4
#endif

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#    define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

void aws_array_list_get_at_harness(void) {
    /* 1. Declare and set up the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Set up output buffer: must be writable for item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Non-deterministic index */
    size_t index;

    /* 4. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;

    /* 5. Call function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size and alloc never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    /* list structure should not be modified by get_at */
    assert(list.length == old_length);
    assert(list.current_size == old_current_size);
    assert(list.data == old_data);

    if (result == AWS_OP_SUCCESS) {
        /* On success: index must have been within bounds */
        assert(index < old_length);
        /* val must have been populated */
        assert(val != NULL);
        /* The value retrieved matches what's in the list */
        if (old_data != NULL && old_item_size > 0) {
            void *stored = (void *)((uint8_t *)old_data + (old_item_size * index));
            assert(memcmp(val, stored, old_item_size) == 0);
        }
    } else {
        /* On failure: index must have been out of bounds */
        assert(index >= old_length);
    }
}
