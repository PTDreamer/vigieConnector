#ifndef MODBUSFRAME_H
#define MODBUSFRAME_H

#include <QtCore>

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

class ModbusFrame
{
public :
        enum FunctionCode
{
    ReadCoils						= 1,
    ReadDiscreteInputs				= 2,
    ReadHoldingRegisters			= 3,
    ReadInputRegisters				= 4,
    WriteSingleCoil					= 5,
    WriteSingleRegister				= 6,
    WriteMultipleCoils				= 15,
    WriteMultipleRegisters			= 16,
    ReadFileRecord					= 20,
    WriteFileRecord					= 21,
    MaskWriteRegister				= 22,
    ReadWriteMultipleRegisters		= 23,
    ReadFIFOQueue					= 24,
    EncapsulatedInterfaceTransport	= 43
};

enum ExceptionCode
{
    IllegalFunction						= 1,
    IllegalDataAddress					= 2,
    IllegalDataValue					= 3,
    SlaveDeviceFailure					= 4,
    Acknowledge							= 5,
    SlaveDeviceBusy						= 6,
    MemoryParityError					= 7,
    GatewayPathUnavailable				= 10,
    GatewayTargetDeviceFailedToRespond	= 11
                                      };

ModbusFrame();
ModbusFrame(const QByteArray frame);
ModbusFrame(unsigned short unitId, unsigned short functionCode);
ModbusFrame(unsigned short unitId, unsigned short functionCode, unsigned short exceptionCode);
ModbusFrame(unsigned short unitId, unsigned short functionCode, unsigned short referenceNumber, unsigned short wordCount);
ModbusFrame(const ModbusFrame &copy);

void copyMobdbusHeader(const ModbusFrame &copy);

QByteArray toQByteArray();
QString toQString();
QString toText();
QString function();
QString typeOfFrameString();
std::string toStdString();

bool checkProtId();
unsigned short getTransId();
unsigned short getLength();
unsigned short getFunctionCode();
unsigned short getUnitId();
unsigned short getReferenceNumber();
unsigned short getReferenceNumber(int firstOrSecondReferenceNumber); // Dû à la fonction spéciale 23 (0x17) Read/Write Multiple registers
unsigned short getWordCount();
unsigned short getWordCount(int firstOrSecondWordCount);// Dû à la fonction spéciale 23 (0x17) Read/Write Multiple registers
QVector <QVariant> getData(QMetaType::Type dataType);
QVariantList getData(QList <QMetaType::Type> &dataTypes);
QByteArray getData();
void removeData();
void setTransId(unsigned short transId);
void setFunctionCode(unsigned short functionCode);
void setReferenceNumber(unsigned short referenceNumber);
void setWordCount(unsigned short wordCount);
void setExceptionCode(unsigned short exceptionCode);
void setLength(unsigned short length);
void setLength();
void setData(const QByteArray &data);
void setData(const QVariantList &data);
void setData(const QVector <QVariant> &data);
void appendData(const QVariant &data);

size_t size();
static QList <ModbusFrame> tcpMbFrameParser(QByteArray tcpFrame);

private:
unsigned short getSeekNumber(unsigned short functionCode);

// Static Attributes
static QMap <int,QString> initfunctionMap();
static QMap <int,QString> functionMap;

// Attributes
QByteArray m_frame;

static unsigned short m_staticTransId;

unsigned short m_transId;
unsigned short m_protID;
unsigned short m_length;
unsigned short m_functionCode;
unsigned short m_unitId;
unsigned short m_referenceNumber;
unsigned short m_wordCount;
unsigned short m_exceptionCode;
};

#endif
