#include <QtGui>
#include <limits>
#include "HexSpinBox.h"
#define UNUSED(x) (void)(x)

HexSpinBox::HexSpinBox(QWidget *parent, unsigned int bytes): QAbstractSpinBox(parent)
{
    connect(lineEdit(), SIGNAL(textEdited(QString)), this, SLOT(interpretText()));
    QString regExp = QString("[0-9A-Fa-f]{1,16}");
    validator = new QRegExpValidator(QRegExp(regExp), this);
    
    m_maxTextLength = 2*bytes;
    setRange(0, ullpow(2, bytes*8)-1);
    setValue(0);
}

qulonglong HexSpinBox::value() const
{
    return m_value;
};

qulonglong HexSpinBox::minimum() const
{
    return m_minimum;
};

void HexSpinBox::setMinimum(qulonglong min)
{
    m_minimum = min;
}

qulonglong HexSpinBox::maximum() const
{
    return m_maximum;
};

void HexSpinBox::setMaximum(qulonglong max, bool limitLength)
{
    m_maximum = max;
    if(m_value > m_maximum)
    {
        setValue(m_maximum);
    }
    if(limitLength)
    {
        /* if solicited (default) change maxTextLength*/
        QString maxTextValue = QString::number(max, 16).toUpper();
        m_maxTextLength = maxTextValue.length();
        /* reload value to have the correct number of zeroes */
        lineEdit()->setText(textFromValue(m_value));
    }
}

void HexSpinBox::setRange(qulonglong min, qulonglong max)
{
    setMinimum(min);
    setMaximum(max);
}

void HexSpinBox::stepBy(int steps)
{
    qulonglong value = m_value;
    if((steps < 0) && (value == 0))
    {
        value = 0;
    }
    else if((value + steps) > m_maximum)
    {
        value = m_maximum;
    }
    else
    {
        value += steps;
    }
    setValue(value);
}

qulonglong HexSpinBox::ullpow(qulonglong base, uint exp)
{
    qulonglong result = 1ULL;
    while(exp)
    {
        if (exp & 1)
        {
            result *= base;
        }
        exp >>= 1;
        base *= base;
    }
    return result;
}

QValidator::State HexSpinBox::validate(QString &input, int &pos) const
{
    UNUSED(pos);
    bool ok = false;
    qulonglong value = input.toULongLong(&ok, 16);
    if (!ok)
        return QValidator::Invalid;

    if (value < m_minimum || value > m_maximum)
        return QValidator::Invalid;

    return QValidator::Acceptable;
}

void HexSpinBox::fixup(QString &input) const
{
    bool ok = false;
    qulonglong value = input.toULongLong(&ok, 16);
    if (value < m_minimum)
        value = m_minimum;
    else if (value > m_maximum)
        value = m_maximum;
}

qulonglong HexSpinBox::valueFromText(const QString &text) const
{
    bool ok;
    return text.toULongLong(&ok, 16);
}

QString HexSpinBox::textFromValue(qulonglong value) const
{
    QString text, zeroes, textValue;
    textValue = QString::number((qulonglong)value, 16).toUpper();
    
    unsigned int numberOfZeroes = m_maxTextLength - textValue.length();
    if(numberOfZeroes > 0)
        while(numberOfZeroes--)
            zeroes += "0";
    
    text = QString("%1%2").arg(zeroes).arg(textValue);
    return text;
}

QAbstractSpinBox::StepEnabled HexSpinBox::stepEnabled() const
{
    return StepUpEnabled | StepDownEnabled;
}
    
void HexSpinBox::setValue(qulonglong value)
{
    if (m_value != value)
    {
        lineEdit()->setText(textFromValue(value));
        m_value = value;
        emit valueChanged(value);
    }
}

void HexSpinBox::interpretText()
{
    QString input = lineEdit()->text();
    int pos = 0;
    if (validate(input, pos) == QValidator::Acceptable)
        setValue(valueFromText(input));
    else
        lineEdit()->setText(textFromValue(m_value));
}