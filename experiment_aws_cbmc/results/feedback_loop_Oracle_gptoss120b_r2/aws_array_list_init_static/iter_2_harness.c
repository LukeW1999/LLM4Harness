#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_static_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(initial_item_allocation > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(initial_item_allocation <= 64);
    __CPROVER_assume(item_size <= 64);

    size_t raw_len;
    __CPROVER_assume(!aws_mul_size_checked(initial_item_allocation, item_size, &raw_len));

    void *raw_array = malloc(raw_len);
    __CPROVER_assume(raw_array != NULL);

    unsigned char *raw_bytes = raw_array;
    for (size_t i = 0; i < raw_len; ++i) {
        raw_bytes[i] = nondet_uint8_t();
    }

    unsigned char *raw_copy = malloc(raw_len);
    __CPROVER_assume(raw_copy != NULL);
    memcpy(raw_copy, raw_bytes, raw_len);

    aws_array_list_init_static(&list, raw_array, initial_item_allocation, item_size);

    assert(aws_array_list_is_valid(&list));
    assert(list.length == 0);
    assert(list.item_size == item_size);
    assert(list.current_size == raw_len);
    assert(list.data == raw_array);
    assert(list.alloc == NULL);
    assert(aws_array_list_capacity(&list) == initial_item_allocation);
    assert(memcmp(raw_array, raw_copy, raw_len) == 0);
}
