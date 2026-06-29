#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_pop_back_harness(void) {
    /* Allocate the list structure */
    struct aws_array_list *list = malloc(sizeof(struct aws_array_list));
    __CPROVER_assume(list != NULL);

    /* Nondeterministic item_size: must be > 0 and bounded for tractability */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 8);
    list->item_size = item_size;

    /* Nondeterministic length */
    size_t length;
    list->length = length;

    /* Nondeterministic current_size */
    size_t current_size;
    list->current_size = current_size;

    /* Ensure required_size = length * item_size does not overflow */
    size_t required_size;
    __CPROVER_assume(!__builtin_mul_overflow(length, item_size, &required_size));

    /* current_size must be >= required_size */
    __CPROVER_assume(current_size >= required_size);

    /* Bound current_size for tractability */
    __CPROVER_assume(current_size <= 64);

    /* data pointer constraints */
    if (current_size == 0) {
        list->data = NULL;
    } else {
        void *data = malloc(current_size);
        __CPROVER_assume(data != NULL);
        list->data = data;
    }

    /* Set allocator */
    list->alloc = aws_default_allocator();

    /* Verify precondition: list is valid */
    __CPROVER_assume(aws_array_list_is_valid(list));

    /* Save state before call */
    size_t old_length = list->length;
    size_t old_current_size = list->current_size;
    size_t old_item_size = list->item_size;
    void *old_data_ptr = list->data;
    struct aws_allocator *old_alloc = list->alloc;

    /* Call the function under test */
    int result = aws_array_list_pop_back(list);

    /* Postcondition: list remains valid */
    assert(aws_array_list_is_valid(list));

    /* Postcondition: return value */
    if (old_length > 0) {
        assert(result == AWS_OP_SUCCESS);
        /* Postcondition: length decreased by 1 */
        assert(list->length == old_length - 1);
    } else {
        assert(result == AWS_OP_ERR);
        /* Postcondition: length unchanged */
        assert(list->length == old_length);
    }

    /* Postcondition: frame conditions */
    assert(list->current_size == old_current_size);
    assert(list->item_size == old_item_size);
    assert(list->data == old_data_ptr);
    assert(list->alloc == old_alloc);
}

void aws_array_list_pop_back_harness(void) {
    aws_array_list_pop_back_harness();
    return 0;
}
