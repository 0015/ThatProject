// Copyright 2017, Paul DeMarco.
// All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#import "FlutterBlePlugin.h"
#import "Flutterblue.pbobjc.h"

@interface CBUUID (CBUUIDAdditionsFlutterBle)
- (NSString *)fullUUIDString;
@end

@implementation CBUUID (CBUUIDAdditionsFlutterBle)
- (NSString *)fullUUIDString {
  if(self.UUIDString.length == 4) {
    return [[NSString stringWithFormat:@"0000%@-0000-1000-8000-00805F9B34FB", self.UUIDString] lowercaseString];
  }
  return [self.UUIDString lowercaseString];
}
@end

typedef NS_ENUM(NSUInteger, LogLevel) {
  emergency = 0,
  alert = 1,
  critical = 2,
  error = 3,
  warning = 4,
  notice = 5,
  info = 6,
  debug = 7
};

@interface FlutterBlePlugin ()
@property(nonatomic, retain) NSObject<FlutterPluginRegistrar> *registrar;
@property(nonatomic, retain) FlutterMethodChannel *channel;
@property(nonatomic, retain) FlutterBleStreamHandler *stateStreamHandler;
@property(nonatomic, retain) FlutterBleStreamHandler *scanResultStreamHandler;
@property(nonatomic, retain) FlutterBleStreamHandler *servicesDiscoveredStreamHandler;
@property(nonatomic, retain) FlutterBleStreamHandler *characteristicReadStreamHandler;
@property(nonatomic, retain) FlutterBleStreamHandler *descriptorReadStreamHandler;
@property(nonatomic, retain) CBCentralManager *centralManager;
@property(nonatomic) NSMutableDictionary *scannedPeripherals;
@property(nonatomic) NSMutableArray *servicesThatNeedDiscovered;
@property(nonatomic) NSMutableArray *characteristicsThatNeedDiscovered;
@property(nonatomic) LogLevel logLevel;
@end

@implementation FlutterBlePlugin
+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  FlutterMethodChannel* channel = [FlutterMethodChannel
                                   methodChannelWithName:NAMESPACE @"/methods"
                                   binaryMessenger:[registrar messenger]];
  FlutterEventChannel* stateChannel = [FlutterEventChannel eventChannelWithName:NAMESPACE @"/state" binaryMessenger:[registrar messenger]];
  FlutterEventChannel* scanResultChannel = [FlutterEventChannel eventChannelWithName:NAMESPACE @"/scanResult" binaryMessenger:[registrar messenger]];
  FlutterEventChannel* servicesDiscoveredChannel = [FlutterEventChannel eventChannelWithName:NAMESPACE @"/servicesDiscovered" binaryMessenger:[registrar messenger]];
  FlutterEventChannel* characteristicReadChannel = [FlutterEventChannel eventChannelWithName:NAMESPACE @"/characteristicRead" binaryMessenger:[registrar messenger]];
  FlutterEventChannel* descriptorReadChannel = [FlutterEventChannel eventChannelWithName:NAMESPACE @"/descriptorRead" binaryMessenger:[registrar messenger]];
  FlutterBlePlugin* instance = [[FlutterBlePlugin alloc] init];
  instance.channel = channel;
  instance.centralManager = [[CBCentralManager alloc] initWithDelegate:instance queue:nil];
  instance.scannedPeripherals = [NSMutableDictionary new];
  instance.servicesThatNeedDiscovered = [NSMutableArray new];
  instance.characteristicsThatNeedDiscovered = [NSMutableArray new];
  instance.logLevel = emergency;

  // STATE
  FlutterBleStreamHandler* stateStreamHandler = [[FlutterBleStreamHandler alloc] init];
  [stateChannel setStreamHandler:stateStreamHandler];
  instance.stateStreamHandler = stateStreamHandler;

  // SCAN RESULTS
  FlutterBleStreamHandler* scanResultStreamHandler = [[FlutterBleStreamHandler alloc] init];
  [scanResultChannel setStreamHandler:scanResultStreamHandler];
  instance.scanResultStreamHandler = scanResultStreamHandler;

  // SERVICES DISCOVERED
  FlutterBleStreamHandler* servicesDiscoveredStreamHandler = [[FlutterBleStreamHandler alloc] init];
  [servicesDiscoveredChannel setStreamHandler:servicesDiscoveredStreamHandler];
  instance.servicesDiscoveredStreamHandler = servicesDiscoveredStreamHandler;

  // CHARACTERISTIC READ
  FlutterBleStreamHandler* characteristicReadStreamHandler = [[FlutterBleStreamHandler alloc] init];
  [characteristicReadChannel setStreamHandler:characteristicReadStreamHandler];
  instance.characteristicReadStreamHandler = characteristicReadStreamHandler;

  // DESCRIPTOR READ
  FlutterBleStreamHandler* descriptorReadStreamHandler = [[FlutterBleStreamHandler alloc] init];
  [descriptorReadChannel setStreamHandler:descriptorReadStreamHandler];
  instance.descriptorReadStreamHandler = descriptorReadStreamHandler;

  [registrar addMethodCallDelegate:instance channel:channel];
}

