#include <aws/common/array_list.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

/* Nondeterministic size_t generator */
size_t nondet_size_t(void);

/* Helper to compare memory byte‑by‑byte */
static void assert_bytes_match(const void *a, const void *b, size_t n) {
    const uint8_t *pa = (const uint8_t *)a;
    const uint8_t *pb = (const uint8_t *)b;
    for (size_t i = 0; i < n; ++i) {
        __CPROVER_assert(pa[i] == pb[i], "bytes must match");
    }
}

void aws_array_list_swap_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(list.length > 0);
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    uint8_t *ptr_a = (uint8_t *)list.data + a * list.item_size;
    uint8_t *ptr_b = (uint8_t *)list.data + b * list.item_size;

    uint8_t *old_elem_a = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
    uint8_t *old_elem_b = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
    __CPROVER_assume(old_elem_a != NULL);
    __CPROVER_assume(old_elem_b != NULL);

    aws_memcpy(old_elem_a, ptr_a, list.item_size);
    aws_memcpy(old_elem_b, ptr_b, list.item_size);

    int rv = aws_array_list_swap(&list, a, b);

    /* Structural invariants must remain unchanged */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    if (rv == AWS_OP_SUCCESS) {
        if (a == b) {
            assert_bytes_match(ptr_a, old_elem_a, list.item_size);
        } else {
            assert_bytes_match(ptr_a, old_elem_b, list.item_size);
            assert_bytes_match(ptr_b, old_elem_a, list.item_size);
        }
    } else {
        assert_bytes_match(ptr_a, old_elem_a, list.item_size);
        assert_bytes_match(ptr_b, old_elem_b, list.item_size);
    }

    assert(aws_array_list_is_valid(&list));
}
