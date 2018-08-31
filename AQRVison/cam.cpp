﻿// Balser.cpp
#include "cam.h"

basler_cam::basler_cam()
{
    //最多5台相机
    m_c_maxCamerasToUse = 5;

    //2592*1944  3840 2748
//    m_cam_width = 3840;
//    m_cam_height = 2748;
    m_cam_width = 2592;
    m_cam_height = 1944;

    m_buffer_num = 20;
    for(int i = 0;i<m_buffer_num;i++)
    {
       pImageBuffer[i] = NULL;
    }

    m_cams = NULL;
    PylonInitialize();

}

basler_cam::~basler_cam()
{
    for(int i = 0;i< m_buffer_num ;i++)
    {
        if(pImageBuffer[i] != NULL)
        {
            delete pImageBuffer[i];
        }
        pImageBuffer[i] = NULL;
    }

    if(m_cams != NULL)
    {
        delete(m_cams);
        m_cams = NULL;
    }

    PylonTerminate();

}

//相机初始化
int basler_cam::init()
{
    try
    {
        CTlFactory& tlFactory = CTlFactory::GetInstance();
        //Check No Cam
        if ( tlFactory.EnumerateDevices(m_devices) == 0 )
        {
            //No Cam and Releases all pylon resources.
            PylonTerminate();
            return -1;
        }

        // Create an array of instant cameras for the found devices and avoid exceeding a maximum number of devices.
        m_cams = new CInstantCameraArray(min( m_devices.size(), m_c_maxCamerasToUse));
        // Create and attach all Pylon Devices.
        for ( size_t i = 0; i < m_cams->GetSize(); ++i)
        {
             (*m_cams)[i].Attach( tlFactory.CreateDevice(m_devices[i]));
             pImageBuffer[i] = new uint8_t[m_cam_width*m_cam_height];
            //Print the model name of the camera.
            //cout << "Using device " << cameras[ i ].GetDeviceInfo().GetModelName() << endl;
            //m_cams[i].GetTLNodeMap()
        }
    }
    catch (const GenericException &e)
    {
        // Error handling      
        cerr<<e.GetDescription();
        return -2;
    }

    return 0;
}

//？？未使用
int basler_cam::open()
{
    return 0;
}

//相机采集
int basler_cam::snap(int cam_index)
{
    //index 超过 cam num
    if(cam_index + 1 > (*m_cams).GetSize())
    {
        return -1;
    }

    try
    {
        CGrabResultPtr ptrGrabResult;
        int imageW = 0;
        int imageH = 0;

        for(int k = 0; k < 5;k++)
        {
            //Using GrabOne()  is more efficient if the pylon Device is already open,
            //otherwise the pylon Device is opened and closed automatically for each call.
           (*m_cams)[cam_index].StartGrabbing(1);
           // Camera.StopGrabbing() is called automatically by the RetrieveResult() method
           // when c_countOfImagesToGrab images have been retrieved.
           while ( (*m_cams)[cam_index].IsGrabbing())
           {   
               // Wait for an image and then retrieve it. A timeout of 2000 ms is used.
               (*m_cams)[cam_index].RetrieveResult( 2000, ptrGrabResult, TimeoutHandling_ThrowException);
           }
           // Image grabbed successfully?
           if (ptrGrabResult->GrabSucceeded())
           {
//               ptrGrabResult->GetWidth();
//               ptrGrabResult->GetHeight();
               break;
           }
           else
           {
               continue;
               //cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
           }
        }
        //循环采集五次，结束后再次判断是否成功，以防五次都采集失败
        if(!ptrGrabResult->GrabSucceeded())
            return -2;
        // Access the image data.
        imageW = ptrGrabResult->GetWidth();
        imageH = ptrGrabResult->GetHeight();
        if((imageW != m_cam_width) || (imageH != m_cam_height))
        {
            return -3;
        }

        (uint8_t *) ptrGrabResult->GetBuffer();
        memcpy(pImageBuffer[cam_index],ptrGrabResult->GetBuffer(), m_cam_width*m_cam_height*sizeof(uint8_t));

    }
    catch (const GenericException &e)
    {
        // Error handling.
        //cerr << "An exception occurred." << endl
        e.GetDescription();
        return -1;
    }

    return 0;
}

//相机关闭
int basler_cam::exit()
{
    for(int i = 0;i< m_buffer_num ;i++)
    {
        if(pImageBuffer[i] != NULL)
        {
            delete pImageBuffer[i];
        }
        pImageBuffer[i] = NULL;
    }

    if(m_cams != NULL)
    {
        delete(m_cams);
        m_cams = NULL;
    }

    PylonTerminate();

    return 0;
}