- (void)handleMethodCall:(FlutterMethodCall*)call result:(FlutterResult)result {
  if ([@"setLogLevel" isEqualToString:call.method]) {
    NSNumber *logLevelIndex = [call arguments];
    _logLevel = (LogLevel)[logLevelIndex integerValue];
    result(nil);
  } else if ([@"state" isEqualToString:call.method]) {
    FlutterStandardTypedData *data = [self toFlutterData:[self toBluetoothStateProto:self->_centralManager.state]];
    result(data);
  } else if([@"isAvailable" isEqualToString:call.method]) {
    if(self.centralManager.state != CBManagerStateUnsupported && self.centralManager.state != CBManagerStateUnknown) {
      result(@(YES));
    } else {
      result(@(NO));
    }
  } else if([@"isOn" isEqualToString:call.method]) {
    if(self.centralManager.state == CBManagerStatePoweredOn) {
      result(@(YES));
    } else {
      result(@(NO));
    }
  } else if([@"startScan" isEqualToString:call.method]) {
    // Clear any existing scan results
    [self.scannedPeripherals removeAllObjects];
    // TODO: Request Permission?
    FlutterStandardTypedData *data = [call arguments];
    ProtosScanSettings *request = [[ProtosScanSettings alloc] initWithData:[data data] error:nil];
    // UUID Service filter
    NSArray *uuids = [NSArray array];
    for (int i = 0; i < [request serviceUuidsArray_Count]; i++) {
      NSString *u = [request.serviceUuidsArray objectAtIndex:i];
      uuids = [uuids arrayByAddingObject:[CBUUID UUIDWithString:u]];
    }
    // TODO: iOS Scan Options (#35)
    [self->_centralManager scanForPeripheralsWithServices:uuids options:nil];
    result(nil);
  } else if([@"stopScan" isEqualToString:call.method]) {
    [self->_centralManager stopScan];
    result(nil);
  } else if([@"connect" isEqualToString:call.method]) {
    FlutterStandardTypedData *data = [call arguments];
    ProtosConnectRequest *request = [[ProtosConnectRequest alloc] initWithData:[data data] error:nil];
    NSString *remoteId = [request remoteId];
    @try {
      CBPeripheral *peripheral = [_scannedPeripherals objectForKey:remoteId];
      if(peripheral == nil) {
        @throw [FlutterError errorWithCode:@"connect"
                                   message:@"Peripheral not found"
                                   details:nil];
      }
      // TODO: Implement Connect options (#36)
      [_centralManager connectPeripheral:peripheral options:nil];
      result(nil);
    } @catch(FlutterError *e) {
      result(e);
    }
  } else if([@"disconnect" isEqualToString:call.method]) {
    NSString *remoteId = [call arguments];
    @try {
      CBPeripheral *peripheral = [self findPeripheral:remoteId];
      [_centralManager cancelPeripheralConnection:peripheral];
      result(nil);
    } @catch(FlutterError *e) {
      result(e);
    }
  } else if([@"deviceState" isEqualToString:call.method]) {
    NSString *remoteId = [call arguments];
    @try {
      CBPeripheral *peripheral = [self findPeripheral:remoteId];
      result([self toFlutterData:[self toDeviceStateProto:peripheral state:peripheral.state]]);
    } @catch(FlutterError *e) {
      result(e);
    }
  } else if([@"discoverServices" isEqualToString:call.method]) {
    NSString *remoteId = [call arguments];
    @try {
      CBPeripheral *peripheral = [self findPeripheral:remoteId];
      // Clear helper arrays
      [_servicesThatNeedDiscovered removeAllObjects];
      [_characteristicsThatNeedDiscovered removeAllObjects ];
      [peripheral discoverServices:nil];
      result(nil);
    } @catch(FlutterError *e) {
      result(e);
    }
  } else if([@"services" isEqualToString:call.method]) {
    NSString *remoteId = [call arguments];
    @try {
      CBPeripheral *peripheral = [self findPeripheral:remoteId];
      result([self toFlutterData:[self toServicesResultProto:peripheral]]);
    } @catch(FlutterError *e) {
      result(e);
    }
  } else if([@"readCharacteristic" isEqualToString:call.method]) {
    FlutterStandardTypedData *data = [call arguments];
    ProtosReadCharacteristicRequest *request = [[ProtosReadCharacteristicRequest alloc] initWithData:[data data] error:nil];
    NSString *remoteId = [request remoteId];
    @try {
      // Find peripheral
      CBPeripheral *peripheral = [self findPeripheral:remoteId];
      // Find characteristic
      CBCharacteristic *characteristic = [self locateCharacteristic:[request characteristicUuid] peripheral:peripheral serviceId:[request serviceUuid] secondaryServiceId:[request secondaryServiceUuid]];
      // Trigger a read
      [peripheral readValueForCharacteristic:characteristic];
      result(nil);
    } @catch(FlutterError *e) {
      result(e);
    }
  } else if([@"readDescriptor" isEqualToString:call.method]) {
    FlutterStandardTypedData *data = [call arguments];
    ProtosReadDescriptorRequest *request = [[ProtosReadDescriptorRequest alloc] initWithData:[data data] error:nil];
    NSString *remoteId = [request remoteId];
    @try {
      // Find peripheral
      CBPeripheral *peripheral = [self findPeripheral:remoteId];
      // Find characteristic
      CBCharacteristic *characteristic = [self locateCharacteristic:[request characteristicUuid] peripheral:peripheral serviceId:[request serviceUuid] secondaryServiceId:[request secondaryServiceUuid]];
      // Find descriptor
      CBDescriptor *descriptor = [self locateDescriptor:[request descriptorUuid] characteristic:characteristic];
      [peripheral readValueForDescriptor:descriptor];
      result(nil);
    } @catch(FlutterError *e) {
      result(e);
    }
  } else if([@"writeCharacteristic" isEqualToString:call.method]) {
    FlutterStandardTypedData *data = [call arguments];
    ProtosWriteCharacteristicRequest *request = [[ProtosWriteCharacteristicRequest alloc] initWithData:[data data] error:nil];
    NSString *remoteId = [request remoteId];
    @try {
      // Find peripheral
      CBPeripheral *peripheral = [self findPeripheral:remoteId];
      // Find characteristic
      CBCharacteristic *characteristic = [self locateCharacteristic:[request characteristicUuid] peripheral:peripheral serviceId:[request serviceUuid] secondaryServiceId:[request secondaryServiceUuid]];
      // Get correct write type
      CBCharacteristicWriteType type = ([request writeType] == ProtosWriteCharacteristicRequest_WriteType_WithoutResponse) ? CBCharacteristicWriteWithoutResponse : CBCharacteristicWriteWithResponse;
      // Write to characteristic
      [peripheral writeValue:[request value] forCharacteristic:characteristic type:type];
      result(nil);
    } @catch(FlutterError *e) {
      result(e);
    }
  } else if([@"writeDescriptor" isEqualToString:call.method]) {
    FlutterStandardTypedData *data = [call arguments];
    ProtosWriteDescriptorRequest *request = [[ProtosWriteDescriptorRequest alloc] initWithData:[data data] error:nil];
    NSString *remoteId = [request remoteId];
    @try {
      // Find peripheral
      CBPeripheral *peripheral = [self findPeripheral:remoteId];
      // Find characteristic
      CBCharacteristic *characteristic = [self locateCharacteristic:[request characteristicUuid] peripheral:peripheral serviceId:[request serviceUuid] secondaryServiceId:[request secondaryServiceUuid]];
      // Find descriptor
      CBDescriptor *descriptor = [self locateDescriptor:[request descriptorUuid] characteristic:characteristic];
      // Write descriptor
      [peripheral writeValue:[request value] forDescriptor:descriptor];
      result(nil);
    } @catch(FlutterError *e) {
      result(e);
    }
  } else if([@"setNotification" isEqualToString:call.method]) {
    FlutterStandardTypedData *data = [call arguments];
    ProtosSetNotificationRequest *request = [[ProtosSetNotificationRequest alloc] initWithData:[data data] error:nil];
    NSString *remoteId = [request remoteId];
    @try {
      // Find peripheral
      CBPeripheral *peripheral = [self findPeripheral:remoteId];
      // Find characteristic
      CBCharacteristic *characteristic = [self locateCharacteristic:[request characteristicUuid] peripheral:peripheral serviceId:[request serviceUuid] secondaryServiceId:[request secondaryServiceUuid]];
      // Set notification value
      [peripheral setNotifyValue:[request enable] forCharacteristic:characteristic];
      result(nil);
    } @catch(FlutterError *e) {
      result(e);
    }
  } else {
    result(FlutterMethodNotImplemented);
  }
}

