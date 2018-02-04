// IndoorAtlas iOS SDK
// IALocationManager.h

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
#import <CoreMotion/CoreMotion.h>
#import <IndoorAtlas/IAFloor.h>

#define INDOORATLAS_API __attribute__((visibility("default")))

// extraInfo dictionary keys
INDOORATLAS_API extern NSString * _Nonnull const kIATraceId;

@class IALocationManager;

/**
 * Defines the type of region.
 */
typedef NS_ENUM(NSInteger, ia_region_type) {
    /**
     * Region type is not known.
     * This may be the result of running outdated SDK.
     */
    kIARegionTypeUnknown,

    /**
     * Region type is floor plan.
     */
    kIARegionTypeFloorPlan,

    /**
     * Region type is venue.
     */
    kIARegionTypeVenue,

    /**
     * Region type is geofence.
     */
    kIARegionTypeGeofence,
};

/**
 * Defines the location service status.
 */
typedef NS_ENUM(NSInteger, ia_status_type) {
    /**
     * Location service is not available and the condition is not expected to resolve itself soon.
     */
    kIAStatusServiceOutOfService = 0,

    /**
     * Location service temporarily unavailable. This could be due to no network connectivity.
     */
    kIAStatusServiceUnavailable = 1,

    /**
     * Location service running normally.
     */
    kIAStatusServiceAvailable = 2,

    /**
     * Location service is running but with limited accuracy and functionality.
     */
    kIAStatusServiceLimited = 10,
};

/**
 * Defines the device calibration quality.
 * The quality of calibration affects location accuracy.
 */
typedef NS_ENUM(NSInteger, ia_calibration) {
    /**
     * Quality is poor.
     */
    kIACalibrationPoor,

    /**
     * Quality is good.
     */
    kIACalibrationGood,

    /**
     * Quality is excellent.
     */
    kIACalibrationExcellent,
};

/**
 * Defines the accuracy of location.
 */
typedef NS_ENUM(NSInteger, ia_location_accuracy) {
    /**
     * Best accuracy.
     */
    kIALocationAccuracyBest,

    /**
     * Low accuracy.
     * Locations with this accuracy are typically obtained with lowest amount of processing to reduce device power drain.
     */
    kIALocationAccuracyLow,
};

/**
 * An IARegion object represents a region on Earth.
 */
INDOORATLAS_API
@interface IARegion : NSObject
/**
 * Region identifier. For objects of type kIARegionTypeFloorPlan this is same as floor plan id.
 */
@property (nonatomic, strong, nonnull) NSString *identifier;
/**
 * Human readable name
 */
@property (nonatomic, strong, nullable) NSString *name;
/**
 * Region type
 *
 * @param type See possible values at [ia_region_type](/Constants/ia_region_type.html)
 */
@property (nonatomic, assign) enum ia_region_type type;
/**
 * If there is an event related to region this is the timestamp of that event.
 */
@property (nonatomic, strong, nullable) NSDate *timestamp;
@end

/**
 * Struct defining bounding box in WGS84 coordinates.
 */
struct ia_bounding_box {
    /**
     * Coordinates for min and max corners of the bounding box.
     */
    CLLocationCoordinate2D coords[2];
};

/**
 * An IAGeofence object provides a way to represent custom regions.
 */
INDOORATLAS_API
@interface IAGeofence : IARegion
/**
 * Bounding box of the geofence.
 *
 * The bounding box must accurately cover the hit testable area by <containsCoordinate:> method.
 *
 * The bounding box is used internally to optimize geofence lookups.
 */
@property (nonatomic, assign) struct ia_bounding_box boundingBox;
/**
 * The floor the geofence is located on.
 */
@property (nonatomic, strong, nullable) IAFloor *floor;
/**
 * Does the geofence contain the coordinate?
 */
- (BOOL)containsCoordinate:(CLLocationCoordinate2D)coordinate;
@end

/**
 * IAPolygonGeofence object represents a polygonal region on Earth.
 */
INDOORATLAS_API
@interface IAPolygonGeofence : IAGeofence
/**
 * The unique points for the polygon.
 */
@property (nonatomic, readonly, strong) NSArray<NSNumber*> * _Nonnull points;
/**
 * Creates a new polygonal region from unique edges.
 * @param identifier Identifier for the geofence.
 * @param <IAFloor> object with level information. Nil <IAFloor> means that the floor is unknown.
 * @param edges Coordinates specifying the polygon.
 *
 * The edges must be supplied in clockwise order for the polygon to be valid.
 */
