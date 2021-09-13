import 'dart:typed_data';

class Bitmap24 {
  /// The width in pixels of the image.
  final int width;

  /// The width in pixels of the image.
  final int height;

  /// A [Uint8List] of bytes in a RGB format.
  final Uint8List content;

  int get size => (width * height) * RGB24BitmapHeader.pixelLength;

  Bitmap24.fromHeadless(this.width, this.height, this.content);

  Bitmap24.fromHeadful(this.width, this.height, Uint8List headedIntList)
      : content = headedIntList.sublist(
          RGB24BitmapHeader.RGB24HeaderSize,
          headedIntList.length,
        );

  Bitmap24.blank(
    this.width,
    this.height,
  ) : content = Uint8List.fromList(
          List.filled(width * height * RGB24BitmapHeader.pixelLength, 0),
        );

  Uint8List buildHeaded() {
    final header = RGB24BitmapHeader(size, width, height)
      ..applyContent(content);
    return header.headerIntList;
  }

  static Uint8List fromBitMap32(int width, int height, Uint8List bitmap32) {
    Uint8List R8G8B8 = Uint8List(width * height * 3);
    int j = 0;
    for (int i = 0; i < (bitmap32.length - 3); i = i + 4) {
      var Red = bitmap32[i + 2];
      var Green = bitmap32[i + 1];
      var Blue = bitmap32[i];

      R8G8B8[j] = Red;
      R8G8B8[j + 1] = Green;
      R8G8B8[j + 2] = Blue;
      j = j + 3;
    }

    return R8G8B8;
  }
}

class RGB24BitmapHeader {
  static const int pixelLength = 3;
  static const int RGB24HeaderSize = 54;

  RGB24BitmapHeader(this.contentSize, int width, int height) {
    headerIntList = Uint8List(fileLength);

    final ByteData bd = headerIntList.buffer.asByteData();
    bd.setUint8(0x0, 0x42);
    bd.setUint8(0x1, 0x4d);
    bd.setInt32(0x2, fileLength, Endian.little);
    bd.setInt32(0xa, RGB24HeaderSize, Endian.little);
    bd.setUint32(0xe, 28, Endian.little);
    bd.setUint32(0x12, width, Endian.little);
    bd.setUint32(0x16, height, Endian.little);
    bd.setUint16(0x1a, 1, Endian.little);
    bd.setUint32(0x1c, 24, Endian.little); // pixel size
    bd.setUint32(0x1e, 0, Endian.little); //BI_BITFIELDS
    bd.setUint32(0x22, contentSize, Endian.little);
  }

  int contentSize;

  void applyContent(Uint8List contentIntList) {
    headerIntList.setRange(
      RGB24HeaderSize,
      fileLength,
      contentIntList,
    );
  }

  late Uint8List headerIntList;

  int get fileLength => contentSize + RGB24HeaderSize;
}
