/**
 * Unit test the RSCrashReporterConfiguration class
 */

#import <XCTest/XCTest.h>

#import "RSCrashReporterConfiguration+Private.h"

#import "RSCCrashSentry.h"
#import "RSCrashReporterClient+Private.h"
#import "RSCrashReporterEndpointConfiguration.h"
#import "RSCrashReporterErrorTypes.h"
#import "RSCrashReporterNotifier.h"
#import "RSCrashReporterSessionTracker.h"
#import "RSCrashReporterTestConstants.h"
#import "RSCrashReporterUser+Private.h"

// =============================================================================
// MARK: - Tests
// =============================================================================

@interface RSCrashReporterConfigurationTests : XCTestCase
@end

@implementation RSCrashReporterConfigurationTests

- (void)tearDown {
    [super tearDown];
    [self deletePersistedUserData];
}

// =============================================================================
// MARK: - Session-related
// =============================================================================

- (void)testDefaultSessionNotNil {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertNotNil(config.sessionOrDefault);
}

- (void)testDefaultSessionConfig {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertTrue([config autoTrackSessions]);
}

- (void)testSessionEndpoints {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];

    // Default endpoints
    XCTAssertEqualObjects([NSURL URLWithString:@"https://sessions.bugsnag.com"], config.sessionURL);

    // Test overriding the session endpoint (use dummy endpoints to avoid hitting production)

    config.endpoints = [[RSCrashReporterEndpointConfiguration alloc] initWithNotify:@"http://localhost:1234"
                                                                   sessions:@"http://localhost:8000"];
    XCTAssertEqualObjects([NSURL URLWithString:@"http://localhost:8000"], config.sessionURL);
}

- (void)testSetEmptySessionsEndpoint {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    config.endpoints = [[RSCrashReporterEndpointConfiguration alloc] initWithNotify:@"http://notify.example.com"
                                                                   sessions:@""];
    RSCrashReporterSessionTracker *sessionTracker
    = [[RSCrashReporterSessionTracker alloc] initWithConfig:config client:nil];

    XCTAssertNil(sessionTracker.runningSession);
    [sessionTracker startNewSession];
    XCTAssertNil(sessionTracker.runningSession);
}

- (void)testSetMalformedSessionsEndpoint {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    config.endpoints = [[RSCrashReporterEndpointConfiguration alloc] initWithNotify:@"http://notify.example.com"
                                                                   sessions:@"f"];
    RSCrashReporterSessionTracker *sessionTracker
    = [[RSCrashReporterSessionTracker alloc] initWithConfig:config client:nil];

    XCTAssertNil(sessionTracker.runningSession);
    [sessionTracker startNewSession];
    XCTAssertNil(sessionTracker.runningSession);
}

/**
 * Test that onSession blocks get called once added
 */
/*- (void)testAddOnSessionBlock {

    // Setup
    __block XCTestExpectation *expectation = [self expectationWithDescription:@"Remove On Session Block"];
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    config.endpoints = [[RSCrashReporterEndpointConfiguration alloc] initWithNotify:@"http://notreal.bugsnag.com"
                                                                   sessions:@"http://notreal.bugsnag.com"];
    XCTAssertEqual([[config onSessionBlocks] count], 0);
    RSCrashReporterOnSessionBlock sessionBlock = ^BOOL(RSCrashReporterSession * _Nonnull sessionPayload) {
        // We expect the session block to be called
        [expectation fulfill];
        return true;
    };
    [config addOnSessionBlock:sessionBlock];
    XCTAssertEqual([[config onSessionBlocks] count], 1);

    // Call onSession blocks
    RSCrashReporterClient *client = [[RSCrashReporterClient alloc] initWithConfiguration:config delegate:nil];
    [client start];
    [client resumeSession];
    [self waitForExpectationsWithTimeout:5.0 handler:nil];
}*/
/**
 * Test that onSession blocks do not get called once they've been removed
 */
/*- (void)testRemoveOnSessionBlock {
    // Setup
    // We expect NOT to be called
    __block XCTestExpectation *calledExpectation = [self expectationWithDescription:@"Remove On Session Block"];
    calledExpectation.inverted = YES;

    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    config.endpoints = [[RSCrashReporterEndpointConfiguration alloc] initWithNotify:@"http://notreal.bugsnag.com"
                                                                   sessions:@"http://notreal.bugsnag.com"];
    XCTAssertEqual([[config onSessionBlocks] count], 0);
    RSCrashReporterOnSessionBlock sessionBlock = ^BOOL(RSCrashReporterSession * _Nonnull sessionPayload) {
        [calledExpectation fulfill];
        return true;
    };

    // It's there (and from other tests we know it gets called) and then it's not there
    RSCrashReporterOnSessionRef callback = [config addOnSessionBlock:sessionBlock];
    XCTAssertEqual([[config onSessionBlocks] count], 1);
    [config removeOnSession:callback];
    XCTAssertEqual([[config onSessionBlocks] count], 0);

    RSCrashReporterClient *client = [[RSCrashReporterClient alloc] initWithConfiguration:config delegate:nil];
    [client start];

    // Wait a second NOT to be called
    [self waitForExpectationsWithTimeout:1.0 handler:nil];
}*/
/**
 * Test that an onSession block is called after being added, then NOT called after being removed.
 * This test could be expanded to verify the behaviour when multiple blocks are added.
 */
