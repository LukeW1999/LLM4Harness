#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

/* s2n struct layout (as in the original header) */
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

/* Function under test */
int s2n_stuffer_write_uint32(struct s2n_stuffer *stuffer, const uint32_t u);

/* Helper to bound a size (MAX_BUFFER_SIZE is provided by the build system) */
#ifndef MAX_BUFFER_SIZE
# define MAX_BUFFER_SIZE 1024
#endif

void s2n_stuffer_write_uint32_harness(void) {
    /* 1. Declare and nondeterministically initialise the stuffer */
    struct s2n_stuffer st;

    /* blob fields */
    st.blob.allocated = nondet_uint32_t();
    __CPROVER_assume(st.blob.allocated <= MAX_BUFFER_SIZE);
    st.blob.size = nondet_uint32_t();
    __CPROVER_assume(st.blob.size <= st.blob.allocated);
    st.blob.growable = nondet_bool();

    uint8_t buffer[MAX_BUFFER_SIZE];
    if (st.blob.allocated > 0) {
        st.blob.data = buffer;
    } else {
        st.blob.data = NULL;
    }

    /* cursor fields */
    st.read_cursor  = nondet_uint32_t();
    st.write_cursor = nondet_uint32_t();
    st.high_water_mark = nondet_uint32_t();

    __CPROVER_assume(st.read_cursor <= st.write_cursor);
    __CPROVER_assume(st.write_cursor <= st.blob.size);
    __CPROVER_assume(st.high_water_mark <= st.blob.size);

    /* stuffer flags */
    st.alloced   = nondet_bool();
    st.growable  = nondet_bool();
    st.tainted   = nondet_bool();

    /* 2. Save old state */
    struct s2n_stuffer old = st;

    /* 3. Nondeterministic value to write */
    uint32_t u = nondet_uint32_t();

    /* 4. Call the function under test */
    int result = s2n_stuffer_write_uint32(&st, u);

    /* 5. Post‑condition checks */
    if (result == 0) { /* success */
        /* write cursor must advance by 4 bytes */
        assert(st.write_cursor == old.write_cursor + sizeof(uint32_t));

        /* high water mark must be the maximum of the old high water mark
           and the new write cursor */
        {
            uint32_t expected_hwm = old.high_water_mark;
            uint32_t new_write = old.write_cursor + sizeof(uint32_t);
            if (new_write > expected_hwm) {
                expected_hwm = new_write;
            }
            assert(st.high_water_mark == expected_hwm);
        }

        /* the 4 bytes written must be the network‑order representation of u */
        __CPROVER_assume(old.write_cursor + sizeof(uint32_t) <= old.blob.size);
        {
            uint8_t expected[4];
            expected[0] = (u >> 24) & 0xFF;
            expected[1] = (u >> 16) & 0xFF;
            expected[2] = (u >> 8)  & 0xFF;
            expected[3] =  u        & 0xFF;

            assert(st.blob.data[old.write_cursor]     == expected[0]);
            assert(st.blob.data[old.write_cursor + 1] == expected[1]);
            assert(st.blob.data[old.write_cursor + 2] == expected[2]);
            assert(st.blob.data[old.write_cursor + 3] == expected[3]);
        }
    } else { /* failure */
        /* on failure the whole stuffer must remain unchanged */
        assert(st.read_cursor      == old.read_cursor);
        assert(st.write_cursor     == old.write_cursor);
        assert(st.high_water_mark  == old.high_water_mark);
        assert(st.alloced          == old.alloced);
        assert(st.growable         == old.growable);
        assert(st.tainted          == old.tainted);

        assert(st.blob.data        == old.blob.data);
        assert(st.blob.size        == old.blob.size);
        assert(st.blob.allocated   == old.blob.allocated);
        assert(st.blob.growable    == old.blob.growable);
    }

    /* 6. Fields that must never change (redundant for clarity) */
    assert(st.blob.growable    == old.blob.growable);
    assert(st.blob.allocated   == old.blob.allocated);
    assert(st.blob.size        == old.blob.size);
    assert(st.blob.data        == old.blob.data);
    assert(st.read_cursor      == (result == 0 ? old.read_cursor : old.read_cursor));
    assert(st.alloced          == old.alloced);
    assert(st.growable         == old.growable);
    assert(st.tainted          == old.tainted);

    /* 7. Basic validity invariants that must always hold */
    assert(st.read_cursor  <= st.write_cursor);
    assert(st.write_cursor <= st.blob.size);
    assert(st.high_water_mark <= st.blob.size);
}
