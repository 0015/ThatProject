import 'dart:async';

import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'package:flutter_animation_progress_bar/flutter_animation_progress_bar.dart';
import 'package:flutter_icons/flutter_icons.dart';
import 'package:marquee/marquee.dart';
import 'package:wave_progress_widget/wave_progress.dart';

import './dht.dart';

class SinglePageApp extends StatefulWidget {
  @override
  _SinglePageAppState createState() => _SinglePageAppState();
}

class _SinglePageAppState extends State<SinglePageApp>
    with SingleTickerProviderStateMixin {
  TabController _tabController;
  int tabIndex = 0;

  final FirebaseAuth _auth = FirebaseAuth.instance;
  DatabaseReference _dhtRef =
      FirebaseDatabase.instance.reference().child('DHT');

  bool _signIn;
  String heatIndexText;
  Timer _timer;

  @override
  void initState() {
    super.initState();
    _tabController = TabController(length: 2, vsync: this);
    _signIn = false;
    heatIndexText = "Showing Heat Index Here Soon ...";

    _timer = Timer.periodic(Duration(seconds: 10), (timer) {
      if (_signIn) {
        setState(() {});
      }
    });

    _signInAnonymously();
  }

  @override
  void dispose() {
    _tabController.dispose();
    _timer.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return _signIn ? mainScaffold() : signInScaffold();
  }

  Widget mainScaffold() {
    return Scaffold(
      appBar: AppBar(
        title: Text("FIREBASE REALTIME DB"),
        bottom: TabBar(
          controller: _tabController,
          onTap: (int index) {
            setState(() {
              tabIndex = index;
            });
          },
          tabs: [
            Tab(
              icon: Icon(MaterialCommunityIcons.temperature_fahrenheit),
            ),
            Tab(
              icon: Icon(MaterialCommunityIcons.water_percent),
            )
          ],
        ),
      ),
      body: Column(
        children: [
          Container(
            height: 30,
            child: _buildMarquee(),
          ),
          Expanded(
            child: StreamBuilder(
                stream: _dhtRef.onValue,
                builder: (context, snapshot) {
                  if (snapshot.hasData &&
                      !snapshot.hasError &&
                      snapshot.data.snapshot.value != null) {
                    var _dht =
                        DHT.fromJson(snapshot.data.snapshot.value['Json']);
                    print(
                        "DHT: ${_dht.temp} / ${_dht.humidity} / ${_dht.heatIndex}");
                    _setMarqueeText(_dht);
                    return IndexedStack(
                      index: tabIndex,
                      children: [
                        _temperatureLayout(_dht),
                        _humidityLayout(_dht)
                      ],
                    );
                  } else {
                    return Center(
                      child: Text("NO DATA YET"),
                    );
                  }
                }),
          ),
        ],
      ),
    );
  }

  Widget _temperatureLayout(DHT _dht) {
    return Center(
        child: Column(
      children: [
        Container(
          padding: const EdgeInsets.only(top: 40),
          child: Text(
            "TEMPERATURE",
            style: TextStyle(fontWeight: FontWeight.bold, fontSize: 24),
          ),
        ),
        Expanded(
          child: Padding(
            padding: const EdgeInsets.symmetric(vertical: 20),
            child: FAProgressBar(
              progressColor: Colors.green,
              direction: Axis.vertical,
              verticalDirection: VerticalDirection.up,
              size: 100,
              currentValue: _dht.temp.round(),
              changeColorValue: 100,
              changeProgressColor: Colors.red,
              maxValue: 150,
              displayText: "°F",
              borderRadius: 16,
              animatedDuration: Duration(milliseconds: 500),
            ),
          ),
        ),
        Container(
          padding: const EdgeInsets.only(bottom: 40),
          child: Text(
            "${_dht.temp.toStringAsFixed(2)} °F",
            style: TextStyle(fontWeight: FontWeight.bold, fontSize: 28),
          ),
        )
      ],
    ));
  }

  Widget _humidityLayout(DHT _dht) {
    return Center(
        child: Column(
      children: [
        Container(
          padding: const EdgeInsets.only(top: 40),
          child: Text(
            "HUMIDITY",
            style: TextStyle(fontWeight: FontWeight.bold, fontSize: 24),
          ),
        ),
        Expanded(
          child: Padding(
              padding: const EdgeInsets.symmetric(vertical: 50),
              child: WaveProgress(
                  180.0, Colors.blue, Colors.blueAccent, _dht.humidity)),
        ),
        Container(
          padding: const EdgeInsets.only(bottom: 40),
          child: Text(
            "${_dht.humidity.toStringAsFixed(2)} %",
            style: TextStyle(fontWeight: FontWeight.bold, fontSize: 28),
          ),
        )
      ],
    ));
  }

  _setMarqueeText(DHT _dht) {
    heatIndexText = "Heat Index: ${_dht.heatIndex.toStringAsFixed(2)} °F, ";
    if (_dht.heatIndex > 80 && _dht.heatIndex <= 90) {
      heatIndexText +=
          "Caution: fatigue is possible with prolonged exposure and activity. Continuing activity could result in heat cramps. ";
    } else if (_dht.heatIndex > 90 && _dht.heatIndex <= 105) {
      heatIndexText +=
          "Extreme caution: heat cramps and heat exhaustion are possible. Continuing activity could result in heat stroke. ";
    } else if (_dht.heatIndex > 105 && _dht.heatIndex <= 130) {
      heatIndexText +=
          "Danger: heat cramps and heat exhaustion are likely; heat stroke is probable with continued activity. ";
    } else if (_dht.heatIndex > 130) {
      heatIndexText += "Extreme danger: heat stroke is imminent. ";
    } else {
      heatIndexText += "Normal. ";
    }
  }

  Widget _buildMarquee() {
    return Marquee(
      text: heatIndexText,
      style: TextStyle(fontStyle: FontStyle.italic, fontSize: 20),
    );
  }

  Widget signInScaffold() {
    return Scaffold(
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text(
              "SIMPLE FIREBASE FLUTTER APP",
              style: TextStyle(fontWeight: FontWeight.bold, fontSize: 20),
            ),
            SizedBox(
              height: 50,
            ),
            RaisedButton(
              textColor: Colors.white,
              color: Colors.red,
              shape: new RoundedRectangleBorder(
                  borderRadius: new BorderRadius.circular(20),
                  side: BorderSide(color: Colors.red)),
              onPressed: () async {
                _signInAnonymously();
              },
              child: Text(
                "ANONYMOUS SIGN-IN",
                style: TextStyle(fontSize: 14),
              ),
            ),
          ],
        ),
      ),
    );
  }

  void _signInAnonymously() async {
    final FirebaseUser user = (await _auth.signInAnonymously()).user;
    print("*** user isAnonymous: ${user.isAnonymous}");
    print("*** user uid: ${user.uid}");

    setState(() {
      if (user != null) {
        _signIn = true;
      } else {
        _signIn = false;
      }
    });
  }
}