+ (nonnull IAPolygonGeofence*)polygonGeofenceWithIdentifier:(nonnull NSString*)identifier andFloor:(nullable IAFloor*)floor edges:(nonnull NSArray<NSNumber*>*)edges;
@end

/**
 * <IAStatus> specifies the current status of locationing service.
 */
INDOORATLAS_API
@interface IAStatus : NSObject
/**
 * Type of status message.
 *
 * @param type See possible values at [ia_status_type](/Constants/ia_status_type.html)
 */
@property (nonatomic, assign) enum ia_status_type type;
@end

/**
 * An IALocation object represents the location data generated by an <IALocationManager> object. This object incorporates the geographical coordinates along with values indicating the accuracy of the measurements and when those measurements were made.
 * This class also reports information about the the course, the direction in which the device is traveling.
 *
 * Typically, you use an <IALocationManager> object to create instances of this class based on the last known location of the user's device.
 * You can create instances yourself, however, if you want to cache custom location data or get the distance between two points.
 *
 * This class is designed to be used as is and should not be subclassed.
 */
INDOORATLAS_API
@interface IALocation : NSObject

/**
 * @name Initializing a Location Object
 *
 * Indicate current location to positioning service. Can be used to set either the explicit location (latitude, longitude, accuracy, floor level) or the explicit region (venue or floorplan id)
 */

/**
 * Initializes and returns a location object with specified CoreLocation information.
 * @param location CLLocation object. Might be initialized in code or from CLLocationManager.
 *
 * An explicit location is used as a hint in the system. This means that the inputted location is used only to determine the initial position and setting the location does not lock the floor or venue context.
 */
+ (nonnull IALocation*)locationWithCLLocation:(nonnull CLLocation*)location;

/**
 * Initializes and returns a location object with specified floor plan id.
 * @param floorPlanId Identifier of the floor plan.
 *
 * An explicit floor plan is used for initialising and locking the positioning to a certain floor. This means that the position estimate is not free to leave the indicated floor.
 * Using explicit location or venue id inputs is generally not recommended, and should only be used in difficult signal environments where getting first fix is not possible otherwise.
 */
+ (nonnull IALocation*)locationWithFloorPlanId:(nonnull NSString*)floorPlanId;

/**
 * Initializes and returns a location object with specified venue id (and floor).
 * @param venueId Identifier of the venue.
 * @param <IAFloor> object with level information. Nil <IAFloor> means that the floor is unknown.
 *
 * An explicit venue is used for locking the positioning to a certain venue context (if a NIL floor is set). This means that the position estimate is not free to leave the indicated venue, but can move between floors. If an explicit floor is also given, the estimate is locked to that floor.
 * Using explicit location or venue id inputs is generally not recommended, and should only be used in difficult signal environments where getting first fix is not possible otherwise.
 */
+ (nonnull IALocation*)locationWithVenueId:(nonnull NSString*)venueId andFloor:(nullable IAFloor*)floor;

/**
 * @name Location Attributes
 */

/**
 * CoreLocation compatible [location information](https://developer.apple.com/library/ios/documentation/CoreLocation/Reference/CLLocation_Class/). (read-only)
 *
 * When running in the simulator, <IALocationManager> provides fake values.
 * You must run your application on an actual iOS device to get the actual location of the device.
 */
@property (nonatomic, readonly, nullable) CLLocation *location;

/**
 * @name Optional attributes
 */

/**
 * The logical floor of the building.
 *
 * This property is included as CLLocation's CLFloor is private interface.
 * Thus it may be deprecated in future.
 */
@property (nonatomic, readwrite, nullable) IAFloor *floor;

/**
 * Region this location was obtained from.
 */
@property (nonatomic, readwrite, nullable) IARegion *region;

@end

/**
 * Object containing heading data. Generated by <IALocationManager>
 */
INDOORATLAS_API
@interface IAHeading : NSObject
/**
 * The heading in degrees. Relative to true north.
 */
@property (nonatomic, readonly) CLLocationDirection trueHeading;
/**
 * Time when heading was obtained
 */
@property(readonly, nonatomic, copy, nullable) NSDate *timestamp;
@end

/**
 * Object containing orientation data. Generated by <IALocationManager>
 */
INDOORATLAS_API
@interface IAAttitude : NSObject
/**
 * The orientation
 */
@property(readonly, nonatomic) CMQuaternion quaternion;
/**
 * Time when orientation was obtained
 */
@property(readonly, nonatomic, copy, nullable) NSDate *timestamp;
@end

