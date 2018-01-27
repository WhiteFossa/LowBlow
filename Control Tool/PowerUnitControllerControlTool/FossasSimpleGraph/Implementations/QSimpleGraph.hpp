/*
Copyright 2017 White Fossa aka Artyom Vetrov.

This file is part of project "LowBlow" (advanced programmable cooler controller).

All parts of "LowBlow" is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

"LowBlow" project files is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with project "LowBlow" files. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QSIMPLEGRAPH_HPP
#define QSIMPLEGRAPH_HPP

#include <cmath>
#include <QPaintEvent>
#include <QSize>
#include <QSizePolicy>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QRect>
#include <FossasSimpleGraph/Interfaces/IQSimpleGraph.hpp>
#include <QtDebug>

/**
	* Define it to draw point-to-point graph instead of ladder one.
	*/
//#define FOSSA_SIMPLE_GRAPH_POINT_TO_POINT_LINE


namespace Fossa
{
	namespace QSimpleGraph
	{
		/**
		 * @brief Possible tooltip directions relative to target point.
		 */
		enum TooltipDirection
		{
			UpLeft,
			UpRight,
			DownLeft,
			DownRight
		};

		/**
		 * @brief QSimpleGraph widget implementation
		 */
		class QSimpleGraph : public Interfaces::IQSimpleGraph
		{
			Q_OBJECT

			// Public members
			public:

	/**
	 * @brief QSimpleGraph widget constructor.
	 * @param parent Parent widget.
	 */
	QSimpleGraph(QWidget *parent = 0);

	/**
	 * @brief Returning hint about widget size, actually - preferred size.
	 * @return QSize with hint about widget size.
	 */
	QSize sizeHint() const;

	/**
	 * @brief Returns minimal possible widget size.
	 * @return Minimal possible widget size.
	 */
	QSize minimumSizeHint() const;

	void SetMinXValue(double minValue);
	void SetMaxXValue(double maxValue);
	void SetMinYValue(double minValue);
	void SetMaxYValue(double maxValue);

	void SetXAxisTitle(QString title);
	void SetYAxisTitle(QString title);

	void ClearAllPoints();
	void AddPoint(double XVal, double YVal);

			// Protected members
			protected:

	/**
	 * @brief Painting method, paints the entrie graph.
	 * @param event Pointer to paint event.
	 */
	void paintEvent(QPaintEvent *event);

	/**
	 * @brief Called when mouse is being moved.
	 * @param event Mouse move parameters.
	 */
	void mouseMoveEvent(QMouseEvent *event);

			// Private members
			private:

	/**
	 * @brief Spacing between widget frame and left side of drawing area.
	 */
	const uint _outerLeftSpacing = 50;

	/**
	 * @brief Spacing between widget frame and right side of drawing area.
	 */
	const uint _outerRightSpacing = 10;

	/**
	 * @brief Spacing between widget frame and top side of drawing area.
	 */
	const uint _outerTopSpacing = 10;

	/**
	 * @brief Spacing between widget frame and bottom side of drawing area.
	 */
	const uint _outerBottomSpacing = 20;

	/**
	 * @brief Vertical cell height
	 */
	const uint _vCellSize = 50;

	/**
	 * @brief Horizontal cell width
	 */
	const uint _hCellSize = 100;

	/**
	 * @brief Tick length in pixels. It will be better if this length will be odd.
	 */
	const uint _tickSize = 11;

	/**
	 * @brief Shift of tick (pixels).
	 */
	const uint _tickShift = _tickSize / 2;

	/**
	 * @brief Shift X and Y axis labels down to this amount of pixels.
	 */
	const uint _labelsVShift = 4;

	/**
	 * @brief Y-axis labels will be shifted left to this amount of pixels.
	 */
	const uint _yAxisTextHShift = _tickShift + 3;

	/**
	 * @brief Y-axis labels will be shifted down to this amount of pixels.
	 */
	const uint _yAxisTextVShift = _labelsVShift - 3;

	/**
	 * @brief X-axis labels will be shifted down to this amount of pixels.
	 */
	const uint _xAxisTextVShift = _tickShift + _labelsVShift;

	/**
	 * @brief Maximal number of digits after point for FitDoubleByWidth() function.
	 */
	const uint _MaxDoublePrecision = 64;

	/**
	 * @brief Shift X-axis title this amount of pixels left from the axis line.
	 */
	const uint _xAxisTitleHShift = 5;

	/**
	 * @brief Shift X-axis title this amount of pixels up from the axis line.
	 */
	const uint _xAxisTitleVShift = 5;

	/**
	 * @brief Shift Y-axis title this amount of pixels right from the axis line.
	 */
	const uint _yAxisTitileHShift = 5;

	/**
	 * @brief Shift Y-axis title this amount of pixels down from the top graph border.
	 */
	const uint _yAxisTitleVShift = 5;

	/**
	 * @brief Radius of point representing circles.
	 */
	const uint _pointRadius = 4;

	/**
	 * @brief Graph line width.
	 */
	const uint _graphLineWidth = 2;

	/**
	 * @brief Add this value to tooltip text width to get bounding rectangle width.
	 */
	const uint _tooltipWidthAdd = 5;

	/**
	 * @brief Add this value to tooltip text height to get bounding rectangle height.
	 */
	const uint _tooltipHeightAdd = 5;

	/**
	 * @brief How far to shift tooltip from graph point.
	 */
	const uint _tooltipXShift = 10;

