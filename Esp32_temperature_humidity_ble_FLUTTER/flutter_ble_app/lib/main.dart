// Copyright 2017, Paul DeMarco.
// All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

import 'dart:async';
import 'dart:convert' show utf8;

import 'package:flutter/material.dart';
import 'package:flutter_ble/flutter_ble.dart';
import 'package:flutter_ble_app/widgets.dart';
import 'package:shared_preferences/shared_preferences.dart';

void main() {
  runApp(new FlutterBleApp());
}

class FlutterBleApp extends StatefulWidget {
  FlutterBleApp({Key key, this.title}) : super(key: key);

  final String title;

  @override
  _FlutterBleAppState createState() => new _FlutterBleAppState();
}

class _FlutterBleAppState extends State<FlutterBleApp> {
  Future<SharedPreferences> _prefs = SharedPreferences.getInstance();
  FlutterBle _flutterBlue = FlutterBle.instance;

  /// Scanning
  StreamSubscription _scanSubscription;
  Map<DeviceIdentifier, ScanResult> scanResults = new Map();
  bool isScanning = false;

  /// State
  StreamSubscription _stateSubscription;
  BluetoothState state = BluetoothState.unknown;

  /// Device
  BluetoothDevice device;

  bool get isConnected => (device != null);
  StreamSubscription deviceConnection;
  StreamSubscription deviceStateSubscription;
  List<BluetoothService> services = new List();
  Map<Guid, StreamSubscription> valueChangedSubscriptions = {};
  BluetoothDeviceState deviceState = BluetoothDeviceState.disconnected;

  static const String CHARACTERISTIC_UUID =
      "beb5483e-36e1-4688-b7f5-ea07361b26a8";
  static const String kMYDEVICE = "myDevice";
  String _myDeviceId;
  String _temperature = "?";
  String _humidity = "?";

  @override
  void initState() {
    super.initState();
    // Immediately get the state of FlutterBle
    _flutterBlue.state.then((s) {
      setState(() {
        state = s;
      });
    });
    // Subscribe to state changes
    _stateSubscription = _flutterBlue.onStateChanged().listen((s) {
      setState(() {
        state = s;
      });
    });

    _loadMyDeviceId();
  }

  _loadMyDeviceId() async {
    SharedPreferences prefs = await _prefs;
    _myDeviceId = prefs.getString(kMYDEVICE) ?? "";
    print("_myDeviceId : " + _myDeviceId);

    if (_myDeviceId.isNotEmpty) {
      _startScan();
    }
  }

  @override
  void dispose() {
    _stateSubscription?.cancel();
    _stateSubscription = null;
    _scanSubscription?.cancel();
    _scanSubscription = null;
    deviceConnection?.cancel();
    deviceConnection = null;
    super.dispose();
  }

  _startScan() {
    _scanSubscription = _flutterBlue
        .scan(
      timeout: const Duration(seconds: 5),
      /*withServices: [
          new Guid('0000180F-0000-1000-8000-00805F9B34FB')
        ]*/
    )
        .listen((scanResult) {
//      print('localName: ${scanResult.advertisementData.localName}');
//      print(
//          'manufacturerData: ${scanResult.advertisementData.manufacturerData}');
//      print('serviceData: ${scanResult.advertisementData.serviceData}');

      if (_myDeviceId == scanResult.device.id.toString()) {
        _stopScan();
        _connect(scanResult.device);
      }

      setState(() {
        scanResults[scanResult.device.id] = scanResult;
      });
    }, onDone: _stopScan);

    setState(() {
      isScanning = true;
    });
  }

  _stopScan() {
    _scanSubscription?.cancel();
    _scanSubscription = null;
    setState(() {
      isScanning = false;
    });
  }

  _connect(BluetoothDevice d) async {
    device = d;
    // Connect to device
    deviceConnection = _flutterBlue
        .connect(device, timeout: const Duration(seconds: 4))
        .listen(
          null,
          onDone: _disconnect,
        );

    // Update the connection state immediately
    device.state.then((s) {
      setState(() {
        deviceState = s;
      });
    });

    // Subscribe to connection changes
    deviceStateSubscription = device.onStateChanged().listen((s) {
      setState(() {
        deviceState = s;
      });
      if (s == BluetoothDeviceState.connected) {
        device.discoverServices().then((s) {
          setState(() {
            services = s;

            print("*** device.id : ${device.id.toString()}");

            _restoreDeviceId(device.id.toString());
            _TurnOnCharacterService();
          });
        });
      }
    });
  }

