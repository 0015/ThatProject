/// @name LineChart
/// @version 0.0.5
/// @description Simple line chart widget
/// @author Patryk "PsychoX" Ludwikowski <patryk.ludwikowski.7+dart@gmail.com>
/// @license MIT License (see https://mit-license.org/)

import 'package:flutter/material.dart';
import 'dart:ui' as ui;
import 'dart:math' as math show min, max;

import './PaintStyle.dart';

class LabelEntry {
  final double value;
  final String label;

  LabelEntry(this.value, this.label);
}

/// Widget that allows to show data on line chart.
///
/// All arguments, values and labels data should be sorted!
/// Since both the arguments and the values must be `double` type,
/// be aware of the precision.
class LineChart extends StatelessWidget {
  /// Constraints for the line chart.
  final BoxConstraints constraints;

  // @TODO ? Both `_LineChartPainter` and `LineChart` have most the same fields.
  //  `LineChart` is just mainly passing them to the painter. Shouldn't there be
  //  only one class containing these data? Some `LineChartData` forged inside here
  //  and then passed and used by the painter? :thinking:

  /// Padding around main drawng area. Necessary for displaying labels (around the chart).
  final EdgeInsets padding;

  /* Arguments */
  /// Collection of doubles as arguments.
  final Iterable<double> arguments;

  /// Mappings of strings for doubles arguments, which allow to specify custom
  /// strings as labels for certain arguments.
  final Iterable<LabelEntry> argumentsLabels;

  /* Values */
  /// Collection of data series as collections of next values on corresponding arguments.
  final Iterable<Iterable<double>> values;

  /// Mappings of string for doubles values, which allow to specify custom
  /// string as labels for certain values.
  final Iterable<LabelEntry> valuesLabels;

  /* Labels & lines styles */
  /// Style of horizontal lines labels
  final TextStyle horizontalLabelsTextStyle;

  /// Style of vertical lines labels
  final TextStyle verticalLabelsTextStyle;

  /// Defines style of horizontal lines. Might be null in order to prevent lines from drawing.
  final Paint horizontalLinesPaint;

  /// Defines style of vertical lines. Might be null in order to prevent lines from drawing.
  final Paint verticalLinesPaint;

  // @TODO . expose it
  final bool snapToLeftLabel = false;
  final bool snapToTopLabel = true;
  final bool snapToRightLabel = false;
  final bool snapToBottomLabel = true;

  /* Series points & lines styles */
  /// List of paint styles for series values points.
  ///
  /// On whole list null would use predefined set of styles.
  /// On list entry null there will be no points for certain series.
  final List<Paint> seriesPointsPaints;

  /// List of paint styles for lines between next series points.
  ///
  /// On null there will be no lines.
  final List<Paint> seriesLinesPaints;

  final double additionalMinimalHorizontalLabelsInterval;
  final double additionalMinimalVerticalLablesInterval;

  LineChart({
    @required this.constraints,
    this.padding = const EdgeInsets.fromLTRB(32, 12, 20, 28),
    this.arguments,
    this.argumentsLabels,
    this.values,
    this.valuesLabels,
    this.horizontalLabelsTextStyle,
    this.verticalLabelsTextStyle,
    PaintStyle horizontalLinesStyle = const PaintStyle(color: Colors.grey),
    PaintStyle verticalLinesStyle, // null for default

    this.additionalMinimalHorizontalLabelsInterval = 8,
    this.additionalMinimalVerticalLablesInterval = 8,
    Iterable<PaintStyle>
        seriesPointsStyles, // null would use predefined set of styles
    Iterable<PaintStyle> seriesLinesStyles, // null for default
  })  : horizontalLinesPaint = horizontalLinesStyle?.toPaint(),
        verticalLinesPaint = verticalLinesStyle?.toPaint(),
        seriesPointsPaints = _prepareSeriesPointsPaints(seriesPointsStyles),
        seriesLinesPaints = _prepareSeriesLinesPaints(seriesLinesStyles) {
    if (seriesPointsStyles.length < values.length &&
        12 /* default paints */ < values.length) {
      throw "Too few `seriesPointsPaintStyle`s! Try define more or limit number of displayed series";
    }
    if (seriesLinesStyles.length < values.length) {
      throw "Too few `seriesLinesStyles`s! Try define more or limit number of displayed series";
    }
  }

