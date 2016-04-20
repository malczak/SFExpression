//
//  ViewController.swift
//  SimpleCalculator
//
//  Created by Mateusz Malczak on 20/04/16.
//  Copyright © 2016 The Pirate Cat. All rights reserved.
//

import UIKit
import SFExpression

class ViewController: UIViewController {

  override func viewDidLoad() {
    super.viewDidLoad()
    
    let parser = SFExpression()
    var x = 2.0;
    do {
      try parser.bindVar("x", ptr: &x)
      try parser.parse("-PI/-x") // compiled to -PI/-1 * x :/
    } catch SFExpression.SFError.ParserError(let e) {
      print("Error \(e)")
    } catch {
//      ??
    }
    
    let e = -M_PI / -2.0
    let v = parser.eval()
    print("Value is \(v) =?= \(e)");

    
    // Do any additional setup after loading the view, typically from a nib.
  }

  override func didReceiveMemoryWarning() {
    super.didReceiveMemoryWarning()
    // Dispose of any resources that can be recreated.
  }


}

