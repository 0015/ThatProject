//flutter_bluetooth_serial_example
//https://github.com/edufolly/flutter_bluetooth_serial/tree/master/example

import 'package:flutter/material.dart';

import './MainPage.dart';

void main() => runApp(new ExampleApplication());

class ExampleApplication extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(home: MainPage());
  }
}
