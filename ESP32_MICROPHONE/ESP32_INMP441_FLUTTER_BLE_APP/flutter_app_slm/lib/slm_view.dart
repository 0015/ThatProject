import 'dart:convert' show utf8;

import 'package:flutter/material.dart';
import 'package:flutter_animation_progress_bar/flutter_animation_progress_bar.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:start_chart/chart/line/line_widget.dart';

class SLMView extends StatefulWidget {
  const SLMView({Key key, this.device}) : super(key: key);
  final BluetoothDevice device;

  @override
  _SLMViewState createState() => _SLMViewState();
}

class _SLMViewState extends State<SLMView> {
  final String SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
  final String CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

  Stream<List<int>> stream;
  bool isReady;
  double dBAValue = 0;
  String dBAStringValue;

  List<double> points = [50];

  @override
  void initState() {
    super.initState();
    isReady = false;
    _discoverServices();
  }

  _disconnectFromDevice() {
    if (widget.device == null) {
      _pop();
      return;
    }

    widget.device.disconnect().whenComplete(() => _pop());
  }

  _discoverServices() async {
    if (widget.device == null) {
      _pop();
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
  }

  _pop() {
    Navigator.of(context).pop(true);
  }

  Future<bool> _onWillPop() {
    return showDialog(
        context: context,
        builder: (context) =>
            AlertDialog(
              title: Text('Are you sure?'),
              content: Text('Do you want to disconnect the device and go back?'),
              actions: <Widget>[
                FlatButton(
                  onPressed: () => Navigator.of(context).pop(false),
                  child: Text('No'),
                ),
                FlatButton(
                  onPressed: () => _disconnectFromDevice(),
                  child: Text('Yes'),
                )
              ],
            ) ??
            false);
  }

  String _dataParser(List<int> dataFromDevice) {
    return utf8.decode(dataFromDevice);
  }

  _streamBody(AsyncSnapshot<List<int>> snapshot) {
    if (snapshot.connectionState == ConnectionState.active) {
      var currentValue = _dataParser(snapshot.data);
      dBAValue = double.tryParse(currentValue) ?? 0;
      dBAStringValue =
          dBAValue > 100 ? '' + dBAValue.toString() : '0' + dBAValue.toString();
      points.add(dBAValue);

      if (points.length > 20) {
        points.removeAt(0);
      }

      return Container(
          color: Colors.black,
          child: Stack(
            fit: StackFit.expand,
            children: <Widget>[
              Column(
                children: <Widget>[
                  Expanded(
                    flex: 3,
                    child: Container(
                      child: Center(
                        child: Row(
                          mainAxisAlignment: MainAxisAlignment.center,
                          crossAxisAlignment: CrossAxisAlignment.end,
                          children: <Widget>[
                            Text(
                              '$dBAStringValue',
                              textAlign: TextAlign.center,
                              style: TextStyle(
                                  fontSize: 70,
                                  color: Colors.white,
                                  fontFamily: 'DSEG7 Modern',
                                  fontWeight: FontWeight.w700),
                            ),
                            Text(
                              'dBA',
                              textAlign: TextAlign.center,
                              style: TextStyle(
                                fontSize: 30,
                                color: Colors.white,
                                fontFamily: 'DSEG7 Modern',
                              ),
                            )
                          ],
                        ),
                      ),
                    ),
                  ),
                  Expanded(
                    flex: 8,
                    child: Container(
                      color: Colors.white,
                      child: Stack(
                        children: <Widget>[
                          Center(
                            child: Container(
                              color: Colors.amber,
                              child: FAProgressBar(
                                backgroundColor: Colors.transparent,
                                borderRadius: 0,
                                size: MediaQuery.of(context).size.width,
                                animatedDuration: Duration(milliseconds: 500),
                                maxValue: 140,
                                direction: Axis.vertical,
                                verticalDirection: VerticalDirection.up,
                                currentValue: dBAValue.toInt(),
                                displayText: ' dBA',
                                changeColorValue: 90,
                                changeProgressColor: Colors.red,
                                progressColor: Colors.green,
                              ),
                            ),
                          ),
                          Center(
                            child: Container(
                              child: Center(
                                child: LineChart(
                                  data: points,
                                  size: Size(
                                      MediaQuery.of(context).size.width - 100,
                                      MediaQuery.of(context).size.height / 3),
                                  lineWidth: 5,
                                  lineColor: Colors.blueGrey,
                                  pointSize: 10,
                                  pointColor: Colors.amberAccent,
                                ),
                              ),
                            ),
                          ),
                        ],
                      ),
                    ),
                  )
                ],
              )
            ],
          ));
    } else {
      return Center(
        child: Text(
          'ConnectionState is not active',
          style: TextStyle(fontSize: 24, color: Colors.white),
        ),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return WillPopScope(
      onWillPop: _onWillPop,
      child: Scaffold(
          appBar: AppBar(
            title: Text('Sound Level Meter Viewer'),
          ),
          body: Container(
              child: !isReady
                  ? Center(
                      child: Text(
                        'Waiting...',
                        style: TextStyle(fontSize: 24, color: Colors.white),
                      ),
                    )
                  : Container(
                      child: StreamBuilder<List<int>>(
                        stream: stream,
                        builder: (BuildContext context,
                            AsyncSnapshot<List<int>> snapshot) {
                          if (snapshot.hasError) {
                            return Text('Error: ${snapshot.error}');
                          }
                          return _streamBody(snapshot);
                        },
                      ),
                    ))),
    );
  }
}
