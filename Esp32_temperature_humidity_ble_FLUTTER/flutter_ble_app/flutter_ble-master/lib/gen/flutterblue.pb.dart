///
//  Generated code. Do not modify.
//  source: flutterblue.proto
///
// ignore_for_file: camel_case_types,non_constant_identifier_names,library_prefixes,unused_import,unused_shown_name

import 'dart:core' as $core show bool, Deprecated, double, int, List, Map, override, String;

import 'package:protobuf/protobuf.dart' as $pb;

import 'flutterblue.pbenum.dart';

export 'flutterblue.pbenum.dart';

class Int32Value extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('Int32Value')
    ..a<$core.int>(1, 'value', $pb.PbFieldType.O3)
    ..hasRequiredFields = false
  ;

  Int32Value() : super();
  Int32Value.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  Int32Value.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  Int32Value clone() => Int32Value()..mergeFromMessage(this);
  Int32Value copyWith(void Function(Int32Value) updates) => super.copyWith((message) => updates(message as Int32Value));
  $pb.BuilderInfo get info_ => _i;
  static Int32Value create() => Int32Value();
  Int32Value createEmptyInstance() => create();
  static $pb.PbList<Int32Value> createRepeated() => $pb.PbList<Int32Value>();
  static Int32Value getDefault() => _defaultInstance ??= create()..freeze();
  static Int32Value _defaultInstance;

  $core.int get value => $_get(0, 0);
  set value($core.int v) { $_setSignedInt32(0, v); }
  $core.bool hasValue() => $_has(0);
  void clearValue() => clearField(1);
}

class BluetoothState extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('BluetoothState')
    ..e<BluetoothState_State>(1, 'state', $pb.PbFieldType.OE, BluetoothState_State.UNKNOWN, BluetoothState_State.valueOf, BluetoothState_State.values)
    ..hasRequiredFields = false
  ;

  BluetoothState() : super();
  BluetoothState.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  BluetoothState.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  BluetoothState clone() => BluetoothState()..mergeFromMessage(this);
  BluetoothState copyWith(void Function(BluetoothState) updates) => super.copyWith((message) => updates(message as BluetoothState));
  $pb.BuilderInfo get info_ => _i;
  static BluetoothState create() => BluetoothState();
  BluetoothState createEmptyInstance() => create();
  static $pb.PbList<BluetoothState> createRepeated() => $pb.PbList<BluetoothState>();
  static BluetoothState getDefault() => _defaultInstance ??= create()..freeze();
  static BluetoothState _defaultInstance;

  BluetoothState_State get state => $_getN(0);
  set state(BluetoothState_State v) { setField(1, v); }
  $core.bool hasState() => $_has(0);
  void clearState() => clearField(1);
}

class AdvertisementData extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('AdvertisementData')
    ..aOS(1, 'localName')
    ..a<Int32Value>(2, 'txPowerLevel', $pb.PbFieldType.OM, Int32Value.getDefault, Int32Value.create)
    ..aOB(3, 'connectable')
    ..m<$core.int, $core.List<$core.int>>(4, 'manufacturerData', 'AdvertisementData.ManufacturerDataEntry',$pb.PbFieldType.O3, $pb.PbFieldType.OY, null, null, null )
    ..m<$core.String, $core.List<$core.int>>(5, 'serviceData', 'AdvertisementData.ServiceDataEntry',$pb.PbFieldType.OS, $pb.PbFieldType.OY, null, null, null )
    ..pPS(6, 'serviceUuids')
    ..hasRequiredFields = false
  ;

  AdvertisementData() : super();
  AdvertisementData.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  AdvertisementData.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  AdvertisementData clone() => AdvertisementData()..mergeFromMessage(this);
  AdvertisementData copyWith(void Function(AdvertisementData) updates) => super.copyWith((message) => updates(message as AdvertisementData));
  $pb.BuilderInfo get info_ => _i;
  static AdvertisementData create() => AdvertisementData();
  AdvertisementData createEmptyInstance() => create();
  static $pb.PbList<AdvertisementData> createRepeated() => $pb.PbList<AdvertisementData>();
  static AdvertisementData getDefault() => _defaultInstance ??= create()..freeze();
  static AdvertisementData _defaultInstance;

  $core.String get localName => $_getS(0, '');
  set localName($core.String v) { $_setString(0, v); }
  $core.bool hasLocalName() => $_has(0);
  void clearLocalName() => clearField(1);

  Int32Value get txPowerLevel => $_getN(1);
  set txPowerLevel(Int32Value v) { setField(2, v); }
  $core.bool hasTxPowerLevel() => $_has(1);
  void clearTxPowerLevel() => clearField(2);

  $core.bool get connectable => $_get(2, false);
  set connectable($core.bool v) { $_setBool(2, v); }
  $core.bool hasConnectable() => $_has(2);
  void clearConnectable() => clearField(3);

  $core.Map<$core.int, $core.List<$core.int>> get manufacturerData => $_getMap(3);

  $core.Map<$core.String, $core.List<$core.int>> get serviceData => $_getMap(4);

  $core.List<$core.String> get serviceUuids => $_getList(5);
}

class ScanSettings extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('ScanSettings')
    ..a<$core.int>(1, 'androidScanMode', $pb.PbFieldType.O3)
    ..pPS(2, 'serviceUuids')
    ..hasRequiredFields = false
  ;

  ScanSettings() : super();
  ScanSettings.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  ScanSettings.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  ScanSettings clone() => ScanSettings()..mergeFromMessage(this);
  ScanSettings copyWith(void Function(ScanSettings) updates) => super.copyWith((message) => updates(message as ScanSettings));
  $pb.BuilderInfo get info_ => _i;
  static ScanSettings create() => ScanSettings();
  ScanSettings createEmptyInstance() => create();
  static $pb.PbList<ScanSettings> createRepeated() => $pb.PbList<ScanSettings>();
  static ScanSettings getDefault() => _defaultInstance ??= create()..freeze();
  static ScanSettings _defaultInstance;

  $core.int get androidScanMode => $_get(0, 0);
  set androidScanMode($core.int v) { $_setSignedInt32(0, v); }
  $core.bool hasAndroidScanMode() => $_has(0);
  void clearAndroidScanMode() => clearField(1);

  $core.List<$core.String> get serviceUuids => $_getList(1);
}

