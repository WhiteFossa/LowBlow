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

---

Part of "LowBlow" project.

Feel free to contact: whitefossa@gmail.com

Repository: https://github.com/WhiteFossa/LowBlow

If repository above is not available, try my LiveJournal: fossa-white.livejournal.com
or as last measure try to search for #WhiteFossa hashtag.
*/

#include <FossasSimpleGraph/Implementations/QSimpleGraph.hpp>

namespace Fossa
{
	namespace QSimpleGraph
	{
		QSimpleGraph::QSimpleGraph(QWidget *parent) : IQSimpleGraph(parent)
		{
			_localMouseX = 0;
			_localMouseY = 0;
			_graphBorder.setRect(0, 0, 1, 1); // Initial set-up
			setMouseTracking(true); // Call mouseMoveEvent() even if no buttons pressed
		}

		QSize QSimpleGraph::minimumSizeHint() const
		{
			return minimumSize();
		}

		QSize QSimpleGraph::sizeHint() const
		{
			return size();
		}

		void QSimpleGraph::mouseMoveEvent(QMouseEvent *event)
		{
			// Are we in graph data area?
			if
			(
				(event->x() >= _graphBorder.left()) && (event->x() <= _graphBorder.right())
				&&
				(event->y() >= _graphBorder.top()) && (event->y() <= _graphBorder.bottom())
			)
			{
				_localMouseX = event->x() - _graphBorder.left();
				_localMouseY = event->y() - _graphBorder.top();
			}

			// We need to repaint
			update();
		}

		void QSimpleGraph::resizeEvent(QResizeEvent *event)
		{
			Q_UNUSED(event);

			// Outer rectangle position
			_graphBorder.setTop(_outerTopSpacing);
			_graphBorder.setLeft(_outerLeftSpacing);
			_graphBorder.setBottom(height() - _outerBottomSpacing);
			_graphBorder.setRight(width() - _outerRightSpacing);

			// Graph dimensions
			_graphAreaWidth = _graphBorder.width();
			_graphAreaHeight = _graphBorder.height();

			// Scaling factors.
			RecalculateXScalingFactor();
			RecalculateYScalingFactor();
		}

