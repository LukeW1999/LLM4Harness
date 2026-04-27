#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_front_harness() {
    /* data structure */
    struct aws_array_list list;

    /* parameters */
    size_t item_size;
    size_t initial_item_allocation;
    size_t len;
    uint8_t *val = malloc(MAX_ITEM_SIZE);

    /* assumptions */
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(!aws_mul_size_checked(initial_item_allocation, item_size, &len));
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, item_size));

    /* initialize list */
    uint8_t *raw_array = malloc(len);
    __CPROVER_assume(raw_array != NULL);
    aws_array_list_init_static(&list, raw_array, initial_item_allocation, item_size);

    /* save old state */
    struct aws_array_list old_list = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(raw_array, len, &old_byte);

    /* perform operation under verification */
    int result = aws_array_list_front(&list, val);

    /* assertions */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
        assert(AWS_BYTES_EQ(val, raw_array, item_size));
    } else {
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
    }

    assert(aws_array_list_is_valid(&list));
    assert_byte_from_buffer_matches(raw_array, &old_byte);
}
