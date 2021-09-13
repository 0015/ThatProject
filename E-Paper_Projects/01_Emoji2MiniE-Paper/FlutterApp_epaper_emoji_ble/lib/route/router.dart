import 'dart:typed_data';

import 'package:epaper_emoji_ble/route/routing_constants.dart';
import 'package:epaper_emoji_ble/screens/ble_list_screen.dart';
import 'package:epaper_emoji_ble/screens/emji_converter_screen.dart';
import 'package:epaper_emoji_ble/screens/emoji_screen.dart';
import 'package:epaper_emoji_ble/screens/splash_screen.dart';
import 'package:epaper_emoji_ble/screens/undefined_screen.dart';
import 'package:flutter/material.dart';

Route<dynamic> generateRoute(RouteSettings settings) {
  switch (settings.name) {
    case SplashScreenRoute:
      return MaterialPageRoute(builder: (context) => const SplashScreen());

    case EmojiScreenRoute:
      return MaterialPageRoute(builder: (context) => const EmojiScreen());

    case EmojiConverterScreenRoute:
      return MaterialPageRoute(
          builder: (context) => EmojiConverterScreen(
              selectedEmoji: settings.arguments as String));

    case BLEScreenRoute:
      return MaterialPageRoute(
          builder: (context) =>
              BleListScreen(headedBitmap: settings.arguments as Uint8List));

    default:
      return MaterialPageRoute(
          builder: (context) => UndefinedView(
                name: settings.name!,
              ));
  }
}
