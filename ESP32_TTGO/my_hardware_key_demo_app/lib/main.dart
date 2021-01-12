import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:my_hardware_key_demo_app/controller/serial_controller.dart';

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
        title: Text("My Hardware Key Demo App"),
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
                    'Hardware Key Connected: ${_.hardwareKeyConnected.value.toString().toUpperCase()}',
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
                            Padding(
                              padding:
                                  const EdgeInsets.symmetric(horizontal: 32),
                              child: TextFormField(
                                controller: _.textController,
                                keyboardType: TextInputType.text,
                                validator: (value) => value.trim().isEmpty
                                    ? 'Key required'
                                    : null,
                              ),
                            ),
                            RaisedButton(
                              child: Text('Confirm'),
                              onPressed: () async {
                                var result = await _.validation();
                                if (result) {
                                  Get.to(Second());
                                }
                              },
                            )
                          ],
                        )
                      : Text(
                          'Please insert your key to proceed!',
                          style: TextStyle(
                              fontWeight: FontWeight.bold, fontSize: 20),
                        )
                ],
              ),
            )
          ],
        ),
      ),
    );
  }
}

class Second extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Second Page'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text(
              'Second Page',
              style: TextStyle(fontWeight: FontWeight.bold, fontSize: 20),
            ),
            Obx(() {
              final key =
                  Get.find<SerialController>().hardwareKeyConnected.value;
              if (!key) {
                Get.back();
              }
              return Text('Key : $key');
            })
          ],
        ),
      ),
    );
  }
}