class ScanResult extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('ScanResult')
    ..a<BluetoothDevice>(1, 'device', $pb.PbFieldType.OM, BluetoothDevice.getDefault, BluetoothDevice.create)
    ..a<AdvertisementData>(2, 'advertisementData', $pb.PbFieldType.OM, AdvertisementData.getDefault, AdvertisementData.create)
    ..a<$core.int>(3, 'rssi', $pb.PbFieldType.O3)
    ..hasRequiredFields = false
  ;

  ScanResult() : super();
  ScanResult.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  ScanResult.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  ScanResult clone() => ScanResult()..mergeFromMessage(this);
  ScanResult copyWith(void Function(ScanResult) updates) => super.copyWith((message) => updates(message as ScanResult));
  $pb.BuilderInfo get info_ => _i;
  static ScanResult create() => ScanResult();
  ScanResult createEmptyInstance() => create();
  static $pb.PbList<ScanResult> createRepeated() => $pb.PbList<ScanResult>();
  static ScanResult getDefault() => _defaultInstance ??= create()..freeze();
  static ScanResult _defaultInstance;

  BluetoothDevice get device => $_getN(0);
  set device(BluetoothDevice v) { setField(1, v); }
  $core.bool hasDevice() => $_has(0);
  void clearDevice() => clearField(1);

  AdvertisementData get advertisementData => $_getN(1);
  set advertisementData(AdvertisementData v) { setField(2, v); }
  $core.bool hasAdvertisementData() => $_has(1);
  void clearAdvertisementData() => clearField(2);

  $core.int get rssi => $_get(2, 0);
  set rssi($core.int v) { $_setSignedInt32(2, v); }
  $core.bool hasRssi() => $_has(2);
  void clearRssi() => clearField(3);
}

class ConnectRequest extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('ConnectRequest')
    ..aOS(1, 'remoteId')
    ..aOB(2, 'androidAutoConnect')
    ..hasRequiredFields = false
  ;

  ConnectRequest() : super();
  ConnectRequest.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  ConnectRequest.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  ConnectRequest clone() => ConnectRequest()..mergeFromMessage(this);
  ConnectRequest copyWith(void Function(ConnectRequest) updates) => super.copyWith((message) => updates(message as ConnectRequest));
  $pb.BuilderInfo get info_ => _i;
  static ConnectRequest create() => ConnectRequest();
  ConnectRequest createEmptyInstance() => create();
  static $pb.PbList<ConnectRequest> createRepeated() => $pb.PbList<ConnectRequest>();
  static ConnectRequest getDefault() => _defaultInstance ??= create()..freeze();
  static ConnectRequest _defaultInstance;

  $core.String get remoteId => $_getS(0, '');
  set remoteId($core.String v) { $_setString(0, v); }
  $core.bool hasRemoteId() => $_has(0);
  void clearRemoteId() => clearField(1);

  $core.bool get androidAutoConnect => $_get(1, false);
  set androidAutoConnect($core.bool v) { $_setBool(1, v); }
  $core.bool hasAndroidAutoConnect() => $_has(1);
  void clearAndroidAutoConnect() => clearField(2);
}

class BluetoothDevice extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('BluetoothDevice')
    ..aOS(1, 'remoteId')
    ..aOS(2, 'name')
    ..e<BluetoothDevice_Type>(3, 'type', $pb.PbFieldType.OE, BluetoothDevice_Type.UNKNOWN, BluetoothDevice_Type.valueOf, BluetoothDevice_Type.values)
    ..hasRequiredFields = false
  ;

  BluetoothDevice() : super();
  BluetoothDevice.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  BluetoothDevice.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  BluetoothDevice clone() => BluetoothDevice()..mergeFromMessage(this);
  BluetoothDevice copyWith(void Function(BluetoothDevice) updates) => super.copyWith((message) => updates(message as BluetoothDevice));
  $pb.BuilderInfo get info_ => _i;
  static BluetoothDevice create() => BluetoothDevice();
  BluetoothDevice createEmptyInstance() => create();
  static $pb.PbList<BluetoothDevice> createRepeated() => $pb.PbList<BluetoothDevice>();
  static BluetoothDevice getDefault() => _defaultInstance ??= create()..freeze();
  static BluetoothDevice _defaultInstance;

  $core.String get remoteId => $_getS(0, '');
  set remoteId($core.String v) { $_setString(0, v); }
  $core.bool hasRemoteId() => $_has(0);
  void clearRemoteId() => clearField(1);

  $core.String get name => $_getS(1, '');
  set name($core.String v) { $_setString(1, v); }
  $core.bool hasName() => $_has(1);
  void clearName() => clearField(2);

  BluetoothDevice_Type get type => $_getN(2);
  set type(BluetoothDevice_Type v) { setField(3, v); }
  $core.bool hasType() => $_has(2);
  void clearType() => clearField(3);
}

