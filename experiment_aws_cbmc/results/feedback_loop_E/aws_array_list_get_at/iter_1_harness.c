#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_harness() {
    /* data structure */
    struct aws_array_list list;

    /* parameters */
    size_t index;
    void *val;
    size_t item_size;
    size_t initial_item_allocation;
    size_t len;

    /* assumptions */
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(!aws_mul_size_checked(initial_item_allocation, item_size, &len));

    /* allocate and initialize the array list */
    uint8_t *raw_array = malloc(len);
    __CPROVER_assume(raw_array != NULL);
    aws_array_list_init_static(&list, raw_array, initial_item_allocation, item_size);

    /* set up index and value */
    __CPROVER_assume(index <= initial_item_allocation);
    val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, item_size));

    /* save old state */
    struct aws_array_list old_list = list;

    /* perform operation under verification */
    int result = aws_array_list_get_at(&list, val, index);

    /* assertions */
    if (result == AWS_OP_SUCCESS) {
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
        assert(list.item_size == old_list.item_size);
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(AWS_MEM_IS_READABLE(val, item_size));
        assert_bytes_match((uint8_t *)val, (uint8_t *)list.data + (index * list.item_size), item_size);
    } else {
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
        assert(list.item_size == old_list.item_size);
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
    }

    assert(aws_array_list_is_valid(&list));
}
