//
//  RSCrashReporterConfiguration.h
//
//  Created by Conrad Irwin on 2014-10-01.
//
//  Copyright (c) 2014 Bugsnag, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall remain in place
// in this source code.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#import <Foundation/Foundation.h>

#import <RSCrashReporter/RSC_KSCrashReportWriter.h>
#import <RSCrashReporter/RSCrashReporterBreadcrumb.h>
#import <RSCrashReporter/RSCrashReporterDefines.h>
#import <RSCrashReporter/RSCrashReporterEvent.h>
#import <RSCrashReporter/RSCrashReporterFeatureFlagStore.h>
#import <RSCrashReporter/RSCrashReporterMetadata.h>
#import <RSCrashReporter/RSCrashReporterMetadataStore.h>
#import <RSCrashReporter/RSCrashReporterPlugin.h>

@class RSCrashReporterUser;
@class RSCrashReporterEndpointConfiguration;
@class RSCrashReporterErrorTypes;

NS_ASSUME_NONNULL_BEGIN

/**
 * Controls whether RSCrashReporter should capture and serialize the state of all threads at the time
 * of an error.
 */
typedef NS_ENUM(NSInteger, RSCThreadSendPolicy) {

    /**
     * Threads should be captured for all events.
     */
    RSCThreadSendPolicyAlways = 0,

    /**
     * Threads should be captured for unhandled events only.
     */
    RSCThreadSendPolicyUnhandledOnly = 1,

    /**
     * Threads should never be captured.
     */
    RSCThreadSendPolicyNever = 2
};

/**
 * Types of telemetry that may be sent to RSCrashReporter for product improvement purposes.
 */
typedef NS_OPTIONS(NSUInteger, RSCTelemetryOptions) {

    /**
     * Errors within the RSCrashReporter SDK.
     */
    RSCTelemetryInternalErrors = (1UL << 0),

    /**
     * Information about how RSCrashReporter has been configured.
     */
    RSCTelemetryUsage = (1UL << 1),

    /**
     * All types of telemetry are enabled by default.
     */
    RSCTelemetryAll = (RSCTelemetryInternalErrors | RSCTelemetryUsage)
};

/**
 * Setting `RSCrashReporterConfiguration.appHangThresholdMillis` to this value disables the reporting of
 * app hangs that ended before the app was terminated.
 */
RSCRASHREPORTER_EXTERN const NSUInteger RSCrashReporterAppHangThresholdFatalOnly API_UNAVAILABLE(watchos);

/**
 *  A configuration block for modifying an error report
 *
 *  @param event the error report to be modified
 */
typedef BOOL (^RSCrashReporterOnErrorBlock)(RSCrashReporterEvent *_Nonnull event);

/**
 *  A handler for modifying data before sending it to RSCrashReporter.
 *
 * onSendErrorBlocks will be invoked on a dedicated
 * background queue, which will be different from the queue where the block was originally added.
 *
 *  @param event The event report.
 *
 *  @return YES if the event should be sent
 */
typedef BOOL (^RSCrashReporterOnSendErrorBlock)(RSCrashReporterEvent *_Nonnull event);

/**
 * An opaque object that identifies and allows the removal of a RSCrashReporterOnSendErrorBlock.
 */
typedef id<NSObject> RSCrashReporterOnSendErrorRef;

/**
 *  A configuration block for modifying a captured breadcrumb
 *
 *  @param breadcrumb The breadcrumb
 */
typedef BOOL (^RSCrashReporterOnBreadcrumbBlock)(RSCrashReporterBreadcrumb *_Nonnull breadcrumb);

/**
 * An opaque object that identifies and allows the removal of a RSCrashReporterOnBreadcrumbBlock.
 */
typedef id<NSObject> RSCrashReporterOnBreadcrumbRef;

/**
 * A configuration block for modifying a session. Intended for internal usage only.
 *
 * @param session The session about to be delivered
 */
typedef BOOL (^RSCrashReporterOnSessionBlock)(RSCrashReporterSession *_Nonnull session);

/**
 * An opaque object that identifies and allows the removal of a RSCrashReporterOnSessionBlock.
 */
typedef id<NSObject> RSCrashReporterOnSessionRef;

// =============================================================================
// MARK: - RSCrashReporterConfiguration
// =============================================================================

/**
 * Contains user-provided configuration, including API key and endpoints.
 */
RSCRASHREPORTER_EXTERN
@interface RSCrashReporterConfiguration : NSObject <RSCrashReporterFeatureFlagStore, RSCrashReporterMetadataStore>