- (CBPeripheral*)findPeripheral:(NSString*)remoteId {
  NSArray<CBPeripheral*> *peripherals = [_centralManager retrievePeripheralsWithIdentifiers:@[[[NSUUID alloc] initWithUUIDString:remoteId]]];
  CBPeripheral *peripheral;
  for(CBPeripheral *p in peripherals) {
    if([[p.identifier UUIDString] isEqualToString:remoteId]) {
      peripheral = p;
      break;
    }
  }
  if(peripheral == nil) {
    @throw [FlutterError errorWithCode:@"findPeripheral"
                               message:@"Peripheral not found"
                               details:nil];
  }
  return peripheral;
}

- (CBCharacteristic*)locateCharacteristic:(NSString*)characteristicId peripheral:(CBPeripheral*)peripheral serviceId:(NSString*)serviceId secondaryServiceId:(NSString*)secondaryServiceId {
  CBService *primaryService = [self getServiceFromArray:serviceId array:[peripheral services]];
  if(primaryService == nil || [primaryService isPrimary] == false) {
    @throw [FlutterError errorWithCode:@"locateCharacteristic"
                               message:@"service could not be located on the device"
                               details:nil];
  }
  CBService *secondaryService;
  if(secondaryServiceId.length) {
    secondaryService = [self getServiceFromArray:secondaryServiceId array:[primaryService includedServices]];
    @throw [FlutterError errorWithCode:@"locateCharacteristic"
                               message:@"secondary service could not be located on the device"
                               details:secondaryServiceId];
  }
  CBService *service = (secondaryService != nil) ? secondaryService : primaryService;
  CBCharacteristic *characteristic = [self getCharacteristicFromArray:characteristicId array:[service characteristics]];
  if(characteristic == nil) {
    @throw [FlutterError errorWithCode:@"locateCharacteristic"
                               message:@"characteristic could not be located on the device"
                               details:nil];
  }
  return characteristic;
}

