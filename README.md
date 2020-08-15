# DerpLang
A btyecode based scripting language with a c like syntax and other unique features.

## Current language features
-Global variables
-Local Variables with scoping
-Variables are dynamic
-Functions
-Main function is called first

## Example

```
function main() {
  var foo = "This is a test";
  print foo;
}
```

```
var foo = 8;
function bar()
{
  print foo;
}

function main() {
  call bar;
}
```
