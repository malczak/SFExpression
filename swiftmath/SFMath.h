//
//  SFMath.h
//  swiftmath
//
//  Created by Mateusz Malczak on 13/05/15.
//  Copyright (c) 2015 ThePirateCat. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SFMath : NSObject

@property (nonatomic, readonly) NSString *expression;

-(instancetype) init;

-(BOOL) parseExpression:(NSString*) expression;

-(void) setValue:(double)value forVar:(NSString *)name;

-(double) eval;

@end
