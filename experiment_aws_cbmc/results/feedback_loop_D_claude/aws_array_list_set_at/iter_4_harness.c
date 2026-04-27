#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 8
#endif

void aws_array_list_set_at_harness() {
    /* Setup list with bounded parameters */
    struct aws_array_list list;

    /* Initialize list fields directly to ensure validity */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    list.item_size = item_size;

    size_t initial_item_count;
    __CPROVER_assume(initial_item_count <= MAX_INITIAL_ITEM_ALLOCATION);

    /* current_size is item_size * some count */
    list.current_size = initial_item_count * item_size;

    /* length <= initial_item_count */
    __CPROVER_assume(list.length <= initial_item_count);

    /* Allocate data if current_size > 0 */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* Set up allocator */
    list.alloc = aws_default_allocator();
    __CPROVER_assume(list.alloc != NULL);

    /* Verify list is valid before calling */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Setup val - must be readable for item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Nondeterministic index - allow both in-bounds and out-of-bounds */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + 2);

    /* Save old state */
    size_t old_length = list.length;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Frame conditions: alloc and item_size never change */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);

    /* Result must be either success or failure */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* List must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* data must be non-null after successful set */
        assert(list.data != NULL);

        /* Length update: if index >= old_length, new length = index + 1 */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }

        /* current_size must be large enough to hold the data */
        assert(list.current_size >= list.length * list.item_size);

        /* current_size must be at least as large as before */
        assert(list.current_size >= old_current_size);

    } else {
        /* On failure, length should be unchanged */
        assert(list.length == old_length);
        /* item_size unchanged */
        assert(list.item_size == old_item_size);
        /* alloc unchanged */
        assert(list.alloc == old_alloc);
        /* current_size unchanged on failure */
        assert(list.current_size == old_current_size);
    }
}