		void QSimpleGraph::paintEvent(QPaintEvent *event)
		{
			Q_UNUSED(event);

			// Setting up painter
			QPainter painter(this);
			_painter = &painter;

			painter.setRenderHint(QPainter::Antialiasing);

			// Foreground pen
			QPen fgPen(palette().color(QPalette::WindowText));
			_painter->setPen(fgPen);
			_painter->setBrush(Qt::NoBrush);

			_painter->drawRect(_graphBorder);

			// Background grid
			QPen backGridPen(palette().color(QPalette::Mid));
			backGridPen.setStyle(Qt::DotLine);
			_painter->setPen(backGridPen);


			// Horizontal lines
			int hPos = _graphBorder.bottom() - _vCellSize;
			while (hPos > _graphBorder.top())
			{
				_painter->drawLine(_graphBorder.left(), hPos, _graphBorder.right(), hPos); // Lines

				DrawTick(hPos, false); // Ticks

				hPos -= _vCellSize;
			}

			// Vertical lines
			int vPos = _graphBorder.left() + _hCellSize;
			while (vPos < _graphBorder.right())
			{
				_painter->drawLine(vPos, _graphBorder.top(), vPos, _graphBorder.bottom());

				DrawTick(vPos, true);

				vPos += _hCellSize;
			}

			// Axis titles
			_painter->setPen(fgPen);
			_painter->save();
			QFont boldFont = _painter->font();
			boldFont.setBold(true);
			_painter->setFont(boldFont);

			QFontMetrics textMetrics(_painter->font());
			QRect titleRect;

			// X
			titleRect = textMetrics.tightBoundingRect(_xAxisTitle);
			_painter->drawText(_graphBorder.right() - _xAxisTitleHShift - titleRect.width(), _graphBorder.bottom() - _xAxisTitleVShift, _xAxisTitle);

			// Y
			titleRect = textMetrics.tightBoundingRect(_yAxisTitle);
			_painter->drawText(_graphBorder.left() + _yAxisTitileHShift, _graphBorder.top() + _yAxisTitleVShift + titleRect.height(), _yAxisTitle);
			_painter->restore();

			// Drawing graph contents
			DrawGraphContents();

			// Drawing crosshair if we need it
			QMap<double, double>::const_iterator crosshairPoint = GetClosestPoint(_localMouseX);
			if (crosshairPoint != NULL)
			{
				int crosshairX = GetScreenXByValue(crosshairPoint.key());
				int crosshairY = GetScreenYByValue(crosshairPoint.value());

				_painter->drawLine(crosshairX, _graphBorder.top(), crosshairX, _graphBorder.bottom());
				_painter->drawLine(_graphBorder.left(), crosshairY, _graphBorder.right(), crosshairY);

				// Drawing tooltip with values
				bool tooltipYDirDown = crosshairY < _graphBorder.center().y();
				bool tooltipXDirRight = crosshairX < _graphBorder.center().x();
				TooltipDirection tooltipDirection;

				if (tooltipYDirDown)
				{
					if (tooltipXDirRight)
					{
						tooltipDirection = TooltipDirection::DownRight;
					}
					else
					{
						tooltipDirection = TooltipDirection::DownLeft;
					}
				}
				else
				{
					if (tooltipXDirRight)
					{
						tooltipDirection = TooltipDirection::UpRight;
					}
					else
					{
						tooltipDirection = TooltipDirection::UpLeft;
					}
				}

				QString tooltipText = QString("%1: %2\n%3: %4")
						.arg(_xAxisTitle)
						.arg(crosshairPoint.key())
						.arg(_yAxisTitle)
						.arg(crosshairPoint.value());
				DrawTooltip(QPoint(crosshairX, crosshairY), tooltipDirection, tooltipText);
			}
		}

		void QSimpleGraph::DrawTick(uint pos, bool IsXAxis)
		{
			// To not affect painter state outside
			_painter->save();


			QPen tickPen;
			tickPen.setColor(palette().color(QPalette::WindowText));
			_painter->setPen(tickPen);
			_painter->setBrush(Qt::NoBrush);

			double posValue;

			if (IsXAxis)
			{
				_painter->drawLine(pos, _graphBorder.bottom() - _tickShift, pos, _graphBorder.bottom() + _tickShift + 1);

				posValue = GetValByX(pos - _graphBorder.left());
			}
			else
			{
				_painter->drawLine(_graphBorder.left() - _tickShift - 1, pos, _graphBorder.left() + _tickShift, pos);

				posValue = GetValByY(_graphBorder.bottom() - pos);
			}

			if (IsXAxis)
			{
				// X axis labels
				QString textValue = FitDoubleByWidth(posValue, _hCellSize);
				DrawXLabel(pos, textValue);
			}
			else
			{
				// Y axis labels
				QString textValue = FitDoubleByWidth(posValue, _graphBorder.left() - _yAxisTextHShift);
				DrawYLabel(pos, textValue);
			}

			_painter->restore();
		}

		QString QSimpleGraph::FitDoubleByWidth(double val, uint MaxWidth)
		{
			QFontMetrics textMetrics(_painter->font());
			uint precision = _MaxDoublePrecision;

			QString result;

			do
			{
				result = QString("%1").arg(val, 1, 'g', precision);

				if (precision <= 1)
				{
					// Can't fit at all
					break;
				}

				precision --;
			}
			while((uint)textMetrics.width(result) > MaxWidth);

			return result;
		}

		void QSimpleGraph::DrawXLabel(uint xPos, QString label)
		{
			QFontMetrics textMetrics(_painter->font());
			QRect textRect = textMetrics.tightBoundingRect(label);
			int textHalfWidth = textRect.width() / 2;

			_painter->drawText(xPos - textHalfWidth, _graphBorder.bottom() + textRect.height() + _xAxisTextVShift, label); // Text below X-axis
		}

