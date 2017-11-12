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

#include <FossasSimpleGraph/Implementations/QSimpleGraph.hpp>

namespace Fossa
{
	namespace QSimpleGraph
	{
		QSimpleGraph::QSimpleGraph(QWidget *parent) : IQSimpleGraph(parent)
		{
			this->_localMouseX = 0;
			this->_localMouseY = 0;
			this->_graphBorder.setRect(0, 0, 1, 1); // Initial set-up
			this->setMouseTracking(true); // Call mouseMoveEvent() even if no buttons pressed
		}

		QSize QSimpleGraph::minimumSizeHint() const
		{
			return this->minimumSize();
		}

		QSize QSimpleGraph::sizeHint() const
		{
			return this->size();
		}

		void QSimpleGraph::mouseMoveEvent(QMouseEvent *event)
		{
			// Are we in graph data area?
			if
			(
	(event->x() >= this->_graphBorder.left()) && (event->x() <= this->_graphBorder.right())
	&&
	(event->y() >= this->_graphBorder.top()) && (event->y() <= this->_graphBorder.bottom())
			)
			{
	this->_localMouseX = event->x() - this->_graphBorder.left();
	this->_localMouseY = event->y() - this->_graphBorder.top();
			}

			// We need to repaint
			this->update();
		}

		void QSimpleGraph::paintEvent(QPaintEvent *event)
		{
			// Setting up painter
			QPainter painter(this);
			this->_painter = &painter;

			painter.setRenderHint(QPainter::Antialiasing);

			// Drawing outer rectangle
			this->_graphBorder.setTop(this->_outerTopSpacing);
			this->_graphBorder.setLeft(this->_outerLeftSpacing);
			this->_graphBorder.setBottom(this->height() - this->_outerBottomSpacing);
			this->_graphBorder.setRight(this->width() - this->_outerRightSpacing);

			// Graph dimensions
			this->_graphAreaWidth = this->_graphBorder.width();
			this->_graphAreaHeight = this->_graphBorder.height();

			// Scaling factors. TODO: Move to resize event
			this->RecalculateXScalingFactor();
			this->RecalculateYScalingFactor();

			// Foreground pen
			QPen fgPen(this->palette().color(QPalette::WindowText));
			this->_painter->setPen(fgPen);
			this->_painter->setBrush(Qt::NoBrush);

			this->_painter->drawRect(this->_graphBorder);

			// Background grid
			QPen backGridPen(this->palette().color(QPalette::Mid));
			backGridPen.setStyle(Qt::DotLine);
			this->_painter->setPen(backGridPen);


			// Horizontal lines
			int hPos = this->_graphBorder.bottom() - this->_vCellSize;
			while (hPos > this->_graphBorder.top())
			{
	this->_painter->drawLine(this->_graphBorder.left(), hPos, this->_graphBorder.right(), hPos); // Lines

	this->DrawTick(hPos, false); // Ticks

	hPos -= this->_vCellSize;
			}

			// Vertical lines
			int vPos = this->_graphBorder.left() + this->_hCellSize;
			while (vPos < this->_graphBorder.right())
			{
	this->_painter->drawLine(vPos, this->_graphBorder.top(), vPos, this->_graphBorder.bottom());

	this->DrawTick(vPos, true);

	vPos += this->_hCellSize;
			}

			// Axis titles
			this->_painter->setPen(fgPen);
			this->_painter->save();
			QFont boldFont = this->_painter->font();
			boldFont.setBold(true);
			this->_painter->setFont(boldFont);

			QFontMetrics textMetrics(this->_painter->font());
			QRect titleRect;

			// X
			titleRect = textMetrics.tightBoundingRect(this->_xAxisTitle);
			this->_painter->drawText(this->_graphBorder.right() - this->_xAxisTitleHShift - titleRect.width(), this->_graphBorder.bottom() - this->_xAxisTitleVShift, this->_xAxisTitle);

			// Y
			titleRect = textMetrics.tightBoundingRect(this->_yAxisTitle);
			this->_painter->drawText(this->_graphBorder.left() + this->_yAxisTitileHShift, this->_graphBorder.top() + this->_yAxisTitleVShift + titleRect.height(), this->_yAxisTitle);
			this->_painter->restore();

			// Drawing graph contents
			this->DrawGraphContents();

			// Drawing crosshair if we need it
			QMap<double, double>::const_iterator crosshairPoint = this->GetClosestPoint(this->_localMouseX);
			if (crosshairPoint != NULL)
	{
	int crosshairX = this->GetScreenXByValue(crosshairPoint.key());
	int crosshairY = this->GetScreenYByValue(crosshairPoint.value());

	this->_painter->drawLine(crosshairX, this->_graphBorder.top(), crosshairX, this->_graphBorder.bottom());
	this->_painter->drawLine(this->_graphBorder.left(), crosshairY, this->_graphBorder.right(), crosshairY);

	// Drawing tooltip with values
	bool tooltipYDirDown = crosshairY < this->_graphBorder.center().y();
	bool tooltipXDirRight = crosshairX < this->_graphBorder.center().x();
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
			.arg(this->_xAxisTitle)
			.arg(crosshairPoint.key())
			.arg(this->_yAxisTitle)
			.arg(crosshairPoint.value());
	this->DrawTooltip(QPoint(crosshairX, crosshairY), tooltipDirection, tooltipText);
			}
		}

