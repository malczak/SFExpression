//: Playground - noun: a place where people can play

import SFExpression

var parser = SFExpression()
do {
  try parser.parse("  7 &amp; 2")
} catch SFExpression.SFError.ParserError(let e) {
  print("Error \(e)")
}

let v = parser.eval()
