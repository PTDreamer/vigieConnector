#include "ModbusFrame.h"

//Functions List

//01 (0x01) Read Coils
//02 (0x02) Read Discrete Inputs
//03 (0x03) Read Holding Registers
//04 (0x04) Read Input Registers
//05 (0x05) Write Single Coil
//06 (0x06) Write Single Register
//15 (0x0F) Write Multiple Coils
//16 (0x10) Write Multiple registers
//20 (0x14) Read File Record										(To Do)
//21 (0x15) Write File Record										(To Do)
//22 (0x16) Mask Write Register										(To Do)
//23 (0x17) Read/Write Multiple registers							(To test)
//24 (0x18) Read FIFO Queue											(To Do)
//43 (0x2B) Encapsulated Interface Transport						(To Do)



unsigned short ModbusFrame::m_staticTransId = 0;
QMap <int,QString> ModbusFrame::initfunctionMap()
{
    QMap <int,QString> functionMap;
    functionMap[1] = "Read Coils";
    functionMap[2] = "Read Discrete Inputs";
    functionMap[3] = "Read Holding Registers";
    functionMap[4] = "Read Input Registers";
    functionMap[5] = "Write Single Coil";
    functionMap[6] = "Write Single Register";
    functionMap[7] = "Read Exception Status";
    functionMap[8] = "Diagnostics";
    functionMap[11] = "Get Comm Event Counter";
    functionMap[12] = "Get Comm Event Log";
    functionMap[15] = "Write Multiple Coils";
    functionMap[16] = "Write Multiple registers";
    functionMap[17] = "Report Slave ID";
    functionMap[20] = "Read File Record";
    functionMap[21] = "Write File Record";
    functionMap[22] = "Mask Write Register";
    functionMap[23] = "Read/Write Multiple registers";
    functionMap[24] = "Read FIFO Queue";
    functionMap[43] = "Encapsulated Interface Transport";
    return functionMap;

}
QMap <int,QString> ModbusFrame::functionMap = initfunctionMap();
ModbusFrame::ModbusFrame()
{
    ;
}
ModbusFrame::ModbusFrame(QByteArray frame)
{
    m_frame = frame;
    m_transId = m_frame.mid(0,2).toHex().toUShort(0,16);
    m_protID = m_frame.mid(2,2).toHex().toUShort(0,16);
    m_length = m_frame.mid(4,2).toHex().toUShort(0,16);
    m_unitId = m_frame.mid(6,1).toHex().toUShort(0,16);
    m_functionCode = m_frame.mid(7,1).toHex().toUShort(0,16);
    m_referenceNumber = m_frame.mid(8,2).toHex().toUShort(0,16);
    m_wordCount = m_frame.mid(10,2).toHex().toUShort(0,16);

    if (functionMap.find(m_functionCode) == functionMap.end())
    {
        this->setExceptionCode(ModbusFrame::IllegalFunction);
    }
}

ModbusFrame::ModbusFrame(unsigned short unitId, unsigned short functionCode)
{
    m_transId = m_staticTransId++;
    m_length = 0;
    m_unitId = unitId;
    m_functionCode = functionCode;

    QDataStream out(&m_frame,QIODevice::WriteOnly);

    out << (qint16) m_staticTransId
            << (qint16) 0
            << (qint16) 0
            << (qint8) m_unitId
            << (qint8) functionCode;

    this->setLength();
}
ModbusFrame::ModbusFrame(unsigned short unitId, unsigned short functionCode, unsigned short exceptionCode)
{
    m_transId = m_staticTransId++;
    m_length = 0;
    m_unitId = unitId;
    m_functionCode = functionCode;
    m_exceptionCode = exceptionCode;

    QDataStream out(&m_frame,QIODevice::WriteOnly);

    out << (qint16) m_staticTransId
            << (qint16) 0
            << (qint16) 0
            << (qint8) m_unitId
            << (qint8) functionCode
            << (qint8) exceptionCode;

    this->setLength();
}
ModbusFrame::ModbusFrame(unsigned short unitId, unsigned short functionCode, unsigned short referenceNumber, unsigned short wordCount)
{
    m_transId = m_staticTransId++;
    m_length = 0;
    m_unitId = unitId;
    m_functionCode = functionCode;
    m_referenceNumber = referenceNumber;
    m_wordCount = wordCount;

    QDataStream out(&m_frame,QIODevice::WriteOnly);

    out << (qint16) m_staticTransId
            << (qint16) 0
            << (qint16) 0
            << (qint8) m_unitId
            << (qint8) functionCode
            << (qint16) referenceNumber
            << (qint16) wordCount;

    this->setLength();
}
ModbusFrame::ModbusFrame(const ModbusFrame &copy)
{
    m_transId = copy.m_exceptionCode;
    m_length = copy.m_length;
    m_functionCode = copy.m_functionCode;
    m_unitId = copy.m_unitId;
    m_referenceNumber = copy.m_referenceNumber;
    m_wordCount = copy.m_wordCount;
    m_frame = copy.m_frame;
    m_exceptionCode = copy.m_exceptionCode;
}
void ModbusFrame::copyMobdbusHeader(const ModbusFrame &copy)
{
    QDataStream out(&this->m_frame,QIODevice::WriteOnly);
    this->m_transId = copy.m_transId;
    this->m_unitId = copy.m_unitId;
    this->m_functionCode = copy.m_functionCode;
    out << (qint16) copy.m_transId
            << (qint16) 0
            << (qint16) 0
            << (qint8) copy.m_unitId
            << (qint8) copy.m_functionCode;

    this->setLength();
}

