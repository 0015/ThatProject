import 'dart:async';
import 'dart:convert' show utf8;

import 'package:flutter/material.dart';
import 'package:flutter_3d_obj/flutter_3d_obj.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:vector_math/vector_math.dart' as VMath;

class Mpu6050Page extends StatefulWidget {
  final BluetoothDevice device;

  const Mpu6050Page({Key key, this.device}) : super(key: key);

  @override
  _Mpu6050PageState createState() => _Mpu6050PageState();
}

class _Mpu6050PageState extends State<Mpu6050Page> {
  final String SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
  final String CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
  bool isReady;
  Stream<List<int>> stream;
  List<double> traceDust = List();

  @override
  void initState() {
    super.initState();
    isReady = false;
    connectToDevice();
  }

  connectToDevice() async {
    if (widget.device == null) {
      _Pop();
      return;
    }

    new Timer(const Duration(seconds: 15), () {
      if (!isReady) {
        disconnectFromDevice();
        _Pop();
      }
    });

    await widget.device.connect();
    discoverServices();
  }

  disconnectFromDevice() {
    if (widget.device == null) {
      _Pop();
      return;
    }

    widget.device.disconnect();
  }

  discoverServices() async {
    if (widget.device == null) {
      _Pop();
      return;
    }

    List<BluetoothService> services = await widget.device.discoverServices();
    services.forEach((service) {
      if (service.uuid.toString() == SERVICE_UUID) {
        service.characteristics.forEach((characteristic) {
          if (characteristic.uuid.toString() == CHARACTERISTIC_UUID) {
            characteristic.setNotifyValue(!characteristic.isNotifying);
            stream = characteristic.value;

            setState(() {
              isReady = true;
            });
          }
        });
      }
    });

    if (!isReady) {
      _Pop();
    }
  }

  Future<bool> _onWillPop() {
    return showDialog(
        context: context,
        builder: (context) =>
            new AlertDialog(
              title: Text('Are you sure?'),
              content: Text('Do you want to disconnect device and go back?'),
              actions: <Widget>[
                new FlatButton(
                    onPressed: () => Navigator.of(context).pop(false),
                    child: new Text('No')),
                new FlatButton(
                    onPressed: () {
                      disconnectFromDevice();
                      Navigator.of(context).pop(true);
                    },
                    child: new Text('Yes')),
              ],
            ) ??
            false);
  }

  _Pop() {
    Navigator.of(context).pop(true);
  }

  VMath.Vector3 _dataParser(List<int> dataFromDevice) {
    var eulerString = utf8.decode(dataFromDevice);
    var eulerList = eulerString.split(',');
    if (eulerList.length == 3) {
      double x = double.tryParse(eulerList[0]) ?? 0;
      double y = double.tryParse(eulerList[1]) ?? 0;
      double z = double.tryParse(eulerList[2]) ?? 0;

      return VMath.Vector3(x, y, z);
    } else {
      return VMath.Vector3.zero();
    }
  }

  @override
  Widget build(BuildContext context) {
    return WillPopScope(
      onWillPop: _onWillPop,
      child: Scaffold(
        appBar: AppBar(
          title: Text('MPU6050 Sensor'),
        ),
        body: Container(
            child: !isReady
                ? Center(
                    child: Text(
                      "Waiting...",
                      style: TextStyle(fontSize: 24, color: Colors.red),
                    ),
                  )
                : Container(
                    child: StreamBuilder<List<int>>(
                      stream: stream,
                      builder: (BuildContext context,
                          AsyncSnapshot<List<int>> snapshot) {
                        if (snapshot.hasError)
                          return Text('Error: ${snapshot.error}');

                        if (snapshot.connectionState ==
                            ConnectionState.active) {
                          var currentValue = _dataParser(snapshot.data);

                          return Center(
                              child: Stack(
                            children: <Widget>[
                              Container(
                                child: Center(
                                  child: new Object3D(
                                    size: const Size(300, 300),
                                    path: "assets/small_breadBoard.obj",
                                    asset: true,
                                    angleX: currentValue.x,
                                    angleY: currentValue.y,
                                    angleZ: currentValue.z,
                                    zoom: 50,
                                  ),
                                ),
                                color: Colors.blueGrey,
                              ),
                              Column(children: <Widget>[
                                Text('Current value from Sensor',
                                    style: TextStyle(fontSize: 14)),
                                Text('${currentValue.x}',
                                    style: TextStyle(
                                        fontWeight: FontWeight.bold,
                                        fontSize: 16)),
                                Text('${currentValue.y}',
                                    style: TextStyle(
                                        fontWeight: FontWeight.bold,
                                        fontSize: 16)),
                                Text('${currentValue.z}',
                                    style: TextStyle(
                                        fontWeight: FontWeight.bold,
                                        fontSize: 16))
                              ]),
                            ],
                          ));
                        } else {
                          return Text('Check the stream');
                        }
                      },
                    ),
                  )),
      ),
    );
  }
}