/**
 * Create a new configuration from the main bundle's infoDictionary, using keys nested under
 * the "bugsnag" key.
 *
 * @return a RSCrashReporterConfiguration containing the options set in the plist file
 */
+ (instancetype)loadConfig;

/**
 * Initializes a new configuration object with the provided API key.
 */
- (instancetype)initWithApiKey:(nullable NSString *)apiKey NS_DESIGNATED_INITIALIZER NS_SWIFT_NAME(init(_:));

/**
 * Required declaration to suppress a superclass designated-initializer error
 */
- (instancetype)init NS_UNAVAILABLE NS_SWIFT_UNAVAILABLE("Use initWithApiKey:");

// -----------------------------------------------------------------------------
// MARK: - Properties
// -----------------------------------------------------------------------------

/**
 *  The API key of a RSCrashReporter project
 */
@property (copy, nonatomic) NSString *apiKey;

/**
 *  The release stage of the application, such as production, development, beta
 *  et cetera
 */
@property (copy, nullable, nonatomic) NSString *releaseStage;

/**
 *  Release stages which are allowed to notify RSCrashReporter
 */
@property (copy, nullable, nonatomic) NSSet<NSString *> *enabledReleaseStages;

/**
 * Sets which values should be removed from any Metadata objects before
 * sending them to RSCrashReporter. Use this if you want to ensure you don't send
 * sensitive data such as passwords, and credit card numbers to our
 * servers. Any keys which contain a match will be filtered.
 *
 * By default, redactedKeys is set to ["password"]. Both string literals and regex
 * values can be supplied to this property.
 */
@property (copy, nullable, nonatomic) NSSet<id> *redactedKeys;

/**
 * A set of strings and / or NSRegularExpression objects that determine which errors should
 * be discarded based on their `errorClass`.
 *
 * Comparisons are case sensitive.
 *
 * OnError / OnSendError blocks will not be called for discarded errors.
 *
 * Some examples of errorClass are: Objective-C exception names like "NSRangeException",
 * signal names like "SIGABRT", mach exception names like "EXC_BREAKPOINT", and Swift
 * error names like "Fatal error".
 */
@property (copy, nullable, nonatomic) NSSet<id> *discardClasses;

/**
 *  A general summary of what was occuring in the application
 */
@property (copy, nullable, atomic) NSString *context;

/**
 *  The version of the application
 */
@property (copy, nullable, nonatomic) NSString *appVersion;

/**
 *  The URL session used to send requests to RSCrashReporter.
 */
@property (readwrite, strong, nonnull, nonatomic) NSURLSession *session;

/**
 * Controls whether RSCrashReporter should capture and serialize the state of all threads at the time
 * of an error.
 *
 * By default sendThreads is set to RSCThreadSendPolicyAlways. This can be set to
 * RSCThreadSendPolicyNever to disable or RSCThreadSendPolicyUnhandledOnly
 * to only do so for unhandled errors.
 */
@property (nonatomic) RSCThreadSendPolicy sendThreads API_UNAVAILABLE(watchos);

/**
 *  Optional handler invoked when an error or crash occurs
 */
@property (nullable, nonatomic) void (* onCrashHandler)(const RSC_KSCrashReportWriter *);

/**
 *  YES if uncaught exceptions and other crashes should be reported automatically
 */
@property (nonatomic) BOOL autoDetectErrors;

/**
 * The minimum number of milliseconds of main thread unresponsiveness that will trigger the
 * detection and reporting of an app hang.
 *
 * Set to `RSCrashReporterAppHangThresholdFatalOnly` to disable reporting of app hangs that did not
 * end with the app being force quit by the user or terminated by the system watchdog.
 *
 * By default this is `RSCrashReporterAppHangThresholdFatalOnly`, and can be set to a minimum of 250
 * milliseconds.
 */
@property (nonatomic) NSUInteger appHangThresholdMillis API_UNAVAILABLE(watchos);

/**
 * Whether RSCrashReporter should report app hangs that occur while the app is in the background.
 *
 * By default this is false.
 */
@property (nonatomic) BOOL reportBackgroundAppHangs API_UNAVAILABLE(watchos);

/**
 * Determines whether app sessions should be tracked automatically. By default this value is true.
 * If this value is updated after +[RSCrashReporter start] is called, only subsequent automatic sessions
 * will be captured.
 *
 * Note: automatic session tracking is not available in App Extensions.
 */
@property (nonatomic) BOOL autoTrackSessions;

