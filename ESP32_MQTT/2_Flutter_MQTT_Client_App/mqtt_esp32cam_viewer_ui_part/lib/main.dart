import 'package:flutter/material.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'MQTT ESP32CAM VIEWER',
      themeMode: ThemeMode.dark,
      darkTheme: ThemeData.dark(),
      home: MQTTClient(),
    );
  }
}

class MQTTClient extends StatefulWidget {
  const MQTTClient({Key? key}) : super(key: key);

  @override
  _MQTTClientState createState() => _MQTTClientState();
}

class _MQTTClientState extends State<MQTTClient> {
  String statusText = "Status Text";
  bool isConnected = false;
  TextEditingController idTextController = TextEditingController();

  @override
  void dispose() {
    idTextController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SafeArea(
        child: Column(
          children: [header(), body(), footer()],
        ),
      ),
    );
  }

  Widget header() {
    return Expanded(
      child: Container(
        child: Text(
          'ESP32CAM Viewer\n- AWS IoT -',
          style: TextStyle(
              fontSize: 28, color: Colors.white, fontWeight: FontWeight.bold),
          textAlign: TextAlign.center,
        ),
      ),
      flex: 3,
    );
  }

  Widget body() {
    return Expanded(
      child: Container(
        color: Colors.black26,
        child: Row(
          children: [
            Expanded(
              child: Container(
                color: Colors.black,
                child: Column(
                  children: [
                    Padding(
                      padding: const EdgeInsets.symmetric(
                          horizontal: 16, vertical: 8),
                      child: TextFormField(
                          enabled: !isConnected,
                          controller: idTextController,
                          decoration: InputDecoration(
                              border: UnderlineInputBorder(),
                              labelText: 'MQTT Client Id',
                              labelStyle: TextStyle(fontSize: 10),
                              suffixIcon: IconButton(
                                icon: Icon(Icons.subdirectory_arrow_left),
                                onPressed: () {},
                              ))),
                    ),
                    isConnected
                        ? TextButton(
                            onPressed: () {}, child: Text('Disconnect'))
                        : Container()
                  ],
                ),
              ),
              flex: 3,
            ),
            Expanded(
              child: Container(color: Colors.white),
              flex: 7,
            )
          ],
        ),
      ),
      flex: 20,
    );
  }

  Widget footer() {
    return Expanded(
      child: Container(
        alignment: Alignment.centerRight,
        padding: const EdgeInsets.only(right: 16),
        child: Text(
          statusText,
          style: TextStyle(
              fontWeight: FontWeight.normal, color: Colors.amberAccent),
        ),
      ),
      flex: 1,
    );
  }
}
