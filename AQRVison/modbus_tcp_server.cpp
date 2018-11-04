﻿#include "modbus_tcp_server.h"
#include <QModbusTcpServer>
#include <QRegularExpression>
#include <QUrl>
#include <QDebug>

//究极转换,从float转成quint16
enum byteOrder{
        ABCD,
        BADC,
        CDAB,
        DCBA
    };

quint16 fromCharArray(const unsigned *data)
{
    uintptr_t value(0);
    for(int i(0); i < 2; ++i){
        value += data[i] << 8 *i;
    }
    return value;
}

QVector<quint16> fromFloat(float abcd, byteOrder order)
{
    short A(0),B(0),C(0),D(0);
    switch (order) {
    case ABCD:A = 0; B = 1; C = 2; D = 3;break;
    case BADC:A = 1; B = 0; C = 3; D = 2;break;
    case CDAB:A = 2; B = 3; C = 0; D = 1;break;
    case DCBA:A = 3; B = 2; C = 1; D = 0;break;
    }

    unsigned char *cArray = reinterpret_cast<unsigned char *>(&abcd);
    unsigned value1[] = {cArray[A],cArray[B]};
    unsigned value2[] = {cArray[C],cArray[D]};

    QVector<quint16> values;
    values.append(fromCharArray(value1));
    values.append(fromCharArray(value2));

    return values;
}




bool modbus_tcp_server::main_thread_quit=false;

modbus_tcp_server::modbus_tcp_server()
    :modbusDevice(nullptr)
{
    qRegisterMetaType<QModbusDataUnit>("QModbusDataUnit");
    qRegisterMetaType<QModbusDataUnit::RegisterType>("QModbusDataUnit::RegisterType");
    qRegisterMetaType<QModbusDevice::State>("QModbusDevice::State");
    qRegisterMetaType<QModbusTcpServer*>("QModbusTcpServer*");
    get_settings();
    ini_modbus_server();
}

modbus_tcp_server::~modbus_tcp_server()
{
    if (modbusDevice)
        modbusDevice->disconnectDevice();
    delete modbusDevice;
    modbusDevice=nullptr;
}

int modbus_tcp_server::get_settings()
{
    //!!此处应该在设置界面添加LineEdit，显示port号
    //读取连接参数
    m_ini_reader.read("TCP_Param","port",port);
    m_ini_reader.read("TCP_Param","server_address",server_address);
    //注册写入参数
    QModbusDataUnit data_1(QModbusDataUnit::HoldingRegisters,0,10);
    write_data=data_1;
    //注册读取参数
    QModbusDataUnit data_2(QModbusDataUnit::HoldingRegisters,20,10);
    read_data=data_2;

    return 0;
}

int modbus_tcp_server::ini_modbus_server()
{
    if(modbusDevice)
    {
        modbusDevice->disconnect();
        delete modbusDevice;
        modbusDevice=nullptr;
    }

    modbusDevice=new QModbusTcpServer(this);
    if(!modbusDevice)
    {
        //!!此处在主界面添加连接按钮,并且设置使能信号
        //ui->connectbutton->setEsnable(true);
        return -1;
        qDebug()<<"Could not create Modbus server.";
    }
    else
    {
        //初始化modbus类，设置各项参数
        QModbusDataUnitMap reg;
        reg.insert(QModbusDataUnit::Coils, { QModbusDataUnit::Coils, 0, 10 });
        reg.insert(QModbusDataUnit::DiscreteInputs, { QModbusDataUnit::DiscreteInputs, 0, 10 });
        reg.insert(QModbusDataUnit::InputRegisters, { QModbusDataUnit::InputRegisters, 0, 10 });
        reg.insert(QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, 0, 50 });

        modbusDevice->setMap(reg);

        connect(modbusDevice, &QModbusServer::stateChanged,
                this, &modbus_tcp_server::onStateChanged);
        connect(modbusDevice, &QModbusServer::errorOccurred,
                this, &modbus_tcp_server::handleDeviceError);
        connect(modbusDevice, &QModbusServer::dataWritten,
                this, &modbus_tcp_server::updateWidgets);//连接数据读取
    }
    return 0;
}

