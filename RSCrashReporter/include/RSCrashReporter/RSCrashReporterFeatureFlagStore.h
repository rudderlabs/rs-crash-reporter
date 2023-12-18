//
//  RSCrashReporterFeatureFlagStore.h
//  RSCrashReporter
//
//  Created by Nick Dowell on 11/11/2021.
//  Copyright © 2021 Bugsnag Inc. All rights reserved.
//

#import <RSCrashReporter/RSCrashReporterFeatureFlag.h>

NS_ASSUME_NONNULL_BEGIN

@protocol RSCrashReporterFeatureFlagStore

- (void)addFeatureFlagWithName:(NSString *)name variant:(nullable NSString *)variant
NS_SWIFT_NAME(addFeatureFlag(name:variant:));

- (void)addFeatureFlagWithName:(NSString *)name
NS_SWIFT_NAME(addFeatureFlag(name:));

- (void)addFeatureFlags:(NSArray<RSCrashReporterFeatureFlag *> *)featureFlags
NS_SWIFT_NAME(addFeatureFlags(_:));

- (void)clearFeatureFlagWithName:(NSString *)name
NS_SWIFT_NAME(clearFeatureFlag(name:));

- (void)clearFeatureFlags;

@end

NS_ASSUME_NONNULL_END
