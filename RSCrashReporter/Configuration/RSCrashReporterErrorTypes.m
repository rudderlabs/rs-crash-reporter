//
//  RSCrashReporterErrorTypes.m
//  RSCrashReporter
//
//  Created by Jamie Lynch on 22/04/2020.
//  Copyright © 2020 Bugsnag. All rights reserved.
//

#import "RSCrashReporterErrorTypes.h"

@implementation RSCrashReporterErrorTypes

- (instancetype)init {
    if ((self = [super init])) {
        _appHangs = YES;
        _unhandledExceptions = YES;
        _signals = YES;
        _cppExceptions = YES;
        _machExceptions = YES;
        _unhandledRejections = YES;
        _ooms = YES;
        _thermalKills = YES;
    }
    return self;
}

@end
