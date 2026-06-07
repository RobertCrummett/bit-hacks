#show link: (x) => underline(text(x, fill: rgb(20, 0, 220)))

#let code-snippet(content, num) = {
  let s = str(num)
  let pattern = regex(
    "(?s)//\\s*BEGIN:\\s*method" + s + "\\s*(.*?)\\s*//\\s*END:\\s*method" + s
  )
  let match = content.match(pattern)
  if match != none {
    return match.captures.at(0).trim()
  } else {
    return "// SNIPPET - Method `" + s + "` not matched"
  }
}

= Compute the Sign of an Integer
#let code = read("\\src\\compute_the_sign_of_an_integer.c")

#raw(code-snippet(code, 1), lang: "c")

#raw(code-snippet(code, 2), lang: "c")

#raw(code-snippet(code, 3), lang: "c")

= REFERENCES

#link("https://graphics.stanford.edu/~seander/bithacks.html")

