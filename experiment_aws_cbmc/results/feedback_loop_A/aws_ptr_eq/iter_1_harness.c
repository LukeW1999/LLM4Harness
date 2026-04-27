#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_hash_table_init_harness() {
    /* 1. Declare and bound data structures */
    struct aws_hash_table map;
    struct aws_allocator alloc;
    size_t size = nondet_size_t();
    aws_hash_fn *hash_fn = (aws_hash_fn *)nondet_void_ptr();
    aws_hash_callback_eq_fn *equals_fn = (aws_hash_callback_eq_fn *)nondet_void_ptr();
    aws_hash_callback_destroy_fn *destroy_key_fn = (aws_hash_callback_destroy_fn *)nondet_void_ptr();
    aws_hash_callback_destroy_fn *destroy_value_fn = (aws_hash_callback_destroy_fn *)nondet_void_ptr();

    /* Bound the size to a reasonable value */
    __CPROVER_assume(size <= MAX_HASH_TABLE_SIZE);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_hash_table old_map = map;

    /* 3. Call function under test */
    int result = aws_hash_table_init(&map, &alloc, size, hash_fn, equals_fn, destroy_key_fn, destroy_value_fn);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Fields that change on success */
        assert(map.p_impl != NULL);
    } else {
        /* Fields that remain unchanged on failure */
        assert(map.p_impl == old_map.p_impl);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(map.allocator == &alloc);
    assert(map.p_impl == old_map.p_impl || result == AWS_OP_SUCCESS);

    /* 6. Assert validity invariants always holds */
    assert(aws_hash_table_is_valid(&map));
}
