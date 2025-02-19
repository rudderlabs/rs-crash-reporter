//
//  RSCJSONSerialization.h
//  RSCrashReporter
//
//  Created by Karl Stenerud on 03.09.20.
//  Copyright © 2020 Bugsnag Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/* Wraps NSJSONSerialization to trap any exceptions and return them as errors.
 *
 * NSJSONSerialization sometimes returns errors and sometimes throws exceptions,
 * with no specification as to which mechanism will trigger for what kinds of errors.
 * This wrapper catches all exceptions and forces everything to be returned as an error.
 */

BOOL RSCJSONDictionaryIsValid(NSDictionary *dictionary, NSError **error);

NSData *_Nullable RSCJSONDataFromDictionary(NSDictionary *dictionary, NSError **error);

NSDictionary *_Nullable RSCJSONDictionaryFromData(NSData *data, NSJSONReadingOptions options, NSError **error);

BOOL RSCJSONWriteToFileAtomically(NSDictionary *dictionary, NSString *file, NSError **error);

NSDictionary *_Nullable RSCJSONDictionaryFromFile(NSString *file, NSJSONReadingOptions options, NSError **error);

NS_ASSUME_NONNULL_END
