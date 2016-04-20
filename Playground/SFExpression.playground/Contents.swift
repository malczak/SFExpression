//: Playground - noun: a place where people can play

import SFExpression

var parser = SFExpression()
do {
  try parser.parse(" sin(2)(-1+2)")
} catch SFExpression.SFError.ParserError(let e) {
  print("Error \(e)")
}

let v = parser.eval()
let expected = 2.727892280477045
v == expected