#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#    define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#    define MAX_ITEM_SIZE 8
#endif

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;

    /* Use nondet values with explicit bounds */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    list.current_size = nondet_size_t();
    __CPROVER_assume(list.current_size <= MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE);

    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Ensure current_size is consistent with length and item_size */
    __CPROVER_assume(list.current_size >= list.length * list.item_size);

    /* Allocate data if current_size > 0 */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* Set allocator */
    list.alloc = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Create a non-deterministic val buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Non-deterministic index - keep it bounded to avoid state space explosion */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* item_size must not change */
        assert(list.item_size == old_item_size);

        /* allocator must not change */
        assert(list.alloc == old_alloc);

        /* data pointer must be non-null after successful set */
        assert(list.data != NULL);

        /* length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was within old length, length stays the same */
        if (index < old_length) {
            assert(list.length == old_length);
        } else {
            /* length becomes index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be large enough to hold length elements */
        assert(list.current_size >= list.length * list.item_size);

        /* current_size must not shrink */
        assert(list.current_size >= old_current_size);

    } else {
        /* On failure: item_size and alloc must not change */
        assert(list.item_size == old_item_size);
        assert(list.alloc == old_alloc);
    }

    /* 7. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
