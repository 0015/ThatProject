// Copyright 2017, Paul DeMarco.
// All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.
part of flutter_ble;


class Guid {
  final List<int> _bytes;
  final int _hashCode;

  Guid._internal(List<int> bytes)
      : _bytes = bytes,
        _hashCode = _calcHashCode(bytes);

  Guid(String input) : this._internal(_fromString(input));

  Guid.fromMac(String input) : this._internal(_fromMacString(input));

  Guid.empty() : this._internal(new List.filled(16, 0));

  static List<int> _fromMacString(input) {
    var bytes = new List<int>.filled(16, 0);

    if (input == null) {
      throw new ArgumentError("Input was null");
    }
    input = input.toLowerCase();

    final RegExp regex = new RegExp('[0-9a-f]{2}');
    Iterable<Match> matches = regex.allMatches(input);

    if (matches.length != 6) {
      throw new FormatException("The format is invalid: " + input);
    }

    int i = 0;
    for (Match match in matches) {
      var hexString = input.substring(match.start, match.end);
      bytes[i] = hex.decode(hexString)[0];
      i++;
    }

    return bytes;
  }

  static List<int> _fromString(input) {
    var bytes = new List<int>.filled(16, 0);
    if (input == null) {
      throw new ArgumentError("Input was null");
    }
    if (input.length < 32) {
      throw new FormatException("The format is invalid");
    }
    input = input.toLowerCase();

    final RegExp regex = new RegExp('[0-9a-f]{2}');
    Iterable<Match> matches = regex.allMatches(input);
    if (matches.length != 16) {
      throw new FormatException("The format is invalid");
    }
    int i = 0;
    for (Match match in matches) {
      var hexString = input.substring(match.start, match.end);
      bytes[i] = hex.decode(hexString)[0];
      i++;
    }
    return bytes;
  }

  static int _calcHashCode(List<int> bytes) {
    const equality = const ListEquality<int>();
    return equality.hash(bytes);
  }

  @override
  String toString() {
    String one = hex.encode(_bytes.sublist(0, 4));
    String two = hex.encode(_bytes.sublist(4, 6));
    String three = hex.encode(_bytes.sublist(6, 8));
    String four = hex.encode(_bytes.sublist(8, 10));
    String five = hex.encode(_bytes.sublist(10, 16));
    return "$one-$two-$three-$four-$five";
  }

  String toMac() {
    String one = hex.encode(_bytes.sublist(0, 1));
    String two = hex.encode(_bytes.sublist(1, 2));
    String three = hex.encode(_bytes.sublist(2, 3));
    String four = hex.encode(_bytes.sublist(3, 4));
    String five = hex.encode(_bytes.sublist(4, 5));
    String six = hex.encode(_bytes.sublist(5, 6));
    return "$one:$two:$three:$four:$five:$six".toUpperCase();
  }

  List<int> toByteArray() {
    return _bytes;
  }

  operator ==(other) => other is Guid && this.hashCode == other.hashCode;

  int get hashCode => _hashCode;
}
