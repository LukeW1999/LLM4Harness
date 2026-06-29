#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound the list length to keep the proof tractable */
#define MAX_LIST_LENGTH 4
#define MAX_ITEM_SIZE   8

void aws_array_list_push_back_harness(void) {
    /* Allocate and non-deterministically initialize the list */
    struct aws_array_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);

    /* item_size must be > 0 and bounded */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    list->item_size = item_size;

    /* length must be bounded */
    size_t length;
    __CPROVER_assume(length <= MAX_LIST_LENGTH);
    list->length = length;

    /* current_size must be >= length * item_size and not overflow */
    size_t required_size;
    __CPROVER_assume(!aws_mul_size_checked(length, item_size, &required_size));
    size_t current_size;
    __CPROVER_assume(current_size >= required_size);
    /* Bound current_size to keep proof tractable */
    __CPROVER_assume(current_size <= (MAX_LIST_LENGTH + 1) * MAX_ITEM_SIZE);
    list->current_size = current_size;

    /* data pointer constraints */
    if (current_size == 0) {
        list->data = NULL;
    } else {
        void *data = malloc(current_size);
        __CPROVER_assume(data != NULL);
        list->data = data;
    }

    /* alloc: either NULL (static) or aws_default_allocator() (dynamic) */
    bool use_dynamic;
    if (use_dynamic) {
        list->alloc = aws_default_allocator();
    } else {
        list->alloc = NULL;
    }

    /* Verify the list is valid before calling */
    __CPROVER_assume(aws_array_list_is_valid(list));

    /* Set up val: non-null, readable for item_size bytes */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Save pre-call state */
    size_t old_length      = list->length;
    size_t old_item_size   = list->item_size;
    size_t old_current_size = list->current_size;
    void  *old_alloc       = (void *)list->alloc;
    void  *old_data        = list->data;

    /* Call the function under test */
    int result = aws_array_list_push_back(list, val);

    /* Postcondition: list is still valid */
    assert(aws_array_list_is_valid(list));

    /* Postcondition: return value is 0 or -1 */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* Postcondition: item_size and alloc are unchanged */
    assert(list->item_size == old_item_size);
    assert((void *)list->alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* Postcondition: length increased by 1 */
        assert(list->length == old_length + 1);
        /* Postcondition: capacity is sufficient */
        assert(list->current_size >= list->length * list->item_size);
    } else {
        /* Postcondition: on failure, length is unchanged */
        assert(list->length == old_length);

        /* Postcondition: static list failure raises LIST_EXCEEDS_MAX_SIZE */
        if (list->alloc == NULL) {
            assert(aws_last_error() == AWS_ERROR_LIST_EXCEEDS_MAX_SIZE);
        }

        /* Postcondition: static list data pointer and current_size unchanged */
        if (list->alloc == NULL) {
            assert(list->current_size == old_current_size);
            assert(list->data == old_data);
        }
    }
}