class BluetoothService extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('BluetoothService')
    ..aOS(1, 'uuid')
    ..aOS(2, 'remoteId')
    ..aOB(3, 'isPrimary')
    ..pc<BluetoothCharacteristic>(4, 'characteristics', $pb.PbFieldType.PM,BluetoothCharacteristic.create)
    ..pc<BluetoothService>(5, 'includedServices', $pb.PbFieldType.PM,BluetoothService.create)
    ..hasRequiredFields = false
  ;

  BluetoothService() : super();
  BluetoothService.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  BluetoothService.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  BluetoothService clone() => BluetoothService()..mergeFromMessage(this);
  BluetoothService copyWith(void Function(BluetoothService) updates) => super.copyWith((message) => updates(message as BluetoothService));
  $pb.BuilderInfo get info_ => _i;
  static BluetoothService create() => BluetoothService();
  BluetoothService createEmptyInstance() => create();
  static $pb.PbList<BluetoothService> createRepeated() => $pb.PbList<BluetoothService>();
  static BluetoothService getDefault() => _defaultInstance ??= create()..freeze();
  static BluetoothService _defaultInstance;

  $core.String get uuid => $_getS(0, '');
  set uuid($core.String v) { $_setString(0, v); }
  $core.bool hasUuid() => $_has(0);
  void clearUuid() => clearField(1);

  $core.String get remoteId => $_getS(1, '');
  set remoteId($core.String v) { $_setString(1, v); }
  $core.bool hasRemoteId() => $_has(1);
  void clearRemoteId() => clearField(2);

  $core.bool get isPrimary => $_get(2, false);
  set isPrimary($core.bool v) { $_setBool(2, v); }
  $core.bool hasIsPrimary() => $_has(2);
  void clearIsPrimary() => clearField(3);

  $core.List<BluetoothCharacteristic> get characteristics => $_getList(3);

  $core.List<BluetoothService> get includedServices => $_getList(4);
}

class BluetoothCharacteristic extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('BluetoothCharacteristic')
    ..aOS(1, 'uuid')
    ..aOS(2, 'serviceUuid')
    ..aOS(3, 'secondaryServiceUuid')
    ..pc<BluetoothDescriptor>(4, 'descriptors', $pb.PbFieldType.PM,BluetoothDescriptor.create)
    ..a<CharacteristicProperties>(5, 'properties', $pb.PbFieldType.OM, CharacteristicProperties.getDefault, CharacteristicProperties.create)
    ..a<$core.List<$core.int>>(6, 'value', $pb.PbFieldType.OY)
    ..hasRequiredFields = false
  ;

  BluetoothCharacteristic() : super();
  BluetoothCharacteristic.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  BluetoothCharacteristic.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  BluetoothCharacteristic clone() => BluetoothCharacteristic()..mergeFromMessage(this);
  BluetoothCharacteristic copyWith(void Function(BluetoothCharacteristic) updates) => super.copyWith((message) => updates(message as BluetoothCharacteristic));
  $pb.BuilderInfo get info_ => _i;
  static BluetoothCharacteristic create() => BluetoothCharacteristic();
  BluetoothCharacteristic createEmptyInstance() => create();
  static $pb.PbList<BluetoothCharacteristic> createRepeated() => $pb.PbList<BluetoothCharacteristic>();
  static BluetoothCharacteristic getDefault() => _defaultInstance ??= create()..freeze();
  static BluetoothCharacteristic _defaultInstance;

  $core.String get uuid => $_getS(0, '');
  set uuid($core.String v) { $_setString(0, v); }
  $core.bool hasUuid() => $_has(0);
  void clearUuid() => clearField(1);

  $core.String get serviceUuid => $_getS(1, '');
  set serviceUuid($core.String v) { $_setString(1, v); }
  $core.bool hasServiceUuid() => $_has(1);
  void clearServiceUuid() => clearField(2);

  $core.String get secondaryServiceUuid => $_getS(2, '');
  set secondaryServiceUuid($core.String v) { $_setString(2, v); }
  $core.bool hasSecondaryServiceUuid() => $_has(2);
  void clearSecondaryServiceUuid() => clearField(3);

  $core.List<BluetoothDescriptor> get descriptors => $_getList(3);

  CharacteristicProperties get properties => $_getN(4);
  set properties(CharacteristicProperties v) { setField(5, v); }
  $core.bool hasProperties() => $_has(4);
  void clearProperties() => clearField(5);

  $core.List<$core.int> get value => $_getN(5);
  set value($core.List<$core.int> v) { $_setBytes(5, v); }
  $core.bool hasValue() => $_has(5);
  void clearValue() => clearField(6);
}

class BluetoothDescriptor extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('BluetoothDescriptor')
    ..aOS(1, 'uuid')
    ..aOS(2, 'serviceUuid')
    ..aOS(3, 'characteristicUuid')
    ..a<$core.List<$core.int>>(4, 'value', $pb.PbFieldType.OY)
    ..hasRequiredFields = false
  ;

  BluetoothDescriptor() : super();
  BluetoothDescriptor.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  BluetoothDescriptor.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  BluetoothDescriptor clone() => BluetoothDescriptor()..mergeFromMessage(this);
  BluetoothDescriptor copyWith(void Function(BluetoothDescriptor) updates) => super.copyWith((message) => updates(message as BluetoothDescriptor));
  $pb.BuilderInfo get info_ => _i;
  static BluetoothDescriptor create() => BluetoothDescriptor();
  BluetoothDescriptor createEmptyInstance() => create();
  static $pb.PbList<BluetoothDescriptor> createRepeated() => $pb.PbList<BluetoothDescriptor>();
  static BluetoothDescriptor getDefault() => _defaultInstance ??= create()..freeze();
  static BluetoothDescriptor _defaultInstance;

  $core.String get uuid => $_getS(0, '');
  set uuid($core.String v) { $_setString(0, v); }
  $core.bool hasUuid() => $_has(0);
  void clearUuid() => clearField(1);

  $core.String get serviceUuid => $_getS(1, '');
  set serviceUuid($core.String v) { $_setString(1, v); }
  $core.bool hasServiceUuid() => $_has(1);
  void clearServiceUuid() => clearField(2);

  $core.String get characteristicUuid => $_getS(2, '');
  set characteristicUuid($core.String v) { $_setString(2, v); }
  $core.bool hasCharacteristicUuid() => $_has(2);
  void clearCharacteristicUuid() => clearField(3);

  $core.List<$core.int> get value => $_getN(3);
  set value($core.List<$core.int> v) { $_setBytes(3, v); }
  $core.bool hasValue() => $_has(3);
  void clearValue() => clearField(4);
}