		void QSimpleGraph::DrawYLabel(uint yPos, QString label)
		{
			QFontMetrics textMetrics(_painter->font());
			QRect textRect = textMetrics.tightBoundingRect(label);
			uint textHalfHeight = textRect.height() / 2;

			_painter->drawText(_graphBorder.left() - textRect.width() - _yAxisTextHShift, yPos + textHalfHeight + _yAxisTextVShift, label); // Text lefter than axis
		}

		void QSimpleGraph::SetMinXValue(double minValue)
		{
			_minX = minValue;
			RecalculateXScalingFactor();
		}

		void QSimpleGraph::SetMaxXValue(double maxValue)
		{
			_maxX = maxValue;
			RecalculateXScalingFactor();
		}

		void QSimpleGraph::SetMinYValue(double minValue)
		{
			_minY = minValue;
			RecalculateYScalingFactor();
		}

		void QSimpleGraph::SetMaxYValue(double maxValue)
		{
			_maxY = maxValue;
			RecalculateYScalingFactor();
		}

		void QSimpleGraph::RecalculateXScalingFactor()
		{
			_xScalingFactor = _graphAreaWidth / (_maxX - _minX);
		}

		void QSimpleGraph::RecalculateYScalingFactor()
		{
			_yScalingFactor = _graphAreaHeight / (_maxY - _minY);
		}

		uint QSimpleGraph::GetXByVal(double val)
		{
			return floor(_xScalingFactor * (val - _minX) + 0.5);
		}

		uint QSimpleGraph::GetYByVal(double val)
		{
			return floor(_yScalingFactor * (val - _minY) + 0.5);
		}

		double QSimpleGraph::GetValByX(uint X)
		{
			return X / _xScalingFactor + _minX;
		}

		double QSimpleGraph::GetValByY(uint Y)
		{
			return Y / _yScalingFactor + _minY;
		}

		void QSimpleGraph::SetXAxisTitle(QString title)
		{
			_xAxisTitle = title;
		}

		void QSimpleGraph::SetYAxisTitle(QString title)
		{
			_yAxisTitle = title;
		}

		void QSimpleGraph::ClearAllPoints()
		{
			_points.clear();
		}

		void QSimpleGraph::AddPoint(double XVal, double YVal)
		{
			// Checking for border values
			if (XVal > _maxX)
			{
				SetMaxXValue(XVal);
			}

			if (XVal < _minX)
			{
				SetMinXValue(XVal);
			}

			if (YVal > _maxY)
			{
				SetMaxYValue(YVal);
			}

			if (YVal < _minY)
			{
				SetMinYValue(YVal);
			}

			_points.insert(XVal, YVal);
		}

		void QSimpleGraph::DrawGraphContents()
		{
			_painter->save();

			// Pen and brush to draw
			QPen pen(palette().color(QPalette::WindowText));
			_painter->setPen(pen);

			QBrush brush(palette().color(QPalette::WindowText), Qt::SolidPattern);
			_painter->setBrush(brush);

			// Iterating through points
			uint prevX, prevY; // Previous point coordinates
			QMap<double, double>::const_iterator item = _points.constBegin();
			while (item != _points.constEnd())
			{
				// Point coordinates
				uint xCoord = GetScreenXByValue(item.key());
				uint yCoord = GetScreenYByValue(item.value());

				if (item != _points.constBegin())
				{
					// Draw line
					_painter->save();

					QPen linePen(palette().color(QPalette::WindowText));
					linePen.setWidth(_graphLineWidth);
					_painter->setPen(linePen);

			#ifdef FOSSA_SIMPLE_GRAPH_POINT_TO_POINT_LINE
					_painter->drawLine(prevX, prevY, xCoord, yCoord);
			#else

					_painter->drawLine(prevX, prevY, xCoord, prevY);
					_painter->drawLine(xCoord, prevY, xCoord, yCoord);
			#endif

					_painter->restore();
				}

				// * 2 because parameters are widht and height, not radiuses.
				_painter->drawEllipse(xCoord - _pointRadius, yCoord - _pointRadius, _pointRadius * 2, _pointRadius * 2);

				prevX = xCoord;
				prevY = yCoord;

				item ++;
			}

			_painter->restore();
		}