/*- (void)testAddOnSessionBlockThenRemove {

    __block int called = 0; // A counter

    // Setup
    __block XCTestExpectation *expectation1 = [self expectationWithDescription:@"Remove On Session Block 1"];
    __block XCTestExpectation *expectation2 = [self expectationWithDescription:@"Remove On Session Block 2"];
    __block XCTestExpectation *expectation3 = [self expectationWithDescription:@"Remove On Session Block 3"];
    __block XCTestExpectation *expectation4 = [self expectationWithDescription:@"Remove On Session Block 4"];
    expectation3.inverted = YES;
    expectation4.inverted = YES;

    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    config.endpoints = [[RSCrashReporterEndpointConfiguration alloc] initWithNotify:@"http://notreal.bugsnag.com"
                                                                   sessions:@"http://notreal.bugsnag.com"];
    XCTAssertEqual([[config onSessionBlocks] count], 0);

    RSCrashReporterOnSessionBlock sessionBlock = ^BOOL(RSCrashReporterSession * _Nonnull sessionPayload) {
        switch (called) {
        case 0:
            [expectation1 fulfill];
            break;
        case 1:
            [expectation2 fulfill];
            break;
        case 2:
            // Should NOT be called
            [expectation3 fulfill];
            break;
        case 3:
            // Should NOT be called
            [expectation4 fulfill];
            break;
        }
        return true;
    };

    RSCrashReporterOnSessionRef callback = [config addOnSessionBlock:sessionBlock];
    XCTAssertEqual([[config onSessionBlocks] count], 1);

    // Call onSession blocks
    RSCrashReporterClient *client = [[RSCrashReporterClient alloc] initWithConfiguration:config delegate:nil];
    [client start];
    [client resumeSession];
    [self waitForExpectations:@[expectation1] timeout:1.0];

    // Check it's called on new session start
    [client pauseSession];
    called++;
    [client startSession];
    [self waitForExpectations:@[expectation2] timeout:1.0];

    // Check block is not called after removing and initialisation
    [client pauseSession];
    called++;
    [config removeOnSession:callback];
    [client startSession];
    [self waitForExpectations:@[expectation3] timeout:1.0];

    // Check it's NOT called on session resume
    [client pauseSession];
    called++;
    [config addOnSessionBlock:sessionBlock];
    [client resumeSession];
    [self waitForExpectations:@[expectation4] timeout:1.0];
}*/

/**
 * Make sure slightly invalid removals and duplicate additions don't break things
 */
- (void)testRemoveNonexistentOnSessionBlocks {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertEqual([[config onSessionBlocks] count], 0);
    RSCrashReporterOnSessionBlock sessionBlock1 = ^BOOL(RSCrashReporterSession * _Nonnull sessionPayload) { return true; };
    RSCrashReporterOnSessionBlock sessionBlock2 = ^BOOL(RSCrashReporterSession * _Nonnull sessionPayload) { return true; };

    RSCrashReporterOnSessionRef callback = [config addOnSessionBlock:sessionBlock1];
    XCTAssertEqual([[config onSessionBlocks] count], 1);
    [config removeOnSession:sessionBlock2];
    XCTAssertEqual([[config onSessionBlocks] count], 1);
    [config removeOnSession:callback];
    XCTAssertEqual([[config onSessionBlocks] count], 0);
    [config removeOnSession:sessionBlock2];
    XCTAssertEqual([[config onSessionBlocks] count], 0);
    [config removeOnSession:callback];
    XCTAssertEqual([[config onSessionBlocks] count], 0);

    [config addOnSessionBlock:sessionBlock1];
    XCTAssertEqual([[config onSessionBlocks] count], 1);
    [config addOnSessionBlock:sessionBlock1];
    XCTAssertEqual([[config onSessionBlocks] count], 2);
    [config addOnSessionBlock:sessionBlock1];
    XCTAssertEqual([[config onSessionBlocks] count], 3);
}

- (void)testRemoveInvalidOnSessionDoesNotCrash {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    [config addOnSessionBlock:^BOOL(RSCrashReporterSession *session) { return NO; }];
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnonnull"
    [config removeOnSession:nil];
#pragma clang diagnostic pop
    [config removeOnSession:[[NSObject alloc] init]];
    [config removeOnSession:^{}];
    XCTAssertEqual(config.onSessionBlocks.count, 1);
}

// =============================================================================
// MARK: - Release stage-related
// =============================================================================

- (void)testEnabledReleaseStagesDefaultSends {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertTrue([config shouldSendReports]);
}

- (void)testEnabledReleaseStagesNilSends {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    config.releaseStage = @"beta";
    config.enabledReleaseStages = nil;
    XCTAssertTrue([config shouldSendReports]);
}

- (void)testEnabledReleaseStagesEmptySends {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    config.releaseStage = @"beta";
    config.enabledReleaseStages = [NSSet setWithArray:@[]];
    XCTAssertTrue([config shouldSendReports]);
}

- (void)testEnabledReleaseStagesIncludedSends {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    config.releaseStage = @"beta";
    config.enabledReleaseStages = [NSSet setWithArray:@[ @"beta" ]];
    XCTAssertTrue([config shouldSendReports]);
}

