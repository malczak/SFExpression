//
//  SFExpression.swift
//  swiftmath
//
//  Created by Mateusz Malczak on 11/01/16.
//  Copyright © 2016 ThePirateCat. All rights reserved.
//

import sffe

public func v(a: UnsafeMutablePointer<sfarg>) -> Double {
  return a.memory.value.memory
}

public func p1(p: UnsafeMutablePointer<sfarg>) -> UnsafeMutablePointer<sfarg> {
  return p.memory.parg
}

public func p2(p: UnsafeMutablePointer<sfarg>) -> UnsafeMutablePointer<sfarg> {
  return p1(p).memory.parg
}

public func p3(p: UnsafeMutablePointer<sfarg>) -> UnsafeMutablePointer<sfarg> {
  return p2(p).memory.parg
}

public func p4(p: UnsafeMutablePointer<sfarg>) -> UnsafeMutablePointer<sfarg> {
  return p3(p).memory.parg
}


public struct SFArgument {
  private var ptr:UnsafeMutablePointer<sfarg>;
  
  public var value:Double {
    get {
      return ptr.memory.value.memory
    }
    set {
      ptr.memory.value.memory = newValue
    }
  }
  
  public var memory:UnsafeMutablePointer<sfarg> {
    return ptr
  }
  
  public init(_ ptr:UnsafeMutablePointer<sfarg>) {
    self.ptr = ptr;
  }
  
  public subscript(index:UInt8) -> SFArgument {
    var counter = index
    var newPtr = ptr
    while counter > 0 {
      counter -= 1;
      newPtr = newPtr.memory.parg;
    }
    return SFArgument(newPtr);
  }
  
  public func param1() -> SFArgument {
    return SFArgument(p1(ptr))
  }
  
  public func param2() -> SFArgument {
    return SFArgument(p2(ptr))
  }
  
  public func param3() -> SFArgument {
    return SFArgument(p3(ptr))
  }
  
  public func param4() -> SFArgument {
    return SFArgument(p4(ptr))
  }
  
}


public class SFExpression {  
  public typealias SFFunction = sffptr

  public enum SFError: ErrorType {
    case VariableAlreadyExist
    case ParserError(type: sffe_error)
  }

  private var valid = false

  lazy private var parser: UnsafeMutablePointer<sffe> = sffe_alloc()

  public var expression: String {
    get {
      if (valid) {
        if let s = String.fromCString(parser.memory.expression) {
          return s
        }
      }
      return ""
    }
  }
  
  public init() {
  }

  public subscript(name: String) -> Double {
    get {
      let variable = sffe_regvar(&parser, nil, name)
      return variable.memory.value.memory
    }
    set {
      let variable = sffe_regvar(&parser, nil, name)
      variable.memory.value.memory = newValue
    }
  }

  public func addVar(name: String, value: Double = 0.0) {
    sffe_setvar(&parser, value, name)
  }

  public func addFunction(name: String, params: UInt8, function: SFFunction) {
    sffe_regfunc(&parser, name, UInt32(params), function, nil)
  }

  public func addFunction(name: String, params: UInt8, function: SFFunction, payload: UnsafeMutablePointer<Void>) {
    sffe_regfunc(&parser, name, UInt32(params), function, payload)
  }

  public func bindVar(name: String, ptr: UnsafeMutablePointer<Double>) throws -> UnsafePointer<Double> {
    var variable = sffe_var(parser, name)
    if (variable != nil) {
      throw SFError.VariableAlreadyExist
    }

    variable = sffe_regvar(&parser, ptr, name)
    let pvalue = variable.memory.value
    return UnsafePointer<Double>(pvalue)
  }

  public func parse(let expression: String) throws {
    let utf8 = expression.utf8
    if (utf8.count > 0) {
      let error = sffe_parse(&parser, expression)
      if error != SFFE_ERROR_OK {
        throw SFError.ParserError(type: error)
      }
      valid = true
    }
  }

  public func eval() -> Double {
    if (!valid) {
      return 0.0
    }
    return sffe_eval(parser)
  }

  deinit {
    sffe_free(&parser)
  }
  
}