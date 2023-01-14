**Added:**

* Added Precision and Rounding contexts to make normal arithmetic available in a convenient way in the C++ interface.

**Removed:**

* Removed yap expression building. In practice this turned out not to be terribly useful. If somebody wants to micro-optimize arithmetic, they are going to use the C API directly anyway.

**Performance:**

* Improved performance of operations that suffer a lot from an extra call-overhead. In principle LTO should be able to do much of this inlining at link-time. However, when building a shared library this does not work reliably, not even for calls inside the shared library even if they live in the same object file (with GCC.) I did dig into the source of GCC and ld to understand why this happens but unfortunately I forget the details now. Maybe this gets fixed some day but currently, no mix of `-fno-semantic-interposition`, whole program optimization, inline attributes and keywords, makes this work with GCC without moving code to the header files (and that's what we do here.)
