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

    /* Bound the list to keep verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    /* Setup val - must be readable for item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Nondeterministic index - allow both in-bounds and out-of-bounds */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + 1);

    /* Save old state */
    size_t old_length = list.length;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Frame conditions: alloc and item_size never change */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);

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

    } else {
        /* On failure, length should be unchanged */
        assert(list.length == old_length);
        /* item_size unchanged */
        assert(list.item_size == old_item_size);
        /* alloc unchanged */
        assert(list.alloc == old_alloc);
    }
}