		QMap<double, double>::const_iterator QSimpleGraph::GetClosestPoint(uint x)
		{
			if (0 == _points.count())
			{
				return NULL;
			}

			// X value
			double xVal = GetValByX(x);

			// Before all
			if (xVal < _points.constBegin().key())
			{
				return _points.constBegin();
			}

			QMap<double, double>::const_iterator prevIterator = _points.constBegin();
			QMap<double, double>::const_iterator nextIterator = prevIterator + 1;
			while (nextIterator != _points.constEnd())
			{
				double prevIteratorX = prevIterator.key();
				double nextIteratorX = nextIterator.key();

				if ((xVal >= prevIteratorX) && (xVal <= nextIteratorX))
				{
					// Who is closer?
					if ((xVal - prevIteratorX) < (nextIteratorX - xVal))
					{
						return prevIterator;
					}
					else
					{
						return nextIterator;
					}
				}

				// Next point
				prevIterator = nextIterator;
				nextIterator ++;
			}

			return nullptr; // To silence warning
		}

		uint QSimpleGraph::GetScreenXByValue(double xVal)
		{
			return _graphBorder.left() + GetXByVal(xVal);
		}

		uint QSimpleGraph::GetScreenYByValue(double yVal)
		{
			return _graphBorder.bottom() - GetYByVal(yVal);
		}

		void QSimpleGraph::DrawTooltip(QPoint targetPoint, TooltipDirection direction, QString text)
		{
			QRect textRect = QSimpleGraph::MultilineBoundingRect(_painter->font(), text);

			QRect borderRect = textRect;
			borderRect.adjust(0, 0, _tooltipWidthAdd, _tooltipHeightAdd);

			QPoint shiftedTargetPoint;
			switch(direction)
			{
				case TooltipDirection::DownRight:
					shiftedTargetPoint.setX(targetPoint.x() + _tooltipXShift);
					shiftedTargetPoint.setY(targetPoint.y() + _tooltipYShift);
					borderRect.moveTopLeft(shiftedTargetPoint);
				break;
				case TooltipDirection::DownLeft:
					shiftedTargetPoint.setX(targetPoint.x() - _tooltipXShift);
					shiftedTargetPoint.setY(targetPoint.y() + _tooltipYShift);
					borderRect.moveTopRight(shiftedTargetPoint);
				break;
				case TooltipDirection::UpRight:
					shiftedTargetPoint.setX(targetPoint.x() + _tooltipXShift);
					shiftedTargetPoint.setY(targetPoint.y() - _tooltipYShift);
					borderRect.moveBottomLeft(shiftedTargetPoint);
				break;
				case TooltipDirection::UpLeft:
					shiftedTargetPoint.setX(targetPoint.x() - _tooltipXShift);
					shiftedTargetPoint.setY(targetPoint.y() - _tooltipYShift);
					borderRect.moveBottomRight(shiftedTargetPoint);
				break;
			}

			_painter->save();

			// Bounding rectangle
			_painter->setPen(palette().color(QPalette::ToolTipText));
			QBrush tooltipBrush(palette().color(QPalette::ToolTipBase), Qt::SolidPattern);
			_painter->setBrush(tooltipBrush);
			_painter->drawRect(borderRect);

			// Drawing text
			_painter->drawText(borderRect, text, Qt::AlignLeft | Qt::AlignVCenter);

			_painter->restore();
		}

		QRect QSimpleGraph::MultilineBoundingRect(QFont font, QString text)
		{
			QFontMetrics textMetrics(font);

			int width = 0;
			int height = 0;
			// Text metrics can't work with text with line breaks, so splitting by hands
			foreach (QString line, text.split('\n'))
			{
				QRect boundRect = textMetrics.boundingRect(line);

				if (boundRect.width() > width)
				{
					width = boundRect.width();
				}

				height += boundRect.height();
			}

			return QRect(0, 0, width, height);
		}
	}
}
