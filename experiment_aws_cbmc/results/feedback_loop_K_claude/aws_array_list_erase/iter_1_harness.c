#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound the list size for CBMC tractability */
#define MAX_LIST_LENGTH 4
#define ITEM_SIZE 4

void aws_array_list_erase_harness(void) {
    /* Allocate and initialize the list structure */
    struct aws_array_list *list = malloc(sizeof(struct aws_array_list));
    __CPROVER_assume(list != NULL);

    /* Set up bounded, concrete parameters */
    size_t length;
    __CPROVER_assume(length > 0 && length <= MAX_LIST_LENGTH);

    size_t item_size = ITEM_SIZE;
    size_t current_size = length * item_size;

    /* Allocate backing data */
    void *data = malloc(current_size);
    __CPROVER_assume(data != NULL);

    /* Initialize list fields */
    list->alloc        = aws_default_allocator();
    list->current_size = current_size;
    list->length       = length;
    list->item_size    = item_size;
    list->data         = data;

    /* Verify precondition: list is valid */
    __CPROVER_assume(aws_array_list_is_valid(list));

    /* Record pre-call state */
    size_t old_length       = list->length;
    size_t old_current_size = list->current_size;
    size_t old_item_size    = list->item_size;
    struct aws_allocator *old_alloc = list->alloc;
    void *old_data          = list->data;

    /* Unconstrained index to cover all branches */
    size_t index;

    /* Call the function under test */
    int result = aws_array_list_erase(list, index);

    /* Postconditions (validity) */
    assert(list != NULL);
    assert(aws_array_list_is_valid(list));

    if (index >= old_length) {
        /* Invalid index: error path */
        assert(result == AWS_OP_ERR);
        assert(aws_last_error() == AWS_ERROR_INVALID_INDEX);

        /* Postconditions (length): length unchanged */
        assert(list->length == old_length);
    } else {
        /* Valid index: success path */
        assert(result == AWS_OP_SUCCESS);

        /* Postconditions (length): length decremented by 1 */
        assert(list->length == old_length - 1);
    }

    /* Postconditions (frame): these fields must not change */
    assert(list->item_size    == old_item_size);
    assert(list->alloc        == old_alloc);
    assert(list->current_size == old_current_size);
    assert(list->data         == old_data);

    return 0;
}
