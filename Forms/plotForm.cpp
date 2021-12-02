/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   plotForm.cpp
 * Author: Aaron
 *
 * Created on 20 de junio de 2017, 17:00
 */

#include "plotForm.h"

#ifdef PLOTS
plotForm::plotForm()
{
    widget.setupUi(this);
    setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint
            | Qt::WindowCloseButtonHint);
}

unsigned int plotForm::ConfigurePlots(chart * pCharts,
        unsigned int numberOfCharts, monitor * pMonitors,
        unsigned int numberOfMonitors)
{    
    plots = 0;
    scale = NULL;
    unsigned int r[MAX_PLOTS_PER_CHART], g[MAX_PLOTS_PER_CHART], b[MAX_PLOTS_PER_CHART];
    enum QwtSymbol::Style symbols[MAX_PLOTS_PER_CHART];
    
    /* check if there are any plot */
    plots = 1;
    for(unsigned int mnt=0; mnt<numberOfMonitors; ++mnt)
    {
        if(pMonitors[mnt].type == monitor::PLOT)
        {
            plots = 0;
            break;
        }
    }
    if(plots == 1)
        return 0;
        
    /* intialize colours (red, green, blue, purple, cyan) */
    for(unsigned int color=0; color<MAX_PLOTS_PER_CHART; ++color)
    {
        r[color] = 0;
        g[color] = 0;
        b[color] = 0;
    }
    r[0] = 255;
    g[1] = 255;
    b[2] = 255;
    r[3] = 255;
    b[3] = 255;
    g[4] = 255;
    b[4] = 255;
    r[5] = 255;
    g[5] = 255;
    r[6] = 128;
    g[7] = 128;
    b[8] = 128;
    r[9] = 128;
    b[9] = 128;
    g[10] = 128;
    b[10] = 128;
    r[11] = 128;
    g[11] = 128;
    
    symbols[0] = QwtSymbol::Star2;
    symbols[1] = QwtSymbol::Star1;
    symbols[2] = QwtSymbol::Diamond;
    symbols[3] = QwtSymbol::Triangle;
    symbols[4] = QwtSymbol::Rect;
    symbols[5] = QwtSymbol::XCross;
    symbols[6] = QwtSymbol::Hexagon;
    symbols[7] = QwtSymbol::DTriangle;
    symbols[8] = QwtSymbol::UTriangle;
    symbols[9] = QwtSymbol::LTriangle;
    symbols[10] = QwtSymbol::RTriangle;
    symbols[11] = QwtSymbol::Ellipse;
    
    /* intialize chart and curve variables */
    for(unsigned int ch=0; ch<MAX_CHARTS; ++ch)
    {
        tab[ch] = NULL;
        plot[ch] = NULL;
        maxTime[ch] = 0;
        plotsPerChart[ch] = 0;
    }
    for(unsigned int mnt=0; mnt<MAX_MONITORINGS; ++mnt)
    {
        curveCntr[mnt] = -1;
        plotRef[mnt] = -1;
        samples[mnt] = NULL;
        curve[mnt] = NULL;
        symbol[mnt] = NULL;
    }
    
    /* create charts */
    for(unsigned int ch=0; ch<numberOfCharts; ++ch)
    {
        /* first remove example tab from UI (can't be removed in Qt Designer */
        if(ch == 0)
        {
            scale = new QwtLinearScaleEngine();
            widget.plotsTab->removeTab(0);
        }
        tab[ch] = new QWidget();
        tab[ch]->setObjectName(QString("plotTab" + QString::number(ch)));
        widget.plotsTab->addTab(tab[ch], pCharts[ch].name);
        
        QHBoxLayout * horizontalLayout = new QHBoxLayout(tab[ch]);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        
        plot[ch] = new QwtPlot(tab[ch]);
        plot[ch]->setGeometry(QRect(20, 20, 540, 340));
        plot[ch]->setAxisTitle(QwtPlot::yLeft,
                QString(pCharts[ch].yAxisUnit));
        plot[ch]->setAxisTitle(QwtPlot::xBottom, QString("Time"));
        plot[ch]->setAxisScaleDiv(QwtPlot::yLeft, scale->divideScale(0,
                pCharts[ch].yMax, 10, 10));
        plot[ch]->setAxisScaleDiv(QwtPlot::xBottom, scale->divideScale(0,
                X_AXIS_LEN, X_MAX_MAJOR_STEPS, X_MAX_MINOR_STEPS));
        plot[ch]->updateAxes();
        maxTime[ch] = X_AXIS_LEN;
        legend[ch] = new QwtLegend(tab[ch]);
        legend[ch]->setDefaultItemMode(QwtLegendData::ReadOnly);
        plot[ch]->insertLegend(legend[ch]);
        pCharts[ch].numberOfPlots = 0;
        horizontalLayout->addWidget(plot[ch]);
    }
    
    /* create monitor plots */
    for(unsigned int mnt=0; mnt<numberOfMonitors; ++mnt)
    {
        if(pMonitors[mnt].type == monitor::PLOT)
        {
            curveCntr[mnt] = plots;
            plotRef[mnt] = pMonitors[mnt].data.chartRef;
            
            samples[plots] = new QVector<QPointF>;
            curve[plots] = new QwtPlotCurve(QString(pMonitors[mnt].name));
            curve[plots]->setStyle(QwtPlotCurve::Dots);
            symbol[plots] = new QwtSymbol(symbols[pCharts[plotRef[mnt]].numberOfPlots]);
            symbol[plots]->setPen(QColor(r[pCharts[plotRef[mnt]].numberOfPlots],
                    g[pCharts[plotRef[mnt]].numberOfPlots],
                    b[pCharts[plotRef[mnt]].numberOfPlots]), 3.0);
            curve[plots]->setSymbol(symbol[plots]);
            curve[plots]->setPen(QColor(r[pCharts[plotRef[mnt]].numberOfPlots],
                    g[pCharts[plotRef[mnt]].numberOfPlots],
                    b[pCharts[plotRef[mnt]].numberOfPlots]), 3.0);
            curve[plots]->attach(plot[plotRef[mnt]]);
            pCharts[plotRef[mnt]].numberOfPlots++;
            plots++;
        }
    }
    
    /* remove charts if no plots related */
    for(unsigned int ch=0; ch<numberOfCharts; ++ch)
    {
       if(pCharts[ch].numberOfPlots == 0)
       {
            widget.plotsTab->removeTab(ch);
       }
    }
    if(plots)
    {
        this->setWindowTitle(QString("Plots"));
        this->show();
    }
    return plots;
}

