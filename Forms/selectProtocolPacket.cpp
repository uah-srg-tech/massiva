/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   selectProtocolPacket.cpp
 * Author: Aaron
 *
 * Created on 27 de julio de 2017, 18:30
 */
#include <limits>
#include "selectProtocolPacket.h"
#include "../TesterClasses/TxStep.h"
#include "../CommonClasses/MathTools.h"

selectProtocolPacket::selectProtocolPacket(gssStructs * origGssStruct, Logs * origLogs,
            TestManager * origTestMgr, mainForm * origMainGui, RxTxInfo * origTxRxTabs,
            SpecialPackets * origSpecials, MonitorPlots * origMonitors)
{
    widget.setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
    widget.gridLayout->setAlignment(Qt::AlignTop);
    
    pGssStruct = origGssStruct;
    pLogs = origLogs;
    pTestMgr = origTestMgr;
    pMainGui = origMainGui;
    pSpecials = origSpecials;
    pMonitors = origMonitors;
    pTxRxTabs = origTxRxTabs;
    
    pps = 0;
    ppAction = NULL;
    numberOfSpinBoxes = 0;
    hexSpinBoxes = NULL;
    labels = NULL;
    sbFieldMap = NULL;
    sbIndexMap = NULL;
    isFirstCopy = NULL;
    extraInfoField = false;

    connect(widget.okButton, SIGNAL(released()), this, SLOT(processProtocolPacket()));
    connect(widget.cancelButton, SIGNAL(released()), this, SLOT(reject()));
}

void selectProtocolPacket::createProtocolPacketMenu()
{
    if(pGssStruct->getNumberOfProtocolPackets())
    {
        pMainGui->mainContent.menuSend_Protocol_Packets->menuAction()->setEnabled(true);
        pps = pGssStruct->getNumberOfProtocolPackets();
        ppAction = new QAction*[pps];
        for(unsigned int pp=0; pp<pps; ++pp)
        {
            ppAction[pp] = new QAction(pMainGui->mainContent.menuSend_Protocol_Packets);
            ppAction[pp]->setObjectName(QString::fromUtf8(pGssStruct->getProtocolPacketName(pp)));
            pMainGui->mainContent.menuSend_Protocol_Packets->addAction(ppAction[pp]);
            ppAction[pp]->setText(pGssStruct->getProtocolPacketName(pp));
            connect(ppAction[pp], SIGNAL(triggered()), this, SLOT(selectProtocolPacketMenu()));
        }
    }
    else
    {
        pMainGui->mainContent.menuSend_Protocol_Packets->menuAction()->setEnabled(false);
    }
}

void selectProtocolPacket::destroyProtocolPacketMenu()
{
    clearSpinBoxes();
    if(ppAction != NULL)
    {
        for(unsigned int pp=0; pp<pps; ++pp)
        {
            pMainGui->mainContent.menuSend_Protocol_Packets->removeAction(ppAction[pp]);
            delete ppAction[pp];
        }
        delete [] ppAction;
        ppAction = NULL;
    }
}

