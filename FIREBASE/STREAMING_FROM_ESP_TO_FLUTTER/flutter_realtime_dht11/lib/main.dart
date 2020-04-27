import 'package:flutter/material.dart';

import './singlepage_app.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      home: SinglePageApp(),
      theme: ThemeData(
          primaryColor: Colors.amber, primarySwatch: Colors.deepOrange),
      darkTheme: ThemeData.dark(),
    );
  }
}
