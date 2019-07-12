#import <Flutter/Flutter.h>
#import <CoreBluetooth/CoreBluetooth.h>

#define NAMESPACE @"ai.longev.flutter/flutter_ble"

@interface FlutterBlePlugin : NSObject<FlutterPlugin, CBCentralManagerDelegate, CBPeripheralDelegate>
@end

@interface FlutterBleStreamHandler : NSObject<FlutterStreamHandler>
@property FlutterEventSink sink;
@end