	/**
	 * @brief How far to shift tooltip from graph point.
	 */
	const uint _tooltipYShift = 10;

	/**
	 * @brief Outer graph border.
	 */
	QRect _graphBorder;

	/**
	 * @brief Pointer to painter. This pointer is being updated every time when paintEvent() being called.
	 */
	QPainter* _painter;

	/**
	 * @brief _GraphAreaWidth Width of the graph area
	 */
	uint _graphAreaWidth;

	/**
	 * @brief _GraphAreaHeight Height of the graph area
	 */
	uint _graphAreaHeight;

	/**
	 * @brief _minX Minimal X value.
	 */
	double _minX;

	/**
	 * @brief _maxX Maximal X value.
	 */
	double _maxX;

	/**
	 * @brief _minY Minimal Y value.
	 */
	double _minY;

	/**
	 * @brief _maxY Maximal Y value.
	 */
	double _maxY;

	/**
	 * @brief _XScalingFactor Scaling factor for X coordinate, see GetXByVal() and GetValByX().
	 */
	double _xScalingFactor;

	/**
	 * @brief _YScalingFactor As _XScalingFactor, but for Y
	 */
	double _yScalingFactor;

	/**
	 * @brief X-Axis title.
	 */
	QString _xAxisTitle;

	/**
	 * @brief Y-Axis title.
	 */
	QString _yAxisTitle;

	/**
	 * @brief Graph points.
	 */
	QMap<double, double> _points;

	/**
	 * @brief Graph data area relative last X mouse coordinate.
	 */
	int _localMouseX;

	/**
	 * @brief As _localMouseX, but for Y coordinate.
	 */
	int _localMouseY;


	/**
	 * @brief Draws a tick
	 * @param pos - X-position (in pixels) if tick on X axis, Y-position - if on Y. Position is relative from left bottom corner of coordinate grid
	 * @param IsXAxis - true, if tick is on X axis.
	 */
	void DrawTick(uint pos, bool IsXAxis);

	/**
	 * @brief Recalculating _XScalingFactor basing on _minX, _maxX and _GraphAreaWidth.
	 */
	void RecalculateXScalingFactor();

	/**
	 * @brief As RecalculateXScalingFactor(), but for Y axis.
	 */
	void RecalculateYScalingFactor();

	/**
	 * @brief Returns X coordinate for given value.
	 * @param val X value.
	 * @return X coordinate, 0 is the leftmost axis pixel.
	 */
	uint GetXByVal(double val);

	/**
	 * @brief Returns X value for given X-axis coordinate.
	 * @param X X-axis coordinate, 0 is the leftmost axis pixel.
	 * @return X value.
	 */
	double GetValByX(uint X);

	/**
	 * @brief As GetXByVal(), but for Y axis.
	 * @param val Y value.
	 * @return Y coordinate, 0 is the bottommost axis pixel.
	 */
	uint GetYByVal(double val);

	/**
	 * @brief Returns Y value for given Y-axis coordinate.
	 * @param Y Y-axis coordinate, 0 is the bottommost axis pixel.
	 * @return Y value.
	 */
	double GetValByY(uint Y);

	/**
	 * @brief Draws label for value on X-axis.
	 * @param xPos Tick poistion for value.
	 * @param label Label text.
	 */
	void DrawXLabel(uint xPos, QString label);

	/**
	 * @brief Draws label for value on Y-axis.
	 * @param yPos Tick position for value.
	 * @param label Label text.
	 */
	void DrawYLabel(uint yPos, QString label);

	/**
	 * @brief Formats double value (by varying number of digits after point) to display it no wider than MaxWidth.
	 * If double can't be fitted into MaxWidth (e.g. MaxWidth is very small) - returns as short string as possible.
	 * See _MaxDoublePrecision constant for additional information.
	 * @param val Value to fit.
	 * @param MaxWidth Maximal allowed width (in pixels).
	 * @return Text, what fits into MaxWidth.
	 */
	QString FitDoubleByWidth(double val, uint MaxWidth);

	/**
	 * @brief Draws graph points and lines.
	 */
	void DrawGraphContents();

	/**
	 * @brief Returns closest point for given coordinate.
	 * @param x X coordinate withing graph.
	 * @return Iterator for _points with closest point.
	 */
	QMap<double, double>::const_iterator GetClosestPoint(uint x);

	/**
	 * @brief Returns screen X coordinate for given X value.
	 * @param xVal X value.
	 * @return Screen X position.
	 */
	uint GetScreenXByValue(double xVal);

	/**
	 * @brief Returns screen Y coordinate for given Y value.
	 * @param yVal Y value.
	 * @return Screen Y position.
	 */
	uint GetScreenYByValue(double yVal);

	/**
	 * @brief Draws a tooltip with given text.
	 * @param targetPoint Coordinates of the target point.
	 * @param direction Tooltip direction relative target point.
	 * @param text Tooltip text.
	 */
	void DrawTooltip(QPoint targetPoint, TooltipDirection direction, QString text);

	/**
	 * @brief As QFontMetrics::boundingRect(), but can work with multiline text.
	 * @param font Font to use.
	 * @param text Text, for what sizes have to be found.
	 * @return Area, covered by text.
	 */
	static QRect MultilineBoundingRect(QFont font, QString text);
		};
	}
}


#endif // QSIMPLEGRAPH_HPP
