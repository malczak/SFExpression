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

func calc(a:UnsafeMutablePointer<sfarg>) -> UnsafeMutablePointer<sfarg>
{
    return a;
}

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {

    var window: UIWindow?


    func application(application: UIApplication, didFinishLaunchingWithOptions launchOptions: [NSObject: AnyObject]?) -> Bool {
        let math = SFMath();
        let sf = sfarg();
//        sffptr
        math.setValue(1.2, forVar: "mati");
//        let s:sffptr = sffptr(&calc);
//        math.addFunction(&calc)
        var vptr:UnsafeMutablePointer<Double> = math.getPointerForVar("mati");
//        math.addFunction(nil);
//        math.parseExpression("2 * test()");
//        let v = math.eval(); //0.686457546999009
//        NSLog("Output \(v)");
//        return true;

        math.parseExpression("mati * 2 + 2/mati - sin(mati*pi)cos(mati*pi)");
        NSLog("Calculate: '%@'", math.expression);
        var delta = 0.0;
        var avgT = 0.0;
        var Ts = [CFTimeInterval](count: 100, repeatedValue: 0);
        for(var i=0; i<100; i+=1)
        {
            vptr.memory = 0.0;
            var T = CACurrentMediaTime();
            while( vptr.memory < 1000000 )
            {
//                            let vn = c(vptr.memory); //0.250615886759806
                let vp = math.eval(); //0.653897768329989 (ONLY_COUNT), 0.727334245540042 (FULL PTR)
                //            delta = (vp-vn);
                //            NSLog("Value for %f is %f", vptr.memory, v);
                //            math.setValue(vptr.memory, forVar: "mati");
                vptr.memory += 0.4;
            }
            Ts[i] = CACurrentMediaTime() - T;
        }
        var a2 = (Ts.reduce(0,combine: { (a,b) in return a + b})) / Double(Ts.count);
        NSLog ("Time: \(a2) delta: \(delta)");
        return true
    }
    
    func c(x:Double) -> Double
    {
        return x * 2 + 2/x - sin(x*M_PI) * cos(x*M_PI);
    }

    func applicationWillResignActive(application: UIApplication) {
        // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
        // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    }

    func applicationDidEnterBackground(application: UIApplication) {
        // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
        // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    }

    func applicationWillEnterForeground(application: UIApplication) {
        // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
    }

    func applicationDidBecomeActive(application: UIApplication) {
        // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    }

    func applicationWillTerminate(application: UIApplication) {
        // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    }


}