/**
 * The IALocationManagerDelegate protocol defines the methods used to receive location updates from an <IALocationManager> object.
 *
 * Upon receiving a successful location update, you can use the result to update your user interface or perform other actions.
 *
 * The methods of your delegate object are called from the thread in which you started the corresponding location services.
 * That thread must itself have an active run loop, like the one found in your application's main thread.
 */
INDOORATLAS_API
@protocol IALocationManagerDelegate <NSObject>
@optional

/**
 * @name Responding to Location Events
 */

/**
 * Tells the delegate that new location data is available.
 *
 * Implementation of this method is optional but recommended.
 *
 * @param manager The location manager object that generated the update event.
 * @param locations An array of <IALocation> objects containing the location data. This array always contains at least one object representing the current location.
 * If updates were deferred or if multiple locations arrived before they could be delivered, the array may contain additional entries.
 * The objects in the array are organized in the order in which they occured. Threfore, the most recent location update is at the end of the array.
 */
- (void)indoorLocationManager:(nonnull IALocationManager*)manager didUpdateLocations:(nonnull NSArray*)locations;

/**
 * Tells the delegate that the user entered the specified region.
 * @param manager The location manager object that generated the event.
 * @param region The region related to event.
 */
- (void)indoorLocationManager:(nonnull IALocationManager*)manager didEnterRegion:(nonnull IARegion*)region;

/**
 * Tells the delegate that the user left the specified region.
 * @param manager The location manager object that generated the event.
 * @param region The region related to event.
 */
- (void)indoorLocationManager:(nonnull IALocationManager*)manager didExitRegion:(nonnull IARegion*)region;

/**
 * Tells that <IALocationManager> status changed. This is used to signal network connection issues.
 * @param manager The location manager object that generated the event.
 * @param status The status at the time of the event.
 */
- (void)indoorLocationManager:(nonnull IALocationManager*)manager statusChanged:(nonnull IAStatus*)status;

/**
 * Tells that calibration quality changed.
 * @param manager The location manager object that generated the event.
 * @param quality The calibration quality at the time of the event.
 */
- (void)indoorLocationManager:(nonnull IALocationManager*)manager calibrationQualityChanged:(enum ia_calibration)quality;

/**
 * Tells that extra information dictionary was received. This dictionary contains
 * identifier for debugging positioning.
 * @param manager The location manager object that generated the event.
 * @param extraInfo NSDictionary containing extra information about positioning.
 */
- (void)indoorLocationManager:(nonnull IALocationManager*)manager didReceiveExtraInfo:(nonnull NSDictionary*)extraInfo;

/**
 * Tells the delegate that updated heading information is available.
 * @param manager The location manager object that generated the event.
 * @param newHeading New heading data.
 */
- (void)indoorLocationManager:(nonnull IALocationManager*)manager didUpdateHeading:(nonnull IAHeading*)newHeading;

/**
 * Tells the delegate that updated attitude (orientation) information is available.
 * @param manager The location manager object that generated the event.
 * @param newAttitude New attitude data.
 */
- (void)indoorLocationManager:(nonnull IALocationManager*)manager didUpdateAttitude:(nonnull IAAttitude*)newAttitude;
@end

/**
 * The IALocationManager class is central point for configuring the delivery of indoor location related events to your app.
 * You use and instance of this class to establish the parameters that determine when location events should be delivered and to start and stop the actual delivery of those events.
 * You can also use a location manager object to retrieve the most recent location data.
 */
INDOORATLAS_API
@interface IALocationManager : NSObject
/**
 * The latest calibration quality value
 *
 * @param calibration See possible values at [ia_calibration](/Constants/ia_calibration.html)
 */
@property (nonatomic, readonly) enum ia_calibration calibration;

/**
 * The latest location update.
 *
 * This property can be set for a custom location.
 */
@property (nonatomic, readwrite, nullable) IALocation *location;

/**
 * The latest sample of device attitude.
 *
 */
@property (nonatomic, readwrite, nullable) IAAttitude *attitude;

/**
 * The latest sample of device heading.
 *
 */
@property (nonatomic, readwrite, nullable) IAHeading *heading;

/**
 * The minimum distance measured in meters that the device must move
 * horizontally before an update event is generated.
 * Default value is 0.7 meters.
 * Uses CoreLocation [CLLocationDistance](https://developer.apple.com/library/ios/documentation/CoreLocation/Reference/CoreLocationDataTypesRef/index.html#//apple_ref/c/tdef/CLLocationDistance).
 */
