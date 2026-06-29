#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_clear_harness(void) {
    struct aws_array_list list;

    /* Nondeterministically choose between a zeroed list and a valid list */
    bool use_zeroed;
    __CPROVER_assume(use_zeroed == 0 || use_zeroed == 1);

    if (use_zeroed) {
        /* Case 1: fully zeroed list */
        AWS_ZERO_STRUCT(list);
        /* Precondition: AWS_IS_ZEROED(*list) */
        __CPROVER_assume(AWS_IS_ZEROED(list));
    } else {
        /* Case 2: valid list */
        /* item_size must be non-zero */
        size_t item_size;
        __CPROVER_assume(item_size > 0);
        list.item_size = item_size;

        /* current_size must be a multiple of item_size (or zero) */
        size_t current_size;
        __CPROVER_assume(current_size % item_size == 0);
        list.current_size = current_size;

        /* length: length * item_size <= current_size, no overflow */
        size_t length;
        size_t required_size;
        __CPROVER_assume(!aws_mul_size_checked(length, item_size, &required_size));
        __CPROVER_assume(required_size <= current_size);
        list.length = length;

        /* data pointer constraints */
        if (current_size == 0) {
            list.data = NULL;
        } else {
            void *data = malloc(current_size);
            __CPROVER_assume(data != NULL);
            list.data = data;
        }

        /* alloc: can be NULL (static) or non-NULL (dynamic) */
        /* We leave alloc unconstrained for generality */
        /* list.alloc is already set by the above or left as whatever malloc gives */

        /* Verify precondition holds */
        __CPROVER_assume(aws_array_list_is_valid(&list));
    }

    /* Save pre-call state for frame condition checks */
    void   *old_data         = list.data;
    size_t  old_current_size = list.current_size;
    size_t  old_item_size    = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under verification */
    aws_array_list_clear(&list);

    /* Postcondition: validity invariant preserved */
    assert(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));

    /* Postcondition (length): if data was non-NULL, length is now 0 */
    if (old_data != NULL) {
        assert(list.length == 0);
    }

    /* Postcondition (length): if data was NULL, length is still 0 */
    if (old_data == NULL) {
        assert(list.length == 0);
    }

    /* Postcondition (frame): data pointer unchanged */
    assert(list.data == old_data);

    /* Postcondition (frame): current_size unchanged */
    assert(list.current_size == old_current_size);

    /* Postcondition (frame): item_size unchanged */
    assert(list.item_size == old_item_size);

    /* Postcondition (frame): alloc unchanged */
    assert(list.alloc == old_alloc);
}

void aws_array_list_clear_harness(void) {
    aws_array_list_clear_harness();
    return 0;
}
