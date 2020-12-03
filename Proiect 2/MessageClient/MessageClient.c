// MessageClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// communication library
#include "communication_api.h"

#include <windows.h>
typedef struct _Error {
    int CodeA;
    CM_ERROR CodeB;
} Error;

#define SUCCESS(err) (err.CodeA == 0 && err.CodeB == 0)

Error CreateError(int CodeA, CM_ERROR CodeB)
{
    Error error;
    error.CodeA = CodeA;
    error.CodeB = CodeB;
    return error;
}
const Error SUCCESS_ERROR = { .CodeA = 0, .CodeB = 0 };

Error Send(CM_CLIENT* client, void* data, SIZE_T data_size)
{
    CM_DATA_BUFFER* dataToSend = NULL;
    CM_SIZE dataToSendSize = (CM_SIZE)data_size;

    CM_ERROR error = CreateDataBuffer(&dataToSend, dataToSendSize);
    if (CM_IS_ERROR(error))
    {
        Error customError = CreateError(1001, error);
        return customError;
    }

    error = CopyDataIntoBuffer(dataToSend, (const CM_BYTE*)data, (CM_SIZE)data_size);
    if (CM_IS_ERROR(error))
    {
        Error customError = CreateError(1002, error);
        DestroyDataBuffer(dataToSend);
        return customError;
    }

    CM_SIZE sendByteCount = 0;
    error = SendDataToServer(client, dataToSend, &sendByteCount);
    if (CM_IS_ERROR(error))
    {
        Error customError = CreateError(1003, error);
        DestroyDataBuffer(dataToSend);
        return customError;
    }
    return SUCCESS_ERROR;
}

Error Receive(CM_CLIENT* client, void** data, CM_SIZE data_size)
{

    CM_DATA_BUFFER* dataToReceive = NULL;
    CM_SIZE dataToReceiveSize = data_size;

    CM_ERROR error = CreateDataBuffer(&dataToReceive, dataToReceiveSize);
    if (CM_IS_ERROR(error))
    {
        Error customError = CreateError(2001, error);
        return customError;
    }

    CM_SIZE receivedByteCount = 0;
    error = ReceiveDataFormServer(client, dataToReceive, &receivedByteCount);
    if (CM_IS_ERROR(error))
    {
        Error customError = CreateError(2002, error);
        DestroyDataBuffer(dataToReceive);
        return customError;
    }
    *data = dataToReceive;

    return SUCCESS_ERROR;
}

Error GetNextSize(CM_CLIENT* client, CM_SIZE* size)
{
    CM_DATA_BUFFER* receivedData;
    Error error = Receive(client, &receivedData, sizeof(CM_SIZE));
    if (!SUCCESS(error))
    {
        return error;
    }
    *size = (CM_SIZE)*receivedData->DataBuffer;
    DestroyDataBuffer(receivedData);
    return error;
}

Error ReceiveFile(CM_CLIENT* client)
{
    Error error;
    CM_SIZE size = 0;
    CM_DATA_BUFFER* data = NULL;
    error = GetNextSize(client, &size);
    if (!SUCCESS(error))
    {
        //PrintError(error);
        return error;
    }
    error = Receive(client, &data, size);
    if (!SUCCESS(error))
    {
        //PrintError(error);
        return error;
    }
    char FileName[256] = "";
    strcpy_s(FileName, 255, (char*)data->DataBuffer);
    FILE* file = fopen(FileName, "wb");
    while (size != sizeof(int) || *(int*)data->DataBuffer != -1)
    {
        error = GetNextSize(client, &size);
        if (!SUCCESS(error))
        {
            //PrintError(error);
            fclose(file);
            return error;
        }
        error = Receive(client, &data, size);
        if (!SUCCESS(error))
        {
            //PrintError(error);
            fclose(file);
            return error;
        }
        if (size == sizeof(int) && *(int*)data->DataBuffer == -1)
        {
            break;
        }
        fwrite(data->DataBuffer, size, 1, file);
    }
    fclose(file);
    return SUCCESS_ERROR;
}

DWORD WINAPI ReceiveThread(PVOID clientP)
{
    CM_CLIENT* client = (CM_CLIENT*)clientP;
    CM_SIZE size = 0;
    CM_DATA_BUFFER* data = NULL;
    Error error;
    while (TRUE)
    {
        error = GetNextSize(client, &size);
        if (!SUCCESS(error))
        {
            //PrintError(error);
            break;
        }
        error = Receive(client, &data, size);
        if (!SUCCESS(error))
        {
            //PrintError(error);
            break;
        }
        if (size == sizeof(int) && *(int*)data->DataBuffer == -1)
        {
            ReceiveFile(client);
        }
        _tprintf_s(TEXT("%S\n"), (char*) data->DataBuffer);
        //ProcessCommand(clientInfo, (char*)data->DataBuffer);
        DestroyDataBuffer(data);
    }
    return 0;
}

int _tmain(int argc, TCHAR* argv[])
{
    (void)argc;
    (void)argv;

    EnableCommunicationModuleLogger();

    CM_ERROR error = InitCommunicationModule();
    if (CM_IS_ERROR(error))
    {
        _tprintf_s(TEXT("Unexpected error: InitCommunicationModule failed with err-code=0x%X!\n"), error);
        return -1;
    }

    CM_CLIENT* client = NULL;
    error = CreateClientConnectionToServer(&client);
    if (CM_IS_ERROR(error) && error == CM_NO_RESOURCES_FOR_SERVER_CLIENT)
    {
        _tprintf_s(TEXT("Error: no running server found\n"));
        UninitCommunicationModule();
        return -1;
    }
    CM_SIZE SPACE_AVAILABLE;

    Error customError = GetNextSize(client, &SPACE_AVAILABLE);
    if (!SPACE_AVAILABLE)
    {
        _tprintf_s(TEXT("Error: maximum concurrent connection count reached\n"));
        DestroyClient(client);
        UninitCommunicationModule();
        return -1;
    }

    _tprintf_s(TEXT("Successful connection\n"));
    CM_SIZE size = (CM_SIZE) 255 * sizeof(char);
    char command[256];
    DWORD threadId;
    HANDLE* threadHandle = CreateThread(NULL, 0, ReceiveThread, client, 0, &threadId);

    while (TRUE)
    {
        fgets(command, 255, stdin);
        customError = Send(client, &size, sizeof(CM_SIZE));
        if (!SUCCESS(customError))
        {
            break;
        }
        customError = Send(client, command, size);
        if (!SUCCESS(customError))
        {
            break;
        }
    }

    CloseHandle(threadHandle);

    _tprintf_s(TEXT("Client is shutting down now...\n"));
    DestroyClient(client);
    UninitCommunicationModule();

    return 0;
}