- (void)testEnabledReleaseStagesIncludedInManySends {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    config.releaseStage = @"beta";
    config.enabledReleaseStages = [NSSet setWithArray:@[ @"beta", @"production" ]];
    XCTAssertTrue([config shouldSendReports]);
}

- (void)testEnabledReleaseStagesExcludedSkipsSending {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    config.releaseStage = @"beta";
    config.enabledReleaseStages = [NSSet setWithArray:@[ @"production" ]];
    XCTAssertFalse([config shouldSendReports]);
}

// =============================================================================
// MARK: - Endpoint-related
// =============================================================================

- (void)testNotifyEndpoint {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertEqualObjects([NSURL URLWithString:@"https://notify.bugsnag.com"], config.notifyURL);

    // Test overriding the notify endpoint (use dummy endpoints to avoid hitting production)
    config.endpoints = [[RSCrashReporterEndpointConfiguration alloc] initWithNotify:@"http://localhost:1234"
                                                                   sessions:@"http://localhost:8000"];
    XCTAssertEqualObjects([NSURL URLWithString:@"http://localhost:1234"], config.notifyURL);
}

- (void)testSetEndpoints {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    config.endpoints = [[RSCrashReporterEndpointConfiguration alloc] initWithNotify:@"http://notify.example.com"
                                                                   sessions:@"http://sessions.example.com"];
    XCTAssertEqualObjects([NSURL URLWithString:@"http://notify.example.com"], config.notifyURL);
    XCTAssertEqualObjects([NSURL URLWithString:@"http://sessions.example.com"], config.sessionURL);
}

// in debug these throw exceptions though in release are "tolerated"
- (void)testSetNilNotifyEndpoint {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    NSString *notify = @"foo";
    notify = nil;
#if DEBUG
    XCTAssertThrowsSpecificNamed([config setEndpoints:[[RSCrashReporterEndpointConfiguration alloc] initWithNotify:notify
                                                                                                  sessions:@"http://sessions.example.com"]],
            NSException, NSInternalInconsistencyException);
#else
    XCTAssertNoThrow([config setEndpoints:[[RSCrashReporterEndpointConfiguration alloc] initWithNotify:@""
                                                                                                  sessions:@"http://sessions.example.com"]]);
#endif
}

// in debug these throw exceptions though in release are "tolerated"
- (void)testSetEmptyNotifyEndpoint {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
#if DEBUG
    XCTAssertThrowsSpecificNamed([config setEndpoints:[[RSCrashReporterEndpointConfiguration alloc] initWithNotify:@""
            sessions:@"http://sessions.example.com"]],
            NSException, NSInternalInconsistencyException);
#else
    XCTAssertNoThrow([config setEndpoints:[[RSCrashReporterEndpointConfiguration alloc] initWithNotify:@""
            sessions:@"http://sessions.example.com"]]);
#endif
}

// in debug these throw exceptions though in release are "tolerated"
- (void)testSetMalformedNotifyEndpoint {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
#if DEBUG
    XCTAssertThrowsSpecificNamed([config setEndpoints:[[RSCrashReporterEndpointConfiguration alloc] initWithNotify:@"http://"
                                                                                                  sessions:@"http://sessions.example.com"]],
            NSException, NSInternalInconsistencyException);
#else
    XCTAssertNoThrow([config setEndpoints:[[RSCrashReporterEndpointConfiguration alloc] initWithNotify:@"http://"
            sessions:@"http://sessions.example.com"]]);
#endif
}

// =============================================================================
// MARK: - User persistence tests
// =============================================================================

// Helper
- (void)getName:(NSString **)name email:(NSString **)email id:(NSString **  )id {
    RSCrashReporterUser *user = RSCGetPersistedUser();
    *email = user.email;
    *id = user.id;
    *name = user.name;
}

- (void)deletePersistedUserData {
    RSCSetPersistedUser(nil);
}

- (void)testUserPersistence {
    NSString *userDefaultEmail, *userDefaultName, *userDefaultUserId;
    NSString *email  = @"test@example.com";
    NSString *name   = @"foo";
    NSString *userId = @"123";

    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];

    // Check property defaults to True
    XCTAssertTrue(config.persistUser);

    // Start with no persisted user data
    [self deletePersistedUserData];
    [self getName:&userDefaultName email:&userDefaultEmail id:&userDefaultUserId];
    XCTAssertNil(userDefaultEmail);
    XCTAssertNil(userDefaultName);
    XCTAssertNil(userDefaultUserId);

    // user should be persisted by default
    [config setUser:userId withEmail:email andName:name];

    // Check values manually
    [self getName:&userDefaultName email:&userDefaultEmail id:&userDefaultUserId];
    XCTAssertEqualObjects(userDefaultEmail, email);
    XCTAssertEqualObjects(userDefaultName, name);
    XCTAssertEqualObjects(userDefaultUserId, userId);

    // Check persistence between invocations (when values have been set)
    RSCrashReporterConfiguration *config2 = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertEqualObjects(config2.user.email, email);
    XCTAssertEqualObjects(config2.user.name, name);
    XCTAssertEqualObjects(config2.user.id, userId);

    // Check that values we know to have been persisted are actuallty deleted.
    [self deletePersistedUserData];
    [self getName:&userDefaultName email:&userDefaultEmail id:&userDefaultUserId];
    XCTAssertNil(userDefaultEmail);
    XCTAssertNil(userDefaultName);
    XCTAssertNil(userDefaultUserId);
}

