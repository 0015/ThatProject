import 'dart:async';

import 'package:flutter/material.dart';

class BlinkingTimer extends StatefulWidget {
  @override
  _BlinkingTimerState createState() => _BlinkingTimerState();
}

class _BlinkingTimerState extends State<BlinkingTimer>
    with SingleTickerProviderStateMixin {
  AnimationController _animationController;
  DateTime currentTime;
  String _timeString;
  Timer _timer;

  @override
  void initState() {
    super.initState();
    _animationController =
        AnimationController(vsync: this, duration: Duration(seconds: 1));
    _animationController.repeat();

    _timeString = "00:00";
    currentTime = DateTime.now();
    _timer = Timer.periodic(Duration(seconds: 1), (Timer t) => _getTimer());
  }

  @override
  void dispose() {
    _animationController.dispose();
    _timer.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Container(
      width: 100,
      height: 30,
      decoration: BoxDecoration(
          color: Colors.black54,
          borderRadius: BorderRadius.all(Radius.circular(5.0))),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.center,
        children: <Widget>[
          FadeTransition(
            opacity: _animationController,
            child: Container(
              width: 20,
              height: 20,
              decoration:
                  BoxDecoration(color: Colors.red, shape: BoxShape.circle),
            ),
          ),
          SizedBox(
            width: 10,
          ),
          Text(_timeString)
        ],
      ),
    );
  }

  _getTimer() {
    final DateTime now = DateTime.now();
    Duration d = now.difference(currentTime);

    setState(() {
      _timeString =
          "${twoDigits(d.inMinutes.remainder(60))}:${twoDigits(d.inSeconds.remainder(60))}";
    });
  }

  String twoDigits(int n) {
    if (n >= 10) return "$n";
    return "0$n";
  }
}