  _disconnect() {
    // Remove all value changed listeners
    valueChangedSubscriptions.forEach((uuid, sub) => sub.cancel());
    valueChangedSubscriptions.clear();
    deviceStateSubscription?.cancel();
    deviceStateSubscription = null;
    deviceConnection?.cancel();
    deviceConnection = null;
    setState(() {
      device = null;
    });
  }

  _readCharacteristic(BluetoothCharacteristic c) async {
    await device.readCharacteristic(c);
    setState(() {});
  }

  _writeCharacteristic(BluetoothCharacteristic c) async {
    await device.writeCharacteristic(c, [0x12, 0x34],
        type: CharacteristicWriteType.withResponse);
    setState(() {});
  }

  _readDescriptor(BluetoothDescriptor d) async {
    await device.readDescriptor(d);
    setState(() {});
  }

  _writeDescriptor(BluetoothDescriptor d) async {
    await device.writeDescriptor(d, [0x12, 0x34]);
    setState(() {});
  }

  _setNotification(BluetoothCharacteristic c) async {
    if (c.isNotifying) {
      await device.setNotifyValue(c, false);
      // Cancel subscription
      valueChangedSubscriptions[c.uuid]?.cancel();
      valueChangedSubscriptions.remove(c.uuid);
    } else {
      await device.setNotifyValue(c, true);
      // ignore: cancel_subscriptions
      final sub = device.onValueChanged(c).listen((d) {
        final decoded = utf8.decode(d);
        _DataParser(decoded);

//        setState(() {
//          print('onValueChanged $d');
//        });
      });
      // Add to map
      valueChangedSubscriptions[c.uuid] = sub;
    }
    setState(() {});
  }

  _refreshDeviceState(BluetoothDevice d) async {
    var state = await d.state;
    setState(() {
      deviceState = state;
      print('State refreshed: $deviceState');
    });
  }

  _buildScanningButton() {
    if (isConnected || state != BluetoothState.on) {
      return null;
    }
    if (isScanning) {
      return new FloatingActionButton(
        child: new Icon(Icons.stop),
        onPressed: _stopScan,
        backgroundColor: Colors.red,
      );
    } else {
      return new FloatingActionButton(
          child: new Icon(Icons.search), onPressed: _startScan);
    }
  }

  _buildScanResultTiles() {
    return scanResults.values
        .map((r) => ScanResultTile(
              result: r,
              onTap: () => _connect(r.device),
            ))
        .toList();
  }

  List<Widget> _buildServiceTiles() {
    return services
        .map(
          (s) => new ServiceTile(
            service: s,
            characteristicTiles: s.characteristics
                .map(
                  (c) => new CharacteristicTile(
                    characteristic: c,
                    onReadPressed: () => _readCharacteristic(c),
                    onWritePressed: () => _writeCharacteristic(c),
                    onNotificationPressed: () => _setNotification(c),
                    descriptorTiles: c.descriptors
                        .map(
                          (d) => new DescriptorTile(
                            descriptor: d,
                            onReadPressed: () => _readDescriptor(d),
                            onWritePressed: () => _writeDescriptor(d),
                          ),
                        )
                        .toList(),
                  ),
                )
                .toList(),
          ),
        )
        .toList();
  }

  _buildActionButtons() {
    if (isConnected) {
      return <Widget>[
        new IconButton(
          icon: const Icon(Icons.cancel),
          onPressed: () => _disconnect(),
        )
      ];
    }
  }

  _buildAlertTile() {
    return new Container(
      color: Colors.redAccent,
      child: new ListTile(
        title: new Text(
          'Bluetooth adapter is ${state.toString().substring(15)}',
          style: Theme.of(context).primaryTextTheme.subhead,
        ),
        trailing: new Icon(
          Icons.error,
          color: Theme.of(context).primaryTextTheme.subhead.color,
        ),
      ),
    );
  }

