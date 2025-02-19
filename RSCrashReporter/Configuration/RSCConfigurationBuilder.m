#import "RSCConfigurationBuilder.h"

#import "RSCDefines.h"
#import "RSCrashReporterEndpointConfiguration.h"
#import "RSCrashReporterLogger.h"

static id PopValue(NSMutableDictionary *dictionary, NSString *key) {
    id value = dictionary[key];
    dictionary[key] = nil;
    return value;
}

static void LoadBoolean     (RSCrashReporterConfiguration *config, NSMutableDictionary *options, NSString *key);
static void LoadString      (RSCrashReporterConfiguration *config, NSMutableDictionary *options, NSString *key);
static void LoadNumber      (RSCrashReporterConfiguration *config, NSMutableDictionary *options, NSString *key);
static void LoadStringSet   (RSCrashReporterConfiguration *config, NSMutableDictionary *options, NSString *key);
static void LoadEndpoints   (RSCrashReporterConfiguration *config, NSMutableDictionary *options);
static void LoadSendThreads (RSCrashReporterConfiguration *config, NSMutableDictionary *options);

#pragma mark -

RSCrashReporterConfiguration * RSCConfigurationWithOptions(NSDictionary *options) {
    RSCrashReporterConfiguration *config;
    NSMutableDictionary *dict = [options mutableCopy];

    NSString *apiKey = PopValue(dict, RSC_KEYPATH(config, apiKey));
    if (apiKey != nil && ![apiKey isKindOfClass:[NSString class]]) {
        @throw [NSException exceptionWithName:NSInvalidArgumentException reason:@"RSCrashReporter apiKey must be a string" userInfo:nil];
    }

    config = [[RSCrashReporterConfiguration alloc] initWithApiKey:apiKey];

    LoadBoolean     (config, dict, RSC_KEYPATH(config, attemptDeliveryOnCrash));
    LoadBoolean     (config, dict, RSC_KEYPATH(config, autoDetectErrors));
    LoadBoolean     (config, dict, RSC_KEYPATH(config, autoTrackSessions));
    LoadBoolean     (config, dict, RSC_KEYPATH(config, persistUser));
    LoadBoolean     (config, dict, RSC_KEYPATH(config, sendLaunchCrashesSynchronously));
    LoadEndpoints   (config, dict);
    LoadNumber      (config, dict, RSC_KEYPATH(config, launchDurationMillis));
    LoadNumber      (config, dict, RSC_KEYPATH(config, maxBreadcrumbs));
    LoadNumber      (config, dict, RSC_KEYPATH(config, maxPersistedEvents));
    LoadNumber      (config, dict, RSC_KEYPATH(config, maxPersistedSessions));
    LoadNumber      (config, dict, RSC_KEYPATH(config, maxStringValueLength));
    LoadSendThreads (config, dict);
    LoadString      (config, dict, RSC_KEYPATH(config, appType));
    LoadString      (config, dict, RSC_KEYPATH(config, appVersion));
    LoadString      (config, dict, RSC_KEYPATH(config, bundleVersion));
    LoadString      (config, dict, RSC_KEYPATH(config, releaseStage));
    LoadStringSet   (config, dict, RSC_KEYPATH(config, discardClasses));
    LoadStringSet   (config, dict, RSC_KEYPATH(config, enabledReleaseStages));
    LoadStringSet   (config, dict, RSC_KEYPATH(config, redactedKeys));

#if RSC_HAVE_APP_HANG_DETECTION
    LoadBoolean     (config, dict, RSC_KEYPATH(config, reportBackgroundAppHangs));
    LoadNumber      (config, dict, RSC_KEYPATH(config, appHangThresholdMillis));
#endif

    if (dict.count > 0) {
        rsc_log_warn(@"Ignoring unexpected Info.plist values: %@", dict);
    }

    return config;
}

static void LoadBoolean(RSCrashReporterConfiguration *config, NSMutableDictionary *options, NSString *key) {
    id value = PopValue(options, key);
    if (value && CFGetTypeID((__bridge CFTypeRef)value) == CFBooleanGetTypeID()) {
        [config setValue:value forKey:key];
    }
}

static void LoadString(RSCrashReporterConfiguration *config, NSMutableDictionary *options, NSString *key) {
    id value = PopValue(options, key);
    if ([value isKindOfClass:[NSString class]]) {
        [config setValue:value forKey:key];
    }
}

static void LoadNumber(RSCrashReporterConfiguration *config, NSMutableDictionary *options, NSString *key) {
    id value = PopValue(options, key);
    if ([value isKindOfClass:[NSNumber class]]) {
        [config setValue:value forKey:key];
    }
}

static void LoadStringSet(RSCrashReporterConfiguration *config, NSMutableDictionary *options, NSString *key) {
    id val = PopValue(options, key);
    if ([val isKindOfClass:[NSArray class]]) {
        for (NSString *obj in val) {
            if (![obj isKindOfClass:[NSString class]]) {
                return;
            }
        }
        [config setValue:[NSSet setWithArray:val] forKey:key];
    }
}

static void LoadEndpoints(RSCrashReporterConfiguration *config, NSMutableDictionary *options) {
    NSDictionary *endpoints = PopValue(options, RSC_KEYPATH(config, endpoints));
    if ([endpoints isKindOfClass:[NSDictionary class]]) {
        NSString *notify = endpoints[RSC_KEYPATH(config.endpoints, notify)];
        if ([notify isKindOfClass:[NSString class]]) {
            config.endpoints.notify = notify;
        }
        NSString *sessions = endpoints[RSC_KEYPATH(config.endpoints, sessions)];
        if ([sessions isKindOfClass:[NSString class]]) {
            config.endpoints.sessions = sessions;
        }
    }
}

static void LoadSendThreads(RSCrashReporterConfiguration *config, NSMutableDictionary *options) {
#if RSC_HAVE_MACH_THREADS
    NSString *sendThreads = [PopValue(options, RSC_KEYPATH(config, sendThreads)) lowercaseString];
    if ([sendThreads isKindOfClass:[NSString class]]) {
        if ([@"unhandledonly" isEqualToString:sendThreads]) {
            config.sendThreads = RSCThreadSendPolicyUnhandledOnly;
        } else if ([@"always" isEqualToString:sendThreads]) {
            config.sendThreads = RSCThreadSendPolicyAlways;
        } else if ([@"never" isEqualToString:sendThreads]) {
            config.sendThreads = RSCThreadSendPolicyNever;
        }
    }
#endif
}
