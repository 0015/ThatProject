import 'dart:convert';
import 'dart:typed_data';

class WavHeader {
  static Uint8List toBytes(String str) {
    var encoder = AsciiEncoder();
    return encoder.convert(str);
  }

  static List<int> createWavHeader(int wavSize) {
    List<int> _byte = List<int>();

    var chunkIDBytes = toBytes('RIFF');
    var _chunkIDBytes = chunkIDBytes.buffer.asByteData();
    for (int i = 0; i < 4; i++) {
      print("chunkIDBytes Hex: ${_chunkIDBytes.getUint8(i).toRadixString(16)}");
      _byte.add(_chunkIDBytes.getUint8(i));
    }

    var chunkSize = ByteData(4);
    int fileSize = wavSize + 44 - 8;
    chunkSize.setUint32(0, fileSize, Endian.little);
    for (int i = 0; i < 4; i++) {
      print("ChunkSize HEX: ${chunkSize.getUint8(i).toRadixString(16)}");
      _byte.add(chunkSize.getUint8(i));
    }

    var waveFmt = toBytes('WAVEfmt ');
    var _waveFmt = waveFmt.buffer.asByteData();
    for (int i = 0; i < 8; i++) {
      _byte.add(_waveFmt.getUint8(i));
    }

    var chunkLength = ByteData(4);
    chunkLength.setUint32(0, 16, Endian.little);
    for (int i = 0; i < 4; i++) {
      _byte.add(chunkLength.getUint8(i));
    }

    var audioFormat = ByteData(2);
    audioFormat.setUint16(0, 1, Endian.little);
    for (int i = 0; i < 2; i++) {
      _byte.add(audioFormat.getUint8(i));
    }

    var numChannel = ByteData(2);
    numChannel.setUint16(0, 1, Endian.little);
    for (int i = 0; i < 2; i++) {
      _byte.add(numChannel.getUint8(i));
    }

    var sampleRate = ByteData(4);
    sampleRate.setUint32(0, 16000, Endian.little);
    for (int i = 0; i < 4; i++) {
      _byte.add(sampleRate.getUint8(i));
    }

    var byteRate = ByteData(4);
    byteRate.setUint32(0, 32000, Endian.little);
    for (int i = 0; i < 4; i++) {
      _byte.add(byteRate.getUint8(i));
    }

    var blockAlign = ByteData(2);
    blockAlign.setUint16(0, 2, Endian.little);
    for (int i = 0; i < 2; i++) {
      _byte.add(blockAlign.getUint8(i));
    }

    var bitsPerSample = ByteData(2);
    bitsPerSample.setUint16(0, 16, Endian.little);
    for (int i = 0; i < 2; i++) {
      _byte.add(bitsPerSample.getUint8(i));
    }

    var subChunk2ID = toBytes('data');
    var _subChunk2ID = subChunk2ID.buffer.asByteData();
    for (int i = 0; i < 4; i++) {
      _byte.add(_subChunk2ID.getUint8(i));
    }

    var subChunk2Size = ByteData(4);
    subChunk2Size.setUint32(0, wavSize, Endian.little);
    for (int i = 0; i < 4; i++) {
      _byte.add(subChunk2Size.getUint8(i));
    }
    print("*** MADE header length = ${_byte.length}");
    return _byte;
  }
}