  static Iterable<Paint> _prepareSeriesPointsPaints(
      Iterable<PaintStyle> seriesPointsStyles) {
    if (seriesPointsStyles == null) {
      // Default paint for points
      return List<Paint>.unmodifiable(<Paint>[
        PaintStyle(strokeWidth: 1.7, color: Colors.blue).toPaint(),
        PaintStyle(strokeWidth: 1.7, color: Colors.red).toPaint(),
        PaintStyle(strokeWidth: 1.7, color: Colors.yellow).toPaint(),
        PaintStyle(strokeWidth: 1.7, color: Colors.green).toPaint(),

        PaintStyle(strokeWidth: 1.7, color: Colors.purple).toPaint(),
        PaintStyle(strokeWidth: 1.7, color: Colors.deepOrange).toPaint(),
        PaintStyle(strokeWidth: 1.7, color: Colors.brown).toPaint(),
        PaintStyle(strokeWidth: 1.7, color: Colors.lime).toPaint(),

        PaintStyle(strokeWidth: 1.7, color: Colors.indigo).toPaint(),
        PaintStyle(strokeWidth: 1.7, color: Colors.pink).toPaint(),
        PaintStyle(strokeWidth: 1.7, color: Colors.amber).toPaint(),
        PaintStyle(strokeWidth: 1.7, color: Colors.teal).toPaint(),

        // For more, user should specify them :F
      ]);
    } else {
      return seriesPointsStyles.map((style) => style?.toPaint()).toList();
    }
  }

  static Iterable<Paint> _prepareSeriesLinesPaints(
      Iterable<PaintStyle> seriesLinesStyles) {
    if (seriesLinesStyles == null) {
      return null;
    } else {
      return seriesLinesStyles.map((style) => style.toPaint()).toList();
    }
  }

  @override
  Widget build(BuildContext context) {
    return ConstrainedBox(
        constraints: this.constraints,
        child: CustomPaint(
            painter: _LineChartPainter(
          padding: padding,
          arguments: arguments,
          argumentsLabels: argumentsLabels,
          values: values,
          valuesLabels: valuesLabels,
          horizontalLabelsTextStyle:
              horizontalLabelsTextStyle ?? Theme.of(context).textTheme.caption,
          verticalLabelsTextStyle:
              verticalLabelsTextStyle ?? Theme.of(context).textTheme.caption,
          horizontalLinesPaint: horizontalLinesPaint,
          verticalLinesPaint: verticalLinesPaint,
          additionalMinimalHorizontalLabelsInterval:
              additionalMinimalHorizontalLabelsInterval,
          additionalMinimalVerticalLablesInterval:
              additionalMinimalVerticalLablesInterval,
          seriesPointsPaints: seriesPointsPaints,
          seriesLinesPaints: seriesLinesPaints,
        )));
  }
}

class _LineChartPainter extends CustomPainter {
  /// Padding around main drawng area. Necessary for displaying labels (around the chart).
  final EdgeInsets padding;

  /* Arguments */
  /// Collection of doubles as arguments.
  final Iterable<double> arguments;

  /// Mappings of strings for doubles arguments, which allow to specify custom
  /// strings as labels for certain arguments.
  final Iterable<LabelEntry> argumentsLabels;

  /* Values */
  /// Collection of data series as collections of next values on corresponding arguments.
  final Iterable<Iterable<double>> values;

  /// Mappings of string for doubles values, which allow to specify custom
  /// string as labels for certain values.
  final Iterable<LabelEntry> valuesLabels;

  /* Labels & lines styles */
  /// Style of horizontal lines labels
  final TextStyle horizontalLabelsTextStyle;

  /// Style of vertical lines labels
  final TextStyle verticalLabelsTextStyle;

  /// Defines style of horizontal lines. Might be null in order to prevent lines from drawing.
  final Paint horizontalLinesPaint;

  /// Defines style of vertical lines. Might be null in order to prevent lines from drawing.
  final Paint verticalLinesPaint;

  // @TODO . expose it
  final bool snapToLeftLabel = false;
  final bool snapToTopLabel = true;
  final bool snapToRightLabel = false;
  final bool snapToBottomLabel = true;

