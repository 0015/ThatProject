import 'dart:typed_data';
import 'dart:ui' as ui;

import 'package:adaptive_dialog/adaptive_dialog.dart';
import 'package:bitmap/bitmap.dart';
import 'package:epaper_emoji_ble/const/custom_styles.dart';
import 'package:epaper_emoji_ble/route/routing_constants.dart';
import 'package:epaper_emoji_ble/util/bitmap24.dart';
import 'package:flutter/material.dart';
import 'package:flutter/rendering.dart';
import 'package:image/image.dart' as img;

class EmojiConverterScreen extends StatefulWidget {
  final String selectedEmoji;

  const EmojiConverterScreen({Key? key, required this.selectedEmoji})
      : super(key: key);

  @override
  _EmojiConverterScreenState createState() => _EmojiConverterScreenState();
}

class _EmojiConverterScreenState extends State<EmojiConverterScreen> {
  GlobalKey globalKey = GlobalKey();
  Uint8List? bitmap24WithHeader;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Emoji Converter'),
      ),
      body: ListView(children: [
        Center(
          child: Column(
            children: [
              const SizedBox(
                height: 40,
              ),
              RepaintBoundary(
                key: globalKey,
                child: Container(
                  color: Colors.transparent,
                  width: 200,
                  height: 200,
                  child: Center(
                      child: Text(
                    widget.selectedEmoji,
                    style: const TextStyle(fontSize: 160),
                  )),
                ),
              ),
              const SizedBox(
                height: 40,
              ),
              bitmap24WithHeader == null
                  ? Container(
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
                        onPressed: _convertImage,
                        child: Text(
                          'Convert to Bitmap',
                          style: kButtonText.copyWith(color: Colors.white),
                        ),
                      ))
                  : Container(
                      height: 100,
                      width: 100,
                      decoration: BoxDecoration(
                        color: Colors.grey[300],
                        borderRadius: BorderRadius.circular(18),
                      ),
                      child: Image.memory(
                        bitmap24WithHeader!,
                      )),
              const SizedBox(
                height: 40,
              ),
              bitmap24WithHeader != null
                  ? Container(
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
                        onPressed: _findBLEDevice,
                        child: Text(
                          'Find & Select Your Mini E-Paper',
                          style: kButtonText.copyWith(color: Colors.white),
                        ),
                      ))
                  : Container()
            ],
          ),
        ),
      ]),
    );
  }

  _convertImage() async {
    final RenderRepaintBoundary boundary =
        globalKey.currentContext!.findRenderObject()! as RenderRepaintBoundary;
    final ui.Image image = await boundary.toImage();
    final ByteData? byteData =
        await image.toByteData(format: ui.ImageByteFormat.png);
    final Uint8List pngBytes = byteData!.buffer.asUint8List();

    var pngImage;
    try {
      pngImage = img.decodePng(pngBytes);
    } catch (e) {
      await showOkAlertDialog(
        context: context,
        title: 'Error',
        message: e.toString(),
      );
      return;
    }

    var resizedImage = img.copyResize(pngImage, width: 80, height: 80);
    var grayImage = img.grayscale(resizedImage);
    var imgThreeColor = img.quantize(grayImage, numberOfColors: 3);
    var flipImage = img.flip(imgThreeColor, img.Flip.vertical);

    Bitmap bitmap = Bitmap.fromHeadless(
        resizedImage.width, resizedImage.height, flipImage.getBytes());

    Uint8List bitmap24Data = Bitmap24.fromBitMap32(
        resizedImage.width, resizedImage.height, bitmap.content);
    Bitmap24 bitmap24 = Bitmap24.fromHeadless(
        resizedImage.width, resizedImage.height, bitmap24Data);

    setState(() {
      bitmap24WithHeader = bitmap24.buildHeaded();
    });
  }

  _findBLEDevice() async {
    if (bitmap24WithHeader == null) {
      await showOkAlertDialog(
        context: context,
        title: 'Error',
        message: 'Check the Bitmap Emoji',
      );
      return;
    }
    Navigator.pushNamed(context, BLEScreenRoute, arguments: bitmap24WithHeader);
  }
}
