#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Nondeterministic helpers */
size_t nondet_size_t(void);
_Bool nondet_bool(void);

void aws_array_list_length_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Reasonable bounds */
    const size_t MAX_ITEM_SIZE = 8;
    const size_t MAX_CAPACITY  = 64;
    const size_t MAX_LENGTH    = 8;

    /* Initialize the list with a bounded capacity */
    __CPROVER_assume(
        aws_array_list_init(&list, allocator, MAX_CAPACITY * MAX_ITEM_SIZE, MAX_ITEM_SIZE) == AWS_OP_SUCCESS);

    /* Set a nondeterministic length within bounds */
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= MAX_LENGTH);
    __CPROVER_assume(list.length * list.item_size <= list.current_size);

    /* Snapshot original state */
    size_t old_length = list.length;
    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under test */
    size_t result = aws_array_list_length(&list);

    /* Postconditions */
    assert(result == old_length);
    assert(list.length == old_length);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);
    assert(list.alloc == old_alloc);
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    aws_array_list_clean_up(&list);
}
