//
//  SFExpression.swift
//  swiftmath
//
//  Created by Mateusz Malczak on 11/01/16.
//  Copyright © 2016 ThePirateCat. All rights reserved.
//

import Foundation

class SFExpression
{
    enum SFError : ErrorType
    {
        case VariableAlreadyExist;
    }
    
    typealias SFFunction = sffptr;
    
    private var valid = false;
    
    lazy private var parser:UnsafeMutablePointer<sffe> = sffe_alloc();
    
    var expression : String {
        get {
            if(valid)
            {
                if let s = String.fromCString(parser.memory.expression) {
                    return s;
                }                
            }
            return "";
        }
    }
    
    subscript(name: String) -> Double
    {
        get {
            let variable = sffe_regvar(&parser, nil, name);
            return variable.memory.value.memory;
        }
        set {
            let variable = sffe_regvar(&parser, nil, name);
            variable.memory.value.memory = newValue;
        }
    }
        
    func addVar(name:String, value:Double = 0.0)
    {
        sffe_setvar(&parser, value, name);
    }

    func addFunction(name:String, params:UInt8, function:SFFunction)
    {
        sffe_regfunc(&parser, name, UInt32(params), function, nil);
    }
    
    func addFunction(name:String, params:UInt8, function:SFFunction, payload:UnsafeMutablePointer<Void>)
    {
        sffe_regfunc(&parser, name, UInt32(params), function, payload);
    }
    
    func bindVar(name:String, ptr:UnsafeMutablePointer<Double>) throws -> UnsafePointer<Double>
    {
        var variable = sffe_var(parser, name);
        if(variable != nil)
        {
            throw SFError.VariableAlreadyExist;
        }
        
        variable = sffe_regvar(&parser, ptr, name);
        let pvalue = variable.memory.value;
        return UnsafePointer<Double>(pvalue);
    }
    
    func parse(let expression:String)
    {
        let utf8 = expression.utf8;
        if(utf8.count > 0)
        {
            sffe_parse(&parser, expression);
            valid = true;
        }
    }
    
    func eval() -> Double
    {
        if(!valid)
        {
            return 0.0;
        }
        return sffe_eval(parser);
    }
    
    deinit{
        sffe_free(&parser);
    }
}