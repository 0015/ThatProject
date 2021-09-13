import 'package:epaper_emoji_ble/const/emoji_data.dart';
import 'package:epaper_emoji_ble/route/routing_constants.dart';
import 'package:flutter/material.dart';

class EmojiScreen extends StatelessWidget {
  const EmojiScreen({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Padding(
        padding: const EdgeInsets.only(top: 40, bottom: 10),
        child: Center(child: emojiGridView(context)),
      ),
    );
  }

  Widget emojiGridView(BuildContext context) {
    return MediaQuery.removePadding(
      context: context,
      removeTop: true,
      child: GridView.builder(
          physics: const BouncingScrollPhysics(),
          padding: const EdgeInsets.all(8),
          gridDelegate: SliverGridDelegateWithFixedCrossAxisCount(
            crossAxisCount: MediaQuery.of(context).size.width >
                    MediaQuery.of(context).size.height
                ? 10
                : 5,
          ),
          itemCount: emojiArray.length,
          itemBuilder: (BuildContext context, int index) {
            return Card(
              color: Colors.amber,
              child: InkWell(
                onTap: () {
                  Navigator.pushNamed(context, EmojiConverterScreenRoute,
                      arguments: emojiArray[index]);
                },
                child: Center(
                  child: Text(
                    emojiArray[index],
                    style: const TextStyle(fontSize: 30),
                  ),
                ),
              ),
            );
          }),
    );
  }
}