/**
 * Test that user data is (as far as we can tell) not persisted
 */
- (void)testUserNonPesistence {
    NSString *userDefaultEmail, *userDefaultName, *userDefaultUserId;

    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    config.persistUser = false;
    [self deletePersistedUserData];

    // Should be no persisted data, and should not persist between invocations
    [self deletePersistedUserData];
    [self getName:&userDefaultName email:&userDefaultEmail id:&userDefaultUserId];
    XCTAssertNil(userDefaultEmail);
    XCTAssertNil(userDefaultName);
    XCTAssertNil(userDefaultUserId);

    RSCrashReporterConfiguration *config2 = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertNotNil(config2.user);
    XCTAssertNil(config2.user.id);
    XCTAssertNil(config2.user.name);
    XCTAssertNil(config2.user.email);
}

/**
 * Test partial parsistence
 */

- (void)testPartialPesistence {
    NSString *userDefaultEmail, *userDefaultName, *userDefaultUserId;
    [self getName:&userDefaultName email:&userDefaultEmail id:&userDefaultUserId];
    
    NSString *email  = @"test@example.com";
    NSString *name   = @"foo";
    NSString *userId = @"123";
    
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertTrue(config.persistUser);
    [self deletePersistedUserData];

    // Should be no persisted data
    [self deletePersistedUserData];
    [self getName:&userDefaultName email:&userDefaultEmail id:&userDefaultUserId];
    XCTAssertNil(userDefaultEmail);
    XCTAssertNil(userDefaultName);
    XCTAssertNil(userDefaultUserId);
    
    [config setUser:userId withEmail:nil andName:nil];
    [self getName:&userDefaultName email:&userDefaultEmail id:&userDefaultUserId];
    XCTAssertNil(userDefaultEmail);
    XCTAssertNil(userDefaultName);
    XCTAssertEqualObjects(userDefaultUserId, userId);
    
    [config setUser:nil withEmail:email andName:nil];
    [self getName:&userDefaultName email:&userDefaultEmail id:&userDefaultUserId];
    XCTAssertNil(userDefaultName);
    XCTAssertEqualObjects(userDefaultEmail, email);
    XCTAssertNil(userDefaultUserId);

    [config setUser:nil withEmail:nil andName:name];
    [self getName:&userDefaultName email:&userDefaultEmail id:&userDefaultUserId];
    XCTAssertEqualObjects(userDefaultName, name);
    XCTAssertNil(userDefaultEmail);
    XCTAssertNil(userDefaultUserId);
}

/**
 * Test that persisting a RSCrashReporterUser with all nil fields behaves as expected
 */
- (void)testAllUserDataNilPersistence {
    NSString *userDefaultEmail, *userDefaultName, *userDefaultUserId;
    
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertTrue(config.persistUser);
    [self deletePersistedUserData];

    [config setUser:nil withEmail:nil andName:nil];

    // currentUser should have been set
    XCTAssertNotNil(config.user);

    // But there hould be no persisted data
    [self getName:&userDefaultName email:&userDefaultEmail id:&userDefaultUserId];
    XCTAssertNil(userDefaultName);
    XCTAssertNil(userDefaultEmail);
    XCTAssertNil(userDefaultUserId);
}

/**
 * Test that the configuration metadata is set correctly.
 */
- (void)testUserPersistenceAndMetadata {
    NSString *userDefaultEmail, *userDefaultName, *userDefaultUserId;
    NSString *email  = @"test@example.com";
    NSString *name   = @"foo";
    NSString *userId = @"123";

    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertTrue(config.persistUser);
    [self deletePersistedUserData];

    // Should be no persisted data
    [self getName:&userDefaultName email:&userDefaultEmail id:&userDefaultUserId];
    XCTAssertNil(userDefaultName);
    XCTAssertNil(userDefaultEmail);
    XCTAssertNil(userDefaultUserId);

    // Persist user data
    [config setUser:userId withEmail:email andName:name];

    // Check that retrieving persisted user data also sets configuration metadata
    // Check persistence between invocations (when values have been set)
    RSCrashReporterConfiguration *config2 = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertEqualObjects(config2.user.email, email);
    XCTAssertEqualObjects(config2.user.name, name);
    XCTAssertEqualObjects(config2.user.id, userId);
    
    XCTAssertEqualObjects(config2.  user.email, email);
    XCTAssertEqualObjects(config2.user.name, name);
    XCTAssertEqualObjects(config2.user.id, userId);
}

- (void)testSettingPersistUser {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertTrue(config.persistUser);
    [config setPersistUser:false];
    XCTAssertFalse(config.persistUser);
    [config setPersistUser:true];
    XCTAssertTrue(config.persistUser);
}

// =============================================================================
// MARK: - Max Persisted Events
// =============================================================================

- (void)testMaxPersistedEvents {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertEqual(config.maxPersistedEvents, 32, @"maxPersistedEvents should default to 32");

    config.maxPersistedEvents = 10;
    XCTAssertEqual(config.maxPersistedEvents, 10, @"Valid values should be accepted");

    config.maxPersistedEvents = 1000;
    XCTAssertEqual(config.maxPersistedEvents, 1000, @"No maximum bound should be applied");

    config.maxPersistedEvents = 1;
    XCTAssertEqual(config.maxPersistedEvents, 1, @"A value of 1 should be accepted");

    config.maxPersistedEvents = 0;
    XCTAssertEqual(config.maxPersistedEvents, 1, @"Setting to zero should have no effect");
}

