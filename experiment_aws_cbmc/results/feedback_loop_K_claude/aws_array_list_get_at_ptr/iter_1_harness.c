#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_get_at_ptr_harness(void) {
    /* Allocate and initialize the list */
    struct aws_array_list *list = malloc(sizeof(struct aws_array_list));
    __CPROVER_assume(list != NULL);

    /* Nondet fields */
    size_t item_size;
    size_t length;
    size_t current_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(length <= current_size);
    /* Prevent overflow: item_size * current_size must not overflow */
    __CPROVER_assume(current_size == 0 || item_size <= (SIZE_MAX / current_size));

    list->item_size    = item_size;
    list->length       = length;
    list->current_size = current_size;
    list->alloc        = aws_default_allocator();

    /* Set up data buffer */
    if (current_size > 0) {
        void *data = malloc(item_size * current_size);
        __CPROVER_assume(data != NULL);
        list->data = data;
    } else {
        list->data = NULL;
    }

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_array_list_is_valid(list));

    /* val must be a valid non-null pointer to void* */
    void **val = malloc(sizeof(void *));
    __CPROVER_assume(val != NULL);

    /* Nondet index */
    size_t index;

    /* Snapshot state before call */
    size_t old_length       = list->length;
    size_t old_current_size = list->current_size;
    size_t old_item_size    = list->item_size;
    void  *old_data         = list->data;
    struct aws_allocator *old_alloc = list->alloc;

    /* Call the function under test */
    int result = aws_array_list_get_at_ptr(list, val, index);

    /* Postconditions (validity) */
    if (index < old_length) {
        /* Success path */
        assert(result == AWS_OP_SUCCESS);
        assert(*val != NULL);
        assert(*val == (void *)((uint8_t *)old_data + old_item_size * index));
    } else {
        /* Failure path */
        assert(result == AWS_OP_ERR);
        assert(aws_last_error() == AWS_ERROR_INVALID_INDEX);
    }

    /* Postconditions (frame): list fields must be unchanged */
    assert(list->length       == old_length);
    assert(list->current_size == old_current_size);
    assert(list->item_size    == old_item_size);
    assert(list->data         == old_data);
    assert(list->alloc        == old_alloc);

    /* Postconditions (length): list remains valid */
    assert(aws_array_list_is_valid(list));
}
