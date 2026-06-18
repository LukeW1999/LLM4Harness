#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

#define MAX_INITIAL_ITEM_ALLOCATION 256
#define MAX_ITEM_SIZE 256

size_t nondet_size_t(void);

void aws_array_list_swap_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    struct aws_array_list list;
    int init_rc = aws_array_list_init(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);
    __CPROVER_assume(init_rc == AWS_OP_SUCCESS);

    /* Constrain length to be within allocated capacity */
    size_t max_len = list.current_size / (list.item_size == 0 ? 1 : list.item_size);
    __CPROVER_assume(list.length <= max_len);
    __CPROVER_assume(list.length > 0);

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    struct aws_array_list old = list;

    size_t item_sz = list.item_size;
    uint8_t old_a[MAX_ITEM_SIZE];
    uint8_t old_b[MAX_ITEM_SIZE];
    if (item_sz > 0) {
        memcpy(old_a, (uint8_t *)list.data + a * item_sz, item_sz);
        memcpy(old_b, (uint8_t *)list.data + b * item_sz, item_sz);
    }

    int rc = aws_array_list_swap(&list, a, b);

    /* Structural fields must remain unchanged */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    if (rc == AWS_OP_SUCCESS) {
        if (a == b || item_sz == 0) {
            if (item_sz > 0) {
                assert(memcmp((uint8_t *)list.data + a * item_sz, old_a, item_sz) == 0);
                assert(memcmp((uint8_t *)list.data + b * item_sz, old_b, item_sz) == 0);
            }
        } else {
            assert(memcmp((uint8_t *)list.data + a * item_sz, old_b, item_sz) == 0);
            assert(memcmp((uint8_t *)list.data + b * item_sz, old_a, item_sz) == 0);
        }
    }

    assert(aws_array_list_is_valid(&list));

    aws_array_list_clean_up(&list);
}
