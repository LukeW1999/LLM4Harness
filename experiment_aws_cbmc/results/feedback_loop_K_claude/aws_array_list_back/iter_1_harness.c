#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound constants for CBMC tractability */
#define MAX_ITEM_SIZE 16
#define MAX_LENGTH    4

void aws_array_list_back_harness(void) {
    /* ---------------------------------------------------------------
     * 1. Allocate and non-deterministically initialise the list
     * --------------------------------------------------------------- */
    struct aws_array_list *list =
        (struct aws_array_list *)malloc(sizeof(struct aws_array_list));
    assert(list != NULL);

    /* item_size: must be > 0 and bounded */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    list->item_size = item_size;

    /* length: bounded */
    size_t length;
    __CPROVER_assume(length <= MAX_LENGTH);
    list->length = length;

    /* current_size: must be >= length * item_size, and consistent */
    size_t current_size;
    /* Prevent overflow in length * item_size */
    __CPROVER_assume(length == 0 || (current_size / item_size >= length));
    __CPROVER_assume(current_size >= length * item_size);
    /* current_size == 0 iff length == 0 (simplest valid case) or
       current_size > 0 with data != NULL */
    list->current_size = current_size;

    /* data: must be non-NULL when current_size > 0 */
    if (current_size > 0) {
        void *data = malloc(current_size);
        __CPROVER_assume(data != NULL);
        list->data = data;
    } else {
        list->data = NULL;
    }

    /* alloc: use default allocator */
    list->alloc = aws_default_allocator();

    /* Establish the validity predicate */
    __CPROVER_assume(aws_array_list_is_valid(list));

    /* ---------------------------------------------------------------
     * 2. Allocate val: writable buffer of item_size bytes
     * --------------------------------------------------------------- */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* ---------------------------------------------------------------
     * 3. Snapshot list fields before the call (frame check)
     * --------------------------------------------------------------- */
    size_t old_length       = list->length;
    size_t old_current_size = list->current_size;
    size_t old_item_size    = list->item_size;
    void  *old_data         = list->data;
    struct aws_allocator *old_alloc = list->alloc;

    /* ---------------------------------------------------------------
     * 4. Call the function under test
     * --------------------------------------------------------------- */
    int result = aws_array_list_back(list, val);

    /* ---------------------------------------------------------------
     * 5. Postcondition checks
     * --------------------------------------------------------------- */

    /* 5a. Validity is preserved */
    assert(aws_array_list_is_valid(list));

    /* 5b. Return value semantics */
    if (old_length > 0) {
        assert(result == AWS_OP_SUCCESS);
    } else {
        assert(result != AWS_OP_SUCCESS);
    }

    /* 5c. Frame: list fields are unchanged */
    assert(list->length       == old_length);
    assert(list->current_size == old_current_size);
    assert(list->item_size    == old_item_size);
    assert(list->data         == old_data);
    assert(list->alloc        == old_alloc);

    /* 5d. item_size invariant still positive */
    assert(list->item_size > 0);

    /* 5e. length * item_size still fits in current_size */
    assert(list->length * list->item_size <= list->current_size);
}

void aws_array_list_back_harness(void) {
    aws_array_list_back_harness();
    return 0;
}
