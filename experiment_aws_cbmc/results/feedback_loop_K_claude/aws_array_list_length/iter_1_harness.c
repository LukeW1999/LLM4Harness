#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_length_harness(void) {
    struct aws_array_list list;

    /* Nondeterministic choice: zeroed list or valid list */
    bool use_zeroed;
    __CPROVER_assume(use_zeroed == 0 || use_zeroed == 1);

    if (use_zeroed) {
        /* Case 1: zeroed list */
        AWS_ZERO_STRUCT(list);
    } else {
        /* Case 2: valid list */

        /* item_size must be nonzero */
        size_t item_size;
        __CPROVER_assume(item_size > 0);
        list.item_size = item_size;

        /* current_size must be a multiple of item_size (or zero) */
        size_t current_size;
        __CPROVER_assume(current_size == 0 || (current_size % item_size == 0));
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
            /* Allocate a concrete buffer of current_size bytes */
            void *data = malloc(current_size);
            __CPROVER_assume(data != NULL);
            list.data = data;
        }

        /* alloc: can be NULL (static) or non-NULL (dynamic) */
        /* We don't need a real allocator for length, just set nondeterministically */
        bool has_alloc;
        if (has_alloc) {
            list.alloc = aws_default_allocator();
        } else {
            list.alloc = NULL;
        }

        /* Verify our setup satisfies aws_array_list_is_valid */
        assert(aws_array_list_is_valid(&list));
    }

    /* Snapshot state before call */
    size_t old_length      = list.length;
    size_t old_item_size   = list.item_size;
    size_t old_current_size = list.current_size;
    void  *old_data        = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under test */
    size_t result = aws_array_list_length(&list);

    /* Postcondition: return value equals list->length */
    assert(result == old_length);

    /* Postcondition: frame — no fields modified */
    assert(list.length       == old_length);
    assert(list.item_size    == old_item_size);
    assert(list.current_size == old_current_size);
    assert(list.data         == old_data);
    assert(list.alloc        == old_alloc);

    /* Postcondition: validity preserved */
    if (use_zeroed) {
        /* zeroed list stays zeroed (length==0, item_size==0, etc.) */
        assert(list.length == 0);
        assert(list.item_size == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    } else {
        assert(aws_array_list_is_valid(&list));
    }
}

void aws_array_list_length_harness(void) {
    aws_array_list_length_harness();
    return 0;
}