  /* Series points & lines styles */
  /// Collection of paint styles for series values points.
  ///
  /// On whole argument null would use predefined set of styles.
  /// On collection entry null there will be no points for certain series.
  final Iterable<Paint> seriesPointsPaints;

  /// Collection of paint styles for lines between next series points.
  ///
  /// On null there will be no lines.
  final Iterable<Paint> seriesLinesPaints;

  /* Runtime */
  /// Minimal allowed interval between horizontal lines. Calculated from font size.
  final double minimalHorizontalLabelsInterval;

  /// Maximal value of all data series values
  double maxValue = -double.maxFinite;

  /// Minimal value of all data series values
  double minValue = double.maxFinite;

  double _minimalHorizontalRatio = 0;
  double _minimalVerticalRatio = 0;

  /// Creates `_LineChartPainter` (`CustomPainter`) with given data and styling.
  _LineChartPainter({
    this.padding = const EdgeInsets.fromLTRB(40, 8, 8, 32),
    this.arguments,
    this.argumentsLabels,
    this.values,
    this.valuesLabels,
    @required this.horizontalLabelsTextStyle,
    @required this.verticalLabelsTextStyle,
    @required this.horizontalLinesPaint,
    @required this.verticalLinesPaint,
    double additionalMinimalHorizontalLabelsInterval = 8,
    double additionalMinimalVerticalLablesInterval = 8,
    @required this.seriesPointsPaints,
    @required this.seriesLinesPaints,
  }) : this.minimalHorizontalLabelsInterval =
            horizontalLabelsTextStyle.fontSize +
                additionalMinimalHorizontalLabelsInterval {
    // Find max & min values of data to be show
    for (Iterable<double> series in values) {
      for (double value in series) {
        if (value > maxValue) {
          maxValue = value;
        } else if (value < minValue) {
          minValue = value;
        }
      }
    }

    if (valuesLabels != null) {
      // Find minimal vertical ratio to fit all provided values labels
      Iterator<LabelEntry> entry = valuesLabels.iterator;
      entry.moveNext();
      double lastValue = entry.current.value;

      while (entry.moveNext()) {
        final double goodRatio =
            minimalHorizontalLabelsInterval / (entry.current.value - lastValue);
        if (goodRatio > _minimalVerticalRatio) {
          _minimalVerticalRatio = goodRatio;
        }

        lastValue = entry.current.value;
      }
    }

    if (argumentsLabels != null) {
      // Find minimal horizontal ratio to fit all provided arguments labels
      Iterator<LabelEntry> entry = argumentsLabels.iterator;
      entry.moveNext();
      double lastValue = entry.current.value;
      double lastWidth =
          _getLabelTextPainter(entry.current.label, verticalLabelsTextStyle)
              .width;

      while (entry.moveNext()) {
        final double nextValue = entry.current.value;
        final double nextWidth =
            _getLabelTextPainter(entry.current.label, verticalLabelsTextStyle)
                .width;

        final double goodRatio = ((lastWidth + nextWidth) / 2 +
                additionalMinimalVerticalLablesInterval) /
            (nextValue - lastValue);
        if (goodRatio > _minimalHorizontalRatio) {
          _minimalHorizontalRatio = goodRatio;
        }

        lastValue = nextValue;
        lastWidth = nextWidth;
      }
    }
  }

