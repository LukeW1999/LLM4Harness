#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_array_list_front_harness(void) {
    /* ------------------------------------------------------------------ */
    /* Symbolic list setup                                                  */
    /* ------------------------------------------------------------------ */
    struct aws_array_list list;

    /* item_size must be non-zero and bounded to keep verification tractable */
    __CPROVER_assume(list.item_size > 0 && list.item_size <= 8);

    /* current_size must be a valid multiple of item_size */
    size_t max_items;
    __CPROVER_assume(max_items <= 4);
    size_t current_size;
    __CPROVER_assume(!aws_mul_size_checked(max_items, list.item_size, &current_size));
    list.current_size = current_size;

    /* length must satisfy: length * item_size <= current_size */
    __CPROVER_assume(list.length <= max_items);

    /* data pointer constraints */
    if (list.current_size == 0) {
        list.data = NULL;
    } else {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    }

    /* alloc can be anything (not used by aws_array_list_front) */
    list.alloc = aws_default_allocator();

    /* Ensure the list is valid before calling */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* ------------------------------------------------------------------ */
    /* Symbolic output buffer                                               */
    /* ------------------------------------------------------------------ */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* ------------------------------------------------------------------ */
    /* Snapshot state before call                                           */
    /* ------------------------------------------------------------------ */
    size_t old_length       = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size    = list.item_size;
    void  *old_data         = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* Save a copy of the first element before the call */
    uint8_t *first_elem_copy = NULL;
    if (old_length > 0 && old_data != NULL) {
        first_elem_copy = malloc(old_item_size);
        __CPROVER_assume(first_elem_copy != NULL);
        memcpy(first_elem_copy, old_data, old_item_size);
    }

    /* ------------------------------------------------------------------ */
    /* Call the function under test                                         */
    /* ------------------------------------------------------------------ */
    int result = aws_array_list_front(&list, val);

    /* ------------------------------------------------------------------ */
    /* Postcondition checks                                                 */
    /* ------------------------------------------------------------------ */

    /* The list must still be valid */
    assert(aws_array_list_is_valid(&list));

    /* Frame conditions: list internals must not be modified */
    assert(list.length       == old_length);
    assert(list.current_size == old_current_size);
    assert(list.item_size    == old_item_size);
    assert(list.data         == old_data);
    assert(list.alloc        == old_alloc);

    if (old_length > 0) {
        /* Must succeed */
        assert(result == AWS_OP_SUCCESS);

        /* val must now equal the first element of the list (using saved copy) */
        if (first_elem_copy != NULL) {
            assert(memcmp(val, first_elem_copy, old_item_size) == 0);
        }
    } else {
        /* Must fail with LIST_EMPTY */
        assert(result == AWS_OP_ERR);
        assert(aws_last_error() == AWS_ERROR_LIST_EMPTY);
    }
}
