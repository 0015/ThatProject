import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:flutter_progress_dialog/flutter_progress_dialog.dart';
import 'package:flutterappslm/slm_view.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  @override
  Widget build(BuildContext context) {
    return ProgressDialog(
      orientation: ProgressOrientation.vertical,
      child: MaterialApp(
        debugShowCheckedModeBanner: false,
        theme: ThemeData(
            brightness: Brightness.light,
            primaryColor: Color.fromRGBO(58, 66, 86, 1)),
        home: StreamBuilder<BluetoothState>(
          stream: FlutterBlue.instance.state,
          initialData: BluetoothState.unknown,
          builder: (c, snapshot) {
            final state = snapshot.data;
            if (state == BluetoothState.on) {
              return BluetoothOnScreen();
            } else {
              return BluetoothOffScreen(
                state: state,
              );
            }
          },
        ),
      ),
    );
  }
}

class BluetoothOnScreen extends StatefulWidget {
  @override
  _BluetoothOnScreenState createState() => _BluetoothOnScreenState();
}

class _BluetoothOnScreenState extends State<BluetoothOnScreen> {
  static String targetDeviceName = 'ESP32-THAT-PROJECT';

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Sound Level Meter'),
      ),
      body: Center(
        child: RaisedButton(
          child: Text('Connect Device'),
          onPressed: _deviceSearchingStart,
        ),
      ),
    );
  }

  _deviceSearchingStart() {
    showProgressDialog(loadingText: 'Searching Device...');

    FlutterBlue.instance
        .startScan(timeout: Duration(seconds: 3))
        .whenComplete(() {
      FlutterBlue.instance.stopScan();
      _scanResult();
      dismissProgressDialog();
    });
  }

  _scanResult() {
    FlutterBlue.instance.scanResults.first.then((element) {
      print('Device Num: ${element.length}');
      element.forEach((value) {
        print('**** ${value.device.id} / ${value.device.name} ****');
        if (value.device.name == targetDeviceName) {
          print('**** FOUND Target Device ****');
          _connectDevice(value.device);
        }
      });
    });
  }

  _connectDevice(BluetoothDevice _device) {
    _device.connect().whenComplete(() {
      Navigator.of(context).push(MaterialPageRoute(
          builder: (context) => SLMView(
                device: _device,
              )));
    });
  }
}

class BluetoothOffScreen extends StatelessWidget {
  const BluetoothOffScreen({Key key, this.state}) : super(key: key);
  final BluetoothState state;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: <Widget>[
            Icon(
              Icons.bluetooth_disabled,
              size: 200,
              color: Colors.black,
            ),
            Text(
              'Bluetooth Adapter is ${state.toString().substring(15)}',
              style: Theme.of(context)
                  .primaryTextTheme
                  .subtitle1
                  .copyWith(color: Colors.black),
            )
          ],
        ),
      ),
    );
  }
}
