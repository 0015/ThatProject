
<br>
<p align="center">
<img alt="FlutterBle" src="https://github.com/pauldemarco/flutter_blue/blob/master/site/flutterblue.png?raw=true" />
</p>
<br><br>

## Introduction

FlutterBle is a bluetooth plugin for [Flutter](http://www.flutter.io), a new mobile SDK to help developers build modern apps for iOS and Android.
fork of [FlutterBlue](https://github.com/pauldemarco/flutter_blue) 

## Cross-Platform Bluetooth LE
FlutterBle aims to offer the most from both platforms (iOS and Android).

Using the FlutterBle instance, you can scan for and connect to nearby devices ([BluetoothDevice](#bluetoothdevice-api)).
Once connected to a device, the BluetoothDevice object can discover services ([BluetoothService](lib/src/bluetooth_service.dart)), characteristics ([BluetoothCharacteristic](lib/src/bluetooth_characteristic.dart)), and descriptors ([BluetoothDescriptor](lib/src/bluetooth_descriptor.dart)).
The BluetoothDevice object is then used to directly interact with characteristics and descriptors.

## Usage
### Obtain an instance
```dart
FlutterBle flutterBlue = FlutterBle.instance;
```

### Scan for devices
```dart
/// Start scanning
var scanSubscription = flutterBlue.scan().listen((scanResult) {
    // do something with scan result
});

/// Stop scanning
scanSubscription.cancel();
```

### Connect to a device
```dart
/// Create a connection to the device
var deviceConnection = flutterBlue.connect(device).listen((s) {
    if(s == BluetoothDeviceState.connected) {
        // device is connected, do something
    }
});

/// Disconnect from device
deviceConnection.cancel();
```

### Discover services
```dart
List<BluetoothService> services = await device.discoverServices();
services.forEach((service) {
    // do something with service
});
```

### Read and write characteristics
```dart
// Reads all characteristics
var characteristics = service.characteristics;
for(BluetoothCharacteristic c in characteristics) {
    List<int> value = await device.readCharacteristic(c);
    print(value);
}

// Writes to a characteristic
await device.writeCharacteristic(c, [0x12, 0x34])
```

### Read and write descriptors
```dart
// Reads all descriptors
var descriptors = characteristic.descriptors;
for(BluetoothDescriptor d in descriptors) {
    List<int> value = await device.readDescriptor(d);
    print(value);
}

// Writes to a descriptor
await device.writeDescriptor(d, [0x12, 0x34])
```

### Set notifications
```dart
await device.setNotifyValue(characteristic, true);
device.onValueChanged(characteristic).listen((value) {
    // do something with new value
});
```

## Reference
### FlutterBle API
|                  |      Android       |         iOS          |             Description            |
| :--------------- | :----------------: | :------------------: |  :-------------------------------- |
| scan             | :white_check_mark: |  :white_check_mark:  | Starts a scan for Bluetooth Low Energy devices. |
| connect          | :white_check_mark: |  :white_check_mark:  | Establishes a connection to the Bluetooth Device. |
| state            | :white_check_mark: |  :white_check_mark:  | Gets the current state of the Bluetooth Adapter. |
| onStateChanged   | :white_check_mark: |  :white_check_mark:  | Stream of state changes for the Bluetooth Adapter. |

### BluetoothDevice API
|                             |       Android        |         iOS          |             Description            |
| :-------------------------- | :------------------: | :------------------: |  :-------------------------------- |
| discoverServices            |  :white_check_mark:  |  :white_check_mark:  | Discovers services offered by the remote device as well as their characteristics and descriptors. |
| services                    |  :white_check_mark:  |  :white_check_mark:  | Gets a list of services. Requires that discoverServices() has completed. |
| readCharacteristic          |  :white_check_mark:  |  :white_check_mark:  | Retrieves the value of a specified characteristic.  |
| readDescriptor              |  :white_check_mark:  |  :white_check_mark:  | Retrieves the value of a specified descriptor.  |
| writeCharacteristic         |  :white_check_mark:  |  :white_check_mark:  | Writes the value of a characteristic. |
| writeDescriptor             |  :white_check_mark:  |  :white_check_mark:  | Writes the value of a descriptor. |
| setNotifyValue              |  :white_check_mark:  |  :white_check_mark:  | Sets notifications or indications on the specified characteristic. |
| onValueChanged              |  :white_check_mark:  |  :white_check_mark:  | Notifies when the characteristic's value has changed. |
| state                       |  :white_check_mark:  |  :white_check_mark:  | Gets the current state of the Bluetooth Device. |
| onStateChanged              |  :white_check_mark:  |  :white_check_mark:  | Notifies of state changes for the Bluetooth Device. |


## Troubleshooting
### Scanning for service UUID's doesn't return any results
Make sure the device is advertising which service UUID's it supports.  This is found in the advertisement
packet as **UUID 16 bit complete list** or **UUID 128 bit complete list**.