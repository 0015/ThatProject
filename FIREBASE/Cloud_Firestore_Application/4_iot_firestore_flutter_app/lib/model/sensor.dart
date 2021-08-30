import 'package:flutter/foundation.dart';

@immutable
class Sensor {
  Sensor({
    required this.humidity,
    required this.temperature,
  });

  Sensor.fromJson(Map<String, Object?> json)
      : this(
          humidity: json['humidity']! as double,
          temperature: json['temperature']! as double,
        );

  final double humidity;
  final double temperature;

  Map<String, Object?> toJson() {
    return {
      'humidity': humidity,
      'temperature': temperature,
    };
  }
}
