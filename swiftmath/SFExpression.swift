//
//  SFExpression.swift
//  swiftmath
//
//  Created by Mateusz Malczak on 11/01/16.
//  Copyright © 2016 ThePirateCat. All rights reserved.
//

import Foundation


func fzval(a: UnsafeMutablePointer<sfarg>) -> Double {
  return a.memory.value.memory
}

func fparam1(p: UnsafeMutablePointer<sfarg>) -> UnsafeMutablePointer<sfarg> {
  return p.memory.parg
}

func fparam2(p: UnsafeMutablePointer<sfarg>) -> UnsafeMutablePointer<sfarg> {
  return fparam1(p).memory.parg
}

func fparam3(p: UnsafeMutablePointer<sfarg>) -> UnsafeMutablePointer<sfarg> {
  return fparam2(p).memory.parg
}

func fparam4(p: UnsafeMutablePointer<sfarg>) -> UnsafeMutablePointer<sfarg> {
  return fparam3(p).memory.parg
}


struct SFArgument {
  private var ptr:UnsafeMutablePointer<sfarg>;
  
  var value:Double {
    get {
      return ptr.memory.value.memory
    }
    set {
      ptr.memory.value.memory = newValue
    }
  }
  
  var memory:UnsafeMutablePointer<sfarg> {
    return ptr
  }
  
  init(_ ptr:UnsafeMutablePointer<sfarg>) {
    self.ptr = ptr;
  }
  
  subscript(index:UInt8) -> UnsafeMutablePointer<sfarg> {
    var counter = index
    var newPtr = ptr
    while counter > 0 {
      counter -= 1;
      newPtr = newPtr.memory.parg;
    }
    return newPtr;
  }
  
  func param1() -> UnsafeMutablePointer<sfarg> {
    return ptr.memory.parg
  }
  
  func param2() -> UnsafeMutablePointer<sfarg> {
    return param1().memory.parg
  }
  
  func param3() -> UnsafeMutablePointer<sfarg> {
    return param2().memory.parg
  }
  
  func param4() -> UnsafeMutablePointer<sfarg> {
    return param3().memory.parg
  }
  
}


class SFExpression {
  
  typealias SFFunction = sffptr

  enum SFError: ErrorType {
    case VariableAlreadyExist
  }

  private var valid = false

  lazy private var parser: UnsafeMutablePointer<sffe> = sffe_alloc()

  var expression: String {
    get {
      if (valid) {
        if let s = String.fromCString(parser.memory.expression) {
          return s
        }
      }
      return ""
    }
  }

  subscript(name: String) -> Double {
    get {
      let variable = sffe_regvar(&parser, nil, name)
      return variable.memory.value.memory
    }
    set {
      let variable = sffe_regvar(&parser, nil, name)
      variable.memory.value.memory = newValue
    }
  }

  func addVar(name: String, value: Double = 0.0) {
    sffe_setvar(&parser, value, name)
  }

  func addFunction(name: String, params: UInt8, function: SFFunction) {
    sffe_regfunc(&parser, name, UInt32(params), function, nil)
  }

  func addFunction(name: String, params: UInt8, function: SFFunction, payload: UnsafeMutablePointer<Void>) {
    sffe_regfunc(&parser, name, UInt32(params), function, payload)
  }

  func bindVar(name: String, ptr: UnsafeMutablePointer<Double>) throws -> UnsafePointer<Double> {
    var variable = sffe_var(parser, name)
    if (variable != nil) {
      throw SFError.VariableAlreadyExist
    }

    variable = sffe_regvar(&parser, ptr, name)
    let pvalue = variable.memory.value
    return UnsafePointer<Double>(pvalue)
  }

  func parse(let expression: String) {
    let utf8 = expression.utf8
    if (utf8.count > 0) {
      sffe_parse(&parser, expression)
      valid = true
    }
  }

  func eval() -> Double {
    if (!valid) {
      return 0.0
    }
    return sffe_eval(parser)
  }

  deinit {
    sffe_free(&parser)
  }
  
}