class CharacteristicProperties extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('CharacteristicProperties')
    ..aOB(1, 'broadcast')
    ..aOB(2, 'read')
    ..aOB(3, 'writeWithoutResponse')
    ..aOB(4, 'write')
    ..aOB(5, 'notify')
    ..aOB(6, 'indicate')
    ..aOB(7, 'authenticatedSignedWrites')
    ..aOB(8, 'extendedProperties')
    ..aOB(9, 'notifyEncryptionRequired')
    ..aOB(10, 'indicateEncryptionRequired')
    ..hasRequiredFields = false
  ;

  CharacteristicProperties() : super();
  CharacteristicProperties.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  CharacteristicProperties.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  CharacteristicProperties clone() => CharacteristicProperties()..mergeFromMessage(this);
  CharacteristicProperties copyWith(void Function(CharacteristicProperties) updates) => super.copyWith((message) => updates(message as CharacteristicProperties));
  $pb.BuilderInfo get info_ => _i;
  static CharacteristicProperties create() => CharacteristicProperties();
  CharacteristicProperties createEmptyInstance() => create();
  static $pb.PbList<CharacteristicProperties> createRepeated() => $pb.PbList<CharacteristicProperties>();
  static CharacteristicProperties getDefault() => _defaultInstance ??= create()..freeze();
  static CharacteristicProperties _defaultInstance;

  $core.bool get broadcast => $_get(0, false);
  set broadcast($core.bool v) { $_setBool(0, v); }
  $core.bool hasBroadcast() => $_has(0);
  void clearBroadcast() => clearField(1);

  $core.bool get read => $_get(1, false);
  set read($core.bool v) { $_setBool(1, v); }
  $core.bool hasRead() => $_has(1);
  void clearRead() => clearField(2);

  $core.bool get writeWithoutResponse => $_get(2, false);
  set writeWithoutResponse($core.bool v) { $_setBool(2, v); }
  $core.bool hasWriteWithoutResponse() => $_has(2);
  void clearWriteWithoutResponse() => clearField(3);

  $core.bool get write => $_get(3, false);
  set write($core.bool v) { $_setBool(3, v); }
  $core.bool hasWrite() => $_has(3);
  void clearWrite() => clearField(4);

  $core.bool get notify => $_get(4, false);
  set notify($core.bool v) { $_setBool(4, v); }
  $core.bool hasNotify() => $_has(4);
  void clearNotify() => clearField(5);

  $core.bool get indicate => $_get(5, false);
  set indicate($core.bool v) { $_setBool(5, v); }
  $core.bool hasIndicate() => $_has(5);
  void clearIndicate() => clearField(6);

  $core.bool get authenticatedSignedWrites => $_get(6, false);
  set authenticatedSignedWrites($core.bool v) { $_setBool(6, v); }
  $core.bool hasAuthenticatedSignedWrites() => $_has(6);
  void clearAuthenticatedSignedWrites() => clearField(7);

  $core.bool get extendedProperties => $_get(7, false);
  set extendedProperties($core.bool v) { $_setBool(7, v); }
  $core.bool hasExtendedProperties() => $_has(7);
  void clearExtendedProperties() => clearField(8);

  $core.bool get notifyEncryptionRequired => $_get(8, false);
  set notifyEncryptionRequired($core.bool v) { $_setBool(8, v); }
  $core.bool hasNotifyEncryptionRequired() => $_has(8);
  void clearNotifyEncryptionRequired() => clearField(9);

  $core.bool get indicateEncryptionRequired => $_get(9, false);
  set indicateEncryptionRequired($core.bool v) { $_setBool(9, v); }
  $core.bool hasIndicateEncryptionRequired() => $_has(9);
  void clearIndicateEncryptionRequired() => clearField(10);
}

class DiscoverServicesResult extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('DiscoverServicesResult')
    ..aOS(1, 'remoteId')
    ..pc<BluetoothService>(2, 'services', $pb.PbFieldType.PM,BluetoothService.create)
    ..hasRequiredFields = false
  ;

  DiscoverServicesResult() : super();
  DiscoverServicesResult.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  DiscoverServicesResult.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  DiscoverServicesResult clone() => DiscoverServicesResult()..mergeFromMessage(this);
  DiscoverServicesResult copyWith(void Function(DiscoverServicesResult) updates) => super.copyWith((message) => updates(message as DiscoverServicesResult));
  $pb.BuilderInfo get info_ => _i;
  static DiscoverServicesResult create() => DiscoverServicesResult();
  DiscoverServicesResult createEmptyInstance() => create();
  static $pb.PbList<DiscoverServicesResult> createRepeated() => $pb.PbList<DiscoverServicesResult>();
  static DiscoverServicesResult getDefault() => _defaultInstance ??= create()..freeze();
  static DiscoverServicesResult _defaultInstance;

  $core.String get remoteId => $_getS(0, '');
  set remoteId($core.String v) { $_setString(0, v); }
  $core.bool hasRemoteId() => $_has(0);
  void clearRemoteId() => clearField(1);

  $core.List<BluetoothService> get services => $_getList(1);
}

