import 'dart:convert';
import 'dart:typed_data';
import 'dart:ui' as ui;

import 'package:flutter/material.dart';
import 'package:flutter/rendering.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'package:image/image.dart' as img;
import 'package:qr_flutter/qr_flutter.dart';

class DetailPage extends StatefulWidget {
  final BluetoothDevice server;

  const DetailPage({this.server});

  @override
  _DetailPageState createState() => _DetailPageState();
}

class _DetailPageState extends State<DetailPage> {
  BluetoothConnection connection;
  bool isConnecting = true;

  bool get isConnected => connection != null && connection.isConnected;
  bool isDisconnecting = false;

  String qrMsg = '[Test] https://www.google.com';
  GlobalKey _globalKey = new GlobalKey();

  TextEditingController _textEditingController = new TextEditingController();

  @override
  void initState() {
    super.initState();
    _getBTConnection();
  }

  @override
  void dispose() {
    if (isConnected) {
      isDisconnecting = true;
      connection.dispose();
      connection = null;
    }
    super.dispose();
  }

  _getBTConnection() {
    BluetoothConnection.toAddress(widget.server.address).then((_connection) {
      connection = _connection;
      isConnecting = false;
      isDisconnecting = false;
      setState(() {});
      connection.input.listen(_onDataReceived).onDone(() {
        if (isDisconnecting) {
          print('Disconnecting locally');
        } else {
          print('Disconnecting remotely');
        }
        if (this.mounted) {
          setState(() {});
        }
        Navigator.of(context).pop();
      });
    }).catchError((error) {
      Navigator.of(context).pop();
    });
  }

  void _onDataReceived(Uint8List data) {}

  void _sendByte(Uint8List byteData) async {
    try {
      print(" **** SENDING **** ");
      connection.output.add(utf8.encode("0"));
      await connection.output.allSent;

      await Future.delayed(Duration(seconds: 1));
      connection.output.add(byteData);
      await connection.output.allSent;

      await Future.delayed(Duration(seconds: 1));
      connection.output.add(utf8.encode("0"));
      await connection.output.allSent;
    } catch (e) {
      setState(() {});
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        backgroundColor: Colors.white,
        appBar: AppBar(
          title: (isConnecting
              ? Text('Connecting to ${widget.server.name} ...')
              : isConnected
                  ? Text('Connected with ${widget.server.name}')
                  : Text('Disconnected with ${widget.server.name}')),
          actions: [
            FlatButton(
              child: Text("Edit"),
              textColor: Colors.white,
              shape: CircleBorder(side: BorderSide(color: Colors.transparent)),
              onPressed: () {
                _editingDialog(context);
              },
            )
          ],
        ),
        body: SafeArea(
          child: isConnected
              ? Column(
                  children: <Widget>[
                    Expanded(
                      child: Center(
                        child: RepaintBoundary(
                          key: _globalKey,
                          child: Container(
                            width: 128, //SSD1351 width and height
                            child: CustomPaint(
                              size: Size.square(128),
                              painter: QrPainter(
                                data: qrMsg,
                                version: QrVersions.auto,
                                color: Colors.white,
                                emptyColor: Colors.black,
                              ),
                            ),
                          ),
                        ),
                      ),
                    ),
                    Padding(
                      padding: const EdgeInsets.symmetric(
                          vertical: 20, horizontal: 40),
                      child: Text(qrMsg),
                    ),
                    sendButton(),
                  ],
                )
              : Center(
                  child: Text(
                    "Connecting...",
                    style: TextStyle(
                        fontSize: 24,
                        fontWeight: FontWeight.bold,
                        color: Colors.white),
                  ),
                ),
        ));
  }

  Widget sendButton() {
    return Container(
      padding: const EdgeInsets.all(16),
      child: RaisedButton(
        shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(18),
            side: BorderSide(color: Colors.red)),
        onPressed: () {
          _captureQR();
        },
        color: Colors.red,
        textColor: Colors.white,
        child: Padding(
          padding: const EdgeInsets.all(8.0),
          child: Text(
            'Send to ESP32',
            style: TextStyle(fontSize: 24),
          ),
        ),
      ),
    );
  }

  Future<void> _captureQR() async {
    RenderRepaintBoundary boundary =
        _globalKey.currentContext.findRenderObject();
    ui.Image image = await boundary.toImage(pixelRatio: 1.0);
    ByteData byteData = await image.toByteData(format: ui.ImageByteFormat.png);
    var pngBytes = byteData.buffer.asUint8List();

    var pngImage = img.decodePng(pngBytes);
    _sendByte(img.encodeJpg(pngImage));
  }

  _editingDialog(BuildContext context) async {
    return showDialog(
        context: context,
        builder: (context) {
          return AlertDialog(
            title: Text('Change message in QR'),
            content: TextField(
              controller: _textEditingController,
              decoration: InputDecoration(hintText: "No longer than 80 chars"),
            ),
            actions: [
              FlatButton(
                child: Text("Done"),
                onPressed: () {
                  Navigator.of(context).pop();
                  qrMsg = _textEditingController.text;
                  setState(() {});
                },
              )
            ],
          );
        });
  }
}
