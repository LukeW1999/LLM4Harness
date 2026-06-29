#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_length_harness(void) {
    struct aws_array_list list;

    /* Nondeterministically choose between a zeroed list and a valid list */
    bool use_zeroed;
    __CPROVER_assume(use_zeroed == 0 || use_zeroed == 1);

    if (use_zeroed) {
        /* Test with a zeroed (empty) list */
        AWS_ZERO_STRUCT(list);
    } else {
        /* Build a valid list with nondeterministic contents */
        size_t item_size;
        __CPROVER_assume(item_size > 0 && item_size <= 128);

        size_t initial_item_allocation;
        __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= 16);

        /* Use static initialization to avoid dynamic allocation complexity */
        size_t buf_size;
        __CPROVER_assume(!aws_mul_size_checked(initial_item_allocation, item_size, &buf_size));
        __CPROVER_assume(buf_size > 0 && buf_size <= 2048);

        void *raw_array = malloc(buf_size);
        __CPROVER_assume(raw_array != NULL);

        aws_array_list_init_static(&list, raw_array, initial_item_allocation, item_size);

        /* Nondeterministically set length within valid bounds */
        size_t length;
        __CPROVER_assume(length <= initial_item_allocation);
        list.length = length;

        /* Ensure the list is valid before calling the function */
        __CPROVER_assume(aws_array_list_is_valid(&list));
    }

    /* Save state before the call for frame condition checks */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under test */
    size_t result = aws_array_list_length(&list);

    /* Postcondition 1: Return value correctness
     * The returned length must equal list->length */
    assert(result == old_length);

    /* Postcondition 2: Length invariants
     * The list length should not have changed */
    assert(list.length == old_length);

    /* Postcondition 3: Frame conditions
     * The function should not modify any fields of the list */
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);
    assert(list.alloc == old_alloc);

    /* Postcondition 4: Validity preserved
     * The list should still be valid (or zeroed) after the call */
    assert(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));

    /* Postcondition 5: Consistency check
     * If the list is valid (non-zeroed), the result must be consistent
     * with the capacity */
    if (!use_zeroed && aws_array_list_is_valid(&list)) {
        size_t capacity = list.current_size / list.item_size;
        assert(result <= capacity);
    }

    /* Postcondition 6: If length > 0, data must not be NULL */
    if (result > 0) {
        assert(list.data != NULL);
    }
}
