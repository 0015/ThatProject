import 'dart:async';
import 'dart:io';
import 'dart:typed_data';

import 'package:path_provider/path_provider.dart';

class VideoUtil {
  static String workPath;
  static String appTempDir;

  static Future<void> getAppTempDirectory() async {
    appTempDir = '${(await getTemporaryDirectory()).path}/$workPath';
  }

  static Future<void> saveImageFileToDirectory(
      Uint8List byteData, String localName) async {
    Directory(appTempDir).create().then((Directory directory) async {
      final file = File('${directory.path}/$localName');

      await file.writeAsBytesSync(byteData.buffer
          .asUint8List(byteData.offsetInBytes, byteData.lengthInBytes));
      print("filePath : ${file.path}");
    });
  }

  static Future<void> deleteTempDirectory() {
    Directory(appTempDir).deleteSync(recursive: true);
  }

  static String generateEncodeVideoScript(String videoCodec, String fileName) {
    String outputPath = appTempDir + "/" + fileName;
    return "-hide_banner -y -i '" +
        appTempDir +
        "/" +
        "image_%d.jpg" +
        "' " +
        "-c:v " +
        videoCodec +
        " -r 12 " +
        outputPath;
  }
}