class ReadCharacteristicRequest extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('ReadCharacteristicRequest')
    ..aOS(1, 'remoteId')
    ..aOS(2, 'characteristicUuid')
    ..aOS(3, 'serviceUuid')
    ..aOS(4, 'secondaryServiceUuid')
    ..hasRequiredFields = false
  ;

  ReadCharacteristicRequest() : super();
  ReadCharacteristicRequest.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  ReadCharacteristicRequest.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  ReadCharacteristicRequest clone() => ReadCharacteristicRequest()..mergeFromMessage(this);
  ReadCharacteristicRequest copyWith(void Function(ReadCharacteristicRequest) updates) => super.copyWith((message) => updates(message as ReadCharacteristicRequest));
  $pb.BuilderInfo get info_ => _i;
  static ReadCharacteristicRequest create() => ReadCharacteristicRequest();
  ReadCharacteristicRequest createEmptyInstance() => create();
  static $pb.PbList<ReadCharacteristicRequest> createRepeated() => $pb.PbList<ReadCharacteristicRequest>();
  static ReadCharacteristicRequest getDefault() => _defaultInstance ??= create()..freeze();
  static ReadCharacteristicRequest _defaultInstance;

  $core.String get remoteId => $_getS(0, '');
  set remoteId($core.String v) { $_setString(0, v); }
  $core.bool hasRemoteId() => $_has(0);
  void clearRemoteId() => clearField(1);

  $core.String get characteristicUuid => $_getS(1, '');
  set characteristicUuid($core.String v) { $_setString(1, v); }
  $core.bool hasCharacteristicUuid() => $_has(1);
  void clearCharacteristicUuid() => clearField(2);

  $core.String get serviceUuid => $_getS(2, '');
  set serviceUuid($core.String v) { $_setString(2, v); }
  $core.bool hasServiceUuid() => $_has(2);
  void clearServiceUuid() => clearField(3);

  $core.String get secondaryServiceUuid => $_getS(3, '');
  set secondaryServiceUuid($core.String v) { $_setString(3, v); }
  $core.bool hasSecondaryServiceUuid() => $_has(3);
  void clearSecondaryServiceUuid() => clearField(4);
}

class ReadCharacteristicResponse extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('ReadCharacteristicResponse')
    ..aOS(1, 'remoteId')
    ..a<BluetoothCharacteristic>(2, 'characteristic', $pb.PbFieldType.OM, BluetoothCharacteristic.getDefault, BluetoothCharacteristic.create)
    ..hasRequiredFields = false
  ;

  ReadCharacteristicResponse() : super();
  ReadCharacteristicResponse.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  ReadCharacteristicResponse.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  ReadCharacteristicResponse clone() => ReadCharacteristicResponse()..mergeFromMessage(this);
  ReadCharacteristicResponse copyWith(void Function(ReadCharacteristicResponse) updates) => super.copyWith((message) => updates(message as ReadCharacteristicResponse));
  $pb.BuilderInfo get info_ => _i;
  static ReadCharacteristicResponse create() => ReadCharacteristicResponse();
  ReadCharacteristicResponse createEmptyInstance() => create();
  static $pb.PbList<ReadCharacteristicResponse> createRepeated() => $pb.PbList<ReadCharacteristicResponse>();
  static ReadCharacteristicResponse getDefault() => _defaultInstance ??= create()..freeze();
  static ReadCharacteristicResponse _defaultInstance;

  $core.String get remoteId => $_getS(0, '');
  set remoteId($core.String v) { $_setString(0, v); }
  $core.bool hasRemoteId() => $_has(0);
  void clearRemoteId() => clearField(1);

  BluetoothCharacteristic get characteristic => $_getN(1);
  set characteristic(BluetoothCharacteristic v) { setField(2, v); }
  $core.bool hasCharacteristic() => $_has(1);
  void clearCharacteristic() => clearField(2);
}

class ReadDescriptorRequest extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('ReadDescriptorRequest')
    ..aOS(1, 'remoteId')
    ..aOS(2, 'descriptorUuid')
    ..aOS(3, 'serviceUuid')
    ..aOS(4, 'secondaryServiceUuid')
    ..aOS(5, 'characteristicUuid')
    ..hasRequiredFields = false
  ;

  ReadDescriptorRequest() : super();
  ReadDescriptorRequest.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  ReadDescriptorRequest.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  ReadDescriptorRequest clone() => ReadDescriptorRequest()..mergeFromMessage(this);
  ReadDescriptorRequest copyWith(void Function(ReadDescriptorRequest) updates) => super.copyWith((message) => updates(message as ReadDescriptorRequest));
  $pb.BuilderInfo get info_ => _i;
  static ReadDescriptorRequest create() => ReadDescriptorRequest();
  ReadDescriptorRequest createEmptyInstance() => create();
  static $pb.PbList<ReadDescriptorRequest> createRepeated() => $pb.PbList<ReadDescriptorRequest>();
  static ReadDescriptorRequest getDefault() => _defaultInstance ??= create()..freeze();
  static ReadDescriptorRequest _defaultInstance;

  $core.String get remoteId => $_getS(0, '');
  set remoteId($core.String v) { $_setString(0, v); }
  $core.bool hasRemoteId() => $_has(0);
  void clearRemoteId() => clearField(1);

  $core.String get descriptorUuid => $_getS(1, '');
  set descriptorUuid($core.String v) { $_setString(1, v); }
  $core.bool hasDescriptorUuid() => $_has(1);
  void clearDescriptorUuid() => clearField(2);

  $core.String get serviceUuid => $_getS(2, '');
  set serviceUuid($core.String v) { $_setString(2, v); }
  $core.bool hasServiceUuid() => $_has(2);
  void clearServiceUuid() => clearField(3);

  $core.String get secondaryServiceUuid => $_getS(3, '');
  set secondaryServiceUuid($core.String v) { $_setString(3, v); }
  $core.bool hasSecondaryServiceUuid() => $_has(3);
  void clearSecondaryServiceUuid() => clearField(4);

  $core.String get characteristicUuid => $_getS(4, '');
  set characteristicUuid($core.String v) { $_setString(4, v); }
  $core.bool hasCharacteristicUuid() => $_has(4);
  void clearCharacteristicUuid() => clearField(5);
}

