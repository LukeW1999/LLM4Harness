#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 256
#endif

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 256
#endif

void aws_array_list_swap_harness() {
    struct aws_array_list list;

    /* ensure the list is bounded and has valid backing data */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* allocate and initialize the list data member */
    struct aws_allocator *allocator = aws_default_allocator();
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t length = aws_array_list_length(&list);
    size_t a, b;
    __CPROVER_assume(a < length);
    __CPROVER_assume(b < length);

    /* capture the state before the swap */
    struct aws_array_list old = list;
    uint8_t old_data[MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE];
    size_t buf_size = sizeof(old_data);
    __CPROVER_assume(list.current_size <= buf_size);
    memcpy(old_data, list.data, list.current_size);

    aws_array_list_swap(&list, a, b);

    /* structural invariants must hold */
    assert(aws_array_list_is_valid(&list));
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);

    /* create a local copy of the expected post-swap data */
    uint8_t expected[MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE];
    memcpy(expected, old_data, list.current_size);

    uint8_t tmp[MAX_ITEM_SIZE];
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    memcpy(tmp, expected + a * list.item_size, list.item_size);
    memcpy(expected + a * list.item_size, expected + b * list.item_size, list.item_size);
    memcpy(expected + b * list.item_size, tmp, list.item_size);

    /* byte-wise comparison because some CBMC versions choke on memcmp in assert */
    size_t i;
    for (i = 0; i < list.current_size; i++) {
        assert(((uint8_t *)list.data)[i] == expected[i]);
    }
}
