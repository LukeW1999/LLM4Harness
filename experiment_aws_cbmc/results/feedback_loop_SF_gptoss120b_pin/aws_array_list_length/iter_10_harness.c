#include <assert.h>
#include <stddef.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

size_t nondet_size_t(void);

#define MAX_ITEM_SIZE 8
#define MAX_CAPACITY 64
#define MAX_LENGTH 8

void aws_array_list_length_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(aws_array_list_init(&list, allocator, MAX_CAPACITY, MAX_ITEM_SIZE) == AWS_OP_SUCCESS);

    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= MAX_LENGTH);
    __CPROVER_assume(list.length <= list.capacity);
    __CPROVER_assume(list.length * list.item_size <= list.current_size);

    size_t old_length = list.length;
    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    size_t result = aws_array_list_length(&list);

    assert(result == old_length);
    assert(list.length == old_length);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);
    assert(list.alloc == old_alloc);
    assert(aws_array_list_is_valid(&list));
}
