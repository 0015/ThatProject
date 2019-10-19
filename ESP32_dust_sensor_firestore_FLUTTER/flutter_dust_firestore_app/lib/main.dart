import 'dart:async';

import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:flutter/material.dart';
import 'package:oscilloscope/oscilloscope.dart';
import 'package:pie_chart/pie_chart.dart';

void main() {
  runApp(MaterialApp(
    debugShowCheckedModeBanner: false,
    theme: ThemeData.dark(),
    title: "Dust Sensor Through Firestore",
    home: DustApp(),
  ));
}

class DustApp extends StatefulWidget {
  @override
  _DustAppState createState() => _DustAppState();
}

class _DustAppState extends State<DustApp> {
  final FirebaseAuth _auth = FirebaseAuth.instance;

  bool _success;
  List<double> traceData = List();
  Timer _timer;
  Map<String, double> dataMap = new Map();
  List<Color> colorList = [Colors.red, Colors.green];
  double globalCurrentSensorValue = 0;

  @override
  void dispose() {
    super.dispose();
    _timer.cancel();
  }

  @override
  void initState() {
    super.initState();
    _success = false;
    _SigninWithAnonymous();

    _timer = Timer.periodic(Duration(milliseconds: 500), _generateTrace);
    dataMap.putIfAbsent("Contamination", () => 0);
    dataMap.putIfAbsent("Clean", () => 100);
  }

  _SigninWithAnonymous() async {
    final FirebaseUser user = (await _auth.signInAnonymously()).user;

    setState(() {
      if (user != null) {
        print('Login Success');
        _success = true;
      } else {
        print('Login Failure');
        _success = false;
      }
    });
  }

  _generateTrace(Timer t) {
    setState(() {
      traceData.add(globalCurrentSensorValue);
    });
  }

  Widget buildStreamBuilder() {
    Oscilloscope graph = Oscilloscope(
      showYAxis: true,
      yAxisColor: Colors.orange,
      padding: 20.0,
      backgroundColor: Colors.black,
      traceColor: Colors.green,
      yAxisMax: 3000.0,
      yAxisMin: 0.0,
      dataSet: traceData,
    );

    return Container(
        child: !_success
            ? Center(
                child: CircularProgressIndicator(
                  valueColor: AlwaysStoppedAnimation<Color>(Colors.red),
                ),
              )
            : StreamBuilder(
                stream: Firestore.instance.collection('sensors').snapshots(),
                builder: (context, snapshot) {
                  if (!snapshot.hasData) {
                    return Center(
                      child: CircularProgressIndicator(
                        valueColor: AlwaysStoppedAnimation<Color>(Colors.red),
                      ),
                    );
                  } else {
                    snapshot.data.documents.forEach((f) {
                      if (f.documentID == '9vga20P652xhl6pgLidS') {
                        print('current value = ${f['dust']}');
                        globalCurrentSensorValue = f['dust'].toDouble();

                        var tempContaminationValue =
                            globalCurrentSensorValue / 3000 * 100;
                        var tempCleanValue = 100 - tempContaminationValue;

                        dataMap["Contamination"] = tempContaminationValue;
                        dataMap["Clean"] = tempCleanValue;
                      }
                    });

                    return Center(
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: <Widget>[
                          Expanded(
                            flex: 1,
                            child: graph,
                          ),
                          Expanded(
                            flex: 1,
                            child: Column(
                              mainAxisAlignment: MainAxisAlignment.center,
                              children: <Widget>[
                                Text(
                                  'Current value from Sensor',
                                  style: TextStyle(fontSize: 14),
                                ),
                                Text(
                                  '${globalCurrentSensorValue} ug/m3',
                                  style: TextStyle(
                                      fontWeight: FontWeight.bold,
                                      fontSize: 24),
                                ),
                                PieChart(
                                  dataMap: dataMap,
                                  legendFontColor: Colors.grey[400],
                                  legendFontSize: 14.0,
                                  legendFontWeight: FontWeight.w500,
                                  animationDuration:
                                      Duration(milliseconds: 800),
                                  chartLegendSpacing: 32.0,
                                  chartRadius:
                                      MediaQuery.of(context).size.width / 2.7,
                                  showChartValues: true,
                                  showChartValuesInPercentage: true,
                                  showChartValuesOutside: false,
                                  chartValuesColor:
                                      Colors.blueGrey[900].withOpacity(0.9),
                                  colorList: colorList,
                                  showLegends: true,
                                  decimalPlaces: 1,
                                )
                              ],
                            ),
                          )
                        ],
                      ),
                    );
                  }
                },
              ));
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Dust Sensor Through Firestore'),
      ),
      body: buildStreamBuilder(),
    );
  }
}