// =============================================================================
// MARK: - Max Persisted Sessions
// =============================================================================

- (void)testMaxPersistedSessions {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertEqual(config.maxPersistedSessions, 128, @"maxPersistedSessions should default to 128");

    config.maxPersistedSessions = 10;
    XCTAssertEqual(config.maxPersistedSessions, 10, @"Valid values should be accepted");

    config.maxPersistedSessions = 1000;
    XCTAssertEqual(config.maxPersistedSessions, 1000, @"No maximum bound should be applied");

    config.maxPersistedSessions = 1;
    XCTAssertEqual(config.maxPersistedSessions, 1, @"A value of 1 should be accepted");

    config.maxPersistedSessions = 0;
    XCTAssertEqual(config.maxPersistedSessions, 1, @"Setting to zero should have no effect");
}

// =============================================================================
// MARK: - Max Breadcrumb
// =============================================================================

- (void)testMaxBreadcrumb {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertEqual(100, config.maxBreadcrumbs);

    // alter to valid value
    config.maxBreadcrumbs = 10;
    XCTAssertEqual(10, config.maxBreadcrumbs);

    // alter to max value
    config.maxBreadcrumbs = 500;
    XCTAssertEqual(500, config.maxBreadcrumbs);

    // alter to min value
    config.maxBreadcrumbs = 0;
    XCTAssertEqual(0, config.maxBreadcrumbs);

    // alter to negative value
    config.maxBreadcrumbs = -1;
    XCTAssertEqual(0, config.maxBreadcrumbs);

    // alter to too large value
    config.maxBreadcrumbs = 501;
    XCTAssertEqual(0, config.maxBreadcrumbs);
}

// =============================================================================
// MARK: - Default configuration values
// =============================================================================

- (void)testDefaultConfigurationValues {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];

#if TARGET_OS_TV
    XCTAssertEqualObjects(@"tvOS", config.appType);
#elif TARGET_OS_IOS
    XCTAssertEqualObjects(@"iOS", config.appType);
#elif TARGET_OS_OSX
    XCTAssertEqualObjects(@"macOS", config.appType);
#endif

    XCTAssertEqualObjects(config.appVersion, NSBundle.mainBundle.infoDictionary[@"CFBundleShortVersionString"]);
    XCTAssertTrue(config.autoDetectErrors);
    XCTAssertTrue(config.autoTrackSessions);
    XCTAssertEqualObjects(NSBundle.mainBundle.infoDictionary[@"CFBundleVersion"], config.bundleVersion);
    XCTAssertNil(config.context);
    XCTAssertEqual(RSCEnabledBreadcrumbTypeAll, config.enabledBreadcrumbTypes);
    XCTAssertTrue(config.enabledErrorTypes.cppExceptions);
    XCTAssertTrue(config.enabledErrorTypes.unhandledExceptions);
    XCTAssertTrue(config.enabledErrorTypes.unhandledRejections);
#if !TARGET_OS_WATCH
    XCTAssertTrue(config.enabledErrorTypes.machExceptions);
    XCTAssertTrue(config.enabledErrorTypes.signals);
    XCTAssertTrue(config.enabledErrorTypes.ooms);
#endif

    XCTAssertNil(config.enabledReleaseStages);
    XCTAssertEqualObjects(@"https://notify.bugsnag.com", config.endpoints.notify);
    XCTAssertEqualObjects(@"https://sessions.bugsnag.com", config.endpoints.sessions);
    XCTAssertEqual(config.maxStringValueLength, 10000);
    XCTAssertTrue(config.persistUser);
    XCTAssertEqual(1, [config.redactedKeys count]);
    XCTAssertEqualObjects(@"password", [config.redactedKeys allObjects][0]);

#if DEBUG
    XCTAssertEqualObjects(@"development", config.releaseStage);
#else
    XCTAssertEqualObjects(@"production", config.releaseStage);
#endif

#if !TARGET_OS_WATCH
    XCTAssertEqual(RSCThreadSendPolicyAlways, config.sendThreads);
#endif
}

// =============================================================================
// MARK: - Other tests
// =============================================================================

- (void)testDictionaryRepresentation {
    RSCrashReporterConfiguration *configuration = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertNotNil(configuration.dictionaryRepresentation[@"appType"]);
    XCTAssertNotNil(configuration.dictionaryRepresentation[@"releaseStage"]);
    
    configuration.appVersion = @"1.2.3";
    XCTAssertEqualObjects(configuration.dictionaryRepresentation[@"appVersion"], @"1.2.3");
    
    configuration.bundleVersion = @"2001";
    XCTAssertEqualObjects(configuration.dictionaryRepresentation[@"bundleVersion"], @"2001");
    
    XCTAssertNil(configuration.dictionaryRepresentation[@"context"]);
    configuration.context = @"lorem ipsum";
    XCTAssertEqualObjects(configuration.dictionaryRepresentation[@"context"], @"lorem ipsum");
    
    configuration.releaseStage = @"release";
    XCTAssertEqualObjects(configuration.dictionaryRepresentation[@"releaseStage"], @"release");
    
    XCTAssertNil(configuration.dictionaryRepresentation[@"enabledReleaseStages"]);
    configuration.enabledReleaseStages = [NSSet setWithArray:@[@"release"]];
    XCTAssertEqualObjects(configuration.dictionaryRepresentation[@"enabledReleaseStages"], @[@"release"]);
}