void plotForm::UnconfigPlots(chart * pCharts, unsigned int numberOfCharts,
        monitor * monitors, unsigned int numberOfMonitors)
{
    this->hide();
    
    /* delete plots */
    for(unsigned int mnt=0; mnt<numberOfMonitors; ++mnt)
    {
        if(monitors[mnt].type == monitor::PLOT)
        {
            delete plot[curveCntr[mnt]];
        }
    }
    
    /* remove charts */
    for(unsigned int ch=0; ch<numberOfCharts; ++ch)
    {
        if(pCharts[ch].numberOfPlots != 0)
        {
            widget.plotsTab->removeTab(ch);
        }
    }
}

void plotForm::Plot(unsigned int mntId, double time, double value)
{
    unsigned int multiplier = 0;
    if(time > maxTime[plotRef[mntId]])
    {
        multiplier = time/X_AXIS_LEN;
        plot[plotRef[mntId]]->setAxisScaleDiv(QwtPlot::xBottom,
                scale->divideScale(multiplier*X_AXIS_LEN, (multiplier+1)*X_AXIS_LEN,
                X_MAX_MAJOR_STEPS, X_MAX_MINOR_STEPS));
        plot[plotRef[mntId]]->updateAxes();
        maxTime[plotRef[mntId]] = (multiplier+1)*X_AXIS_LEN;
        samples[curveCntr[mntId]]->clear();
    }
    samples[curveCntr[mntId]]->push_back(QPointF(time, value));
    curve[curveCntr[mntId]]->setSamples(*samples[curveCntr[mntId]]);
    emit replot(plot[plotRef[mntId]]);
}

void plotForm::closePlotForm()
{
    this->hide();
}
#endif