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

Simple example in Swift

```swift
let ex = SFExpression();
ex["x"] = 2.0;
ex.parse("2 * sin(x) * cos(x)");

var r = ex.eval();
NSLog("f(\(ex["x"])) is \(r)");

ex["x"] = 4.0;
r = ex.eval();
NSLog("f(\(ex["x"])) is \(r)");
```


More complicated example with custom function and variable binding
```swift
var y = Double(4.0);
let expr = SFExpression();
expr["x"] = 2.0;
try! expr.bindVar("y", ptr: &y);

expr.addFunction("magick", params: 2){ (p:UnsafeMutablePointer<sfarg>, payload:UnsafeMutablePointer<Void>) -> UnsafeMutablePointer<sfarg> in
    let param1 = p.memory.parg;
    let param2 = p.memory.parg.memory.parg;
    p.memory.value.memory = param1.memory.value.memory + param2.memory.value.memory;
    return param2;
};
expr.parse("2 * magick(x;y)");

var v = expr.eval();
NSLog("2 * magick(x;y) === 2 * (x + y) === 2 * \(expr["x"]) * \(y) -> \(v)");

y = 6;
v = expr.eval();
NSLog("2 * magick(x;y) === 2 * (x * y) === 2 * \(expr["x"]) * \(y) -> \(v)");
```