		void QSimpleGraph::DrawTick(uint pos, bool IsXAxis)
		{
			// To not affect painter state outside
			this->_painter->save();


			QPen tickPen;
			tickPen.setColor(this->palette().color(QPalette::WindowText));
			this->_painter->setPen(tickPen);
			this->_painter->setBrush(Qt::NoBrush);

			double posValue;

			if (IsXAxis)
			{
	this->_painter->drawLine(pos, this->_graphBorder.bottom() - this->_tickShift, pos, this->_graphBorder.bottom() + this->_tickShift + 1);

	posValue = this->GetValByX(pos - this->_graphBorder.left());
			}
			else
			{
	this->_painter->drawLine(this->_graphBorder.left() - this->_tickShift - 1, pos, this->_graphBorder.left() + this->_tickShift, pos);

	posValue = this->GetValByY(this->_graphBorder.bottom() - pos);
			}

			if (IsXAxis)
			{
	// X axis labels
	QString textValue = this->FitDoubleByWidth(posValue, this->_hCellSize);
	this->DrawXLabel(pos, textValue);
			}
			else
			{
	// Y axis labels
	QString textValue = this->FitDoubleByWidth(posValue, this->_graphBorder.left() - this->_yAxisTextHShift);
	this->DrawYLabel(pos, textValue);
			}

			this->_painter->restore();
		}

		QString QSimpleGraph::FitDoubleByWidth(double val, uint MaxWidth)
		{
			QFontMetrics textMetrics(this->_painter->font());
			uint precision = this->_MaxDoublePrecision;

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
			QFontMetrics textMetrics(this->_painter->font());
			QRect textRect = textMetrics.tightBoundingRect(label);
			int textHalfWidth = textRect.width() / 2;

			this->_painter->drawText(xPos - textHalfWidth, this->_graphBorder.bottom() + textRect.height() + this->_xAxisTextVShift, label); // Text below X-axis
		}

		void QSimpleGraph::DrawYLabel(uint yPos, QString label)
		{
			QFontMetrics textMetrics(this->_painter->font());
			QRect textRect = textMetrics.tightBoundingRect(label);
			uint textHalfHeight = textRect.height() / 2;

			this->_painter->drawText(this->_graphBorder.left() - textRect.width() - this->_yAxisTextHShift, yPos + textHalfHeight + this->_yAxisTextVShift, label); // Text lefter than axis
		}

		void QSimpleGraph::SetMinXValue(double minValue)
		{
			this->_minX = minValue;
			this->RecalculateXScalingFactor();
		}

		void QSimpleGraph::SetMaxXValue(double maxValue)
		{
			this->_maxX = maxValue;
			this->RecalculateXScalingFactor();
		}

		void QSimpleGraph::SetMinYValue(double minValue)
		{
			this->_minY = minValue;
			this->RecalculateYScalingFactor();
		}

		void QSimpleGraph::SetMaxYValue(double maxValue)
		{
			this->_maxY = maxValue;
			this->RecalculateYScalingFactor();
		}

		void QSimpleGraph::RecalculateXScalingFactor()
		{
			this->_xScalingFactor = this->_graphAreaWidth / (this->_maxX - this->_minX);
		}

		void QSimpleGraph::RecalculateYScalingFactor()
		{
			this->_yScalingFactor = this->_graphAreaHeight / (this->_maxY - this->_minY);
		}

		uint QSimpleGraph::GetXByVal(double val)
		{
			return floor(this->_xScalingFactor * (val - this->_minX) + 0.5);
		}

		uint QSimpleGraph::GetYByVal(double val)
		{
			return floor(this->_yScalingFactor * (val - this->_minY) + 0.5);
		}

		double QSimpleGraph::GetValByX(uint X)
		{
			return X / this->_xScalingFactor + this->_minX;
		}

		double QSimpleGraph::GetValByY(uint Y)
		{
			return Y / this->_yScalingFactor + this->_minY;
		}

		void QSimpleGraph::SetXAxisTitle(QString title)
		{
			this->_xAxisTitle = title;
		}

		void QSimpleGraph::SetYAxisTitle(QString title)
		{
			this->_yAxisTitle = title;
		}

		void QSimpleGraph::ClearAllPoints()
		{
			this->_points.clear();
		}

