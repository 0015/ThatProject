import 'package:flutter/material.dart';

class FileEntityListTile extends ListTile {
  FileEntityListTile({
    @required String filePath,
    int fileSize,
    GestureTapCallback onTap,
    GestureLongPressCallback onLongPress,
  }) : super(
            onTap: onTap,
            onLongPress: onLongPress,
            leading: Icon(Icons.insert_drive_file),
            title: Text(filePath),
            subtitle: Text("$fileSize byte"),
            trailing: Row(
              mainAxisSize: MainAxisSize.min,
              children: [Icon(Icons.play_circle_outline)],
            ));
}
