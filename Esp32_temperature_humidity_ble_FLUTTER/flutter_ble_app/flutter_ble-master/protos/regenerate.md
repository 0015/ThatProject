// Copyright 2017, Paul DeMarco.\
// All rights reserved. Use of this source code is governed by a\
// BSD-style license that can be found in the LICENSE file.

# Generate protobuf files in Dart
1. If upgrading, delete all proto files from /home/.pub-cache/bin
1. Clone the latest dart-protoc-plugin from https://github.com/dart-lang/dart-protoc-plugin
1. Run `pub install` inside dart-protoc-plugin
1. Run `pub global activate protoc_plugin` to get .dart files into /home/.pub-cache/bin/
1. Get the latest linux protoc compiler from https://github.com/google/protobuf/releases/ (protoc-X.X.X-linux-x86_64.zip)
1. Copy /bin/protoc into /home/.pub-cache/bin/
1. Run the following commands from the protos folder
```protoc --dart_out=../lib/gen ./flutterblue.proto```
```protoc --objc_out=../ios/gen ./flutterblue.proto```