//连接状态变更处理
void modbus_tcp_server::onStateChanged(int state)
{
    if(main_thread_quit)
        return;//主线程准备退出，因为析构函数为先调用自身再调用成员变量，则应在析构之前停止发送
    if (state == QModbusDevice::UnconnectedState)
        emit signal_connect_button_status(false);
    else if (state == QModbusDevice::ConnectedState)
        emit signal_connect_button_status(true);
}

void modbus_tcp_server::handleDeviceError(QModbusDevice::Error newError)
{
    if (newError == QModbusDevice::NoError || !modbusDevice)
        return;
}

//接收数据
void modbus_tcp_server::updateWidgets(QModbusDataUnit::RegisterType table, int address, int size)
{
    //非使能信号过滤
    if(address<20)
        return;
    modbusDevice->data(&read_data);
    float screwdriver=quint_to_float(read_data.value(0),read_data.value(1));
    float screw=quint_to_float(read_data.value(2),read_data.value(3));
    float enable=quint_to_float(read_data.value(4),read_data.value(5));
    float receive=quint_to_float(read_data.value(6),read_data.value(7));
    float reserve=quint_to_float(read_data.value(8),read_data.value(9));
    if(receive==1.0)
        setupDeviceData(0,0,0,0,0);
    //触发检测函数
    emit signal_read_data(screwdriver,screw,enable,receive,reserve);

}

//接收数据转换
float modbus_tcp_server::quint_to_float(quint16 AB,quint16 CD)
{
    float data;
    int high_bit=CD;
    int low_bit=AB;
    int whole;
    whole=high_bit*pow(0x10,4)+low_bit;
    data=*((float*)&whole);//内存格式转换

    return data;
}

//发送数据
int modbus_tcp_server::setupDeviceData(float x_coor,float y_coor,\
                                       float complete,float heartbeat,float reserve)
{
    //heartbeat信息由单独线程发送
    if(!modbusDevice)
        return -1;
    //数据转换
    float_to_quint(x_coor,0);
    float_to_quint(y_coor,2);
    float_to_quint(complete,4);
    //数据发送
    modbusDevice->setData(write_data);

    return 0;
}

//待发送数据转换
int modbus_tcp_server::float_to_quint(float data,quint16 start_address)
{
    QVector<quint16> values=fromFloat(data,ABCD);
    write_data.setValue(start_address+1,values[1]);
    write_data.setValue(start_address,values[0]);

    return 0;
}

//连接服务器
int modbus_tcp_server::connection(bool connect_flag)
{
    bool intendToConnect = (modbusDevice->state() == QModbusDevice::UnconnectedState);
    if(intendToConnect&&connect_flag)
    {
        //创建连接
        const QUrl url=QUrl::fromUserInput(port);
        //set url parameters
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter,url.port());
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter,url.host());
        //set server address
        modbusDevice->setServerAddress(server_address);
        if(!modbusDevice->connectDevice())
        {
            //connect failed and show messages
            return -1;
        }
        else
        {
            //connect succeed and show messages
            return 0;
        }
    }
    else if(!intendToConnect&&!connect_flag)
    {
        //断开连接
        modbusDevice->disconnectDevice();
    }
    else
    {
        return 0;
    }
    return 0;
}

//发送心跳连接(在MainWindow类中moveToThread)
void modbus_tcp_server::send_heartbeat_message()
{
    if(!modbusDevice)
        return;//连接失效

    //发送心跳信号
    if(heartbeat_flag)
    {
        modbusDevice->setData(QModbusDataUnit::HoldingRegisters,7,0);

    }
    else
    {
        modbusDevice->setData(QModbusDataUnit::HoldingRegisters,7,0x3f80);//ox3f80 0000 为浮点数1.0
    }
    modbusDevice->setData(QModbusDataUnit::HoldingRegisters,6,0);
    heartbeat_flag=!heartbeat_flag;
}