  @override
  void paint(Canvas canvas, Size size) {
    final double width = size.width - padding.left - padding.right;
    final double height = size.height - padding.top - padding.bottom;

    /* Horizontal lines with labels */
    double valuesOffset = 0; // @TODO ? could be used in future for scrolling
    double verticalRatio;

    {
      Iterable<LabelEntry> labels;

      // If no labels provided - generate them!
      if (valuesLabels == null) {
        final double optimalStepValue =
            _calculateOptimalStepValue(maxValue - minValue, height);
        int stepsNumber = 1;

        // Find bottom line value
        double bottomValue = 0;
        if (minValue > 0) {
          while (bottomValue < minValue) {
            bottomValue += optimalStepValue;
          }
          bottomValue -= optimalStepValue;
        } else {
          while (bottomValue > minValue) {
            bottomValue -= optimalStepValue;
          }
        }
        valuesOffset = bottomValue;

        // Find top line value
        double topValue = bottomValue;
        while (topValue < maxValue) {
          topValue += optimalStepValue;
          stepsNumber += 1;
        }

        // Set labels iterable from prepared generator
        Iterable<LabelEntry> generator(double optimalStepValue, int stepsNumber,
            [double value = 0.0]) sync* {
          //double value = _bottomValue;
          for (int i = 0; i < stepsNumber; i++) {
            yield LabelEntry(
                value,
                value
                    .toString()); // @TODO , choose better precision based on optimal step value while parsing to string
            value += optimalStepValue;
          }
        }

        labels = generator(optimalStepValue, stepsNumber, bottomValue);

        if (!snapToTopLabel) {
          topValue = maxValue;
        }
        if (!snapToBottomLabel) {
          bottomValue = valuesOffset = minValue;
        }

        // Calculate vertical ratio of pixels per value
        // Note: There is no empty space already
        verticalRatio = height / (topValue - bottomValue);
      }
      // If labels provided - use them
      else {
        // Set labels iterable as the provided list
        labels = valuesLabels;

        // Use minimal visible value as offset.
        // Note: `minValue` is calculated in constructor and includes miniaml labels values.
        valuesOffset = minValue;

        // Calculate vertical ratio of pixels per value
        // Note: `_minimalVerticalRatio` is calculated in constructor
        final double topValue = snapToTopLabel
            ? math.max(maxValue, valuesLabels.last.value)
            : maxValue;
        final double bottomValue = snapToBottomLabel
            ? math.min(minValue, valuesLabels.first.value)
            : minValue;
        final double noEmptySpaceRatio = height / (topValue - bottomValue);
        verticalRatio = math.max(_minimalVerticalRatio, noEmptySpaceRatio);
      }

      // Draw the horizontal lines and labels
      for (LabelEntry tuple in labels) {
        if (tuple.value < valuesOffset) continue;
        final double yOffset = (size.height -
            padding.bottom -
            (tuple.value - valuesOffset) * verticalRatio);
        if (yOffset < padding.top) break;

        // Draw line
        if (horizontalLinesPaint != null) {
          canvas.drawLine(
              Offset(padding.left, yOffset),
              Offset(size.width - padding.right, yOffset),
              horizontalLinesPaint);
        }

        // Draw label
        TextPainter(
            text: TextSpan(text: tuple.label, style: horizontalLabelsTextStyle),
            textAlign: TextAlign.right,
            textDirection: TextDirection.ltr)
          ..layout(minWidth: padding.left - 4)
          ..paint(canvas,
              Offset(0, yOffset - horizontalLabelsTextStyle.fontSize / 2 - 1));
      }
    }

    /* Vertical lines with labels */
    double argumentsOffset = 0;
    final double xOffsetLimit = size.width - padding.right;
    double horizontalRatio;

    {
      Iterable<LabelEntry> labels;

      // If no labels provided - generate them!
      if (argumentsLabels == null) {
        throw "not implemented";
        // @TODO . after few hot days of thinking about the problem for 1-2 hour a day, I just gave up.
        // The hardest in the problem is that there must be trade-off between space for labels and max lines,
        // but keep in mind that the label values should be in some human-readable steps (0.5, 10, 0.02...).
      }
      // If labels provided - use them
      else {
        // Set labels iterable as the provided list
        labels = argumentsLabels;

        // Use first visible argument as arguments offset
        argumentsOffset = argumentsLabels.first.value;

        if (!snapToLeftLabel) {
          argumentsOffset = arguments.first;
        }

        // Calculate vertical ratio of pixels per value
        // Note: `_minimalHorizontalRatio` is calculated in constructor
        final double leftMost = snapToLeftLabel
            ? math.min(arguments.first, argumentsLabels.first.value)
            : arguments.first;
        final double rightMost = snapToRightLabel
            ? math.max(arguments.last, argumentsLabels.last.value)
            : arguments.last;
        final double noEmptySpaceRatio = width / (rightMost - leftMost);
        horizontalRatio = math.max(_minimalHorizontalRatio, noEmptySpaceRatio);
      }

      // Draw the vertical lines and labels
      for (LabelEntry tuple in labels) {
        if (tuple.value < argumentsOffset) continue;
        final double xOffset =
            padding.left + (tuple.value - argumentsOffset) * horizontalRatio;
        if (xOffset > xOffsetLimit) break;

        // Draw line
        if (verticalLinesPaint != null) {
          canvas.drawLine(
              Offset(xOffset, padding.top),
              Offset(xOffset, size.height - padding.bottom),
              verticalLinesPaint);
        }

        // Draw label
        final TextPainter textPainter = TextPainter(
            text: TextSpan(text: tuple.label, style: verticalLabelsTextStyle),
            textDirection: TextDirection.ltr)
          ..layout();
        textPainter.paint(
            canvas,
            Offset(xOffset - textPainter.width / 2,
                size.height - verticalLabelsTextStyle.fontSize - 8));
      }
    }

    /* Points and lines between subsequent */
    Iterator<Iterable<double>> series = values.iterator;
    Iterator<Paint> linesPaints = seriesLinesPaints == null
        ? <Paint>[].iterator
        : seriesLinesPaints.iterator;
    Iterator<Paint> pointsPaints = seriesPointsPaints.iterator;
    while (series.moveNext()) {
      List<Offset> points = [];
      Iterator<double> value = series.current.iterator;
      Iterator<double> argument = arguments.iterator;
      while (value.moveNext()) {
        argument.moveNext();
        if (value.current == null || value.current == double.nan) continue;

        if (argument.current < argumentsOffset) continue;
        final double xOffset = padding.left +
            (argument.current - argumentsOffset) * horizontalRatio;
        if (xOffset > xOffsetLimit) break;

        if (value.current < valuesOffset) continue;
        final double yOffset = size.height -
            padding.bottom -
            (value.current - valuesOffset) * verticalRatio;
        if (yOffset < padding.top) continue;

        points.add(Offset(xOffset, yOffset));
      }

      // Lines
      if (linesPaints.moveNext() && linesPaints.current != null) {
        canvas.drawPath(Path()..addPolygon(points, false), linesPaints.current);
      }

      // Points
      if (pointsPaints.moveNext() && pointsPaints.current != null) {
        canvas.drawPoints(ui.PointMode.points, points, pointsPaints.current);
      }
    }
  }