@property (assign, nonatomic) CLLocationDistance distanceFilter;

/**
 * The minimum angular change in degrees required to generate new didUpdateHeading event.
 * Default value is 1 degree.
 */
@property(assign, nonatomic) CLLocationDegrees headingFilter;

/**
 * The minimum angular change in degrees required to generate new didUpdateAttitude event.
 * Default value is 1 degree.
 */
@property(assign, nonatomic) CLLocationDegrees attitudeFilter;

/**
 * The accuracy of the location data.
 *
 * The receiver does its best to achieve the requested accuracy; however, the actual accuracy is not guaranteed.
 * You should assign a value to this property that is appropriate for your usage scenario.
 * Determining a location with greater accuracy requires more time and more power.
 *
 * Default value is kIALocationAccuracyBest.
 *
 * @param desiredAccuracy See possible values at [ia_location_accuracy](/Constants/ia_location_accuracy)
 */
@property(assign, nonatomic) enum ia_location_accuracy desiredAccuracy;

/**
 * The set of geofences monitored by the location manager.
 *
 * You cannot add geofences to this property directly. Instead use the <startMonitoringGeofence:> method.
 */
@property(nonatomic, readonly, strong, nullable) NSArray<IAGeofence*> *monitoredGeofences;

/**
 * The latest extra information dictionary.
 *
 * Used for debugging positioning.
 */
@property (nonatomic, readonly, nullable) NSDictionary *extraInfo;

/**
 * @name Accessing the Delegate
 */

/**
 * The delegate object to receive update events.
 */
@property (nullable, nonatomic, readwrite, weak) id<IALocationManagerDelegate> delegate;

/**
 * @name SDK version
 */

/**
 * Returns SDK version string.
 *
 * The version string returned is in format "major.minor.patch". (see: [Semantic Versioning](http://semver.org/))
 */
+ (nonnull NSString*)versionString;

/**
 * Returns the shared <IALocationManager> instance.
 */
+ (nonnull IALocationManager *)sharedInstance;

/**
 * @name Authenticate your session
 */

/**
 * Set IndoorAtlas API key and secret for authentication.
 *
 * This method must be called before further requests with server requiring authentication.
 *
 * @param key API key used for authentication.
 * @param secret API secret used for authentication.
 */
- (void)setApiKey:(nonnull NSString*)key andSecret:(nonnull NSString*)secret;

/**
 * Starts the generation of updates that report the user's current location.
 *
 * This method returns immediately. Calling this method causes the location manager to obtain an initial location fix (which may take several seconds)
 * and notify your delegate by calling its <indoorLocationManager:didUpdateLocations:> method. After that, the receiver generates update events whenever there is new estimate.
 *
 * Calling this method several times in succession does not automatically result in new events being generated.
 * Calling <stopUpdatingLocation> in-between, however, does cause a new initial event to be sent the next time you call this method.
 *
 * If you start this service and your app is suspended, the system stops the delivery of events until your app starts running again (only in foreground).
 * If your app is terminated, the delivery of new location events stops altogether.
 */
- (void)startUpdatingLocation;

/**
 * Stops the generation of location updates.
 *
 * Call this method whenever your code no longer needs to receive location-related events. Disabling event delivery gives the receiver the option of disabling the
 * appropriate hardware (and thereby saving power) when no clients need location data. You can always restart the generation of location updates by calling the
 * <startUpdatingLocation> method again.
 */
- (void)stopUpdatingLocation;

/**
 * Starts monitoring the specified geofence.
 *
 * You must call this method once for each geofence you want to monitor. If an existing geofence with the same identifier is already being monitored by the app, the old geofence is replaced by the new one.
 * Geofence events are delivered as regions to the <indoorLocationManager:didEnterRegion:> and <indoorLocationManager:didExitRegion:> methods of your delegate.
 *
 * @param geofence The geofence object that defines the boundary to monitor.
 */
- (void)startMonitoringForGeofence:(nonnull IAGeofence*)geofence;

/**
 * Stops monitoring the specified geofence.
 *
 * If the specified geofence object is not currently being monitored, this method has no effect.
 *
 * @param geofence The geofence object currently being monitored.
 */
- (void)stopMonitoringForGeofence:(nonnull IAGeofence*)geofence;

/**
 * Marks init method as deprecated.
 */
- (_Nullable id)init __attribute__((deprecated("[[IALocationManager alloc] init] is depreated. Use [IALocationManager sharedInstance] instead.")));
@end

#undef INDOORATLAS_API