class ReadDescriptorResponse extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('ReadDescriptorResponse')
    ..a<ReadDescriptorRequest>(1, 'request', $pb.PbFieldType.OM, ReadDescriptorRequest.getDefault, ReadDescriptorRequest.create)
    ..a<$core.List<$core.int>>(2, 'value', $pb.PbFieldType.OY)
    ..hasRequiredFields = false
  ;

  ReadDescriptorResponse() : super();
  ReadDescriptorResponse.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  ReadDescriptorResponse.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  ReadDescriptorResponse clone() => ReadDescriptorResponse()..mergeFromMessage(this);
  ReadDescriptorResponse copyWith(void Function(ReadDescriptorResponse) updates) => super.copyWith((message) => updates(message as ReadDescriptorResponse));
  $pb.BuilderInfo get info_ => _i;
  static ReadDescriptorResponse create() => ReadDescriptorResponse();
  ReadDescriptorResponse createEmptyInstance() => create();
  static $pb.PbList<ReadDescriptorResponse> createRepeated() => $pb.PbList<ReadDescriptorResponse>();
  static ReadDescriptorResponse getDefault() => _defaultInstance ??= create()..freeze();
  static ReadDescriptorResponse _defaultInstance;

  ReadDescriptorRequest get request => $_getN(0);
  set request(ReadDescriptorRequest v) { setField(1, v); }
  $core.bool hasRequest() => $_has(0);
  void clearRequest() => clearField(1);

  $core.List<$core.int> get value => $_getN(1);
  set value($core.List<$core.int> v) { $_setBytes(1, v); }
  $core.bool hasValue() => $_has(1);
  void clearValue() => clearField(2);
}

class WriteCharacteristicRequest extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('WriteCharacteristicRequest')
    ..aOS(1, 'remoteId')
    ..aOS(2, 'characteristicUuid')
    ..aOS(3, 'serviceUuid')
    ..aOS(4, 'secondaryServiceUuid')
    ..e<WriteCharacteristicRequest_WriteType>(5, 'writeType', $pb.PbFieldType.OE, WriteCharacteristicRequest_WriteType.WITH_RESPONSE, WriteCharacteristicRequest_WriteType.valueOf, WriteCharacteristicRequest_WriteType.values)
    ..a<$core.List<$core.int>>(6, 'value', $pb.PbFieldType.OY)
    ..hasRequiredFields = false
  ;

  WriteCharacteristicRequest() : super();
  WriteCharacteristicRequest.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  WriteCharacteristicRequest.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  WriteCharacteristicRequest clone() => WriteCharacteristicRequest()..mergeFromMessage(this);
  WriteCharacteristicRequest copyWith(void Function(WriteCharacteristicRequest) updates) => super.copyWith((message) => updates(message as WriteCharacteristicRequest));
  $pb.BuilderInfo get info_ => _i;
  static WriteCharacteristicRequest create() => WriteCharacteristicRequest();
  WriteCharacteristicRequest createEmptyInstance() => create();
  static $pb.PbList<WriteCharacteristicRequest> createRepeated() => $pb.PbList<WriteCharacteristicRequest>();
  static WriteCharacteristicRequest getDefault() => _defaultInstance ??= create()..freeze();
  static WriteCharacteristicRequest _defaultInstance;

  $core.String get remoteId => $_getS(0, '');
  set remoteId($core.String v) { $_setString(0, v); }
  $core.bool hasRemoteId() => $_has(0);
  void clearRemoteId() => clearField(1);

  $core.String get characteristicUuid => $_getS(1, '');
  set characteristicUuid($core.String v) { $_setString(1, v); }
  $core.bool hasCharacteristicUuid() => $_has(1);
  void clearCharacteristicUuid() => clearField(2);

  $core.String get serviceUuid => $_getS(2, '');
  set serviceUuid($core.String v) { $_setString(2, v); }
  $core.bool hasServiceUuid() => $_has(2);
  void clearServiceUuid() => clearField(3);

  $core.String get secondaryServiceUuid => $_getS(3, '');
  set secondaryServiceUuid($core.String v) { $_setString(3, v); }
  $core.bool hasSecondaryServiceUuid() => $_has(3);
  void clearSecondaryServiceUuid() => clearField(4);

  WriteCharacteristicRequest_WriteType get writeType => $_getN(4);
  set writeType(WriteCharacteristicRequest_WriteType v) { setField(5, v); }
  $core.bool hasWriteType() => $_has(4);
  void clearWriteType() => clearField(5);

  $core.List<$core.int> get value => $_getN(5);
  set value($core.List<$core.int> v) { $_setBytes(5, v); }
  $core.bool hasValue() => $_has(5);
  void clearValue() => clearField(6);
}

class WriteCharacteristicResponse extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('WriteCharacteristicResponse')
    ..a<WriteCharacteristicRequest>(1, 'request', $pb.PbFieldType.OM, WriteCharacteristicRequest.getDefault, WriteCharacteristicRequest.create)
    ..aOB(2, 'success')
    ..hasRequiredFields = false
  ;

  WriteCharacteristicResponse() : super();
  WriteCharacteristicResponse.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  WriteCharacteristicResponse.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  WriteCharacteristicResponse clone() => WriteCharacteristicResponse()..mergeFromMessage(this);
  WriteCharacteristicResponse copyWith(void Function(WriteCharacteristicResponse) updates) => super.copyWith((message) => updates(message as WriteCharacteristicResponse));
  $pb.BuilderInfo get info_ => _i;
  static WriteCharacteristicResponse create() => WriteCharacteristicResponse();
  WriteCharacteristicResponse createEmptyInstance() => create();
  static $pb.PbList<WriteCharacteristicResponse> createRepeated() => $pb.PbList<WriteCharacteristicResponse>();
  static WriteCharacteristicResponse getDefault() => _defaultInstance ??= create()..freeze();
  static WriteCharacteristicResponse _defaultInstance;

  WriteCharacteristicRequest get request => $_getN(0);
  set request(WriteCharacteristicRequest v) { setField(1, v); }
  $core.bool hasRequest() => $_has(0);
  void clearRequest() => clearField(1);

  $core.bool get success => $_get(1, false);
  set success($core.bool v) { $_setBool(1, v); }
  $core.bool hasSuccess() => $_has(1);
  void clearSuccess() => clearField(2);
}

