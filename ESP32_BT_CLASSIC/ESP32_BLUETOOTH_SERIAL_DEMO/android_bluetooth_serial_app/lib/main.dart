/////////////////////////////////////////////////////////////////
/*
  ESP32 | BLUETOOTH CLASSIC | FLUTTER - Let's build BT Serial based on the examples. (Ft. Chat App)
  Video Tutorial: https://youtu.be/WUw-_X66dLE
  Created by Eric N. (ThatProject)
*/

// Updated 06-21-2021
// Due to Flutter's update, many parts have changed from the tutorial video.
// You need to keep @dart=2.9 before starting main to avoid null safety in Flutter 2
/////////////////////////////////////////////////////////////////

// @dart=2.9
import 'package:flutter/material.dart';

import './MainPage.dart';

void main() => runApp(new ExampleApplication());

class ExampleApplication extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(debugShowCheckedModeBanner: false, home: MainPage());
  }
}