QString ModbusFrame::toText()
{
    QString text;

    text = this->checkProtId() ? "Valid " : "Invalid ";
    text += "Modbus Protocol Identifier \n";
    text += "Function : " + this->function() + "\n";
    text += "Transaction Identifier : " + QString::number(this->getTransId()) + "\n";
    text += "Length : " + QString::number(this->getLength()) + "\n";
    text += "Unit Identifier : " + QString::number(this->getUnitId()) + "\n";
    text += "Function Code : " + QString::number(this->getFunctionCode()) + "\n";
    text += "Reference Number : " + QString::number(this->getReferenceNumber()) + "\n";
    text += "Word Count : " + QString::number(this->getWordCount()) + "\n";
    text += "Raw Frame : " + this->toQString() + "\n";
    text += "Raw Data : " + this->getData().toHex().toUpper() + "\n";

    return text;
}
QString ModbusFrame::toQString()
{
    return QString(m_frame.toHex().toUpper());
}
QString ModbusFrame::function()
{
    if (functionMap.find(m_functionCode) != functionMap.end())
        return functionMap.find(m_functionCode).value();
    else return QString("Function unknown or not supported");
}

bool ModbusFrame::checkProtId()
{
    return m_protID == 0 ? true : false;
}
void ModbusFrame::setTransId(unsigned short transId)
{
    m_transId= transId;
    QDataStream out(&m_frame,QIODevice::WriteOnly);
    out.device()->seek(0);
    out << (qint16) m_transId;

    this->setLength();
}
void ModbusFrame::setLength(unsigned short length)
{
    QDataStream out(&m_frame,QIODevice::WriteOnly);
    out.device()->seek(4);    //
    out << (quint16) length;//
}
void ModbusFrame::setLength()
{
    QDataStream out(&m_frame,QIODevice::WriteOnly);
    out.device()->seek(4);    //
    m_length = m_frame.size() - 6; //   Length
    out << (quint16) m_length;//
}
void ModbusFrame::setExceptionCode(unsigned short exceptionCode)
{
    m_exceptionCode = exceptionCode;
    m_frame.remove(9,m_frame.size()-9);
    QDataStream out(&m_frame,QIODevice::WriteOnly);
    out.device()->seek(7);
    out << (qint8) (m_functionCode+0x80);
    out << (qint8) exceptionCode;

    this->setLength();
}
void ModbusFrame::setFunctionCode(unsigned short functionCode)
{
    m_functionCode = functionCode;
    QDataStream out(&m_frame,QIODevice::WriteOnly);
    out.device()->seek(7);
    out << (qint16) functionCode;

    this->setLength();
}
void ModbusFrame::setReferenceNumber(unsigned short referenceNumber)
{
    m_referenceNumber = referenceNumber;

    QDataStream out(&m_frame,QIODevice::WriteOnly);
    out.device()->seek(8);
    out << (qint16) referenceNumber;

    this->setLength();
}
void ModbusFrame::setWordCount(unsigned short wordCount)
{
    m_wordCount = wordCount;

    QDataStream out(&m_frame,QIODevice::WriteOnly);
    out.device()->seek(10);
    out << (qint16) wordCount;

    this->setLength();
}
void ModbusFrame::setData(const QByteArray &data)
{
    QDataStream out(&m_frame,QIODevice::WriteOnly);
    out.device()->seek(this->getSeekNumber(m_functionCode));
    out << data;

    this->setLength();
}
void ModbusFrame::setData(const QVector <QVariant> &data)
{
    this->setData(data.toList());
}
void ModbusFrame::setData(const QVariantList &data)
{
    QByteArray m_data;
    QDataStream out(&m_data, QIODevice::WriteOnly);

    qint8 octet = 0;
    int compt = 0;

    foreach (QVariant variant, data)
    {
        switch (variant.type())
        {
        case QVariant::Bool : // test needed
            octet += variant.toBool()*(1<<compt);
            if (compt > 7)
            {
                compt=0;
                out<<octet;
            }
            else compt++;
            break;

			case QVariant::Int :
            out << (qint32) variant.toInt();
            break;

			case QVariant::UInt :
            out << (quint32) variant.toUInt();
            break;

			case QVariant::Double :
            out.setFloatingPointPrecision(QDataStream::DoublePrecision);
            out << (double) variant.toDouble();
            break;
        default:
            break;
        }
    }
    int seekNumber = this->getSeekNumber(m_functionCode)-1; // ByteCount Index
    m_frame.remove(seekNumber,m_frame.size());  // Delete all data including ByteCount
    m_frame.append((quint8) m_data.length()); // Append the new ByteCount
    m_frame.append(m_data);

    this->setLength();
}
void ModbusFrame::removeData()
{
    int seekNumber = this->getSeekNumber(m_functionCode)-1; // ByteCount Index
    m_frame.remove(seekNumber,m_frame.size());  // Delete all data including ByteCount
    this->setLength();
}
QVector <QVariant> ModbusFrame::getData(QMetaType::Type dataType)
{
    QDataStream in(&m_frame,QIODevice::ReadOnly);
    QVector <QVariant> vVar;
    int wordCount = m_wordCount;
    if (m_functionCode == ReadWriteMultipleRegisters) 
        wordCount = this->getWordCount(2);

    in.device()->seek(this->getSeekNumber(m_functionCode));

    switch (dataType)
    {
    case QMetaType::Bool :
        qint8 i8;
        bool boolean;
        int  param;
        for (int i = 0 ; wordCount > 0 ; i++)
        {
            in >> i8;
            param = wordCount/8 > 0 ? 8 : wordCount%8;

            for( int j = 0 ; j < param ; j++)
            {
                boolean = i8 & (1<<j);
                vVar.append(boolean);
            }
            wordCount-=8;
        }
        break;

        case QMetaType::UShort :
        qint16 i16;
        while(!in.atEnd())
        {
            in >> i16;
            vVar.append(QVariant(i16));
        }
        break;

		case QMetaType::Float : // non testé
                    qreal real;
                    while(!in.atEnd())
                    {
                        in >> real;
                        vVar.append(QVariant(real));
                    }
                    break;
    default:
        break;
                }
    return vVar;
}

