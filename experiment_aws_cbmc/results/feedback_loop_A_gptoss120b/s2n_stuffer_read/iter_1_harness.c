#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Definitions from s2n headers (simplified for harness) */
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

/* Function under test and its helper (declared elsewhere) */
int s2n_stuffer_read(struct s2n_stuffer *stuffer, struct s2n_blob *out);
int s2n_stuffer_read_bytes(struct s2n_stuffer *stuffer,
                           uint8_t *dest,
                           uint32_t size);

/* Harness */
void s2n_stuffer_read_harness(void) {
    /* 1. Declare and nondeterministically initialize structures */
    struct s2n_blob out;
    out.size = nondet_uint32_t();
    __CPROVER_assume(out.size <= MAX_BUFFER_SIZE);
    out.allocated = nondet_uint32_t();
    __CPROVER_assume(out.allocated >= out.size);
    out.growable = nondet_bool();

    if (out.size > 0) {
        out.data = malloc(out.size);
        __CPROVER_assume(out.data != NULL);
    } else {
        out.data = NULL;
    }

    struct s2n_stuffer st;
    /* initialize inner blob */
    st.blob.size = nondet_uint32_t();
    __CPROVER_assume(st.blob.size <= MAX_BUFFER_SIZE);
    st.blob.allocated = nondet_uint32_t();
    __CPROVER_assume(st.blob.allocated >= st.blob.size);
    st.blob.growable = nondet_bool();

    if (st.blob.size > 0) {
        st.blob.data = malloc(st.blob.size);
        __CPROVER_assume(st.blob.data != NULL);
    } else {
        st.blob.data = NULL;
    }

    /* cursors */
    st.read_cursor = nondet_uint32_t();
    __CPROVER_assume(st.read_cursor <= st.blob.size);
    st.write_cursor = nondet_uint32_t();
    __CPROVER_assume(st.read_cursor <= st.write_cursor);
    __CPROVER_assume(st.write_cursor <= st.blob.size);
    st.high_water_mark = nondet_uint32_t();
    __CPROVER_assume(st.high_water_mark <= st.blob.size);

    st.alloced = nondet_bool();
    st.growable = nondet_bool();
    st.tainted = nondet_bool();

    /* 2. Save old state */
    struct s2n_stuffer old_st = st;
    struct s2n_blob old_out = out;

    /* 3. Call function under test */
    int result = s2n_stuffer_read(&st, &out);

    /* 4. Postconditions for both success and failure */
    assert(result == 0 || result == -1);

    if (result == 0) {
        /* Success: read_cursor advances by out.size */
        assert(st.read_cursor == old_st.read_cursor + out.size);
    } else {
        /* Failure: stuffer unchanged */
        assert(st.read_cursor == old_st.read_cursor);
        assert(st.write_cursor == old_st.write_cursor);
        assert(st.high_water_mark == old_st.high_water_mark);
        assert(st.alloced == old_st.alloced);
        assert(st.growable == old_st.growable);
        assert(st.tainted == old_st.tainted);
        assert(st.blob.data == old_st.blob.data);
        assert(st.blob.size == old_st.blob.size);
        assert(st.blob.allocated == old_st.blob.allocated);
        assert(st.blob.growable == old_st.blob.growable);
    }

    /* 5. Fields that must not change regardless of result */
    assert(out.data == old_out.data);
    assert(out.size == old_out.size);
    assert(out.allocated == old_out.allocated);
    assert(out.growable == old_out.growable);

    assert(st.blob.data == old_st.blob.data);
    assert(st.blob.size == old_st.blob.size);
    assert(st.blob.allocated == old_st.blob.allocated);
    assert(st.blob.growable == old_st.blob.growable);
    assert(st.write_cursor == old_st.write_cursor);
    assert(st.high_water_mark == old_st.high_water_mark);
    assert(st.alloced == old_st.alloced);
    assert(st.growable == old_st.growable);
    assert(st.tainted == old_st.tainted);

    /* 6. Validity invariants */
    assert(st.read_cursor <= st.write_cursor);
    assert(st.write_cursor <= st.blob.size);
    assert(out.size <= out.allocated);
}