- (void)testValidateThrowsWhenMissingApiKey {
    NSString *nilKey = nil;

    XCTAssertThrows([[[RSCrashReporterConfiguration alloc] initWithApiKey:nilKey] validate]);
    XCTAssertThrows([[[RSCrashReporterConfiguration alloc] initWithApiKey:@""] validate]);
}

/**
 * When passed an invalid API Key we log a warning message but will still use the key
 */
- (void)testInitWithApiKeyUsesInvalidApiKeys {
    RSCrashReporterConfiguration *invalidApiConfig = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_16CHAR];
    XCTAssertNotNil(invalidApiConfig);
    XCTAssertEqualObjects(invalidApiConfig.apiKey, DUMMY_APIKEY_16CHAR);
}

-(void)testDesignatedInitializerValidApiKey {
    RSCrashReporterConfiguration *validApiConfig1 = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertNotNil(validApiConfig1);
    XCTAssertEqual([validApiConfig1 apiKey], DUMMY_APIKEY_32CHAR_1);

    RSCrashReporterConfiguration *validApiConfig2 = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_2];
    XCTAssertNotNil(validApiConfig2);
    XCTAssertEqual([validApiConfig2 apiKey], DUMMY_APIKEY_32CHAR_2);
}

/**
 * [RSCrashReporterConfiguration init] is explicitly made unavailable.
 * Test that it throws if it *is* called.  An explanation of the reason for
 * the slightly involved code to call the method is given here (hint: ARC):
 *
 *     https://stackoverflow.com/a/20058585/2431627
 */
-(void)testUnavailableConvenienceInitializer {
    RSCrashReporterConfiguration *config = [RSCrashReporterConfiguration alloc];
    SEL selector = NSSelectorFromString(@"init");
    IMP imp = [config methodForSelector:selector];
    void (*func)(id, SEL) = (void *)imp;
    XCTAssertThrows(func(config, selector));
}

- (void)testUser {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    
    [config setUser:@"123" withEmail:@"test@example.com" andName:@"foo"];
    
    XCTAssertEqualObjects(@"123", config.user.id);
    XCTAssertEqualObjects(@"foo", config.user.name);
    XCTAssertEqualObjects(@"test@example.com", config.user.email);
}

#if !TARGET_OS_WATCH

-(void)testRSCErrorTypes {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];

    // Test all are set by default
    // See config init for details.  OOMs are disabled in debug.
    config.enabledErrorTypes.ooms = true;

    XCTAssertTrue(config.enabledErrorTypes.ooms);
    XCTAssertTrue(config.enabledErrorTypes.signals);
    XCTAssertTrue(config.enabledErrorTypes.cppExceptions);
    XCTAssertTrue(config.enabledErrorTypes.unhandledExceptions);
    XCTAssertTrue(config.enabledErrorTypes.machExceptions);
    XCTAssertTrue(config.enabledErrorTypes.unhandledRejections);

    // Test that we can set it
    config.enabledErrorTypes.ooms = false;
    config.enabledErrorTypes.signals = false;
    config.enabledErrorTypes.cppExceptions = false;
    config.enabledErrorTypes.unhandledExceptions = false;
    config.enabledErrorTypes.unhandledRejections = false;
    config.enabledErrorTypes.machExceptions = false;
    XCTAssertFalse(config.enabledErrorTypes.ooms);
    XCTAssertFalse(config.enabledErrorTypes.signals);
    XCTAssertFalse(config.enabledErrorTypes.cppExceptions);
    XCTAssertFalse(config.enabledErrorTypes.unhandledExceptions);
    XCTAssertFalse(config.enabledErrorTypes.unhandledRejections);
    XCTAssertFalse(config.enabledErrorTypes.machExceptions);
}

/**
 * Test the mapping between RSCErrorTypes and KSCrashTypes
 */
-(void)testCrashTypeMapping {
    XCTAssertEqual(RSC_KSCrashTypeFromRSCrashReporterErrorTypes([RSCrashReporterErrorTypes new]),
                   RSC_KSCrashTypeNSException |
                   RSC_KSCrashTypeMachException |
                   RSC_KSCrashTypeSignal |
                   RSC_KSCrashTypeCPPException);

    // Check partial sets
    RSCrashReporterErrorTypes *errorTypes = [RSCrashReporterErrorTypes new];
    errorTypes.ooms = false;
    errorTypes.signals = false;
    errorTypes.machExceptions = false;
    XCTAssertEqual(RSC_KSCrashTypeFromRSCrashReporterErrorTypes(errorTypes),
                   RSC_KSCrashTypeNSException | RSC_KSCrashTypeCPPException);

    errorTypes.signals = true;
    errorTypes.cppExceptions = false;
    XCTAssertEqual(RSC_KSCrashTypeFromRSCrashReporterErrorTypes(errorTypes),
                   RSC_KSCrashTypeNSException | RSC_KSCrashTypeSignal);

    errorTypes.cppExceptions = true;
    errorTypes.unhandledExceptions = false;
    XCTAssertEqual(RSC_KSCrashTypeFromRSCrashReporterErrorTypes(errorTypes),
                   RSC_KSCrashTypeCPPException | RSC_KSCrashTypeSignal);
}

