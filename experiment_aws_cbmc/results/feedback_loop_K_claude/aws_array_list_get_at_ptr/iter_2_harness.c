#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>

void aws_array_list_get_at_ptr_harness(void) {
    /* Stack-allocated list */
    struct aws_array_list list;

    /* Nondet fields */
    size_t item_size;
    size_t length;
    size_t current_size;

    __CPROVER_assume(item_size > 0 && item_size <= 128);
    __CPROVER_assume(current_size <= 8);
    __CPROVER_assume(length <= current_size);
    /* Prevent overflow */
    __CPROVER_assume(current_size == 0 || item_size <= (SIZE_MAX / current_size));

    list.item_size    = item_size;
    list.length       = length;
    list.current_size = current_size;
    list.alloc        = aws_default_allocator();

    /* Set up data buffer */
    if (current_size > 0) {
        void *data = malloc(item_size * current_size);
        __CPROVER_assume(data != NULL);
        list.data = data;
    } else {
        list.data = NULL;
    }

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* val must be a valid non-null pointer to void* */
    void **val = malloc(sizeof(void *));
    __CPROVER_assume(val != NULL);

    /* Nondet index - keep small to avoid state explosion */
    size_t index;
    __CPROVER_assume(index <= 8);

    /* Snapshot state before call */
    size_t old_length       = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size    = list.item_size;
    void  *old_data         = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under test */
    int result = aws_array_list_get_at_ptr(&list, (void **)val, index);

    /* Postconditions (frame): list fields must be unchanged */
    assert(list.length       == old_length);
    assert(list.current_size == old_current_size);
    assert(list.item_size    == old_item_size);
    assert(list.data         == old_data);
    assert(list.alloc        == old_alloc);

    /* Postconditions (validity): list remains valid */
    assert(aws_array_list_is_valid(&list));
}
