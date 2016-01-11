//
//  AppDelegate.swift
//  swiftmath
//
//  Created by Mateusz Malczak on 13/05/15.
//  Copyright (c) 2015 ThePirateCat. All rights reserved.
//

import UIKit
import QuartzCore

//typedef sfarg *(*sffptr)(sfarg * const a);

func calc(a:UnsafeMutablePointer<sfarg>, payload:UnsafeMutablePointer<Void>) -> UnsafeMutablePointer<sfarg>
{
    let v = a.memory as sfarg;
    let n = v.value;
    a.memory.value.memory = 10;
    return a;
}

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {

    var window: UIWindow?


    func application(application: UIApplication, didFinishLaunchingWithOptions launchOptions: [NSObject: AnyObject]?) -> Bool {
        
        let ex = SFExpression();
        ex["x"] = 2.0;
        ex.parse("2 * sin(x) * cos(x)");
        var r = ex.eval();
        NSLog("f(\(ex["x"])) is \(r)");
        ex["x"] = 4.0;
        r = ex.eval();
        NSLog("f(\(ex["x"])) is \(r)");
        
        var y = Double(4.0);
        let expr = SFExpression();
        expr["x"] = 2.0;
        try! expr.bindVar("y", ptr: &y);
        /*
        
        sfarg *sflogN( sfarg * const p, void *payload ) /* logN */
        {
            sfvalue(p) = log( sfvalue( sfaram1(p) ) )/log( sfvalue( sfaram2(p) ) );;
            return sfaram2(p);
        };
        
        */
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
        
        NSLog("Calculate: '%@'", expr.expression);
        var evals = 0;
        var sum = 0.0;
        var Ts = [CFTimeInterval](count: 100, repeatedValue: 0);
        for(var i=0; i<100; i+=1)
        {
            sum = 0.0;
            y = 0.0;
            let T = CACurrentMediaTime();
            while( y < 1000000 )
            {
                let vp = expr.eval();
                evals += 1;
                sum += vp;
                y += 0.4;
            }
            Ts[i] = CACurrentMediaTime() - T;
        }
        let totalT = Ts.reduce(0,combine: { (a,b) in return a + b});
        let avgT =  totalT / Double(Ts.count);
        NSLog ("Total time: \(totalT), avg time: \(avgT), evals: \(evals), sum: \(sum)");
        NSLog ("Times \(Ts)");
        
        //CPU : 11.6913628179973 (ONLY_COUNT), 12.2871189000143 (FULL PTR)
        //iPhone6s : 11.6913628179973 (ONLY_COUNT), 12.2871189000143 (FULL PTR)

        return true
    }
    
}

