#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

/*--- s2n data structures (copied from the library) -----------------------*/
struct s2n_blob {
    uint8_t *data;
    uint32_t size;
    uint32_t allocated;
    unsigned growable : 1;
};

struct s2n_stuffer {
    struct s2n_blob blob;
    uint32_t read_cursor;
    uint32_t write_cursor;
    uint32_t high_water_mark;
    unsigned int alloced : 1;
    unsigned int growable : 1;
    unsigned int tainted : 1;
};

/*--- Return codes used by the library ------------------------------------*/
#define S2N_SUCCESS 0

/*--- Stub for the internal implementation ---------------------------------*/
int s2n_stuffer_copy_impl(struct s2n_stuffer *from,
                          struct s2n_stuffer *to,
                          uint32_t len);

/* Global to capture the return value of the stub for post‑condition checks */
int impl_ret;

/* Stub definition – returns a nondeterministic int (may be negative) */
int s2n_stuffer_copy_impl(struct s2n_stuffer *from,
                          struct s2n_stuffer *to,
                          uint32_t len)
{
    (void)from;
    (void)to;
    (void)len;
    impl_ret = nondet_int();
    return impl_ret;
}

/*--- Function under test --------------------------------------------------*/
int s2n_stuffer_copy(struct s2n_stuffer *from,
                     struct s2n_stuffer *to,
                     const uint32_t len);

/*--- Harness -------------------------------------------------------------*/
void s2n_stuffer_copy_harness(void)
{
    /* 1. Allocate and bound the two stuffer objects */
    struct s2n_stuffer from;
    struct s2n_stuffer to;

    /* bound blob sizes */
    from.blob.size = nondet_uint32_t();
    to.blob.size   = nondet_uint32_t();
    __CPROVER_assume(from.blob.size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(to.blob.size   <= MAX_BUFFER_SIZE);

    /* bound allocated fields (may be zero) */
    from.blob.allocated = nondet_uint32_t();
    to.blob.allocated   = nondet_uint32_t();
    __CPROVER_assume(from.blob.allocated >= from.blob.size);
    __CPROVER_assume(to.blob.allocated   >= to.blob.size);

    /* allocate the data buffers if needed */
    if (from.blob.allocated > 0) {
        from.blob.data = malloc(from.blob.allocated);
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(from.blob.data, from.blob.allocated));
    } else {
        from.blob.data = NULL;
    }
    if (to.blob.allocated > 0) {
        to.blob.data = malloc(to.blob.allocated);
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(to.blob.data, to.blob.allocated));
    } else {
        to.blob.data = NULL;
    }

    /* bound the growable flag */
    from.blob.growable = nondet_bool();
    to.blob.growable   = nondet_bool();

    /* initialise cursors within valid ranges */
    from.read_cursor  = nondet_uint32_t();
    from.write_cursor = nondet_uint32_t();
    __CPROVER_assume(from.read_cursor  <= from.write_cursor);
    __CPROVER_assume(from.write_cursor <= from.blob.size);

    to.read_cursor  = nondet_uint32_t();
    to.write_cursor = nondet_uint32_t();
    __CPROVER_assume(to.read_cursor  <= to.write_cursor);
    __CPROVER_assume(to.write_cursor <= to.blob.size);

    /* other fields */
    from.high_water_mark = nondet_uint32_t();
    to.high_water_mark   = nondet_uint32_t();

    from.alloced   = nondet_bool();
    from.growable  = nondet_bool();
    from.tainted   = nondet_bool();

    to.alloced   = nondet_bool();
    to.growable  = nondet_bool();
    to.tainted   = nondet_bool();

    /* 2. Save old state */
    struct s2n_stuffer old_from = from;
    struct s2n_stuffer old_to   = to;

    /* 3. Choose a length for the copy */
    uint32_t len = nondet_uint32_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 4. Call the function under test */
    int result = s2n_stuffer_copy(&from, &to, len);

    /* 5. Post‑condition checks */

    /* The function always returns S2N_SUCCESS */
    assert(result == S2N_SUCCESS);

    /* If the internal implementation failed, cursors must be restored */
    if (impl_ret < 0) {
        assert(from.read_cursor  == old_from.read_cursor);
        assert(to.write_cursor   == old_to.write_cursor);
    } else {
        /* On success the implementation may have advanced the cursors.
         * We only require that they stay within the bounds of their blobs. */
        assert(from.read_cursor  <= from.blob.size);
        assert(to.write_cursor   <= to.blob.size);
    }

    /* Unchanged fields for 'from' */
    assert(from.blob.data      == old_from.blob.data);
    assert(from.blob.size      == old_from.blob.size);
    assert(from.blob.allocated == old_from.blob.allocated);
    assert(from.blob.growable  == old_from.blob.growable);
    assert(from.write_cursor   == old_from.write_cursor);
    assert(from.high_water_mark== old_from.high_water_mark);
    assert(from.alloced        == old_from.alloced);
    assert(from.growable       == old_from.growable);
    assert(from.tainted        == old_from.tainted);

    /* Unchanged fields for 'to' */
    assert(to.blob.data        == old_to.blob.data);
    assert(to.blob.size        == old_to.blob.size);
    assert(to.blob.allocated   == old_to.blob.allocated);
    assert(to.blob.growable    == old_to.blob.growable);
    assert(to.read_cursor      == old_to.read_cursor);
    assert(to.high_water_mark  == old_to.high_water_mark);
    assert(to.alloced          == old_to.alloced);
    assert(to.growable         == old_to.growable);
    assert(to.tainted          == old_to.tainted);

    /* 6. Basic validity invariants for both stuffer objects */
    assert(from.read_cursor  <= from.write_cursor);
    assert(from.write_cursor <= from.blob.size);
    assert(to.read_cursor    <= to.write_cursor);
    assert(to.write_cursor   <= to.blob.size);
}
