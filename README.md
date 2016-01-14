# SFExpression
SFExpression allow to parse mathematics strings expression and calculate them in Swift.

Library is a just a wrapper around a sffe math parser created for XaoS fractal zoomer. Because of a number of operations required to draw, zoom and animate complex plane fractals the library had been designed with execution speed in mind. Its quite slow when it comes to expression parsing but is very fast when comes to expression evaluation.

Internally library is transforming expression into a call stack. Expression evalutaion is done insitu even if expression variables are modified.

Main advantages
 * easily extendable - custom variables, custom functions etc.
 * can work in any number space (even algebraic space)
 * fast :)
 
Weak sides
 * complicated syntax of custom functions
 * not thread save

Custom variable accessing example

```swift
let ex = SFExpression()
ex["x"] = 0.0
ex.parse("2 * sin(x) * cos(x)")

ex["x"] = 2.0
var r = ex.eval() // r = -0.756802495307928

ex["x"] = 4.0
r = ex.eval() // r = 0.989358246623382
```

Variable binding is a way faster way to provide variables
```swift
var x = 0.0
let expr = SFExpression()
try! expr.bindVar("x", ptr: &x)
ex.parse("2 * sin(x) * cos(x)")
    
x = 2.0
var r = ex.eval() // r = -0.756802495307928

x = 4.0
r = ex.eval() // r = 0.989358246623382
```

Swift callback can be provided as a custom function
```swift
var x = 1.0, y = 2.0
let expr = SFExpression()
try! expr.bindVar("x", ptr: &x)
try! expr.bindVar("y", ptr: &y)

expr.addFunction("magick", params: 2) {
      (p: UnsafeMutablePointer<sfarg>, payload: UnsafeMutablePointer<Void>) -> UnsafeMutablePointer<sfarg> in
      var arg = SFArgument(p)
      let in_y = arg.param1()
      let in_x = arg.param2()
      arg.value = sin(in_x.value) + in_y.value;
      return in_x.memory;
    }
    
expr.parse("2 * magick_f(x;y)")
expr.eval()
```

Example above uses helper struct ```SFArgument``` to access method arguments. To improve eval time direct access can be used

```swift
var x = 1.0, y = 2.0
let expr = SFExpression()
try! expr.bindVar("x", ptr: &x)
try! expr.bindVar("y", ptr: &y)

expr.addFunction("magick_", params: 2) {
      (p: UnsafeMutablePointer<sfarg>, payload: UnsafeMutablePointer<Void>) -> UnsafeMutablePointer<sfarg> in
      let arg = p.memory;
      let in_y = arg.parg;
      let in_x = in_y.memory.parg;
      arg.value.memory = sin(in_x.memory.value.memory) + in_y.memory.value.memory
      return in_x;
    }
    
expr.parse("2 * magick_f(x;y)")
expr.eval()
```

Custom methods can also be provided as Swift or C functions.
