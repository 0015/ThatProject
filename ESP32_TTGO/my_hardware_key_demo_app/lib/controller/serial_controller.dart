import 'dart:async';
import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:usb_serial/transaction.dart';
import 'package:usb_serial/usb_serial.dart';

class SerialController extends GetxController {
  UsbPort _port;
  StreamSubscription<String> _subscription;
  Transaction<String> _transaction;
  int _deviceId;
  TextEditingController textController;
  String _keyFromDevice;
  String logMessage = "";
  final hardwareKeyConnected = false.obs;

  Future<bool> _connectTo(device) async {
    if (_subscription != null) {
      _subscription.cancel();
      _subscription = null;
    }

    if (_transaction != null) {
      _transaction.dispose();
      _transaction = null;
    }

    if (_port != null) {
      _port.close();
      _port = null;
    }

    if (device == null) {
      _deviceId = null;
      return true;
    }

    _port = await device.create();
    if (!await _port.open()) {
      return false;
    }

    _deviceId = device.deviceId;
    await _port.setDTR(true);
    await _port.setRTS(true);
    await _port.setPortParameters(
        115200, UsbPort.DATABITS_8, UsbPort.STOPBITS_1, UsbPort.PARITY_NONE);

    _transaction = Transaction.stringTerminated(
        _port.inputStream, Uint8List.fromList([13, 10]));

    _subscription = _transaction.stream.listen((String line) {
      _keyFromDevice = line;
      textController.text = line;
    });

    return true;
  }

  void _getPorts() async {
    List<UsbDevice> devices = await UsbSerial.listDevices();
    print(devices);
    bool targetDeviceFound = false;

    devices.forEach((device) {
      logMessage = "[Log] device pid: ${device.pid}, device vid: ${device.vid}";
      if (device.pid == 60000 && device.vid == 4292) {
        targetDeviceFound = true;

        if (!hardwareKeyConnected.value) {
          _connectTo(device).then((res) {
            hardwareKeyConnected.value = res;
            sendString("key?");
            Get.snackbar("Device Status", "Connected!",
                duration: Duration(seconds: 5));
          });
        }
      }
    });

    if (!targetDeviceFound) {
      hardwareKeyConnected.value = false;
    }
  }

  void sendString(String text) async {
    if (_port == null) return;
    await _port.write(Uint8List.fromList(text.codeUnits));
  }

  Future<bool> validation() async {
    if (textController.text == _keyFromDevice) {
      sendString("done");
      return true;
    } else {
      return false;
    }
  }

  @override
  void onInit() {
    super.onInit();
    textController = TextEditingController();
    UsbSerial.usbEventStream.listen((UsbEvent event) {
      _getPorts();
    });
    _getPorts();
  }

  @override
  void onClose() {
    textController?.dispose();
    _connectTo(null);
    super.onClose();
  }
}
