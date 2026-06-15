#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_hash_table_init_harness() {
    /* 1. Declare and bound data structures */
    struct aws_hash_table map;
    struct aws_allocator *alloc = &aws_default_allocator();
    size_t size = nondet_size_t();
    aws_hash_fn *hash_fn = nondet_uint64_t;
    aws_hash_callback_eq_fn *equals_fn = nondet_bool;
    aws_hash_callback_destroy_fn *destroy_key_fn = NULL;
    aws_hash_callback_destroy_fn *destroy_value_fn = NULL;
    __CPROVER_assume(size > 0);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_hash_table old_map = map;

    /* 3. Call function under test */
    int result = aws_hash_table_init(&map, alloc, size, hash_fn, equals_fn, destroy_key_fn, destroy_value_fn);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(map.p_impl != NULL);
    } else {
        assert(map.p_impl == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(map.p_impl != old_map.p_impl);

    /* 6. Assert validity invariant always holds */
    assert(aws_hash_table_is_valid(&map));
}
