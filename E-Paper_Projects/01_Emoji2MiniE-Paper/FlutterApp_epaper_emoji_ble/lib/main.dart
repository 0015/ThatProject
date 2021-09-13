/////////////////////////////////////////////////////////////////
/*
  Mini E-Paper | Ep1. Uploading Emoji to E-Paper from Flutter App(iOS & Android)
  Video Tutorial: https://youtu.be/pP9YXFNOBhY
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

import 'package:epaper_emoji_ble/route/router.dart' as router;
import 'package:epaper_emoji_ble/route/routing_constants.dart';
import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';

import 'const/custom_colors.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Emoji -> E-Paper App',
      theme: ThemeData(
        textTheme: GoogleFonts.poppinsTextTheme(Theme.of(context).textTheme),
        scaffoldBackgroundColor: kMainBG,
        primarySwatch: Colors.amber,
        visualDensity: VisualDensity.adaptivePlatformDensity,
      ),
      onGenerateRoute: router.generateRoute,
      initialRoute: SplashScreenRoute,
    );
  }
}
