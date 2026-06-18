#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE 32U

/* nondeterministic helpers */
size_t nondet_size_t(void);
int nondet_int(void);

int aws_array_list_push_front_harness(void) {
    /* set up a list */
    struct aws_array_list list;
    size_t init_alloc = nondet_size_t();
    __CPROVER_assume(init_alloc <= MAX_INITIAL_ITEM_ALLOCATION);
    size_t item_sz = nondet_size_t();
    __CPROVER_assume(item_sz > 0 && item_sz <= MAX_ITEM_SIZE);

    /* allocate backing storage */
    uint8_t *buf = (uint8_t *)malloc(init_alloc * item_sz);
    __CPROVER_assume(buf != NULL);

    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;
    list.item_size = item_sz;
    list.current_size = init_alloc * item_sz;
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= init_alloc);
    list.data = buf;

    /* structural validity assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* value to push */
    uint8_t *val = (uint8_t *)malloc(item_sz);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(val && AWS_MEM_IS_READABLE(val, item_sz));

    /* snapshot old state */
    struct aws_array_list old = list;
    uint8_t *old_data = (uint8_t *)malloc(list.current_size);
    __CPROVER_assume(old_data != NULL);
    memcpy(old_data, list.data, list.current_size);

    /* call the function under test */
    int ret = aws_array_list_push_front(&list, val);

    /* -------------------------------------------------------------------- */
    /* Post‑condition checks */

    /* 1. Return value / error code correctness */
    if (ret == 0) {
        /* success: length must increase by one */
        assert(list.length == old.length + 1);
    } else {
        /* failure: length must be unchanged */
        assert(list.length == old.length);
    }

    /* 2. Output buffer length / capacity invariants */
    assert(list.length * list.item_size <= list.current_size);

    /* 3. Frame conditions (memory not modified beyond contract) */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* data beyond the new logical length must be unchanged */
    for (size_t i = list.length * list.item_size; i < list.current_size; ++i) {
        assert(((uint8_t *)list.data)[i] == old_data[i]);
    }

    /* if the call succeeded, the front element must equal the input value */
    if (ret == 0) {
        for (size_t i = 0; i < list.item_size; ++i) {
            assert(((uint8_t *)list.data)[i] == ((uint8_t *)val)[i]);
        }
    }

    /* clean up */
    free(buf);
    free(val);
    free(old_data);

    return 0;
}