- (CBDescriptor*)locateDescriptor:(NSString*)descriptorId characteristic:(CBCharacteristic*)characteristic {
  CBDescriptor *descriptor = [self getDescriptorFromArray:descriptorId array:[characteristic descriptors]];
  if(descriptor == nil) {
    @throw [FlutterError errorWithCode:@"locateDescriptor"
                               message:@"descriptor could not be located on the device"
                               details:nil];
  }
  return descriptor;
}

// Reverse search to find primary service
- (CBService*)findPrimaryService:(CBService*)secondaryService peripheral:(CBPeripheral*)peripheral {
  for(CBService *s in [peripheral services]) {
    for(CBService *ss in [s includedServices]) {
      if([[ss.UUID UUIDString] isEqualToString:[secondaryService.UUID UUIDString]]) {
        return s;
      }
    }
  }
  return nil;
}

- (CBDescriptor*)findCCCDescriptor:(CBCharacteristic*)characteristic {
  for(CBDescriptor *d in characteristic.descriptors) {
    if([d.UUID.UUIDString isEqualToString:@"2902"]) {
      return d;
    }
  }
  return nil;
}

- (CBService*)getServiceFromArray:(NSString*)uuidString array:(NSArray<CBService*>*)array {
  for(CBService *s in array) {
    if([[s UUID] isEqual:[CBUUID UUIDWithString:uuidString]]) {
      return s;
    }
  }
  return nil;
}

- (CBCharacteristic*)getCharacteristicFromArray:(NSString*)uuidString array:(NSArray<CBCharacteristic*>*)array {
  for(CBCharacteristic *c in array) {
    if([[c UUID] isEqual:[CBUUID UUIDWithString:uuidString]]) {
      return c;
    }
  }
  return nil;
}

- (CBDescriptor*)getDescriptorFromArray:(NSString*)uuidString array:(NSArray<CBDescriptor*>*)array {
  for(CBDescriptor *d in array) {
    if([[d UUID] isEqual:[CBUUID UUIDWithString:uuidString]]) {
      return d;
    }
  }
  return nil;
}

