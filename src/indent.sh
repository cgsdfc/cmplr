#!/bin/bash

# clang-format -i ./*/*.[ch]
# Now use more Kernel-style indent

find . -name "*.[ch]" -exec clang-format -i {} \;
