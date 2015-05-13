//
//  SFMath.m
//  swiftmath
//
//  Created by Mateusz Malczak on 13/05/15.
//  Copyright (c) 2015 ThePirateCat. All rights reserved.
//

#import "SFMath.h"
#import "sffe.h"
#import "sffe_real.h"


@interface SFMath ()
{
    sffe *parser;
    sfNumber x;
}

@end

@implementation SFMath

-(instancetype) init
{
    self = [super init];
    if(self)
    {
        parser = NULL;
        [self createParser];
    }
    return self;
}

-(BOOL) parseExpression:(NSString*) expression
{
    [self parse:expression];
    return YES;
}

-(NSString*) expression
{
    const char *expCStr = parser->expression;
    NSString *expression = [NSString stringWithUTF8String:expCStr];
    return expression;
}

-(void) setValue:(double)value forVar:(NSString *)name
{
    const char *varCStr = [name UTF8String];    
    sffe_regvar(&parser, &x, varCStr[0]);
    x = value;
}

-(double) eval
{
    sfNumber value = sffe_eval(parser);
    return (double)value;
}

-(void) parse:(NSString*) expression
{
    const char *expCStr = [expression UTF8String];
    size_t size = strlen(expCStr);
    char *expCpy = malloc(size+1);
    memccpy(expCpy, expCStr, 1, size);
    expCpy[size] = '\0';
    
    sffe_parse(&parser, expCpy);
    
    free(expCpy);
}

-(void) createParser
{
    parser = sffe_alloc();
}

-(void) freeParser
{
    if(parser)
    {
        sffe_free(&parser);
    }
}

-(void)dealloc
{
    [self freeParser];
}

@end
