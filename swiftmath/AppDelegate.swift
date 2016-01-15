//
//  AppDelegate.swift
//  swiftmath
//
//  Created by Mateusz Malczak on 13/05/15.
//  Copyright (c) 2015 ThePirateCat. All rights reserved.
//

import UIKit
import QuartzCore
import SFExpression

//typedef sfarg *(*sffptr)(sfarg * const a)

func calc(a: UnsafeMutablePointer<sfarg>, payload: UnsafeMutablePointer<Void>) -> UnsafeMutablePointer<sfarg> {
  let v = a.memory as sfarg
  let n = v.value
  a.memory.value.memory = 10
  return a
}

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {

  var window: UIWindow?

  var _x = 0.0

  var _y = 0.0

  var _f = {
    (x: Double, y: Double) -> Double in
    return y + sin(x)
  }

  func application(application: UIApplication, didFinishLaunchingWithOptions launchOptions: [NSObject:AnyObject]?) -> Bool {

    let ex = SFExpression()
    ex["x"] = 2.0
    ex.parse("2 * sin(x) * cos(x)")
    var r = ex.eval()
    NSLog("f(\(ex["x"])) is \(r)")
    ex["x"] = 4.0
    r = ex.eval()
    NSLog("f(\(ex["x"])) is \(r)")

    let q = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0)
    dispatch_async(q, {
      [unowned self] in
      self.run()
    })


    return true
  }

  func run() {
    var y = Double(4.0)
    let expr = SFExpression()
    expr["x"] = 2.0
    try! expr.bindVar("y", ptr: &y)
    /*

    sfarg *sflogN( sfarg * const p, void *payload ) /* logN */
    {
    sfvalue(p) = log( sfvalue( sfaram1(p) ) )/log( sfvalue( sfaram2(p) ) );
    return sfaram2(p)
    }

    */
    expr.addFunction("magick", params: 2) {
      (p: UnsafeMutablePointer<sfarg>, payload: UnsafeMutablePointer<Void>) -> UnsafeMutablePointer<sfarg> in
      var arg = SFArgument(p)
      let in_y = arg.param1()
      let in_x = arg.param2()
      arg.value = sin(in_x.value) + in_y.value;
      return in_x.memory;
    }

    expr.addFunction("magick_f", params: 2) {
      (p: UnsafeMutablePointer<sfarg>, payload: UnsafeMutablePointer<Void>) -> UnsafeMutablePointer<sfarg> in
      let arg = p.memory;
      let in_y = arg.parg;
      let in_x = in_y.memory.parg;
      arg.value.memory = sin(in_x.memory.value.memory) + in_y.memory.value.memory
      return in_x;
    }

    expr.addFunction("magick2", params: 2, function: sf_magick2)
    expr.parse("2 * magick_f(x;y)")

    var v = expr.eval()
    NSLog("2 * magick(x;y) === 2 * (x + y) === 2 * \(expr["x"]) * \(y) -> \(v)")

    y = 6
    v = expr.eval()
    NSLog("2 * magick(x;y) === 2 * (x * y) === 2 * \(expr["x"]) * \(y) -> \(v)")

    NSLog("Calculate: '%@'", expr.expression)
    var evals = 0
    var sum = 0.0
    var Ts = [CFTimeInterval](count: 100, repeatedValue: 0)
    for (var i = 0; i < 100; i += 1) {
      sum = 0.0
      y = 0.0
      let T = CACurrentMediaTime()
      while (y < 1000000) {
        let vp = expr.eval()
        evals += 1
        sum += vp
        y += 0.4
      }
      Ts[i] = CACurrentMediaTime() - T
    }
    var totalT = Ts.reduce(0, combine: { (a, b) in return a + b })
    var avgT = totalT / Double(Ts.count)
    NSLog("Total time: \(totalT), avg time: \(avgT), evals: \(evals), sum: \(sum)")


    _x = expr["x"]
    NSLog("Calculate: '%@'", expr.expression)
    evals = 0
    sum = 0.0
    Ts = [CFTimeInterval](count: 100, repeatedValue: 0)
    for (var i = 0; i < 100; i += 1) {
      sum = 0.0
      _y = 0.0
      let T = CACurrentMediaTime()
      while (_y < 1000000) {
        let vp = test()
        evals += 1
        sum += vp
        _y += 0.4
      }
      Ts[i] = CACurrentMediaTime() - T
    }
    totalT = Ts.reduce(0, combine: { (a, b) in return a + b })
    avgT = totalT / Double(Ts.count)
    NSLog("Total time: \(totalT), avg time: \(avgT), evals: \(evals), sum: \(sum)")

    //CPU : 11.6913628179973 (ONLY_COUNT), 12.2871189000143 (FULL PTR)
    //iPhone6s : 15.659310750003 (ONLY_COUNT), 15.7049987500013 (FULL PTR)
  }

  func test() -> Double {
    return 2.0 * _f(_x, _y)
  }

}

