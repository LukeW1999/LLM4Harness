#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/allocator.h>

/*--- nondet helpers ---*/
extern uint32_t nondet_uint32_t(void);
extern _Bool nondet_bool(void);

/*--- constants and stub used by the function ---*/
#define S2N_SUCCESS 0

int s2n_stuffer_validate(const struct s2n_stuffer *stuffer)
{
    /* Assume validation succeeds for the purpose of the harness */
    return 1;
}

/* The function under test */
int s2n_stuffer_reread(struct s2n_stuffer *stuffer)
{
    stuffer->read_cursor = 0;
    return S2N_SUCCESS;
}

/*--- Harness ---------------------------------------------------*/
void s2n_stuffer_reread_harness(void) {
    struct s2n_stuffer stuffer;

    /* nondet initialise scalar fields */
    stuffer.read_cursor   = nondet_uint32_t();
    stuffer.write_cursor  = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced       = nondet_bool();
    stuffer.growable      = nondet_bool();
    stuffer.tainted       = nondet_bool();

    /* initialise the embedded blob */
    stuffer.blob.size      = nondet_uint32_t();
    stuffer.blob.allocated = nondet_uint32_t();
    stuffer.blob.growable  = nondet_bool();

    const uint32_t MAX_BLOB_SIZE = 256U;
    __CPROVER_assume(stuffer.blob.size <= MAX_BLOB_SIZE);
    __CPROVER_assume(stuffer.blob.allocated >= stuffer.blob.size);
    __CPROVER_assume(stuffer.blob.allocated <= MAX_BLOB_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();

    if (stuffer.blob.allocated > 0) {
        stuffer.blob.data = (uint8_t *)aws_mem_acquire(allocator, stuffer.blob.allocated);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    __CPROVER_assume(s2n_stuffer_validate(&stuffer));

    struct s2n_stuffer old = stuffer;

    int result = s2n_stuffer_reread(&stuffer);

    if (result == S2N_SUCCESS) {
        assert(stuffer.read_cursor == 0);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
    } else {
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
    }

    assert(s2n_stuffer_validate(&stuffer));
}
