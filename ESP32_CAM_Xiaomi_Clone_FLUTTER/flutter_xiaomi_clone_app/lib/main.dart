import 'dart:async';
import 'dart:ui';

import 'package:flutter/material.dart';
import 'package:flutter/rendering.dart';
import 'package:flutter_speed_dial/flutter_speed_dial.dart';
import 'package:fluttertoast/fluttertoast.dart';
import 'package:gesture_zoom_box/gesture_zoom_box.dart';
import 'package:intl/intl.dart';
import 'package:save_in_gallery/save_in_gallery.dart';
import 'package:web_socket_channel/io.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      theme: ThemeData.dark(),
      title: "Xiaomi Cam App Clone",
      home: Home(
        channel: IOWebSocketChannel.connect('ws://35.235.87.20:65080'),
      ),
    );
  }
}

class Home extends StatefulWidget {
  final WebSocketChannel channel;

  Home({Key key, @required this.channel}) : super(key: key);

  @override
  _HomeState createState() => _HomeState();
}

class _HomeState extends State<Home> {
  final double videoWidth = 640;
  final double videoHeight = 480;

  double newVideoSizeWidth = 640;
  double newVideoSizeHeight = 480;

  bool isLandscape;
  String _timeString;

  var _globalKey = new GlobalKey();
  final _imageSaver = ImageSaver();

  @override
  void initState() {
    super.initState();
    isLandscape = false;

    _timeString = _formatDateTime(DateTime.now());
    Timer.periodic(Duration(seconds: 1), (Timer t) => _getTime());
  }

  @override
  void dispose() {
    widget.channel.sink.close();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: OrientationBuilder(builder: (context, orientation) {
        var screenWidth = MediaQuery.of(context).size.width;
        var screenHeight = MediaQuery.of(context).size.height;

        if (orientation == Orientation.portrait) {
          //screenWidth < screenHeight

          isLandscape = false;
          newVideoSizeWidth =
              screenWidth > videoWidth ? videoWidth : screenWidth;
          newVideoSizeHeight = videoHeight * newVideoSizeWidth / videoWidth;
        } else {
          isLandscape = true;
          newVideoSizeHeight =
              screenHeight > videoHeight ? videoHeight : screenHeight;
          newVideoSizeWidth = videoWidth * newVideoSizeHeight / videoHeight;
        }

        return Container(
          color: Colors.black,
          child: StreamBuilder(
            stream: widget.channel.stream,
            builder: (context, snapshot) {
              if (!snapshot.hasData) {
                return Center(
                  child: CircularProgressIndicator(
                    valueColor: AlwaysStoppedAnimation<Color>(Colors.white),
                  ),
                );
              } else {
                return Row(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: <Widget>[
                    Column(
                      children: <Widget>[
                        SizedBox(
                          height: isLandscape ? 0 : 30,
                        ),
                        Stack(
                          children: <Widget>[
                            RepaintBoundary(
                              key: _globalKey,
                              child: GestureZoomBox(
                                maxScale: 5.0,
                                doubleTapScale: 2.0,
                                duration: Duration(milliseconds: 200),
                                child: Image.memory(
                                  snapshot.data,
                                  gaplessPlayback: true,
                                  width: newVideoSizeWidth,
                                  height: newVideoSizeHeight,
                                ),
                              ),
                            ),
                            Positioned.fill(
                                child: Align(
                              child: Column(
                                children: <Widget>[
                                  SizedBox(
                                    height: 16,
                                  ),
                                  Text(
                                    'ESP32\'s cam',
                                    style: TextStyle(
                                        fontSize: 14,
                                        fontWeight: FontWeight.w300),
                                  ),
                                  SizedBox(
                                    height: 4,
                                  ),
                                  Text(
                                    'Live | $_timeString',
                                    style: TextStyle(
                                        fontSize: 12,
                                        fontWeight: FontWeight.w300),
                                  ),
                                ],
                              ),
                              alignment: Alignment.topCenter,
                            ))
                          ],
                        ),
                        Expanded(
                          flex: 1,
                          child: Container(
                            color: Colors.black,
                            width: MediaQuery.of(context).size.width,
                            child: Padding(
                              padding: const EdgeInsets.symmetric(vertical: 16),
                              child: Row(
                                crossAxisAlignment: CrossAxisAlignment.start,
                                mainAxisAlignment:
                                    MainAxisAlignment.spaceEvenly,
                                children: <Widget>[
                                  IconButton(
                                      icon: Icon(
                                    Icons.videocam,
                                    size: 24,
                                  )),
                                  IconButton(
                                    icon: Icon(
                                      Icons.photo_camera,
                                      size: 24,
                                    ),
                                    onPressed: takeScreenShot,
                                  ),
                                  IconButton(
                                      icon: Icon(
                                    Icons.mic,
                                    size: 24,
                                  )),
                                  IconButton(
                                      icon: Icon(
                                    Icons.speaker,
                                    size: 24,
                                  )),
                                  IconButton(
                                      icon: Icon(
                                    Icons.add_alert,
                                    size: 24,
                                  ))
                                ],
                              ),
                            ),
                          ),
                        )
                      ],
                    ),
                  ],
                );
              }
            },
          ),
        );
      }),
      floatingActionButton: _getFab(),
    );
  }

  takeScreenShot() async {
    RenderRepaintBoundary boundary =
        _globalKey.currentContext.findRenderObject();
    var image = await boundary.toImage();
    var byteData = await image.toByteData(format: ImageByteFormat.png);
    var pngBytes = byteData.buffer.asUint8List();
    final res = await _imageSaver.saveImage(imageBytes: pngBytes);

    Fluttertoast.showToast(
        msg: res ? "ScreenShot Saved" : "ScreenShot Failure!",
        toastLength: Toast.LENGTH_SHORT,
        gravity: ToastGravity.CENTER,
        timeInSecForIos: 1,
        backgroundColor: Colors.red,
        textColor: Colors.white,
        fontSize: 16.0);
  }

  String _formatDateTime(DateTime dateTime) {
    return DateFormat('MM/dd hh:mm:ss aaa').format(dateTime);
  }

  void _getTime() {
    final DateTime now = DateTime.now();
    setState(() {
      _timeString = _formatDateTime(now);
    });
  }

  Widget _getFab() {
    return SpeedDial(
      animatedIcon: AnimatedIcons.menu_close,
      animatedIconTheme: IconThemeData(size: 22),
      visible: isLandscape,
      curve: Curves.bounceIn,
      children: [
        SpeedDialChild(
          child: Icon(Icons.photo_camera),
          onTap: takeScreenShot,
        ),
        SpeedDialChild(child: Icon(Icons.videocam), onTap: () {})
      ],
    );
  }
}
