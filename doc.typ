#show link: (x) => underline(text(x, fill: rgb(20, 0, 220)))

#let snippet(code, method) = {
  let m = str(method)
  let pattern = regex(
    "(?s)//\\s*BEGIN:\\s*method" + m + 
    "\\s*(.*?)\\s*//\\s*END:\\s*method" + m
  )
  let match = code.match(pattern)
  if match != none {
    return raw(match.captures
      .at(0)
      .split("\n")
      .map(line => line.trim())
      .join("\n"), lang: "c")
  } else {
    return raw("// SNIPPET - Method `" + m + "` not matched", lang: "c")
  }
}

= Compute the sign of an integer
#let code = read("sign_of_an_integer.c")

```c
int v;      // we want to find the sign of v
int sign;   // the result goes here
```
#snippet(code, 1)

#snippet(code, 2)

#snippet(code, 3)

The last expression above evalutates to `sign = v >> 31` for 32-bit integers.
This is one operation faster than the obvious way, `sign=-(v < 0)`. This
trick works because when signed integers are shifted right, the value of the
far left bit is copied to the other bits. The far left bit is 1 when the
value is negative and 0 otherwise; all 1 bits gives -1. Unfortunately, this
behavior is architecture specific.

Alternatively, if you prefer the result be either -1 or +1, then use:
```c
sign = +1 | (v >> (sizeof(int) * CHAR_BIT - 1));
```

On the other hand, if you prefer the result be either -1, 0, or +1, then use:
```c
sign = (v != 0) | -(int)((unsigned int)((int)v) >> (sizeof(int) * CHAR_BIT - 1));
// or, for brevity and (perhaps) speed:
sign = (v > 0) - (v < 0);
```

Caveat: On March 7, 2003, Angus Duggan pointed out 1989 ANSI C specification leaves
the result of the signed right-shift implementation-defined, so on some systems this
hack might not work. Angus recommended the casting versions on March 4, 2006.

= Detect if two intergers have opposite signs
#let code = read("two_integers_have_opposite_signs.c")

```c
int x, y;               // input values to compare signs
```
#snippet(code, 1)

= Compute the integer absolute value (abs) without branching
#let code = read("integer_absolute_value_without_branching.c")

```
int v;           // we want to find the absolute value of v
unsigned int r;  // the result goes here
int const mask = v >> sizeof(int) * CHAR_BIT - 1;
```
#snippet(code, 1)

Patented variation:
#snippet(code, 2)

Some CPUs don't have an integer absolute value instruction (or the compiler fails
to use them). On machines where branching is expensive, the above expression can be
daster than the obvious approach, `r = (v < 0) ? -(unsigned)v : v`, even though the
number of operations is the same.

Caveat: On March 7, 2003, Angus Duggan pointed out 1989 ANSI C specification leaves
the result of the signed right-shift implementation-defined, so on some systems this
hack might not work. ANSI C does not require values to be represented as two's
compliment, so it may not work for that reason as well (on a diminishingly small
number of old machines that still use one's compliment). The evaluation of
`-(unsigned)v` in the obvious approach first converts the negative value of v to an
unsigned by adding $2^N$, yielding a two's compliment representation of `v`'s value.
This value is subsequently negated, giving the desired result.

= Compute the minimum or maximum of two integers without branching
#let code = read("minimum_of_two_integers_without_branching.c")

```c
int x;  // we want to find the minimum of x and y
int y;
int r;  // the result goes here
```
#snippet(code, 1)

On rare machines where branching is very expensive and no conditional move
intructions exist, the above expression might be faster than the obvious approach
`r = (x < y) ? x : y`, even though it involves two more instructions. The obvious
approach should usually be best. It works because if `x < y`, then `-(x < y)` is
all ones in binary (negative one in two's compliment). In this case
`r = y ^ ((x ^ y) & -1) = y ^ x ^ y = x`. On the other hand, if `x >= y`, then
`-(x < y)` is all zeros in binary. In this case `r = y ^ ((x ^ y) & 0) = y ^ 0 = y`.
So `-(x < y)` behaves like a switch, turning the xor operation on and off.

Some machines use a branch instruction to compute `(x < y)`, so there would be
no advantage in this case.

To compute the maximum,
```c
r = x ^ ((y ^ x) & -(x < y)); // max(x, y)
```

== Quick and dirty versions

If you know that `INT_MIN <= x - y <= INT_MAX`, then you can use the following,
which are faster because `(x - y)` only needs to be evaluated once.

#snippet(code, 2)
```c
r = x - ((x - y) & ((x - y) >> (sizeof(int) * CHAR_BIT - 1))); // max(x, y)
```

Note that the 1989 ANSI C specification does not specify the result of a signed
right-shift, so these aren't portable. If exceptions are thrown on overflows,
then the values of `x` and `y` should be case to unsigned integers for the
subtractions to avoid unnecessarily throwing an exception, however the right
shift needs a signed operand to produce all one bits when negative, so cast
to signed integer there.

= Determining if an integer is a power of two
#let code = read("integer_power_of_two.c")

```c
unsigned int v;  // we want to see if v is a power of two
bool f;          // the result goes here
```
#snippet(code, 1)

Note 0 is incorrectly considered a power of two here. To remedy this, use:

#snippet(code, 2)


= Reference

#link("https://graphics.stanford.edu/~seander/bithacks.html")
