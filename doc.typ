#show link: (x) => underline(text(x, fill: rgb(20, 0, 220)))

#let code-snippet(content, num) = {
  let s = str(num)
  let pattern = regex(
    "(?s)//\\s*BEGIN:\\s*method" + s + 
    "\\s*(.*?)\\s*//\\s*END:\\s*method" + s
  )
  let match = content.match(pattern)
  if match != none {
    return match.captures
      .at(0)
      .split("\n")
      .map(line => line.trim())
      .join("\n")
  } else {
    return "// SNIPPET - Method `" + s + "` not matched"
  }
}

= Compute the sign of an integer
#let code = read("compute_the_sign_of_an_integer.c")

```c
int v;      // we want to find the sign of v
int sign;   // the result goes here
```
#raw(code-snippet(code, 1), lang: "c")

#raw(code-snippet(code, 2), lang: "c")

#raw(code-snippet(code, 3), lang: "c")

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
#raw(code-snippet(code, 1), lang: "c")

= Compute the integer absolute value (abs) without branching
#let code = read("integer_absolute_value_without_branching.c")

```
int v;           // we want to find the absolute value of v
unsigned int r;  // the result goes here
int const mask = v >> sizeof(int) * CHAR_BIT - 1;
```
#raw(code-snippet(code, 1), lang: "c")

Patented variation:
#raw(code-snippet(code, 2), lang: "c")

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

= Reference

#link("https://graphics.stanford.edu/~seander/bithacks.html")
