#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void aws_array_list_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size < 1024);

    size_t init_count;
    __CPROVER_assume(init_count <= 64);

    int init_res = aws_array_list_init_dynamic(&list, allocator, init_count, item_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);

    size_t capacity = aws_array_list_capacity(&list);
    size_t length;
    __CPROVER_assume(length <= capacity);
    __CPROVER_assume(length > 0);               /* constrain to non‑empty list */
    list.length = length;

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Initialize first element with nondet data */
    uint8_t *first_elem_copy = malloc(item_size);
    __CPROVER_assume(first_elem_copy != NULL);
    uint8_t *data_ptr = (uint8_t *)list.data;
    for (size_t i = 0; i < item_size; ++i) {
        uint8_t nondet;
        __CPROVER_assume(1);
        data_ptr[i] = nondet;
    }
    memcpy(first_elem_copy, list.data, item_size);

    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    struct aws_array_list list_snapshot = list;
    uint8_t *data_snapshot = NULL;
    if (list.current_size > 0) {
        data_snapshot = malloc(list.current_size);
        __CPROVER_assume(data_snapshot != NULL);
        memcpy(data_snapshot, list.data, list.current_size);
    }

    int ret = aws_array_list_front(&list, val);
    assert(ret == AWS_OP_SUCCESS);

    assert(aws_array_list_is_valid(&list));

    /* Verify list unchanged */
    assert(list.length == list_snapshot.length);
    assert(list.current_size == list_snapshot.current_size);
    assert(list.item_size == list_snapshot.item_size);
    assert(list.alloc == list_snapshot.alloc);
    assert(list.data == list_snapshot.data);
    if (list.current_size > 0) {
        assert(memcmp(list.data, data_snapshot, list.current_size) == 0);
    }

    /* Verify returned value matches first element */
    assert(memcmp(val, first_elem_copy, item_size) == 0);

    aws_array_list_clean_up(&list);
    free(val);
    free(first_elem_copy);
    free(data_snapshot);
}
