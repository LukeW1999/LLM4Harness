#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_push_back_harness() {
    /* data structure */
    struct aws_array_list list;

    /* parameters */
    size_t item_size;
    size_t initial_item_allocation;
    size_t len;
    uint8_t *val = malloc(sizeof(uint8_t));

    /* assumptions */
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(!aws_mul_size_checked(initial_item_allocation, item_size, &len));
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

    /* initialize list */
    aws_array_list_init_dynamic(&list, aws_default_allocator(), initial_item_allocation, item_size);

    /* save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data != NULL) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    /* perform operation under verification */
    int result = aws_array_list_push_back(&list, val);

    /* assertions */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
        assert(list.current_size >= old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data != NULL);
        if (old.data != NULL) {
            assert_bytes_match((uint8_t *)list.data, (uint8_t *)old.data, old.current_size);
        }
        assert_bytes_match((uint8_t *)list.data + old.current_size, val, item_size);
    } else {
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        if (old.data != NULL) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