//
// CBCentralManagerDelegate methods
//
- (void)centralManagerDidUpdateState:(nonnull CBCentralManager *)central {
  if(_stateStreamHandler.sink != nil) {
    FlutterStandardTypedData *data = [self toFlutterData:[self toBluetoothStateProto:self->_centralManager.state]];
    self.stateStreamHandler.sink(data);
  }
}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData RSSI:(NSNumber *)RSSI {
  [self.scannedPeripherals setObject:peripheral
                              forKey:[[peripheral identifier] UUIDString]];
  if(_scanResultStreamHandler.sink != nil) {
    FlutterStandardTypedData *data = [self toFlutterData:[self toScanResultProto:peripheral advertisementData:advertisementData RSSI:RSSI]];
    _scanResultStreamHandler.sink(data);
  }
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral {
  NSLog(@"didConnectPeripheral");
  // Register self as delegate for peripheral
  peripheral.delegate = self;

  // Send connection state
  [_channel invokeMethod:@"DeviceState" arguments:[self toFlutterData:[self toDeviceStateProto:peripheral state:peripheral.state]]];
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error {
  NSLog(@"didDisconnectPeripheral");
  // Unregister self as delegate for peripheral, not working #42
  peripheral.delegate = nil;

  // Send connection state
  [_channel invokeMethod:@"DeviceState" arguments:[self toFlutterData:[self toDeviceStateProto:peripheral state:peripheral.state]]];
}

- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error {
  // TODO:?
}

//
// CBPeripheralDelegate methods
//
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error {
  NSLog(@"didDiscoverServices");
  // Loop through and discover characteristics and secondary services
  [_servicesThatNeedDiscovered addObjectsFromArray:peripheral.services];
  for(CBService *s in [peripheral services]) {
    NSLog(@"Found service: %@", [s.UUID UUIDString]);
    [peripheral discoverCharacteristics:nil forService:s];
    // [peripheral discoverIncludedServices:nil forService:s]; // Secondary services in the future (#8)
  }
}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error {
  NSLog(@"didDiscoverCharacteristicsForService");
  // Loop through and discover descriptors for characteristics
  [_servicesThatNeedDiscovered removeObject:service];
  [_characteristicsThatNeedDiscovered addObjectsFromArray:service.characteristics];
  for(CBCharacteristic *c in [service characteristics]) {
    [peripheral discoverDescriptorsForCharacteristic:c];
  }
}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverDescriptorsForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
  NSLog(@"didDiscoverDescriptorsForCharacteristic");
  [_characteristicsThatNeedDiscovered removeObject:characteristic];
  if(_servicesThatNeedDiscovered.count > 0 || _characteristicsThatNeedDiscovered.count > 0) {
    // Still discovering
    return;
  }
  // Send updated tree
  if(_servicesDiscoveredStreamHandler.sink != nil) {
    ProtosDiscoverServicesResult *result = [self toServicesResultProto:peripheral];
    _servicesDiscoveredStreamHandler.sink([self toFlutterData:result]);
  }
}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverIncludedServicesForService:(CBService *)service error:(NSError *)error {
  NSLog(@"didDiscoverIncludedServicesForService");
  // Loop through and discover characteristics for secondary services
  for(CBService *ss in [service includedServices]) {
    [peripheral discoverCharacteristics:nil forService:ss];
  }
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
  NSLog(@"didUpdateValueForCharacteristic %@", [peripheral.identifier UUIDString]);
  if(_characteristicReadStreamHandler.sink != nil) {
    ProtosReadCharacteristicResponse *result = [[ProtosReadCharacteristicResponse alloc] init];
    [result setRemoteId:[peripheral.identifier UUIDString]];
    [result setCharacteristic:[self toCharacteristicProto:characteristic]];
    _characteristicReadStreamHandler.sink([self toFlutterData:result]);
  }
  // on iOS, this method also handle notification values
  ProtosOnNotificationResponse *result = [[ProtosOnNotificationResponse alloc] init];
  [result setRemoteId:[peripheral.identifier UUIDString]];
  [result setCharacteristic:[self toCharacteristicProto:characteristic]];
  [_channel invokeMethod:@"OnValueChanged" arguments:[self toFlutterData:result]];
}

- (void)peripheral:(CBPeripheral *)peripheral didWriteValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
  NSLog(@"didWriteValueForCharacteristic");
  ProtosWriteCharacteristicRequest *request = [[ProtosWriteCharacteristicRequest alloc] init];
  [request setRemoteId:[peripheral.identifier UUIDString]];
  [request setCharacteristicUuid:[characteristic.UUID fullUUIDString]];
  [request setServiceUuid:[characteristic.service.UUID fullUUIDString]];
  ProtosWriteCharacteristicResponse *result = [[ProtosWriteCharacteristicResponse alloc] init];
  [result setRequest:request];
  [result setSuccess:(error == nil)];
  [_channel invokeMethod:@"WriteCharacteristicResponse" arguments:[self toFlutterData:result]];
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
  NSLog(@"didUpdateNotificationStateForCharacteristic");
  // Read CCC descriptor of characteristic
  CBDescriptor *cccd = [self findCCCDescriptor:characteristic];
  if(cccd == nil || error != nil) {
    // Send error
    ProtosSetNotificationResponse *response = [[ProtosSetNotificationResponse alloc] init];
    [response setRemoteId:[peripheral.identifier UUIDString]];
    [response setCharacteristic:[self toCharacteristicProto:characteristic]];
    [response setSuccess:false];
    [_channel invokeMethod:@"SetNotificationResponse" arguments:[self toFlutterData:response]];
    return;
  }

  // Request a read
  [peripheral readValueForDescriptor:cccd];
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForDescriptor:(CBDescriptor *)descriptor error:(NSError *)error {
  if(_descriptorReadStreamHandler.sink != nil) {
    ProtosReadDescriptorRequest *q = [[ProtosReadDescriptorRequest alloc] init];
    [q setRemoteId:[peripheral.identifier UUIDString]];
    [q setCharacteristicUuid:[descriptor.characteristic.UUID fullUUIDString]];
    [q setDescriptorUuid:[descriptor.UUID fullUUIDString]];
    if([descriptor.characteristic.service isPrimary]) {
      [q setServiceUuid:[descriptor.characteristic.service.UUID fullUUIDString]];
    } else {
      [q setSecondaryServiceUuid:[descriptor.characteristic.service.UUID fullUUIDString]];
      CBService *primaryService = [self findPrimaryService:[descriptor.characteristic service] peripheral:[descriptor.characteristic.service peripheral]];
      [q setServiceUuid:[primaryService.UUID fullUUIDString]];
    }
    ProtosReadDescriptorResponse *result = [[ProtosReadDescriptorResponse alloc] init];
    [result setRequest:q];
    int value = [descriptor.value intValue];
    [result setValue:[NSData dataWithBytes:&value length:sizeof(value)]];
    _descriptorReadStreamHandler.sink([self toFlutterData:result]);
  }
  // If descriptor is CCCD, send a SetNotificationResponse in case anything is awaiting
  if([descriptor.UUID.UUIDString isEqualToString:@"2902"]){
    ProtosSetNotificationResponse *response = [[ProtosSetNotificationResponse alloc] init];
    [response setRemoteId:[peripheral.identifier UUIDString]];
    [response setCharacteristic:[self toCharacteristicProto:descriptor.characteristic]];
    [response setSuccess:true];
    [_channel invokeMethod:@"SetNotificationResponse" arguments:[self toFlutterData:response]];
  }
}

- (void)peripheral:(CBPeripheral *)peripheral didWriteValueForDescriptor:(CBDescriptor *)descriptor error:(NSError *)error {
  ProtosWriteDescriptorRequest *request = [[ProtosWriteDescriptorRequest alloc] init];
  [request setRemoteId:[peripheral.identifier UUIDString]];
  [request setCharacteristicUuid:[descriptor.characteristic.UUID fullUUIDString]];
  [request setDescriptorUuid:[descriptor.UUID fullUUIDString]];
  if([descriptor.characteristic.service isPrimary]) {
    [request setServiceUuid:[descriptor.characteristic.service.UUID fullUUIDString]];
  } else {
    [request setSecondaryServiceUuid:[descriptor.characteristic.service.UUID fullUUIDString]];
    CBService *primaryService = [self findPrimaryService:[descriptor.characteristic service] peripheral:[descriptor.characteristic.service peripheral]];
    [request setServiceUuid:[primaryService.UUID fullUUIDString]];
  }
  ProtosWriteDescriptorResponse *result = [[ProtosWriteDescriptorResponse alloc] init];
  [result setRequest:request];
  [result setSuccess:(error == nil)];
  [_channel invokeMethod:@"WriteDescriptorResponse" arguments:[self toFlutterData:result]];
}

//
// Proto Helper methods
//
- (FlutterStandardTypedData*)toFlutterData:(GPBMessage*)proto {
  FlutterStandardTypedData *data = [FlutterStandardTypedData typedDataWithBytes:[[proto data] copy]];
  return data;
}

- (ProtosBluetoothState*)toBluetoothStateProto:(CBManagerState)state {
  ProtosBluetoothState *result = [[ProtosBluetoothState alloc] init];
  switch(state) {
    case CBManagerStateResetting:
      [result setState:ProtosBluetoothState_State_TurningOn];
      break;
    case CBManagerStateUnsupported:
      [result setState:ProtosBluetoothState_State_Unavailable];
      break;
    case CBManagerStateUnauthorized:
      [result setState:ProtosBluetoothState_State_Unauthorized];
      break;
    case CBManagerStatePoweredOff:
      [result setState:ProtosBluetoothState_State_Off];
      break;
    case CBManagerStatePoweredOn:
      [result setState:ProtosBluetoothState_State_On];
      break;
    default:
      [result setState:ProtosBluetoothState_State_Unknown];
      break;
  }
  return result;
}

- (ProtosScanResult*)toScanResultProto:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData RSSI:(NSNumber *)RSSI {
  ProtosScanResult *result = [[ProtosScanResult alloc] init];
  [result setDevice:[self toDeviceProto:peripheral]];
  [result setRssi:[RSSI intValue]];
  ProtosAdvertisementData *ads = [[ProtosAdvertisementData alloc] init];
  [ads setConnectable:[advertisementData[CBAdvertisementDataIsConnectable] boolValue]];
  [ads setLocalName:advertisementData[CBAdvertisementDataLocalNameKey]];
  // Tx Power Level
  NSNumber *txPower = advertisementData[CBAdvertisementDataTxPowerLevelKey];
  if(txPower != nil) {
    ProtosInt32Value *txPowerWrapper = [[ProtosInt32Value alloc] init];
    [txPowerWrapper setValue:[txPower intValue]];
    [ads setTxPowerLevel:txPowerWrapper];
  }
  // Manufacturer Specific Data
  NSData *manufData = advertisementData[CBAdvertisementDataManufacturerDataKey];
  if(manufData.length > 2) {
    unsigned short manufacturerId;
    [manufData getBytes:&manufacturerId length:2];
    [[ads manufacturerData] setObject:[manufData subdataWithRange:NSMakeRange(2, manufData.length - 2)] forKey:manufacturerId];
  }
  // Service Data
  NSDictionary *serviceData = advertisementData[CBAdvertisementDataServiceDataKey];
  for (CBUUID *uuid in serviceData) {
    [[ads serviceData] setObject:serviceData[uuid] forKey:uuid.UUIDString];
  }
  // Service Uuids
  NSArray *serviceUuids = advertisementData[CBAdvertisementDataServiceUUIDsKey];
  for (CBUUID *uuid in serviceUuids) {
    [[ads serviceUuidsArray] addObject:uuid.UUIDString];
  }
  [result setAdvertisementData:ads];
  return result;
}

- (ProtosBluetoothDevice*)toDeviceProto:(CBPeripheral *)peripheral {
  ProtosBluetoothDevice *result = [[ProtosBluetoothDevice alloc] init];
  [result setName:[peripheral name]];
  [result setRemoteId:[[peripheral identifier] UUIDString]];
  [result setType:ProtosBluetoothDevice_Type_Le]; // TODO: Does iOS differentiate?
  return result;
}

- (ProtosDeviceStateResponse*)toDeviceStateProto:(CBPeripheral *)peripheral state:(CBPeripheralState)state {
  ProtosDeviceStateResponse *result = [[ProtosDeviceStateResponse alloc] init];
  switch(state) {
    case CBPeripheralStateDisconnected:
      [result setState:ProtosDeviceStateResponse_BluetoothDeviceState_Disconnected];
      break;
    case CBPeripheralStateConnecting:
      [result setState:ProtosDeviceStateResponse_BluetoothDeviceState_Connecting];
      break;
    case CBPeripheralStateConnected:
      [result setState:ProtosDeviceStateResponse_BluetoothDeviceState_Connected];
      break;
    case CBPeripheralStateDisconnecting:
      [result setState:ProtosDeviceStateResponse_BluetoothDeviceState_Disconnecting];
      break;
  }
  [result setRemoteId:[[peripheral identifier] UUIDString]];
  return result;
}

- (ProtosDiscoverServicesResult*)toServicesResultProto:(CBPeripheral *)peripheral {
  ProtosDiscoverServicesResult *result = [[ProtosDiscoverServicesResult alloc] init];
  [result setRemoteId:[peripheral.identifier UUIDString]];
  NSMutableArray *servicesProtos = [NSMutableArray new];
  for(CBService *s in [peripheral services]) {
    [servicesProtos addObject:[self toServiceProto:peripheral service:s]];
  }
  [result setServicesArray:servicesProtos];
  return result;
}

- (ProtosBluetoothService*)toServiceProto:(CBPeripheral *)peripheral service:(CBService *)service  {
  ProtosBluetoothService *result = [[ProtosBluetoothService alloc] init];
  NSLog(@"peripheral uuid:%@", [peripheral.identifier UUIDString]);
  NSLog(@"service uuid:%@", [service.UUID fullUUIDString]);
  [result setRemoteId:[peripheral.identifier UUIDString]];
  [result setUuid:[service.UUID fullUUIDString]];
  [result setIsPrimary:[service isPrimary]];

  // Characteristic Array
  NSMutableArray *characteristicProtos = [NSMutableArray new];
  for(CBCharacteristic *c in [service characteristics]) {
    [characteristicProtos addObject:[self toCharacteristicProto:c]];
  }
  [result setCharacteristicsArray:characteristicProtos];

  // Included Services Array
  NSMutableArray *includedServicesProtos = [NSMutableArray new];
  for(CBService *s in [service includedServices]) {
    [includedServicesProtos addObject:[self toServiceProto:peripheral service:s]];
  }
  [result setIncludedServicesArray:includedServicesProtos];

  return result;
}

- (ProtosBluetoothCharacteristic*)toCharacteristicProto:(CBCharacteristic *)characteristic {
  ProtosBluetoothCharacteristic *result = [[ProtosBluetoothCharacteristic alloc] init];
  [result setUuid:[characteristic.UUID fullUUIDString]];
  [result setProperties:[self toCharacteristicPropsProto:characteristic.properties]];
  [result setValue:[characteristic value]];
  NSLog(@"uuid: %@ value: %@", [characteristic.UUID fullUUIDString], [characteristic value]);
  NSMutableArray *descriptorProtos = [NSMutableArray new];
  for(CBDescriptor *d in [characteristic descriptors]) {
    [descriptorProtos addObject:[self toDescriptorProto:d]];
  }
  [result setDescriptorsArray:descriptorProtos];
  if([characteristic.service isPrimary]) {
    [result setServiceUuid:[characteristic.service.UUID fullUUIDString]];
  } else {
    // Reverse search to find service and secondary service UUID
    [result setSecondaryServiceUuid:[characteristic.service.UUID fullUUIDString]];
    CBService *primaryService = [self findPrimaryService:[characteristic service] peripheral:[characteristic.service peripheral]];
    [result setServiceUuid:[primaryService.UUID fullUUIDString]];
  }
  return result;
}

- (ProtosBluetoothDescriptor*)toDescriptorProto:(CBDescriptor *)descriptor {
  ProtosBluetoothDescriptor *result = [[ProtosBluetoothDescriptor alloc] init];
  [result setUuid:[descriptor.UUID fullUUIDString]];
  [result setCharacteristicUuid:[descriptor.characteristic.UUID fullUUIDString]];
  [result setServiceUuid:[descriptor.characteristic.service.UUID fullUUIDString]];
  int value = [descriptor.value intValue];
  [result setValue:[NSData dataWithBytes:&value length:sizeof(value)]];
  return result;
}

- (ProtosCharacteristicProperties*)toCharacteristicPropsProto:(CBCharacteristicProperties)props {
  ProtosCharacteristicProperties *result = [[ProtosCharacteristicProperties alloc] init];
  [result setBroadcast:(props & CBCharacteristicPropertyBroadcast) != 0];
  [result setRead:(props & CBCharacteristicPropertyRead) != 0];
  [result setWriteWithoutResponse:(props & CBCharacteristicPropertyWriteWithoutResponse) != 0];
  [result setWrite:(props & CBCharacteristicPropertyWrite) != 0];
  [result setNotify:(props & CBCharacteristicPropertyNotify) != 0];
  [result setIndicate:(props & CBCharacteristicPropertyIndicate) != 0];
  [result setAuthenticatedSignedWrites:(props & CBCharacteristicPropertyAuthenticatedSignedWrites) != 0];
  [result setExtendedProperties:(props & CBCharacteristicPropertyExtendedProperties) != 0];
  [result setNotifyEncryptionRequired:(props & CBCharacteristicPropertyNotifyEncryptionRequired) != 0];
  [result setIndicateEncryptionRequired:(props & CBCharacteristicPropertyIndicateEncryptionRequired) != 0];
  return result;
}


- (void)log:(LogLevel)level format:(NSString *)format, ... {
  if(level <= _logLevel) {
    va_list args;
    va_start(args, format);
//    NSString* formattedMessage = [[NSString alloc] initWithFormat:format arguments:args];
    NSLog(format, args);
    va_end(args);
  }
}

@end

@implementation FlutterBleStreamHandler

- (FlutterError*)onListenWithArguments:(id)arguments eventSink:(FlutterEventSink)eventSink {
  self.sink = eventSink;
  return nil;
}

- (FlutterError*)onCancelWithArguments:(id)arguments {
  self.sink = nil;
  return nil;
}

@end
