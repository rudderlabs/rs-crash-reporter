//
//  RSCrashReporterClientPayloadInfoTest.m
//  Tests
//
//  Created by Jamie Lynch on 30/04/2020.
//  Copyright © 2020 Bugsnag. All rights reserved.
//

#import <XCTest/XCTest.h>

#import "RSCrashReporterAppWithState+Private.h"
#import "RSCrashReporterConfiguration.h"
#import "RSCrashReporterDeviceWithState+Private.h"
#import "RSCrashReporterTestConstants.h"

@interface RSCrashReporterClientPayloadInfoTest : XCTestCase

@end

@implementation RSCrashReporterClientPayloadInfoTest

- (void)setUp {
    [RSCrashReporter startWithDelegate:nil];
}

- (void)testAppInfo {
    RSCrashReporterClient *client = [RSCrashReporter client];
    client.codeBundleId = @"f00123";
    NSDictionary *app = [[client generateAppWithState:RSCGetSystemInfo()] toDict];
    XCTAssertNotNil(app);
    
    XCTAssertEqualObjects(app[@"codeBundleId"], @"f00123");
    XCTAssertNotNil(app[@"dsymUUIDs"]);
    XCTAssertNotNil(app[@"duration"]);
    XCTAssertNotNil(app[@"durationInForeground"]);
    XCTAssertNotNil(app[@"inForeground"]);
    XCTAssertNotNil(app[@"releaseStage"]);
    XCTAssertNotNil(app[@"type"]);
    
    // Depending on the Info.plist of the unit test runner, these values may not always be present.
    XCTAssertEqualObjects(app[@"bundleVersion"], NSBundle.mainBundle.infoDictionary[@"CFBundleVersion"]);
    XCTAssertEqualObjects(app[@"id"], NSBundle.mainBundle.bundleIdentifier);
    XCTAssertEqualObjects(app[@"version"], NSBundle.mainBundle.infoDictionary[@"CFBundleShortVersionString"]);
}

- (void)testDeviceInfo {
    RSCrashReporterClient *client = [RSCrashReporter client];
    NSDictionary *device = [[client generateDeviceWithState:RSCGetSystemInfo()] toDictionary];
    XCTAssertNotNil(device[@"freeDisk"]);
    XCTAssertNotNil(device[@"freeMemory"]);
//    XCTAssertNotNil(device[@"id"]);
    XCTAssertNotNil(device[@"jailbroken"]);
    XCTAssertNotNil(device[@"locale"]);
    XCTAssertNotNil(device[@"manufacturer"]);
    XCTAssertNotNil(device[@"model"]);
    XCTAssertNotNil(device[@"osName"]);
    XCTAssertNotNil(device[@"osVersion"]);
    XCTAssertNotNil(device[@"runtimeVersions"]);
    XCTAssertNotNil(device[@"time"]);
    XCTAssertNotNil(device[@"totalMemory"]);
    
#if TARGET_OS_IOS
    NSProcessInfo *processInfo = NSProcessInfo.processInfo;
    BOOL isOnMac = [processInfo respondsToSelector:NSSelectorFromString(@"isMacCatalystApp")] &&
                    [[processInfo valueForKey:@"isMacCatalystApp"] boolValue];
    if (!isOnMac) {
        XCTAssertNotNil(device[@"modelNumber"]);
    }
#endif
}

@end