QVariantList ModbusFrame::getData(QList <QMetaType::Type> &dataTypes)
{
    QDataStream in(&m_frame,QIODevice::ReadOnly);
    QVariantList values;

    in.device()->seek(this->getSeekNumber(m_functionCode));

    foreach (QMetaType::Type dataType, dataTypes)
    {
        switch (dataType)
        {
        case QMetaType::Short : // non testé
        case QMetaType::UShort :
            qint16 i16;
            in >> i16;
            values.append(QVariant(i16));
            break;

        case QMetaType::Int : // non testé
        case QMetaType::UInt :
            qint32 i32;
            in >> i32;
            values.append(QVariant(i32));
            break;

        case QMetaType::Float : // non testé
            qreal real;
            in >> real;
            values.append(QVariant(real));
            break;
        default:
            break;
        }
    }
    return values;
}

QByteArray ModbusFrame::getData()
{
    return m_frame.mid(this->getSeekNumber(m_functionCode));
}
QByteArray ModbusFrame::toQByteArray()
{
    return m_frame;
}

std::string ModbusFrame::toStdString()
{
    return (this->toQString()).toStdString();
}
unsigned short ModbusFrame::getTransId()
{
    return m_transId;
}
unsigned short ModbusFrame::getLength()
{
    return m_length;
}
unsigned short ModbusFrame::getFunctionCode()
{
    return m_functionCode;
}
unsigned short ModbusFrame::getUnitId()
{
    return m_unitId;
}
unsigned short ModbusFrame::getReferenceNumber()
{
    return m_referenceNumber;
}
unsigned short ModbusFrame::getReferenceNumber(int firstOrSecondReferenceNumber)
{
    if (firstOrSecondReferenceNumber == 2)
        return m_frame.mid(12,2).toHex().toUShort(0,16);
    else
        return m_referenceNumber;
}
unsigned short ModbusFrame::getWordCount()
{
    return m_wordCount;
}
unsigned short ModbusFrame::getWordCount(int firstOrSecondWordCount)
{
    if (firstOrSecondWordCount == 2)
        return m_frame.mid(14,2).toHex().toUShort(0,16);
    else return m_wordCount;
}

unsigned short ModbusFrame::getSeekNumber(unsigned short functionCode)
{
    short seekNumber;
    switch (functionCode)
    {
    case ModbusFrame::WriteMultipleCoils : // Request
    case ModbusFrame::WriteMultipleRegisters :	// Request
        seekNumber = 13;
        break;

    case ModbusFrame::ReadWriteMultipleRegisters : // Request
        seekNumber = 17;
        break;

    default :
            seekNumber = 9;
}
    return seekNumber;
}
size_t ModbusFrame::size()
{
    return m_frame.size();
}
QList <ModbusFrame> ModbusFrame::tcpMbFrameParser(QByteArray tcpFrame)
{
    QList <ModbusFrame> modbusFrameList;
    while(tcpFrame.mid(4,2).toHex().toUShort(0,16)+6  <= tcpFrame.size() )
    {
        QByteArray requestMb = tcpFrame.mid(0,tcpFrame.mid(4,2).toHex().toUShort(0,16)+6);
        ModbusFrame frame(requestMb);
        modbusFrameList.append(frame);
        tcpFrame.remove(0,tcpFrame.mid(4,2).toHex().toUShort(0,16)+6);
    }
    return modbusFrameList;
}
