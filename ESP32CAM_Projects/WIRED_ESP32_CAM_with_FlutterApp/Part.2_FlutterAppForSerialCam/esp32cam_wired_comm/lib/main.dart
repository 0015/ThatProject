/////////////////////////////////////////////////////////////////
/*
  ESP32CAM | Android(Flutter App) Wired Communication - Base64 Encoding/Decoding [2/2]
  Video Tutorial: https://youtu.be/-jygTxRmroA
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////
import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:image_gallery_saver/image_gallery_saver.dart';
import 'package:pinch_zoom_image_last/pinch_zoom_image_last.dart';
import 'package:string_validator/string_validator.dart';

import 'controller/serial_controller.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();
  runApp(GetMaterialApp(
    home: First(),
    theme: ThemeData.light(),
    darkTheme: ThemeData.dark(),
    themeMode: ThemeMode.system,
    debugShowCheckedModeBanner: false,
  ));
}

class First extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("ESP32CAM WIRED COMM APP For QVGA"),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            GetX<SerialController>(
              init: SerialController(),
              builder: (_) => Column(
                children: [
                  Text(_.logMessage),
                  SizedBox(
                    height: 16,
                  ),
                  Text(
                    'Your ESP32CAM Connected?: ${_.hardwareKeyConnected.value.toString().toUpperCase()}',
                    style: TextStyle(fontWeight: FontWeight.bold, fontSize: 20),
                  ),
                  SizedBox(
                    height: 16,
                  ),
                  _.hardwareKeyConnected.value
                      ? Column(
                          children: [
                            SizedBox(
                              height: 32,
                            ),
                            TextButton(
                              child: Text(
                                'Confirm',
                                style: TextStyle(
                                    fontWeight: FontWeight.bold, fontSize: 24),
                              ),
                              onPressed: () async {
                                Get.to(Second());
                              },
                            )
                          ],
                        )
                      : Padding(
                          padding: const EdgeInsets.only(top: 16),
                          child: Text(
                            'Please connect\nyour ESP32CAM via USB',
                            textAlign: TextAlign.center,
                            style: TextStyle(
                                fontWeight: FontWeight.bold, fontSize: 20),
                          ),
                        ),
                  SizedBox(
                    height: 32,
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class Second extends StatelessWidget {
  var _bytesImage;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        floatingActionButton: FloatingActionButton(
          backgroundColor: const Color(0xffffffff),
          foregroundColor: Colors.black,
          onPressed: () async {
            if (_bytesImage == null) return;

            final result = await ImageGallerySaver.saveImage(_bytesImage);
            Get.snackbar("Image Save", result != null ? "Success!" : "Fail!",
                duration: Duration(seconds: 3));
          },
          child: Icon(Icons.save),
        ),
        appBar: AppBar(
          title: Text('Viewer Page'),
        ),
        body: Center(
          child: Obx(() {
            final connected =
                Get.find<SerialController>().hardwareKeyConnected.value;

            if (connected) {
              final base64data =
                  Get.find<SerialController>().base64data.value.toString();

              if (isBase64(base64data)) {
                _bytesImage = Base64Decoder().convert(base64data);
                return showImageWithPinchZoom();
              } else {
                return showImageWithPinchZoom();
              }
            } else {
              Get.back();
              return Text('Disconnected =.=');
            }
          }),
        ));
  }

  Widget showImageWithPinchZoom() {
    return PinchZoomImage(
      image: ClipRRect(
        borderRadius: BorderRadius.all(Radius.circular(10)),
        child: Image.memory(
          _bytesImage,
          gaplessPlayback: true,
        ),
      ),
      zoomedBackgroundColor: Colors.black.withOpacity(0.5),
    );
  }
}