#endif

/**
 * Test that removeOnSendBlock() performs as expected.
 * Note: We don't test that set blocks are executed since this is tested elsewhere
 * (e.g. in RSCrashReporterBreadcrumbsTest)
 */
- (void) testRemoveOnSendBlock {
    // Prevent sending events
    RSCrashReporterConfiguration *configuration = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertEqual([[configuration onSendBlocks] count], 0);

    RSCrashReporterOnSendErrorBlock block = ^BOOL(RSCrashReporterEvent * _Nonnull event) { return false; };

    RSCrashReporterOnSendErrorRef callback = [configuration addOnSendErrorBlock:block];
    RSCrashReporterClient *client = [[RSCrashReporterClient alloc] initWithConfiguration:configuration delegate:nil];
    [client start];

    XCTAssertEqual([[configuration onSendBlocks] count], 1);

    [configuration removeOnSendError:callback];
    XCTAssertEqual([[configuration onSendBlocks] count], 0);
}

/**
 * Test that clearOnSendBlock() performs as expected.
 */
- (void) testClearOnSendBlock {
    // Prevent sending events
    RSCrashReporterConfiguration *configuration = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertEqual([[configuration onSendBlocks] count], 0);

    RSCrashReporterOnSendErrorBlock block1 = ^BOOL(RSCrashReporterEvent * _Nonnull event) { return false; };
    RSCrashReporterOnSendErrorBlock block2 = ^BOOL(RSCrashReporterEvent * _Nonnull event) { return false; };

    // Add more than one
    [configuration addOnSendErrorBlock:block1];
    [configuration addOnSendErrorBlock:block2];

    RSCrashReporterClient *client = [[RSCrashReporterClient alloc] initWithConfiguration:configuration delegate:nil];
    [client start];

    XCTAssertEqual([[configuration onSendBlocks] count], 2);
}

- (void)testSendThreadsDefault {
#if !TARGET_OS_WATCH
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
    XCTAssertEqual(RSCThreadSendPolicyAlways, config.sendThreads);
#endif
}

- (void)testNSCopying {
    RSCrashReporterConfiguration *config = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];

    // Set some arbirtary values:
    [config setUser:@"foo" withEmail:@"bar@baz.com" andName:@"Bill"];
    [config setApiKey:DUMMY_APIKEY_32CHAR_1];
    [config setAutoDetectErrors:YES];
    [config setContext:@"context1"];
    [config setAppType:@"The most amazing app, a brilliant app, the app to end all apps"];
    [config setNotifier:[[RSCrashReporterNotifier alloc] initWithName:@"Example"
                                                      version:@"0.0.0"
                                                          url:@"https://example.com"
                                                 dependencies:@[[[RSCrashReporterNotifier alloc] init]]]];
    [config setPersistUser:YES];
#if !TARGET_OS_WATCH
    [config setSendThreads:RSCThreadSendPolicyUnhandledOnly];
#endif
    [config setMaxStringValueLength:100];
    [config addPlugin:(id)[NSNull null]];

    RSCrashReporterOnSendErrorBlock onSendBlock1 = ^BOOL(RSCrashReporterEvent * _Nonnull event) { return true; };
    RSCrashReporterOnSendErrorBlock onSendBlock2 = ^BOOL(RSCrashReporterEvent * _Nonnull event) { return true; };

    NSArray *sendBlocks = @[ onSendBlock1, onSendBlock2 ];
    [config setOnSendBlocks:[sendBlocks mutableCopy]]; // Mutable arg required

    // Clone
    RSCrashReporterConfiguration *clone = [config copy];
    XCTAssertNotEqual(config, clone);

    // Change values

    // Redacted keys
    XCTAssertEqualObjects(config.redactedKeys, clone.redactedKeys);

#if !TARGET_OS_WATCH
    // sendThreads
    XCTAssertEqual(config.sendThreads, clone.sendThreads);
#endif

    // Object
    [clone setUser:@"Cthulu" withEmail:@"hp@lovecraft.com" andName:@"Howard"];
    XCTAssertEqualObjects(config.user.id, @"foo");
    XCTAssertEqualObjects(clone.user.id, @"Cthulu");

    // String
    [clone setApiKey:DUMMY_APIKEY_32CHAR_2];
    XCTAssertEqualObjects(config.apiKey, DUMMY_APIKEY_32CHAR_1);
    XCTAssertEqualObjects(clone.apiKey, DUMMY_APIKEY_32CHAR_2);

    // Bool
    [clone setAutoDetectErrors:NO];
    XCTAssertTrue(config.autoDetectErrors);
    XCTAssertFalse(clone.autoDetectErrors);

    // Block
    [clone setOnCrashHandler:config.onCrashHandler];
    XCTAssertEqual(config.onCrashHandler, clone.onCrashHandler);
    [clone setOnCrashHandler:(void *)^(const RSC_KSCrashReportWriter *_Nonnull writer){}];
    XCTAssertNotEqual(config.onCrashHandler, clone.onCrashHandler);

    // Array (of blocks)
    XCTAssertEqual(config.onSendBlocks, clone.onSendBlocks);
    XCTAssertEqual(config.onSendBlocks[0], clone.onSendBlocks[0]);
    [clone setOnSendBlocks:[@[ onSendBlock2 ] mutableCopy]];
    XCTAssertNotEqual(config.onSendBlocks[0], clone.onSendBlocks[0]);
    
    XCTAssertEqualObjects(clone.notifier.name, config.notifier.name);
    XCTAssertEqualObjects(clone.notifier.version, config.notifier.version);
    XCTAssertEqualObjects(clone.notifier.url, config.notifier.url);

    // Plugins
    XCTAssert([clone.plugins containsObject:[NSNull null]]);
    XCTAssertNoThrow([clone.plugins removeObject:[NSNull null]]);
    
    XCTAssertEqual(clone.maxStringValueLength, 100);
}

