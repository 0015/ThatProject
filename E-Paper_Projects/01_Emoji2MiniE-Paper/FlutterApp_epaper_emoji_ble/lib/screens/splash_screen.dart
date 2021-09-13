import 'package:epaper_emoji_ble/const/custom_styles.dart';
import 'package:epaper_emoji_ble/route/routing_constants.dart';
import 'package:flutter/material.dart';

class SplashScreen extends StatelessWidget {
  const SplashScreen({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: ListView(
        children: [
          _getScreen(context),
        ],
      ),
    );
  }

  _getScreen(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.only(left: 20, right: 20, top: 40, bottom: 10),
      child: Column(
        children: [
          Column(
            children: [
              Center(
                child: Container(
                  child: const Image(
                    image: AssetImage(
                      'assets/images/iot_image.png',
                    ),
                    height: 200,
                    color: Colors.white,
                  ),
                ),
              ),
              const SizedBox(
                height: 20,
              ),
              const Text(
                "E-Paper\n^\n|\nBLE\n|\nEmoji😃",
                style: kHeadline,
                textAlign: TextAlign.center,
              ),
              const SizedBox(
                height: 20,
              ),
              SizedBox(
                width: MediaQuery.of(context).size.width * 0.8,
                child: const Text(
                  "It's a project that allows you to send emojis to Mini E-Paper. For detail, check out my channel. www.that-project.com",
                  style: kBodyText,
                  textAlign: TextAlign.center,
                ),
              )
            ],
          ),
          const SizedBox(
            height: 50,
          ),
          Container(
              height: 60,
              width: MediaQuery.of(context).size.width * 0.8,
              decoration: BoxDecoration(
                color: Colors.grey[850],
                borderRadius: BorderRadius.circular(18),
              ),
              child: TextButton(
                style: ButtonStyle(
                  overlayColor: MaterialStateProperty.resolveWith(
                    (states) => Colors.black12,
                  ),
                ),
                onPressed: () {
                  Navigator.pushNamedAndRemoveUntil(context, EmojiScreenRoute,
                      (Route<dynamic> route) => false);
                },
                child: Text(
                  'GET STARTED',
                  style: kButtonText.copyWith(color: Colors.white),
                ),
              ))
        ],
      ),
    );
  }
}