/**
 * The amount of time (in milliseconds) after starting RSCrashReporter that should be considered part of
 * the app's launch.
 *
 * Events that occur during app launch will have the `RSCrashReporterAppWithState.isLaunching` property
 * set to true.
 *
 * By default this value is 5000 milliseconds.
 *
 * Setting this to `0` will cause RSCrashReporter to consider the app to be launching until
 * `+[RSCrashReporter markLaunchCompleted]` or `-[RSCrashReporterClient markLaunchCompleted]` has been called.
 */
@property (nonatomic) NSUInteger launchDurationMillis;

/**
 * Determines whether launch crashes should be sent synchronously during `+[RSCrashReporter start]`.
 *
 * If true and the previous run terminated due to a crash during app launch, `+[RSCrashReporter start]`
 * will block the calling thread for up to 2 seconds while the crash report is sent.
 *
 * By default this value is true.
 */
@property (nonatomic) BOOL sendLaunchCrashesSynchronously;

/**
 * Whether RSCrashReporter should try to send crashing errors prior to app termination.
 *
 * Delivery will only be attempted for uncaught Objective-C exceptions and Mach
 * exceptions, and while in progress will block the crashing thread for up to 3
 * seconds.
 *
 * Delivery will be unreliable due to the necessary short timeout and potential
 * memory corruption that caused the crashing error in the first place.
 *
 * If it fails prior to termination, delivery will be reattempted at next launch
 * (the default behavior). 
 *
 * Use of this feature is discouraged because it:
 * - may cause the app to hang while delivery occurs and impact the hang rate
 *   reported in Xcode Organizer
 * - will result in duplicate crashes in your dashboard for crashes that were
 *   fully sent but without receiving an HTTP response within the timeout
 * - may prevent other crash reporters from detecting the crash.
 *
 * By default this value is false.
 */
@property (nonatomic) BOOL attemptDeliveryOnCrash;

/**
 * The types of breadcrumbs which will be captured. By default, this is all types.
 */
@property (nonatomic) RSCEnabledBreadcrumbType enabledBreadcrumbTypes;

/**
 * The app's bundleVersion, set from the CFBundleVersion.  Equivalent to `versionCode` on Android.
 */
@property (copy, nullable, nonatomic) NSString *bundleVersion;

@property (copy, nullable, nonatomic) NSString *appType;

/**
 * Sets the maximum number of events which will be stored. Once the threshold is reached,
 * the oldest events will be deleted.
 *
 * By default, 32 events are persisted.
 */
@property (nonatomic) NSUInteger maxPersistedEvents;

/**
 * Sets the maximum number of sessions which will be stored. Once the threshold is reached,
 * the oldest sessions will be deleted.
 *
 * By default, 128 sessions are persisted.
 */
@property (nonatomic) NSUInteger maxPersistedSessions;

/**
 * Sets the maximum number of breadcrumbs which will be stored. Once the threshold is reached,
 * the oldest breadcrumbs will be deleted.
 *
 * By default, 100 breadcrumbs are stored: this can be amended up to a maximum of 500.
 */
@property (nonatomic) NSUInteger maxBreadcrumbs;

/**
 * The maximum length of breadcrumb messages and metadata string values.
 * 
 * Values longer than this will be truncated prior to sending, after running any OnSendError blocks.
 *
 * The default value is 10000.
 */
@property (nonatomic) NSUInteger maxStringValueLength;

/**
 * Whether User information should be persisted to disk between application runs.
 * Defaults to True.
 */
@property (nonatomic) BOOL persistUser;

/**
 * A class defining the types of error that are reported. By default,
 * all properties are true.
 */
@property (strong, nonatomic) RSCrashReporterErrorTypes *enabledErrorTypes;

/**
 * Set the endpoints to send data to. By default we'll send error reports to
 * https://notify.bugsnag.com, and sessions to https://sessions.bugsnag.com, but you can
 * override this if you are using RSCrashReporter Enterprise to point to your own RSCrashReporter endpoint.
 *
 * Please note that it is recommended that you set both endpoints. If the notify endpoint is
 * missing, an assertion will be thrown. If the session endpoint is missing, a warning will be
 * logged and sessions will not be sent automatically.
 */
@property (copy, nonatomic) RSCrashReporterEndpointConfiguration *endpoints;

// =============================================================================
// MARK: - User
// =============================================================================

/**
 * The current user
 */
@property(readonly, retain, nonnull, nonatomic) RSCrashReporterUser *user;

