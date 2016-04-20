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
    do {
      try parser.parse("(((4)))")
    } catch SFExpression.SFError.ParserError(let e) {
      print("Error \(e)")
    } catch {
//      ??
    }
    
    _ = parser.eval()

    
    // Do any additional setup after loading the view, typically from a nib.
  }

  override func didReceiveMemoryWarning() {
    super.didReceiveMemoryWarning()
    // Dispose of any resources that can be recreated.
  }


}

