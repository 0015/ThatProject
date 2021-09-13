import 'dart:typed_data';

import 'package:adaptive_dialog/adaptive_dialog.dart';
import 'package:epaper_emoji_ble/const/custom_styles.dart';
import 'package:epaper_emoji_ble/widgets/ble_widget.dart';
import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:quiver/iterables.dart';
import 'package:sn_progress_dialog/progress_dialog.dart';
import 'package:wakelock/wakelock.dart';

class BleListScreen extends StatelessWidget {
  final Uint8List headedBitmap;

  const BleListScreen({Key? key, required this.headedBitmap}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return StreamBuilder<BluetoothState>(
        stream: FlutterBlue.instance.state,
        initialData: BluetoothState.unknown,
        builder: (c, snapshot) {
          final state = snapshot.data;
          if (state == BluetoothState.on) {
            return FindDevicesScreen(
              headedBitmap: headedBitmap,
            );
          }
          return BluetoothOffScreen(state: state);
        });
  }
}

class BluetoothOffScreen extends StatelessWidget {
  const BluetoothOffScreen({Key? key, this.state}) : super(key: key);

  final BluetoothState? state;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.lightBlue,
      body: Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: <Widget>[
            const Icon(
              Icons.bluetooth_disabled,
              size: 200.0,
              color: Colors.white54,
            ),
            Text(
              'Bluetooth Adapter is ${state != null ? state.toString().substring(15) : 'not available'}.',
              style: Theme.of(context)
                  .primaryTextTheme
                  .headline1
                  ?.copyWith(color: Colors.white),
            ),
          ],
        ),
      ),
    );
  }
}

class FindDevicesScreen extends StatelessWidget {
  final Uint8List headedBitmap;

  const FindDevicesScreen({Key? key, required this.headedBitmap})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Find Devices'),
      ),
      body: RefreshIndicator(
        onRefresh: () =>
            FlutterBlue.instance.startScan(timeout: const Duration(seconds: 4)),
        child: SingleChildScrollView(
          child: Column(
            children: <Widget>[
              StreamBuilder<List<BluetoothDevice>>(
                stream: Stream.periodic(const Duration(seconds: 2))
                    .asyncMap((_) => FlutterBlue.instance.connectedDevices),
                initialData: [],
                builder: (c, snapshot) => Column(
                  children: snapshot.data!
                      .map((d) => ListTile(
                            title: Text(
                              d.name,
                              style: kButtonText.copyWith(color: Colors.white),
                            ),
                            subtitle: Text(
                              d.id.toString(),
                              style:
                                  kSubtitleText.copyWith(color: Colors.white),
                            ),
                            trailing: StreamBuilder<BluetoothDeviceState>(
                              stream: d.state,
                              initialData: BluetoothDeviceState.disconnected,
                              builder: (c, snapshot) {
                                if (snapshot.data ==
                                    BluetoothDeviceState.connected) {
                                  return TextButton(
                                      child: Text(
                                        'Send',
                                        style: kButtonText.copyWith(
                                            color: Colors.white),
                                      ),
                                      onPressed: () {
                                        _findTargetUUID(context, d);
                                      });
                                }
                                return Text(snapshot.data.toString());
                              },
                            ),
                          ))
                      .toList(),
                ),
              ),
              StreamBuilder<List<ScanResult>>(
                stream: FlutterBlue.instance.scanResults,
                initialData: [],
                builder: (c, snapshot) => Column(
                  children: snapshot.data!
                      .map(
                        (r) => ScanResultTile(
                          result: r,
                          onTap: () {
                            r.device.connect();
                          },
                        ),
                      )
                      .toList(),
                ),
              ),
            ],
          ),
        ),
      ),
      floatingActionButton: StreamBuilder<bool>(
        stream: FlutterBlue.instance.isScanning,
        initialData: false,
        builder: (c, snapshot) {
          if (snapshot.data!) {
            return FloatingActionButton(
              child: const Icon(Icons.stop),
              onPressed: () => FlutterBlue.instance.stopScan(),
              backgroundColor: Colors.red,
            );
          } else {
            return FloatingActionButton(
                child: const Icon(Icons.search),
                onPressed: () => FlutterBlue.instance
                    .startScan(timeout: const Duration(seconds: 4)));
          }
        },
      ),
    );
  }

  final String SERVICE_UUID = "713d0001-503e-4c75-ba94-3148f18d941e";
  final String CHARACTERISTIC_UUID = "713d0002-503e-4c75-ba94-3148f18d941e";

  _findTargetUUID(BuildContext context, BluetoothDevice device) async {
    print("device :${device.id}");
    try {
      List<BluetoothService> services = await device.discoverServices();

      for (var service in services) {
        print('service!!! : ${service}');
        print('service last uuid!!! : ${service.uuid.toString()}');

        if (service.uuid.toString() == SERVICE_UUID) {
          for (var characteristic in service.characteristics) {
            if (characteristic.uuid.toString() == CHARACTERISTIC_UUID) {
              print(
                  'characteristic uuid!!! : ${characteristic.uuid.toString()}');

              _sendData(context, characteristic);
              // for (var pair in pairs) {
              //   print(pair);
              //   await characteristic.write(pair, withoutResponse: false);
              //
              //   print('SENT!');
              // }
            }
          }
        }
      }
    } catch (e) {
      await showOkAlertDialog(
        context: context,
        title: 'Error',
        message: e.toString(),
      );
    }
  }

  _sendData(
      BuildContext context, BluetoothCharacteristic characteristic) async {
    Wakelock.enable();
    print('headedBitmap length: ${headedBitmap.length}');
    var pairs = partition(headedBitmap, 20);

    var startSignal = await characteristic.write([], withoutResponse: false);
    print('startSignal: $startSignal');
    ProgressDialog pd = ProgressDialog(context: context);

    pd.show(
        max: pairs.length,
        msg: 'Preparing Upload...',
        progressType: ProgressType.valuable,
        backgroundColor: const Color(0xff212121),
        progressValueColor: Colors.amberAccent,
        progressBgColor: Colors.white70,
        msgColor: Colors.white,
        valueColor: Colors.white);

    int i = 0;
    for (var pair in pairs) {
      ++i;
      print(pair);
      pd.update(value: i, msg: 'Bitmap Uploading...');

      await characteristic.write(pair, withoutResponse: false);
    }

    var endSignal = await characteristic.write([], withoutResponse: false);
    print('endSignal: $endSignal');
    Wakelock.disable();
  }
}
