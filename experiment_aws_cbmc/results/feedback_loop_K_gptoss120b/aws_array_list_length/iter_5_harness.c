#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>

extern size_t nondet_size_t(void);

void aws_array_list_length_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0);

    /* Initialize the list; if initialization fails, abort the harness. */
    if (aws_array_list_init(&list, allocator, capacity, item_size) != AWS_OP_SUCCESS) {
        return;
    }

    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);
    list.length = length;

    /* Preserve old state. */
    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    size_t old_length = list.length;
    struct aws_allocator *old_alloc = list.alloc;

    size_t result = aws_array_list_length(&list);

    /* Verify that length returns the stored length and does not modify the list. */
    assert(result == old_length);
    assert(list.data == old_data);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.length == old_length);
    assert(list.alloc == old_alloc);
    assert(aws_array_list_is_valid(&list));

    aws_array_list_clean_up(&list);
}
