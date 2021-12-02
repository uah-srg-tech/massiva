/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   plotForm.h
 * Author: Aaron
 *
 * Created on 20 de junio de 2017, 17:00
 */

#ifndef _PLOTFORM_H
#define _PLOTFORM_H

#include "ui_plotForm.h"
#include "../CommonClasses/gssStructs.h"
#ifdef PLOTS
#include <qwt_plot.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_seriesitem.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#endif

#define X_AXIS_LEN			60.0
#define X_MAX_MAJOR_STEPS		6
#define X_MAX_MINOR_STEPS		10

class plotForm : public QDialog
{
    Q_OBJECT

#ifdef PLOTS
public:
    plotForm();
    unsigned int ConfigurePlots(chart * pCharts, unsigned int numberOfCharts,
        monitor * pMonitors, unsigned int numberOfMonitors);
    void UnconfigPlots(chart * pCharts, unsigned int numberOfCharts,
            monitor * monitors, unsigned int numberOfMonitors);
    void Plot(unsigned int mntId, double time, double value);
    
public slots:
    void closePlotForm();

signals:
    void replot(QwtPlot * pPlot);
    
private:
    Ui::plotForm widget;
    
    unsigned int plots;
    QWidget * tab[MAX_CHARTS];
    QwtPlot * plot[MAX_CHARTS];
    double maxTime[MAX_CHARTS];
    unsigned int plotsPerChart[MAX_CHARTS];
    QwtLegend * legend[MAX_CHARTS];
    QwtLinearScaleEngine * scale;

    int plotRef[MAX_MONITORINGS];
    int curveCntr[MAX_MONITORINGS];
    QVector<QPointF> * samples[MAX_MONITORINGS];
    QwtPlotCurve * curve[MAX_MONITORINGS];
    QwtSymbol * symbol[MAX_MONITORINGS];
#endif
};

#endif /* _PLOTFORM_H */