void selectProtocolPacket::selectProtocolPacketMenu()
{
    QAction * action = qobject_cast<QAction *>(sender());
    unsigned int pp = 0;
    for(pp=0; pp<pps; ++pp)
    {
        if(action == ppAction[pp])
            break;
    }
    pProtPacket = pGssStruct->getPointerToProtocolPacket(pp);
    
    pGssStruct->freeSteps();
    stepStruct ** ppStep = pGssStruct->getPointerToPointerToSteps();;
    
    /* create 1 step and default fill */
    *ppStep = (stepStruct*)calloc(1, sizeof(stepStruct));
    pGssStruct->setNumberOfSteps(1);
    
    ppStep[0]->name[0] = 0;
    ppStep[0]->prevStepIdRef = -1;
    ppStep[0]->outIdRefFromPrevStep = -1;
    ppStep[0]->mode = stepStruct::CONTINUOUS;
    ppStep[0]->intervalInMs = 0.0;
    ppStep[0]->outputsCheckmode = stepStruct::ALL;
    ppStep[0]->outputs = NULL;
    ppStep[0]->numberOfOutputs = 0;
    ppStep[0]->outputsReceived = 0;
    for(unsigned int port=0; port<MAX_INTERFACES; ++port)
    {
        ppStep[0]->numberOfOutputsAtPort[port] = 0;
        ppStep[0]->outputsReceivedAtPort[port] = 0;
        ppStep[0]->nextStepWithOutputsAtPort[port] = -1;
    }
    ppStep[0]->specials = NULL;
    ppStep[0]->numberOfSpecials = 0;
    ppStep[0]->concurrent.list = NULL;
    ppStep[0]->concurrent.number = 0;
    ppStep[0]->concurrent.next = 0;
    ppStep[0]->concurrent.nextIsNotConc = 0;
    ppStep[0]->replays = 1;
    
    /* create 1 input and default fill */
    ppStep[0]->numberOfInputs = 1;
    ppStep[0]->inputs = (input*)calloc(1, sizeof(input));
    pStepIn = &ppStep[0]->inputs[0];
    
    pStepIn->delayInMs = 0;
    pStepIn->ifRef = pProtPacket->ifRef;
    /* by default, numberOfLevels is levelRef */
    /* if there is a format file, numberOfLevels is levelRef+1 */
    pStepIn->numberOfLevels = pProtPacket->levelRef;
    if(pProtPacket->in[1].numberOfTCFields)
        pStepIn->numberOfLevels++;
    pStepIn->level = (levelIn*)calloc(pStepIn->numberOfLevels, sizeof(levelIn));
    for(unsigned int lvl=0; lvl<pStepIn->numberOfLevels; ++lvl)
    {
        pStepIn->level[lvl].numberOfTCFields = 0;
        pStepIn->level[lvl].TCFields = NULL;
        pStepIn->level[lvl].numberOfFDICTCFields = 0;
        pStepIn->level[lvl].numberOfcrcTCFields = 0;
        pStepIn->level[lvl].crcTCFieldRefs = NULL;
        pStepIn->level[lvl].numberOfExportFields = 0;
        pStepIn->level[lvl].exportFields = NULL;
        pStepIn->level[lvl].numberOfActiveDICs = 0;
        pStepIn->level[lvl].ActiveDICs = NULL;
    }
    
    /* get levelRef and fill format */
    unsigned int lvl = pProtPacket->levelRef;
    if(pProtPacket->in[1].numberOfTCFields)
    {
        pStepIn->level[lvl].numberOfTCFields = pProtPacket->in[1].numberOfTCFields;
        pStepIn->level[lvl].TCFields =
                (formatField*) calloc(pStepIn->level[lvl].numberOfTCFields, sizeof(formatField));
        memcpy(pStepIn->level[lvl].TCFields, pProtPacket->in[1].TCFields,
                sizeof(formatField)*pProtPacket->in[1].numberOfTCFields);
    }
    if(pProtPacket->in[1].numberOfFDICTCFields)
    {
        pStepIn->level[lvl].numberOfFDICTCFields = pProtPacket->in[1].numberOfFDICTCFields;
        pStepIn->level[lvl].numberOfcrcTCFields =
                (unsigned int*)calloc(pStepIn->level[lvl].numberOfFDICTCFields, sizeof(unsigned int));
        memcpy(pStepIn->level[lvl].numberOfcrcTCFields, pProtPacket->in[1].numberOfcrcTCFields,
                sizeof(unsigned int)*pProtPacket->in[1].numberOfFDICTCFields);
        pStepIn->level[lvl].crcTCFieldRefs =
                (unsigned int**)calloc(pStepIn->level[lvl].numberOfFDICTCFields, sizeof(unsigned int));
        for(unsigned int i=0; i<pStepIn->level[lvl].numberOfFDICTCFields; ++i)
        {
            pStepIn->level[lvl].crcTCFieldRefs[i] = 
                    (unsigned int*)calloc(pStepIn->level[lvl].numberOfcrcTCFields[i], sizeof(unsigned int));
            memcpy(pStepIn->level[lvl].crcTCFieldRefs[i], pProtPacket->in[1].crcTCFieldRefs[i],
                    sizeof(unsigned int)*pProtPacket->in[1].numberOfcrcTCFields[i]);
        }
    }
    /* fill levelRef-1 export */
    pStepIn->level[lvl-1].numberOfExportFields = pProtPacket->in[0].numberOfExportFields;
    pStepIn->level[lvl-1].exportFields =
            (levelExport*)calloc(pStepIn->level[lvl-1].numberOfExportFields, sizeof(levelExport));
    memcpy(pStepIn->level[lvl-1].exportFields, pProtPacket->in[0].exportFields,
            sizeof(levelExport)*pProtPacket->in[0].numberOfExportFields);
    if(pProtPacket->in[0].numberOfActiveDICs)
    {
        pStepIn->level[lvl-1].numberOfActiveDICs = pProtPacket->in[0].numberOfActiveDICs;
        pStepIn->level[lvl-1].ActiveDICs =
                (unsigned int*)calloc(pStepIn->level[lvl-1].numberOfActiveDICs, sizeof(unsigned int));
        memcpy(pStepIn->level[lvl-1].ActiveDICs, pProtPacket->in[0].ActiveDICs,
                sizeof(unsigned int)*pProtPacket->in[0].numberOfActiveDICs);
    }
    
    /* assume first field as containing field, thus not to be filled */
    if(pProtPacket->in[1].numberOfTCFields < 2)
    {
        sendProtocolPacket();
    }
    else
    {
        clearSpinBoxes();
        this->layout()->activate();
        this->resize(248, 90);
        unsigned int maxItems = 0, itemSize = 0, showIndexRef = 0;
        for(unsigned int fld=1; fld<pProtPacket->in[1].numberOfTCFields; ++fld)
        {
            switch(pProtPacket->in[1].TCFields[fld].type)
            {
                case CSFIELD:
                    numberOfSpinBoxes++;
                    break;
                    
                case AIFIELD:
                    if(((int)itemSize == pProtPacket->in[1].TCFields[fld].totalSizeInBits) &&
                            (fld != (pProtPacket->in[1].numberOfTCFields-1)) &&
                            (pProtPacket->in[1].TCFields[fld+1].type == AIFIELD))
                        /* if AFIELD itemSize is AIFIELD totalSizeInBits */
                        /* (and this is not last field) and next field is AIFIELD */
                        /* this a block for the AIFIELDS, won't be used */
                        continue;
                    numberOfSpinBoxes+=MAX_COPIES;
                    break;
                    
                case AFIELD:
                    showIndexRef = pProtPacket->in[1].TCFields[fld].info.array.fieldRef;
                    maxItems = pProtPacket->in[1].TCFields[fld].info.array.maxItems;
                    itemSize = pProtPacket->in[1].TCFields[fld].info.array.sizeOfItem;
                    break;
                    
                case VSFIELD:
                    showIndexRef = pProtPacket->in[1].TCFields[fld].info.variable.fieldRef;
                    maxItems = pProtPacket->in[1].TCFields[fld].info.variable.maxSizeInBits/8;
                    itemSize = 0;
                    numberOfSpinBoxes++;
                    break;
                    
                default:
                    break;
            }
        }
        pStepIn->level[lvl].numberOfExportFields = numberOfSpinBoxes;
        pStepIn->level[lvl].exportFields =
                (levelExport*)calloc(numberOfSpinBoxes, sizeof(levelExport));
    
        unsigned int numberOfLabels = numberOfSpinBoxes;
        if(maxItems > MAX_COPIES)
        {
            labels = new QLabel *[numberOfSpinBoxes+1];
            numberOfLabels++;
        }
        else
        {
            labels = new QLabel *[numberOfSpinBoxes];
        }
#if QT_VERSION >= 0x050000
        hexSpinBoxes = new QSpinBox*[numberOfSpinBoxes];
#else
        hexSpinBoxes = new HexSpinBox*[numberOfSpinBoxes];
#endif
        sbFieldMap = new unsigned int[numberOfSpinBoxes];
        sbIndexMap = new unsigned int[numberOfSpinBoxes];
        isFirstCopy = new bool[numberOfSpinBoxes];
               
        unsigned int *vertRef = new unsigned int[numberOfSpinBoxes]; 
        unsigned int *horizRef = new unsigned int[numberOfSpinBoxes]; 
        
        unsigned int sb=0, aiFields = 0, firstAiSbs = 0;
        for(unsigned int fld=1; fld<pProtPacket->in[1].numberOfTCFields; ++fld)
        {
            switch(pProtPacket->in[1].TCFields[fld].type)
            {
                case CSFIELD: case VSFIELD:
                    sbFieldMap[sb] = fld;
                    sbIndexMap[sb] = 0;
                    isFirstCopy[sb] = false;
                    horizRef[sb] = 0;
                    vertRef[sb] = sb;
                    sb++;
                    break;
                    
                case AIFIELD:
                    if(((int)itemSize == pProtPacket->in[1].TCFields[fld].totalSizeInBits) &&
                            (fld != (pProtPacket->in[1].numberOfTCFields-1)) &&
                            (pProtPacket->in[1].TCFields[fld+1].type == AIFIELD))
                        /* if AFIELD itemSize is AIFIELD totalSizeInBits */
                        /* (and this is not last field) and next field is AIFIELD */
                        /* this a block for the AIFIELDS, won't be used */
                        continue;
                    if(!aiFields)
                        firstAiSbs = sb;
                    
                    for(unsigned int dup=0; dup<MAX_COPIES; ++dup)
                    {
                        sbFieldMap[sb+dup] = fld;
                        sbIndexMap[sb+dup] = dup;
                        if(!dup)
                        {
                            isFirstCopy[sb] = true;
                        }
                        else
                        {
                            isFirstCopy[sb+dup] = false;                 
                        }
                        horizRef[sb+dup] = aiFields;
                        vertRef[sb+dup] = firstAiSbs+dup;
                    }
                    sb+=MAX_COPIES;
                    aiFields++;
                    break;
                    
                default:
                    break;
            }
        }
        int sbIndex = -1;
        for(unsigned int sb=0; sb<numberOfSpinBoxes; ++sb)
        {
            QString name;
            pStepIn->level[lvl].exportFields[sb].targetFieldRef = sbFieldMap[sb];
            pStepIn->level[lvl].exportFields[sb].index = sbIndexMap[sb];
            
            if(pProtPacket->in[1].TCFields[sbFieldMap[sb]].type == AIFIELD)
            {
                pStepIn->level[lvl].exportFields[sb].type = levelExport::EX_ARRAY_CONST;
                name = QString(pProtPacket->in[1].TCFields[sbFieldMap[sb]].name) +
                        QString(" [") + QString::number(sbIndexMap[sb]) + QString("]");
            }
            else
            {
                pStepIn->level[lvl].exportFields[sb].type = levelExport::EX_CONSTANT;
                name = QString(pProtPacket->in[1].TCFields[sbFieldMap[sb]].name);
            }
            labels[sb] = new QLabel(name, this);
            labels[sb]->setObjectName(QString::fromUtf8("label")+QString::number(sb));
            labels[sb]->setMinimumSize(QSize(100, 13));
            widget.gridLayout->addWidget(labels[sb], vertRef[sb], 2*horizRef[sb]);
            
#if QT_VERSION >= 0x050000
            hexSpinBoxes[sb] = new QSpinBox(this);
            hexSpinBoxes[sb]->setDisplayIntegerBase(16);
#else
			unsigned int bytes = 1;
            if(pProtPacket->in[1].TCFields[sbFieldMap[sb]].type != VSFIELD)
            {
                /* as default N=1 - max bytes length = 2 (thus max Data=0xFF)*/
				bytes = pProtPacket->in[1].TCFields[sbFieldMap[sb]].totalSizeInBits/8;
			}
			hexSpinBoxes[sb] = new HexSpinBox(this, bytes);
#endif

            hexSpinBoxes[sb]->setObjectName(QString::fromUtf8("hexSpinBox") + QString::number(sb));
            hexSpinBoxes[sb]->setMinimumSize(QSize(120, 22));
            widget.gridLayout->addWidget(hexSpinBoxes[sb], vertRef[sb], 2*horizRef[sb]+1);
                        
            if(sbFieldMap[sb] == showIndexRef)
            {
                sbIndex = sb;
                if(maxItems > MAX_COPIES)
                {
                    maxItems = MAX_COPIES;
                    QString name;
                    if(itemSize)
                        name = QString("For more than ") + QString::number(MAX_COPIES) +
                                QString(" items, send as raw command");
                    else
                        name = QString("For more than ") + QString::number(MAX_COPIES) +
                                QString(" bytes, send as raw command");
                    labels[numberOfSpinBoxes] = new QLabel(name, this);
                    labels[numberOfSpinBoxes]->setMinimumSize(QSize(220, 13));
                    widget.gridLayout->addWidget(labels[numberOfSpinBoxes],
                            0, 2, 1, -1);
                    extraInfoField = true;
                }
                if(itemSize)
                {
                    /* AFIELD ref */
                    connect(hexSpinBoxes[sb], SIGNAL(valueChanged(qulonglong)),
                            this, SLOT(updateArrayField(qulonglong)));
                }
                else
                {
                    /* VSFIELD ref */
                    connect(hexSpinBoxes[sb], SIGNAL(valueChanged(qulonglong)),
                            this, SLOT(updateVariableField(qulonglong)));
                }
#if QT_VERSION >= 0x050000
                hexSpinBoxes[sb]->setMaximum(maxItems);
#else
                hexSpinBoxes[sb]->setMaximum(maxItems, false);
#endif
            }
        }
        delete[] vertRef;
        delete[] horizRef;
        
        if(sbIndex != -1)
        {
            /* force first to maxItem to get maxSize initally */
            hexSpinBoxes[sbIndex]->setValue(maxItems);
            this->layout()->activate();
            QSize maxSize = this->size();
            hexSpinBoxes[sbIndex]->setValue(1);
            this->resize(maxSize);
        }
        this->setWindowTitle(QString("Send ") + QString(pProtPacket->name) +
                QString(" via ") + QString(pGssStruct->getPortName(pProtPacket->ifRef)));
        this->exec();
    }
}

