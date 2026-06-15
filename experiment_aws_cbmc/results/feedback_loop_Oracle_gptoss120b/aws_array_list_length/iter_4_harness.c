#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_length_harness(void) {
    struct aws_array_list list;
    size_t item_size;
    size_t length;
    size_t current_size;

    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(current_size <= 1024 * 1024);
    __CPROVER_assume(length <= current_size / item_size);

    uint8_t *data = NULL;
    if (current_size > 0) {
        data = malloc(current_size);
        __CPROVER_assume(data != NULL);
    }

    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;
    list.data = data;
    list.current_size = current_size;
    list.length = length;
    list.item_size = item_size;

    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old_list = list;
    uint8_t *old_data = NULL;
    if (data) {
        old_data = malloc(current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, data, current_size);
    }

    size_t ret = aws_array_list_length(&list);

    assert(ret == old_list.length);
    assert(memcmp(&list, &old_list, sizeof(list)) == 0);
    if (data) {
        assert(memcmp(data, old_data, current_size) == 0);
    }
}