  @override
  bool shouldRepaint(_LineChartPainter old) =>
      (this.arguments != old.arguments ||
          this.values != old.values ||
          this.argumentsLabels != old.argumentsLabels ||
          this.valuesLabels != old.valuesLabels ||
          this.seriesPointsPaints != old.seriesPointsPaints ||
          this.seriesLinesPaints != old.seriesLinesPaints ||
          this.horizontalLabelsTextStyle != old.horizontalLabelsTextStyle ||
          this.verticalLabelsTextStyle != old.verticalLabelsTextStyle ||
          this.padding != old.padding //
      );

  // ..., 0.01, 0.02, 0.05, 0.1, [0.125], 0.2, [0.25], 0.5, 1, 2, 5, 10, 20, 50, 100, 200, 500, ...
  double _calculateOptimalStepValue(double valueRange, double height) {
    final int maxSteps = height ~/ minimalHorizontalLabelsInterval;
    if (maxSteps <= 0) {
      throw "invalid max lines!";
    }
    double interval = valueRange / maxSteps;
    if (interval > 1) {
      int zeros = 0;
      while (interval >= 10) {
        interval = interval / 10;
        zeros += 1;
      }
      /**/ if (interval <= 1) {
        interval = 1;
      } else if (interval <= 2) {
        interval = 2;
      } else if (interval <= 5) {
        interval = 5;
      }
      for (; zeros-- != 0;) {
        interval *= 10;
      }
    } else {
      // @TODO ! not working at all for lower :C
      int zeros = 0;
      while (interval < 0) {
        interval = interval * 10;
        zeros += 1;
      }
      /**/ if (interval <= 1) {
        interval = 1;
      } else if (interval <= 2) {
        interval = 2;
      } else if (interval <= 5) {
        interval = 5;
      }
      for (; zeros-- != 0;) {
        interval /= 10;
      }
    }
    return interval;
  }

  TextPainter _getLabelTextPainter(String text, TextStyle style) {
    return TextPainter(
        text: TextSpan(text: text, style: style),
        textDirection: TextDirection.ltr)
      ..layout();
  }
}
