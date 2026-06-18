#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/allocator.h>
#include <aws/common/memory.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

/*--- s2n data structures (as defined in the header) -------------------*/
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

/*--- external functions used by the harness --------------------------*/
extern int s2n_stuffer_validate(const struct s2n_stuffer *stuffer);
extern int s2n_blob_validate(const struct s2n_blob *blob);
extern int s2n_stuffer_write(struct s2n_stuffer *stuffer,
                             const struct s2n_blob *in);

/*--- bounding constant (provided by the build system) ---------------*/
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024U
#endif

void s2n_stuffer_write_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare and nondeterministically initialise the stuffer */
    struct s2n_stuffer stuffer;
    stuffer.read_cursor     = nondet_uint32_t();
    stuffer.write_cursor    = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced         = nondet_bool();
    stuffer.growable        = nondet_bool();
    stuffer.tainted         = nondet_bool();

    /* initialise the embedded blob */
    stuffer.blob.size      = nondet_uint32_t();
    stuffer.blob.allocated = nondet_uint32_t();
    stuffer.blob.growable  = nondet_bool();

    __CPROVER_assume(stuffer.blob.size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(stuffer.blob.allocated >= stuffer.blob.size);

    if (stuffer.blob.size > 0) {
        stuffer.blob.data = (uint8_t *)aws_mem_acquire(allocator, stuffer.blob.size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* 2. Declare and initialise the input blob (stack‑allocated) */
    struct s2n_blob in;
    in.size      = nondet_uint32_t();
    in.allocated = nondet_uint32_t();
    in.growable  = nondet_bool();

    __CPROVER_assume(in.size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(in.allocated >= in.size);

    if (in.size > 0) {
        in.data = (uint8_t *)aws_mem_acquire(allocator, in.size);
        __CPROVER_assume(in.data != NULL);
    } else {
        in.data = NULL;
    }

    /* 3. Assume the pre‑conditions required by the function */
    __CPROVER_assume(s2n_stuffer_validate(&stuffer));
    __CPROVER_assume(s2n_blob_validate(&in));

    /* 4. Save old state for later comparison */
    struct s2n_stuffer old_stuffer = stuffer;
    struct s2n_blob   old_in       = in;

    /* 5. Call the function under test */
    int result = s2n_stuffer_write(&stuffer, &in);

    /* 6. Post‑condition checks for both success and failure paths */
    if (result == 0) { /* success */
        /* write_cursor must advance by the number of bytes written */
        assert(stuffer.write_cursor == old_stuffer.write_cursor + in.size);

        /* high_water_mark must be the maximum of the old value and the new write_cursor */
        if (stuffer.write_cursor > old_stuffer.high_water_mark) {
            assert(stuffer.high_water_mark == stuffer.write_cursor);
        } else {
            assert(stuffer.high_water_mark == old_stuffer.high_water_mark);
        }

        /* read_cursor is not modified */
        assert(stuffer.read_cursor == old_stuffer.read_cursor);

        /* flags are unchanged */
        assert(stuffer.alloced   == old_stuffer.alloced);
        assert(stuffer.growable  == old_stuffer.growable);
        assert(stuffer.tainted   == old_stuffer.tainted);

        /* the underlying blob of the stuffer is unchanged */
        assert(stuffer.blob.data      == old_stuffer.blob.data);
        assert(stuffer.blob.size      == old_stuffer.blob.size);
        assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable  == old_stuffer.blob.growable);

        /* the input blob is unchanged */
        assert(in.data      == old_in.data);
        assert(in.size      == old_in.size);
        assert(in.allocated == old_in.allocated);
        assert(in.growable  == old_in.growable);
    } else { /* failure */
        /* the whole stuffer must remain unchanged */
        assert(stuffer.read_cursor     == old_stuffer.read_cursor);
        assert(stuffer.write_cursor    == old_stuffer.write_cursor);
        assert(stuffer.high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer.alloced         == old_stuffer.alloced);
        assert(stuffer.growable        == old_stuffer.growable);
        assert(stuffer.tainted         == old_stuffer.tainted);
        assert(stuffer.blob.data       == old_stuffer.blob.data);
        assert(stuffer.blob.size       == old_stuffer.blob.size);
        assert(stuffer.blob.allocated  == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable   == old_stuffer.blob.growable);

        /* the input blob must also remain unchanged */
        assert(in.data      == old_in.data);
        assert(in.size      == old_in.size);
        assert(in.allocated == old_in.allocated);
        assert(in.growable  == old_in.growable);
    }

    /* 7. Invariant: both structures must still be valid */
    assert(s2n_stuffer_validate(&stuffer));
    assert(s2n_blob_validate(&in));
}