- (void)testMetadataMutability {
    RSCrashReporterConfiguration *configuration = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];

    // Immutable in, mutable out
    [configuration addMetadata:@{@"foo" : @"bar"} toSection:@"section1"];
    NSObject *metadata1 = [configuration getMetadataFromSection:@"section1"];
    XCTAssertTrue([metadata1 isKindOfClass:[NSMutableDictionary class]]);

    // Mutable in, mutable out
    [configuration addMetadata:[@{@"foo" : @"bar"} mutableCopy] toSection:@"section2"];
    NSObject *metadata2 = [configuration getMetadataFromSection:@"section2"];
    XCTAssertTrue([metadata2 isKindOfClass:[NSMutableDictionary class]]);
}

- (void)testDiscardClasses {
    XCTAssertNil([[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1].discardClasses, @"discardClasses should be nil be default");
    
    NSArray<NSString *> *errorClasses = @[@"EXC_BAD_ACCESS",
                                          @"EXC_BAD_INSTRUCTION",
                                          @"EXC_BREAKPOINT",
                                          @"Exception",
                                          @"Fatal error",
                                          @"NSError",
                                          @"NSGenericException",
                                          @"NSInternalInconsistencyException",
                                          @"NSMallocException",
                                          @"NSRangeException",
                                          @"SIGABRT",
                                          @"UIViewControllerHierarchyInconsistency",
                                          @"std::__1::system_error"];
    
    __block NSArray *discarded, *kept;
    
    void (^ applyDiscardClasses)(NSSet *) = ^(NSSet *discardClasses){
        RSCrashReporterConfiguration *configuration = [[RSCrashReporterConfiguration alloc] initWithApiKey:DUMMY_APIKEY_32CHAR_1];
        configuration.discardClasses = discardClasses;
        NSPredicate *shouldDiscard = [NSPredicate predicateWithBlock:^BOOL(id evaluatedObject, NSDictionary *bindings) {
            return [configuration shouldDiscardErrorClass:evaluatedObject];
        }];
        discarded = [errorClasses filteredArrayUsingPredicate:shouldDiscard];
        kept = [errorClasses filteredArrayUsingPredicate:[NSCompoundPredicate notPredicateWithSubpredicate:shouldDiscard]];
    };
    
    applyDiscardClasses(nil);
    XCTAssertEqualObjects(discarded, @[]);
    XCTAssertEqualObjects(kept, errorClasses);
    
    applyDiscardClasses([NSSet setWithObjects:@"nserror", nil]);
    XCTAssertEqualObjects(discarded, @[]);
    XCTAssertEqualObjects(kept, errorClasses);
    
    applyDiscardClasses([NSSet setWithObjects:@"EXC_BAD_ACCESS", @"NSError", nil]);
    XCTAssertEqualObjects(discarded, (@[@"EXC_BAD_ACCESS", @"NSError"]));
    XCTAssertEqualObjects(kept, (@[@"EXC_BAD_INSTRUCTION",
                                   @"EXC_BREAKPOINT",
                                   @"Exception",
                                   @"Fatal error",
                                   @"NSGenericException",
                                   @"NSInternalInconsistencyException",
                                   @"NSMallocException",
                                   @"NSRangeException",
                                   @"SIGABRT",
                                   @"UIViewControllerHierarchyInconsistency",
                                   @"std::__1::system_error"]));
    
    applyDiscardClasses([NSSet setWithObjects:@"Exception", @"NSError",
                         [NSRegularExpression regularExpressionWithPattern:@"std::__1::.*" options:0 error:nil], nil]);
    XCTAssertEqualObjects(discarded, (@[@"Exception", @"NSError", @"std::__1::system_error"]));
    XCTAssertEqualObjects(kept, (@[@"EXC_BAD_ACCESS",
                                   @"EXC_BAD_INSTRUCTION",
                                   @"EXC_BREAKPOINT",
                                   @"Fatal error",
                                   @"NSGenericException",
                                   @"NSInternalInconsistencyException",
                                   @"NSMallocException",
                                   @"NSRangeException",
                                   @"SIGABRT",
                                   @"UIViewControllerHierarchyInconsistency"]));
    
    applyDiscardClasses([NSSet setWithObjects:[NSRegularExpression regularExpressionWithPattern:@".*" options:0 error:nil], nil]);
    XCTAssertEqualObjects(discarded, errorClasses);
    XCTAssertEqualObjects(kept, (@[]));
}

@end