class WriteDescriptorRequest extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('WriteDescriptorRequest')
    ..aOS(1, 'remoteId')
    ..aOS(2, 'descriptorUuid')
    ..aOS(3, 'serviceUuid')
    ..aOS(4, 'secondaryServiceUuid')
    ..aOS(5, 'characteristicUuid')
    ..a<$core.List<$core.int>>(6, 'value', $pb.PbFieldType.OY)
    ..hasRequiredFields = false
  ;

  WriteDescriptorRequest() : super();
  WriteDescriptorRequest.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  WriteDescriptorRequest.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  WriteDescriptorRequest clone() => WriteDescriptorRequest()..mergeFromMessage(this);
  WriteDescriptorRequest copyWith(void Function(WriteDescriptorRequest) updates) => super.copyWith((message) => updates(message as WriteDescriptorRequest));
  $pb.BuilderInfo get info_ => _i;
  static WriteDescriptorRequest create() => WriteDescriptorRequest();
  WriteDescriptorRequest createEmptyInstance() => create();
  static $pb.PbList<WriteDescriptorRequest> createRepeated() => $pb.PbList<WriteDescriptorRequest>();
  static WriteDescriptorRequest getDefault() => _defaultInstance ??= create()..freeze();
  static WriteDescriptorRequest _defaultInstance;

  $core.String get remoteId => $_getS(0, '');
  set remoteId($core.String v) { $_setString(0, v); }
  $core.bool hasRemoteId() => $_has(0);
  void clearRemoteId() => clearField(1);

  $core.String get descriptorUuid => $_getS(1, '');
  set descriptorUuid($core.String v) { $_setString(1, v); }
  $core.bool hasDescriptorUuid() => $_has(1);
  void clearDescriptorUuid() => clearField(2);

  $core.String get serviceUuid => $_getS(2, '');
  set serviceUuid($core.String v) { $_setString(2, v); }
  $core.bool hasServiceUuid() => $_has(2);
  void clearServiceUuid() => clearField(3);

  $core.String get secondaryServiceUuid => $_getS(3, '');
  set secondaryServiceUuid($core.String v) { $_setString(3, v); }
  $core.bool hasSecondaryServiceUuid() => $_has(3);
  void clearSecondaryServiceUuid() => clearField(4);

  $core.String get characteristicUuid => $_getS(4, '');
  set characteristicUuid($core.String v) { $_setString(4, v); }
  $core.bool hasCharacteristicUuid() => $_has(4);
  void clearCharacteristicUuid() => clearField(5);

  $core.List<$core.int> get value => $_getN(5);
  set value($core.List<$core.int> v) { $_setBytes(5, v); }
  $core.bool hasValue() => $_has(5);
  void clearValue() => clearField(6);
}

class WriteDescriptorResponse extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('WriteDescriptorResponse')
    ..a<WriteDescriptorRequest>(1, 'request', $pb.PbFieldType.OM, WriteDescriptorRequest.getDefault, WriteDescriptorRequest.create)
    ..aOB(2, 'success')
    ..hasRequiredFields = false
  ;

  WriteDescriptorResponse() : super();
  WriteDescriptorResponse.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  WriteDescriptorResponse.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  WriteDescriptorResponse clone() => WriteDescriptorResponse()..mergeFromMessage(this);
  WriteDescriptorResponse copyWith(void Function(WriteDescriptorResponse) updates) => super.copyWith((message) => updates(message as WriteDescriptorResponse));
  $pb.BuilderInfo get info_ => _i;
  static WriteDescriptorResponse create() => WriteDescriptorResponse();
  WriteDescriptorResponse createEmptyInstance() => create();
  static $pb.PbList<WriteDescriptorResponse> createRepeated() => $pb.PbList<WriteDescriptorResponse>();
  static WriteDescriptorResponse getDefault() => _defaultInstance ??= create()..freeze();
  static WriteDescriptorResponse _defaultInstance;

  WriteDescriptorRequest get request => $_getN(0);
  set request(WriteDescriptorRequest v) { setField(1, v); }
  $core.bool hasRequest() => $_has(0);
  void clearRequest() => clearField(1);

  $core.bool get success => $_get(1, false);
  set success($core.bool v) { $_setBool(1, v); }
  $core.bool hasSuccess() => $_has(1);
  void clearSuccess() => clearField(2);
}

class SetNotificationRequest extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('SetNotificationRequest')
    ..aOS(1, 'remoteId')
    ..aOS(2, 'serviceUuid')
    ..aOS(3, 'secondaryServiceUuid')
    ..aOS(4, 'characteristicUuid')
    ..aOB(5, 'enable')
    ..hasRequiredFields = false
  ;

  SetNotificationRequest() : super();
  SetNotificationRequest.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  SetNotificationRequest.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  SetNotificationRequest clone() => SetNotificationRequest()..mergeFromMessage(this);
  SetNotificationRequest copyWith(void Function(SetNotificationRequest) updates) => super.copyWith((message) => updates(message as SetNotificationRequest));
  $pb.BuilderInfo get info_ => _i;
  static SetNotificationRequest create() => SetNotificationRequest();
  SetNotificationRequest createEmptyInstance() => create();
  static $pb.PbList<SetNotificationRequest> createRepeated() => $pb.PbList<SetNotificationRequest>();
  static SetNotificationRequest getDefault() => _defaultInstance ??= create()..freeze();
  static SetNotificationRequest _defaultInstance;

  $core.String get remoteId => $_getS(0, '');
  set remoteId($core.String v) { $_setString(0, v); }
  $core.bool hasRemoteId() => $_has(0);
  void clearRemoteId() => clearField(1);

  $core.String get serviceUuid => $_getS(1, '');
  set serviceUuid($core.String v) { $_setString(1, v); }
  $core.bool hasServiceUuid() => $_has(1);
  void clearServiceUuid() => clearField(2);

  $core.String get secondaryServiceUuid => $_getS(2, '');
  set secondaryServiceUuid($core.String v) { $_setString(2, v); }
  $core.bool hasSecondaryServiceUuid() => $_has(2);
  void clearSecondaryServiceUuid() => clearField(3);

  $core.String get characteristicUuid => $_getS(3, '');
  set characteristicUuid($core.String v) { $_setString(3, v); }
  $core.bool hasCharacteristicUuid() => $_has(3);
  void clearCharacteristicUuid() => clearField(4);

  $core.bool get enable => $_get(4, false);
  set enable($core.bool v) { $_setBool(4, v); }
  $core.bool hasEnable() => $_has(4);
  void clearEnable() => clearField(5);
}

