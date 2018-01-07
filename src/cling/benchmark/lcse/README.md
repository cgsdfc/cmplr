# This directory benchmarks the lcse optimizer
## About
Lcse or Local Common Subexpression Elimination is a local optimization that delete duplicate
expression. It operates on every basic block of a function and generates reduced IR. In this
file we will talk about the implementation of lcse in cling and introduce certain benchmarks.

## Implementaion
Implemented as `cling/lcse.c`, the optimizer takes exactly one basic block as input and scans through its code to output
the same basic block with some of its IRs deleted and some of its temps renamed. Since the deletion,
jump targets of IRs in the original basic block will be fixed. This means the optimization is done
_in place_ with _one linear pass_.

The most important aspect of this optimization is **how duplications are discovered?** Since what is going to be done is
performed in _one basic block_, knowledge about the history of execution of instructions within the same block is sufficient.
There is no need for data flow analysis. Based on this setting, we make rules for each instruction about how its operands can
produce the same result. For example, we know that if `a` and `b` hold the same value everytime, than `a + b` holds the same
result everytime. Therefore, if only we can track the value of every variable and every instruction that does "real work" or
outputs result that is _not_ duplicate with previous instructions, we can eliminate the duplicate ones. In addition, since
we can store value to an explicit address or a named address, we must have the ability to track the content of each appearing
address, named or not so that duplicate load and store can also be eliminated. Consider the following example:
```c
int A, B;
A=1;
A=1;
B=A;
B=A;
```
which will produce the following IRs:
```
ldimm t0 1
stnam A t0
ldimm t1 1
stnam A t1
ldnam t2 A
stnam B t2
ldnam t3 A
stnam B t3
```
the second `A=1;` and `B=A;` is obviously duplicate, but how can we know that? Here we divide the whole process as two part,
one for eliminating the extra load and one for the extra store.

### Deletion of ldimm
`ldimm` loads an integer into a temp and that integer is tracked for duplication. So after the first `ldimm t0 1`, we attach `t0`
to value `1` and when `ldimm t2 1` appears, we just discover that value `1` has been loaded into `t0` so we simply bind `t2` to
`t0` and delete this load. Since these are all static assignment forms, the value of `t0` will not change.

### Deletion of ldnam
`ldnam` is a little more complicated since it uses two mappings: one from name to address and the other from address to value. We
are not mapping name to value directly because the address to value mapping is meaningful for `stnam` and `stadr`. Now with these
two mappings we can track the address of name and the changes of these value. When the first `stnam A t0` appears, we first give `A`
an address since non of the previous instructions mentioned the address of `A`. Then value `t0` is attached to that address. When the
second `ldnam t2 A` comes up, we know that the value of `A` is not changed since the second `stnam A t1` is duplicate (see below) and
the same value is not going to be loaded twice so `t2` is binded to the value of `A` and the load was deleted.

### Deletion of stnam
Based on the principle that storing the same value to a same address twice is a duplication, the second store `stnam A t1` and `stnam B t3`
were deleted. However, if the store does store a new value to an address, the address-value mapping will be updated to reflect this.

With this example in mind, the overall process which can be expressed as the following python code:
```python
    
def lcse_optimize(basic_block):
  output=[]
  operations={}
  for ir in basic_block:
  lcse_ir = translate(ir)
  if lcse_ir in operations:
    bind_variable(ir)
    else:
  operations.insert(lcse_ir)
  output.append(ir)
return output
```