/**
 *  Set user metadata
 *
 *  @param userId ID of the user
 *  @param name   Name of the user
 *  @param email  Email address of the user
 *
 *  If user ID is nil, a RSCrashReporter-generated Device ID is used for the `user.id` property of events and sessions.
 */
- (void)setUser:(NSString *_Nullable)userId
      withEmail:(NSString *_Nullable)email
        andName:(NSString *_Nullable)name;

// =============================================================================
// MARK: - onSession
// =============================================================================

/**
 *  Add a callback to be invoked before a session is sent to RSCrashReporter.
 *
 *  @param block A block which can modify the session
 *
 *  @returns An opaque reference to the callback which can be passed to `removeOnSession:`
 */
- (RSCrashReporterOnSessionRef)addOnSessionBlock:(RSCrashReporterOnSessionBlock)block
    NS_SWIFT_NAME(addOnSession(block:));

/**
 * Remove a callback that would be invoked before a session is sent to RSCrashReporter.
 *
 * @param callback The opaque reference of the callback, returned by `addOnSessionBlock:`
 */
- (void)removeOnSession:(RSCrashReporterOnSessionRef)callback
    NS_SWIFT_NAME(removeOnSession(_:));

/**
 * Deprecated
 */
- (void)removeOnSessionBlock:(RSCrashReporterOnSessionBlock)block
    RSCRASHREPORTER_DEPRECATED_WITH_REPLACEMENT("removeOnSession:")
    NS_SWIFT_NAME(removeOnSession(block:));

// =============================================================================
// MARK: - onSend
// =============================================================================

/**
 *  Add a callback to be invoked before a report is sent to RSCrashReporter, to
 *  change the report contents as needed
 *
 *  @param block A block which returns YES if the report should be sent
 *
 *  @returns An opaque reference to the callback which can be passed to `removeOnSendError:`
 */
- (RSCrashReporterOnSendErrorRef)addOnSendErrorBlock:(RSCrashReporterOnSendErrorBlock)block
    NS_SWIFT_NAME(addOnSendError(block:));

/**
 * Remove the callback that would be invoked before an event is sent.
 *
 * @param callback The opaque reference of the callback, returned by `addOnSendErrorBlock:`
 */
- (void)removeOnSendError:(RSCrashReporterOnSendErrorRef)callback
    NS_SWIFT_NAME(removeOnSendError(_:));

/**
 * Deprecated
 */
- (void)removeOnSendErrorBlock:(RSCrashReporterOnSendErrorBlock)block
    RSCRASHREPORTER_DEPRECATED_WITH_REPLACEMENT("removeOnSendError:")
    NS_SWIFT_NAME(removeOnSendError(block:));

// =============================================================================
// MARK: - onBreadcrumb
// =============================================================================

/**
 *  Add a callback to be invoked when a breadcrumb is captured by RSCrashReporter, to
 *  change the breadcrumb contents as needed
 *
 *  @param block A block which returns YES if the breadcrumb should be captured
 *
 *  @returns An opaque reference to the callback which can be passed to `removeOnBreadcrumb:`
 */
- (RSCrashReporterOnBreadcrumbRef)addOnBreadcrumbBlock:(RSCrashReporterOnBreadcrumbBlock)block
    NS_SWIFT_NAME(addOnBreadcrumb(block:));

/**
 * Remove the callback that would be invoked when a breadcrumb is captured.
 *
 * @param callback The opaque reference of the callback, returned by `addOnBreadcrumbBlock:`
 */
- (void)removeOnBreadcrumb:(RSCrashReporterOnBreadcrumbRef)callback
    NS_SWIFT_NAME(removeOnBreadcrumb(_:));

/**
 * Deprecated
 */
- (void)removeOnBreadcrumbBlock:(RSCrashReporterOnBreadcrumbBlock)block
    RSCRASHREPORTER_DEPRECATED_WITH_REPLACEMENT("removeOnBreadcrumb:")
    NS_SWIFT_NAME(removeOnBreadcrumb(block:));

// =============================================================================
// MARK: - Telemetry
// =============================================================================

/**
 * The types of telemetry that may be sent to RSCrashReporter for product improvement purposes.
 *
 * By default all types of telemetry are enabled.
 */
@property (nonatomic) RSCTelemetryOptions telemetry;

// =============================================================================
// MARK: - Plugins
// =============================================================================

/**
 * Internal interface for adding custom behavior :nodoc:
 */
- (void)addPlugin:(id<RSCrashReporterPlugin> _Nonnull)plugin;

@end

NS_ASSUME_NONNULL_END
