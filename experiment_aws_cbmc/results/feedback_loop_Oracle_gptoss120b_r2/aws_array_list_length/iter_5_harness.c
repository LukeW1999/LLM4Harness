#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_length_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Allocate and initialize the list with bounded sizes to avoid CBMC blow‑up */
    ensure_array_list_is_allocated(&list, allocator);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Impose reasonable bounds on the internal fields */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= 64);
    __CPROVER_assume(list.current_size <= 256);
    __CPROVER_assume(list.length <= (list.current_size / list.item_size));

    struct aws_array_list old_list = list;
    uint8_t *old_data = NULL;

    if (list.data) {
        old_data = (uint8_t *)aws_mem_acquire(allocator, list.current_size);
        __CPROVER_assume(old_data != NULL);
        for (size_t i = 0; i < list.current_size; ++i) {
            old_data[i] = ((uint8_t *)list.data)[i];
        }
    }

    size_t ret_len = aws_array_list_length(&list);

    assert(ret_len == list.length);
    assert(aws_memcmp(&list, &old_list, sizeof(struct aws_array_list)) == 0);
    if (list.data) {
        for (size_t i = 0; i < list.current_size; ++i) {
            assert(((uint8_t *)list.data)[i] == old_data[i]);
        }
    }

    assert(aws_array_list_is_valid(&list));

    if (old_data) {
        aws_mem_release(allocator, old_data);
    }
}
