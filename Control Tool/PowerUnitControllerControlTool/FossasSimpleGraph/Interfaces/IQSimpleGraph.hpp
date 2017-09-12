#ifndef IFOSSASSIMPLEGRAPH_HPP
#define IFOSSASSIMPLEGRAPH_HPP

#include <QObject>
#include <QtWidgets/QWidget>

/**
 * @brief Namespace for all reusable stuff, created by Fossa
 */
namespace Fossa
{
  /**
   * @brief Simple graph control
   */
  namespace QSimpleGraph
  {
    /**
     *  @brief Simple graph control interfaces
     */
    namespace Interfaces
    {
      /**
       * @brief Main interface of QSimpleGraph
       */
      class IQSimpleGraph : public QWidget
      {
        Q_OBJECT

        // Public members
        public:

          /**
           * @brief Widget constructor.
           * @param parent Parent widget.
           */
          IQSimpleGraph(QWidget *parent = 0) : QWidget(parent)
          {

          }

          /**
           * @brief See SetMaxXValue for details, it have completely the same behaviour.
           * @param minValue New minimal value. If it greater then maximal X value, it will be set to maximal X value.
           */
          virtual void SetMinXValue(double minValue) = 0;

          /**
           * @brief Call it to set maximal value of X-axis. Please note, that if new point added to graph later, and point's X coordinate exceeds maximal X value, then
           * maximal X value will be increased, allowing graph to display such points.
           * @param maxValue New maximal value. If it less then minimal X value, it will be set to minimal X value.
           */
          virtual void SetMaxXValue(double maxValue) = 0;

          /**
           * @brief As SetMinXValue, but for Y-axis.
           * @param minValue See SetMinXValue.
           */
          virtual void SetMinYValue(double minValue) = 0;

          /**
           * @brief As SetMaxXValue, but for Y-axis.
           * @param maxValue See SetMaxXValue.
           */
          virtual void SetMaxYValue(double maxValue) = 0;

          /**
           * @brief Set title of X-Axis.
           * @param title Title.
           */
          virtual void SetXAxisTitle(QString title) = 0;

          /**
           * @brief As SetXAxisTitle(), but for Y-Axis.
           * @param title See SetXAxisTitle().
           */
          virtual void SetYAxisTitle(QString title) = 0;

          /**
           * @brief Removes all points from graph.
           */
          virtual void ClearAllPoints() = 0;

          /**
           * @brief Adds point with given X and Y value to graph. Points collection inside the graph must be self-ordered, so points can be added in
           * arbitraty order. If point outside maximal and minimal values for graph, minimal and maximal values will be expanded to allow display all points.
           * @param XVal X-axis value.
           * @param YVal Y-axis value.
           */
          virtual void AddPoint(double XVal, double YVal) = 0;

        // Protected members
        protected:

        // Private members
        private:

      };
    }
  }
}

#endif // IFOSSASSIMPLEGRAPH_HPP
