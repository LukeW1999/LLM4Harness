#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_array_list_length_harness(void) {
    struct aws_array_list list;

    /* nondet initialization with reasonable bounds */
    size_t item_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(item_size > 0 && item_size <= 1024);

    size_t capacity = (size_t)nondet_uint64_t();
    __CPROVER_assume(capacity <= 16);
    __CPROVER_assume(capacity <= SIZE_MAX / item_size); /* avoid overflow */

    size_t current_size = capacity * item_size;

    size_t length = (size_t)nondet_uint64_t();
    __CPROVER_assume(length <= capacity);

    void *data = NULL;
    if (current_size > 0) {
        data = malloc(current_size);
        __CPROVER_assume(data != NULL);
    }

    struct aws_allocator *allocator = aws_default_allocator();

    list.item_size = item_size;
    list.current_size = current_size;
    list.length = length;
    list.data = data;
    list.alloc = allocator;

    /* Ensure the list satisfies the validity predicate before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save a copy of the list for frame checking */
    struct aws_array_list old_list = list;

    size_t result = aws_array_list_length(&list);

    /* Postcondition: return value equals length */
    assert(result == old_list.length);

    /* Postcondition: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* Frame condition: list fields unchanged */
    assert(list.item_size == old_list.item_size);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.data == old_list.data);
    assert(list.alloc == old_list.alloc);
}
