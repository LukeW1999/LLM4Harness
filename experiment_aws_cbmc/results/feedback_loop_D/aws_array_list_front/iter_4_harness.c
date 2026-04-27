#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_front_harness() {
    struct aws_array_list list;
    size_t max_capacity = nd_size_t();
    size_t item_size = nd_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(max_capacity <= MAX_ITEM_ALLOC / item_size);
    initialize_bounded_array_list(&list, max_capacity, item_size);

    size_t initial_length = nd_size_t();
    __CPROVER_assume(initial_length <= max_capacity);
    list.length = initial_length;

    uint8_t val[item_size];
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, item_size));

    if (initial_length > 0) {
        // Fill the first element with some data to compare later
        for (size_t i = 0; i < item_size; i++) {
            list.data[i] = nd_uint8_t();
        }
    }

    int result = aws_array_list_front(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length > 0);
        assert(memcmp(val, list.data, item_size) == 0);
        assert(list.length == initial_length);
        assert(list.current_size == max_capacity);
        assert(list.item_size == item_size);
    } else {
        assert(result == aws_raise_error(AWS_ERROR_LIST_EMPTY));
        assert(list.length == 0);
    }

    assert(aws_array_list_is_valid(&list));
}