void selectProtocolPacket::updateArrayField(qulonglong newValue)
{
    for(unsigned int sb=0; sb<numberOfSpinBoxes; ++sb)
    {
        if(isFirstCopy[sb])
        {
            for(unsigned int dup=0.0; dup<(unsigned int)newValue; ++dup)
            {
                hexSpinBoxes[sb+dup]->show();
                labels[sb+dup]->show();
            }
            for(unsigned int dup=(unsigned int)newValue; dup<MAX_COPIES; ++dup)
            {
                hexSpinBoxes[sb+dup]->hide();
                labels[sb+dup]->hide();
            }
        }                
    }
}

void selectProtocolPacket::updateVariableField(qulonglong newValue)
{
    for(unsigned int sb=0; sb<numberOfSpinBoxes; ++sb)
    {
        if(pProtPacket->in[1].TCFields[sbFieldMap[sb]].type == VSFIELD)
        {
            hexSpinBoxes[sb]->setMaximum(MathTools::ullpow(2, newValue*8)-1);
        }
    }
}
    
void selectProtocolPacket::processProtocolPacket()
{
    /* get data from widget and export data */
    for(unsigned int sb=0; sb<numberOfSpinBoxes; ++sb)
    {
        pStepIn->level[pProtPacket->levelRef].exportFields[sb].data.constOrSizeInBits = 
                hexSpinBoxes[sb]->value();
    }
    sendProtocolPacket();
    this->close();
}

void selectProtocolPacket::sendProtocolPacket()
{
    pMainGui->setEnabledControls(false, true, true, false);
    unsigned int curTxStep = 0, curInput = 0;
    TxStep tx(pGssStruct, pLogs, pTestMgr, pMainGui, pSpecials, pTxRxTabs,
            pMonitors, 1, &curTxStep, &curInput, NULL);
    tx.send(false);
    pMainGui->setEnabledControls(true, true, false, false);
}

void selectProtocolPacket::clearSpinBoxes()
{
    if(hexSpinBoxes != NULL)
    {
        for(unsigned int sb=0; sb<numberOfSpinBoxes; ++sb)
        {
            widget.gridLayout->removeWidget(labels[sb]);
            delete labels[sb];
            widget.gridLayout->removeWidget(hexSpinBoxes[sb]);
            delete hexSpinBoxes[sb];
        }
        if(extraInfoField)
        {
            widget.gridLayout->removeWidget(labels[numberOfSpinBoxes]);
            delete labels[numberOfSpinBoxes];
        }
        delete[] hexSpinBoxes;
        delete[] labels;
        numberOfSpinBoxes = 0;
        extraInfoField = false;
    }
}