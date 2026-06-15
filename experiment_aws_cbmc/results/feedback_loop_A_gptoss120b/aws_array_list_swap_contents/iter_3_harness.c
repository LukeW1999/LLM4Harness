#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/array_list.h>
#include <aws/common/allocator.h>

/* Bounds for nondeterministic choices */
#define MAX_ITEM_SIZE 64
#define MAX_INITIAL_CAPACITY 8

/* Nondeterministic helpers */
size_t nondet_size_t(void);
bool nondet_bool(void);

void aws_array_list_swap_contents_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    /* 1. Choose nondeterministic but bounded item size and capacity */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t init_capacity = nondet_size_t();
    __CPROVER_assume(init_capacity <= MAX_INITIAL_CAPACITY);

    /* 2. Initialize two array lists with the same allocator and item size */
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    int rc_a = aws_array_list_init_dynamic(&list_a, alloc, item_size, init_capacity);
    int rc_b = aws_array_list_init_dynamic(&list_b, alloc, item_size, init_capacity);
    __CPROVER_assume(rc_a == AWS_OP_SUCCESS);
    __CPROVER_assume(rc_b == AWS_OP_SUCCESS);

    /* 3. Optionally set different lengths (still within capacity) */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= init_capacity);
    __CPROVER_assume(len_b <= init_capacity);
    list_a.length = len_a;
    list_a.current_size = len_a * item_size;
    list_b.length = len_b;
    list_b.current_size = len_b * item_size;

    /* 4. Save old state */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    /* 5. Call function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* 6. Assert that all fields have been swapped */
    assert(list_a.alloc == old_b.alloc);
    assert(list_a.item_size == old_b.item_size);
    assert(list_a.current_size == old_b.current_size);
    assert(list_a.length == old_b.length);
    assert(list_a.data == old_b.data);

    assert(list_b.alloc == old_a.alloc);
    assert(list_b.item_size == old_a.item_size);
    assert(list_b.current_size == old_a.current_size);
    assert(list_b.length == old_a.length);
    assert(list_b.data == old_a.data);

    /* 7. Allocators must remain equal after swap */
    assert(list_a.alloc == list_b.alloc);

    /* 8. The lists must still satisfy their validity invariants */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));

    /* 9. Clean up */
    aws_array_list_clean_up(&list_a);
    aws_array_list_clean_up(&list_b);
}

/* Provide definitions for nondeterministic helpers to satisfy CBMC */
size_t nondet_size_t(void) {
    size_t x;
    return x;
}
bool nondet_bool(void) {
    bool b;
    return b;
}