  _buildDeviceStateTile() {
    return new ListTile(
        leading: (deviceState == BluetoothDeviceState.connected)
            ? const Icon(Icons.bluetooth_connected)
            : const Icon(Icons.bluetooth_disabled),
        title: new Text('Device is ${deviceState.toString().split('.')[1]}.'),
        subtitle: new Text('${device.id}'),
        trailing: new IconButton(
          icon: const Icon(Icons.refresh),
          onPressed: () => _refreshDeviceState(device),
          color: Theme.of(context).iconTheme.color.withOpacity(0.5),
        ));
  }

  _buildProgressBarTile() {
    return new LinearProgressIndicator();
  }

  @override
  Widget build(BuildContext context) {
    var tiles = new List<Widget>();
    if (state != BluetoothState.on) {
      tiles.add(_buildAlertTile());
    }
    if (isConnected) {
//      tiles.add(_buildDeviceStateTile());
//      tiles.addAll(_buildServiceTiles());
    } else {
      tiles.addAll(_buildScanResultTiles());
    }
    return new MaterialApp(
      debugShowCheckedModeBanner: false,
      home: new Scaffold(
        appBar: new AppBar(
          title: const Text('ESP32 MY DHT APP'),
          actions: _buildActionButtons(),
          backgroundColor: Colors.blueGrey,
          centerTitle: true,
        ),
        backgroundColor: Colors.red,
        floatingActionButton: _buildScanningButton(),
        body: new Stack(
          children: <Widget>[
            (isScanning) ? _buildProgressBarTile() : new Container(),
            isConnected
                ? _bulldMyWidget()
                : new ListView(
                    children: tiles,
                  )
          ],
        ),
      ),
    );
  }

  Future<void> _restoreDeviceId(String id) async {
    final SharedPreferences prefs = await _prefs;
    prefs.setString(kMYDEVICE, id);
  }

  _TurnOnCharacterService() {
    services.forEach((service) {
      service.characteristics.forEach((character) {
        if (character.uuid.toString() == CHARACTERISTIC_UUID) {
          _setNotification(character);
        }
      });
    });
  }

  _bulldMyWidget() {
    return Center(
      child: Row(
        mainAxisAlignment: MainAxisAlignment.center,
        crossAxisAlignment: CrossAxisAlignment.center,
        children: <Widget>[
          Card(
            child: Container(
              width: 150,
              height: 200,
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.center,
                children: <Widget>[
                  SizedBox(
                    height: 10,
                  ),
                  Container(
                    width: 100,
                    height: 100,
                    child: Image.asset('images/temperature.png'),
                  ),
                  SizedBox(
                    height: 10,
                  ),
                  Text(
                    "Temperature",
                    style: TextStyle(fontWeight: FontWeight.bold),
                  ),
                  Expanded(
                    child: Container(),
                  ),
                  Text(
                    _temperature,
                    style: TextStyle(fontSize: 30),
                  ),
                  SizedBox(
                    height: 10,
                  ),
                ],
              ),
            ),
          ),
          Card(
            child: Container(
              width: 150,
              height: 200,
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.center,
                children: <Widget>[
                  SizedBox(
                    height: 10,
                  ),
                  Container(
                    width: 100,
                    height: 100,
                    child: Image.asset('images/humidity.png'),
                  ),
                  SizedBox(
                    height: 10,
                  ),
                  Text(
                    "Humidity",
                    style: TextStyle(fontWeight: FontWeight.bold),
                  ),
                  Expanded(
                    child: Container(),
                  ),
                  Text(
                    _humidity,
                    style: TextStyle(fontSize: 30),
                  ),
                  SizedBox(
                    height: 10,
                  ),
                ],
              ),
            ),
          )
        ],
      ),
    );
  }

  _DataParser(String data) {
    if (data.isNotEmpty) {
      var tempValue = data.split(",")[0];
      var humidityValue = data.split(",")[1];

      print("tempValue: ${tempValue}");
      print("humidityValue: ${humidityValue}");

      setState(() {
        _temperature = tempValue + "'C";
        _humidity = humidityValue + "%";
      });
    }
  }
}