		void QSimpleGraph::AddPoint(double XVal, double YVal)
		{
			// Checking for border values
			if (XVal > this->_maxX)
			{
	this->SetMaxXValue(XVal);
			}

			if (XVal < this->_minX)
			{
	this->SetMinXValue(XVal);
			}

			if (YVal > this->_maxY)
			{
	this->SetMaxYValue(YVal);
			}

			if (YVal < this->_minY)
			{
	this->SetMinYValue(YVal);
			}

			this->_points.insert(XVal, YVal);
		}

		void QSimpleGraph::DrawGraphContents()
		{
			this->_painter->save();

			// Pen and brush to draw
			QPen pen(this->palette().color(QPalette::WindowText));
			this->_painter->setPen(pen);

			QBrush brush(this->palette().color(QPalette::WindowText), Qt::SolidPattern);
			this->_painter->setBrush(brush);

			// Iterating through points
			uint prevX, prevY; // Previous point coordinates
			QMap<double, double>::const_iterator item = this->_points.constBegin();
			while (item != this->_points.constEnd())
			{
	// Point coordinates
	uint xCoord = this->GetScreenXByValue(item.key());
	uint yCoord = this->GetScreenYByValue(item.value());

	if (item != this->_points.constBegin())
	{
		// Draw line
		this->_painter->save();

		QPen linePen(this->palette().color(QPalette::WindowText));
		linePen.setWidth(this->_graphLineWidth);
		this->_painter->setPen(linePen);

#ifdef FOSSA_SIMPLE_GRAPH_POINT_TO_POINT_LINE
		this->_painter->drawLine(prevX, prevY, xCoord, yCoord);
#else

		this->_painter->drawLine(prevX, prevY, xCoord, prevY);
		this->_painter->drawLine(xCoord, prevY, xCoord, yCoord);
#endif

		this->_painter->restore();
	}

	// * 2 because parameters are widht and height, not radiuses.
	this->_painter->drawEllipse(xCoord - this->_pointRadius, yCoord - this->_pointRadius, this->_pointRadius * 2, this->_pointRadius * 2);

	prevX = xCoord;
	prevY = yCoord;

	item ++;
			}

			this->_painter->restore();
		}

		QMap<double, double>::const_iterator QSimpleGraph::GetClosestPoint(uint x)
		{
			if (0 == this->_points.count())
			{
	return NULL;
			}

			// X value
			double xVal = this->GetValByX(x);

			// Before all
			if (xVal < this->_points.constBegin().key())
			{
	return this->_points.constBegin();
			}

			QMap<double, double>::const_iterator prevIterator = this->_points.constBegin();
			QMap<double, double>::const_iterator nextIterator = prevIterator + 1;
			while (nextIterator != this->_points.constEnd())
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
		}

		uint QSimpleGraph::GetScreenXByValue(double xVal)
		{
			return this->_graphBorder.left() + this->GetXByVal(xVal);
		}

		uint QSimpleGraph::GetScreenYByValue(double yVal)
		{
			return this->_graphBorder.bottom() - this->GetYByVal(yVal);
		}

		void QSimpleGraph::DrawTooltip(QPoint targetPoint, TooltipDirection direction, QString text)
		{
			QRect textRect = QSimpleGraph::MultilineBoundingRect(this->_painter->font(), text);

			QRect borderRect = textRect;
			borderRect.adjust(0, 0, this->_tooltipWidthAdd, this->_tooltipHeightAdd);

			QPoint shiftedTargetPoint;
			switch(direction)
			{
	case TooltipDirection::DownRight:
		shiftedTargetPoint.setX(targetPoint.x() + this->_tooltipXShift);
		shiftedTargetPoint.setY(targetPoint.y() + this->_tooltipYShift);
		borderRect.moveTopLeft(shiftedTargetPoint);
	break;
	case TooltipDirection::DownLeft:
		shiftedTargetPoint.setX(targetPoint.x() - this->_tooltipXShift);
		shiftedTargetPoint.setY(targetPoint.y() + this->_tooltipYShift);
		borderRect.moveTopRight(shiftedTargetPoint);
	break;
	case TooltipDirection::UpRight:
		shiftedTargetPoint.setX(targetPoint.x() + this->_tooltipXShift);
		shiftedTargetPoint.setY(targetPoint.y() - this->_tooltipYShift);
		borderRect.moveBottomLeft(shiftedTargetPoint);
	break;
	case TooltipDirection::UpLeft:
		shiftedTargetPoint.setX(targetPoint.x() - this->_tooltipXShift);
		shiftedTargetPoint.setY(targetPoint.y() - this->_tooltipYShift);
		borderRect.moveBottomRight(shiftedTargetPoint);
	break;
			}

			this->_painter->save();

			// Bounding rectangle
			this->_painter->setPen(this->palette().color(QPalette::ToolTipText));
			QBrush tooltipBrush(this->palette().color(QPalette::ToolTipBase), Qt::SolidPattern);
			this->_painter->setBrush(tooltipBrush);
			this->_painter->drawRect(borderRect);

			// Drawing text
			this->_painter->drawText(borderRect, text, Qt::AlignLeft | Qt::AlignVCenter);

			this->_painter->restore();
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


