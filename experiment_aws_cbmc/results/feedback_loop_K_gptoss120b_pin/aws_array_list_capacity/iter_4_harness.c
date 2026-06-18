#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_capacity_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 8);

    size_t init_count;
    __CPROVER_assume(init_count <= 10);

    int init_res = aws_array_list_init_dynamic(&list, alloc, init_count, item_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);

    size_t capacity = list.current_size / list.item_size;
    __CPROVER_assume(list.length <= capacity);

    struct aws_array_list old_list = list;
    uint8_t *old_data = NULL;
    if (list.data && list.current_size > 0) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }

    size_t ret_capacity = aws_array_list_capacity(&list);

    assert(ret_capacity == list.current_size / list.item_size);
    assert(ret_capacity >= list.length);
    assert(ret_capacity * list.item_size == list.current_size);
    assert(aws_array_list_is_valid(&list));

    assert(list.item_size == old_list.item_size);
    assert(list.current_size == old_list.current_size);
    assert(list.alloc == old_list.alloc);
    assert(list.length == old_list.length);
    assert(list.data == old_list.data);

    if (list.data && old_data) {
        assert(memcmp(list.data, old_data, list.current_size) == 0);
        free(old_data);
    }

    aws_array_list_clean_up(&list);
}
