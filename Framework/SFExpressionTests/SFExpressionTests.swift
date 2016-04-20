//
//  SFExpressionTests.swift
//  SFExpressionTests
//
//  Created by Mateusz Malczak on 19/04/16.
//  Copyright © 2016 The Pirate Cat. All rights reserved.
//

import XCTest
@testable import SFExpression

class SFExpressionTests: XCTestCase {
    
    override func setUp() {
        super.setUp()
        // Put setup code here. This method is called before the invocation of each test method in the class.
    }
    
    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        super.tearDown()
    }
    
    func testExample() {
      let e = SFExpression()
      e.parse("2+2")
      let r = e.eval()
      XCTAssertEqual(r, Double(4.0))
    }
  
    
}