class SetNotificationResponse extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('SetNotificationResponse')
    ..aOS(1, 'remoteId')
    ..a<BluetoothCharacteristic>(2, 'characteristic', $pb.PbFieldType.OM, BluetoothCharacteristic.getDefault, BluetoothCharacteristic.create)
    ..aOB(3, 'success')
    ..hasRequiredFields = false
  ;

  SetNotificationResponse() : super();
  SetNotificationResponse.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  SetNotificationResponse.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  SetNotificationResponse clone() => SetNotificationResponse()..mergeFromMessage(this);
  SetNotificationResponse copyWith(void Function(SetNotificationResponse) updates) => super.copyWith((message) => updates(message as SetNotificationResponse));
  $pb.BuilderInfo get info_ => _i;
  static SetNotificationResponse create() => SetNotificationResponse();
  SetNotificationResponse createEmptyInstance() => create();
  static $pb.PbList<SetNotificationResponse> createRepeated() => $pb.PbList<SetNotificationResponse>();
  static SetNotificationResponse getDefault() => _defaultInstance ??= create()..freeze();
  static SetNotificationResponse _defaultInstance;

  $core.String get remoteId => $_getS(0, '');
  set remoteId($core.String v) { $_setString(0, v); }
  $core.bool hasRemoteId() => $_has(0);
  void clearRemoteId() => clearField(1);

  BluetoothCharacteristic get characteristic => $_getN(1);
  set characteristic(BluetoothCharacteristic v) { setField(2, v); }
  $core.bool hasCharacteristic() => $_has(1);
  void clearCharacteristic() => clearField(2);

  $core.bool get success => $_get(2, false);
  set success($core.bool v) { $_setBool(2, v); }
  $core.bool hasSuccess() => $_has(2);
  void clearSuccess() => clearField(3);
}

class OnNotificationResponse extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('OnNotificationResponse')
    ..aOS(1, 'remoteId')
    ..a<BluetoothCharacteristic>(2, 'characteristic', $pb.PbFieldType.OM, BluetoothCharacteristic.getDefault, BluetoothCharacteristic.create)
    ..hasRequiredFields = false
  ;

  OnNotificationResponse() : super();
  OnNotificationResponse.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  OnNotificationResponse.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  OnNotificationResponse clone() => OnNotificationResponse()..mergeFromMessage(this);
  OnNotificationResponse copyWith(void Function(OnNotificationResponse) updates) => super.copyWith((message) => updates(message as OnNotificationResponse));
  $pb.BuilderInfo get info_ => _i;
  static OnNotificationResponse create() => OnNotificationResponse();
  OnNotificationResponse createEmptyInstance() => create();
  static $pb.PbList<OnNotificationResponse> createRepeated() => $pb.PbList<OnNotificationResponse>();
  static OnNotificationResponse getDefault() => _defaultInstance ??= create()..freeze();
  static OnNotificationResponse _defaultInstance;

  $core.String get remoteId => $_getS(0, '');
  set remoteId($core.String v) { $_setString(0, v); }
  $core.bool hasRemoteId() => $_has(0);
  void clearRemoteId() => clearField(1);

  BluetoothCharacteristic get characteristic => $_getN(1);
  set characteristic(BluetoothCharacteristic v) { setField(2, v); }
  $core.bool hasCharacteristic() => $_has(1);
  void clearCharacteristic() => clearField(2);
}

class DeviceStateResponse extends $pb.GeneratedMessage {
  static final $pb.BuilderInfo _i = $pb.BuilderInfo('DeviceStateResponse')
    ..aOS(1, 'remoteId')
    ..e<DeviceStateResponse_BluetoothDeviceState>(2, 'state', $pb.PbFieldType.OE, DeviceStateResponse_BluetoothDeviceState.DISCONNECTED, DeviceStateResponse_BluetoothDeviceState.valueOf, DeviceStateResponse_BluetoothDeviceState.values)
    ..hasRequiredFields = false
  ;

  DeviceStateResponse() : super();
  DeviceStateResponse.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromBuffer(i, r);
  DeviceStateResponse.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) : super.fromJson(i, r);
  DeviceStateResponse clone() => DeviceStateResponse()..mergeFromMessage(this);
  DeviceStateResponse copyWith(void Function(DeviceStateResponse) updates) => super.copyWith((message) => updates(message as DeviceStateResponse));
  $pb.BuilderInfo get info_ => _i;
  static DeviceStateResponse create() => DeviceStateResponse();
  DeviceStateResponse createEmptyInstance() => create();
  static $pb.PbList<DeviceStateResponse> createRepeated() => $pb.PbList<DeviceStateResponse>();
  static DeviceStateResponse getDefault() => _defaultInstance ??= create()..freeze();
  static DeviceStateResponse _defaultInstance;

  $core.String get remoteId => $_getS(0, '');
  set remoteId($core.String v) { $_setString(0, v); }
  $core.bool hasRemoteId() => $_has(0);
  void clearRemoteId() => clearField(1);

  DeviceStateResponse_BluetoothDeviceState get state => $_getN(1);
  set state(DeviceStateResponse_BluetoothDeviceState v) { setField(2, v); }
  $core.bool hasState() => $_has(1);
  void clearState() => clearField(2);
}

