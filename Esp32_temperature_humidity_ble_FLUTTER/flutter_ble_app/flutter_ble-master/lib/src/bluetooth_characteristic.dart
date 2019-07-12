// Copyright 2017, Paul DeMarco.
// All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

part of flutter_ble;

class BluetoothCharacteristic {
  final Guid uuid;
  final Guid serviceUuid; // The service that this characteristic belongs to.
  final Guid
      secondaryServiceUuid; // The nested service that this characteristic belongs to.
  final CharacteristicProperties properties;
  final List<BluetoothDescriptor> descriptors;
  bool get isNotifying {
    try {
      var cccd =
          descriptors.singleWhere((d) => d.uuid == BluetoothDescriptor.CCCD);
      return ((cccd.value[0] & 0x01) > 0 || (cccd.value[0] & 0x02) > 0);
    } catch (e) {
      return false;
    }
  }

  List<int> value;

  BluetoothCharacteristic(
      {@required this.uuid,
      @required this.serviceUuid,
      this.secondaryServiceUuid,
      @required this.descriptors,
      @required this.properties});

  BluetoothCharacteristic.fromProto(protos.BluetoothCharacteristic p)
      : uuid = new Guid(p.uuid),
        serviceUuid = new Guid(p.serviceUuid),
        secondaryServiceUuid = (p.secondaryServiceUuid.length > 0)
            ? new Guid(p.secondaryServiceUuid)
            : null,
        descriptors = p.descriptors
            .map((d) => new BluetoothDescriptor.fromProto(d))
            .toList(),
        properties = new CharacteristicProperties.fromProto(p.properties),
        value = p.value;

  void updateDescriptors(List<BluetoothDescriptor> newDescriptors) {
    for (var d in descriptors) {
      for (var newD in newDescriptors) {
        if (d.uuid == newD.uuid) {
          d.value = newD.value;
        }
      }
    }
  }
}

enum CharacteristicWriteType { withResponse, withoutResponse }

class CharacteristicProperties {
  final bool broadcast;
  final bool read;
  final bool writeWithoutResponse;
  final bool write;
  final bool notify;
  final bool indicate;
  final bool authenticatedSignedWrites;
  final bool extendedProperties;
  final bool notifyEncryptionRequired;
  final bool indicateEncryptionRequired;

  CharacteristicProperties(
      {this.broadcast = false,
      this.read = false,
      this.writeWithoutResponse = false,
      this.write = false,
      this.notify = false,
      this.indicate = false,
      this.authenticatedSignedWrites = false,
      this.extendedProperties = false,
      this.notifyEncryptionRequired = false,
      this.indicateEncryptionRequired = false});

  CharacteristicProperties.fromProto(protos.CharacteristicProperties p)
      : broadcast = p.broadcast,
        read = p.read,
        writeWithoutResponse = p.writeWithoutResponse,
        write = p.write,
        notify = p.notify,
        indicate = p.indicate,
        authenticatedSignedWrites = p.authenticatedSignedWrites,
        extendedProperties = p.extendedProperties,
        notifyEncryptionRequired = p.notifyEncryptionRequired,
        indicateEncryptionRequired = p.indicateEncryptionRequired;
}
