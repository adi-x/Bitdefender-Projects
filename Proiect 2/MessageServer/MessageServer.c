// MessageServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// communication library
#include "communication_api.h"
#include <windows.h>

typedef struct ClientThreadInformation {
    CM_SERVER* server;
    CM_SERVER_CLIENT* client;
    void* info;
    char Username[256];
    int isLoggedIn;
} ClientThreadInformation;

typedef struct CM_ServerInformation {
    HANDLE* threadHandles;
    DWORD* threadIds;
    ClientThreadInformation** clientInfo;
    int threadCount;
} CM_ServerInformation;

#define SUCCESS(err) (err.CodeA == 0 && err.CodeB == 0)

// Error Codes:
// 100X - errors related to Sending
typedef struct _Error {
    int CodeA;
    CM_ERROR CodeB;
} Error;

Error CreateError(int CodeA, CM_ERROR CodeB)
{
    Error error;
    error.CodeA = CodeA;
    error.CodeB = CodeB;
    return error;
}
const Error SUCCESS_ERROR = { .CodeA = 0, .CodeB = 0 };
BOOL StringIsInt(TCHAR* string)
{
    int characterIndex = 0;
    while (string[characterIndex])
    {
        if (string[characterIndex] < '0' || string[characterIndex] > '9')
            return FALSE;
        characterIndex++;
    }
    return TRUE;
}

BOOL StringIsIntClassic(char* string)
{
    int characterIndex = 0;
    while (string[characterIndex])
    {
        if (string[characterIndex] < '0' || string[characterIndex] > '9')
            return FALSE;
        characterIndex++;
    }
    return TRUE;
}

void PrintError(Error error)
{
    int Code = error.CodeA;
    switch (Code)
    {
        // "Send" errors
    case 1001:
        _tprintf_s(TEXT("Unexpected error: Failed to create SEND data buffer with err-code=0x%X!\n"), error.CodeB);
        break;
    case 1002:
        _tprintf_s(TEXT("CopyDataIntoBuffer failed with err-code=0x%X!\n"), error.CodeB);
        break;
    case 1003:
        _tprintf_s(TEXT("SendDataToClient failed with err-code=0x%X!\n"), error.CodeB);
        break;
        // "Receive" errors
    case 2001:
        _tprintf_s(TEXT("Failed to create RECEIVE data buffer with err-code=0x%X!\n"), error.CodeB);
        break;
    case 2002:
        _tprintf_s(TEXT("ReceiveDataFromClient failed with err-code=0x%X!\n"), error.CodeB);
        break;
    }
}

Error Send(CM_SERVER_CLIENT* client, void* data, CM_SIZE data_size)
{
    CM_DATA_BUFFER* dataToSend = NULL;
    CM_SIZE dataToSendSize = data_size;

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
    error = SendDataToClient(client, dataToSend, &sendByteCount);
    if (CM_IS_ERROR(error))
    {
        Error customError = CreateError(1003, error);
        DestroyDataBuffer(dataToSend);
        return customError;
    }
    return SUCCESS_ERROR;
}

Error Receive(CM_SERVER_CLIENT* client, void** data, CM_SIZE data_size)
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
    error = ReceiveDataFromClient(client, dataToReceive, &receivedByteCount);
    if (CM_IS_ERROR(error))
    {
        Error customError = CreateError(2002, error);
        DestroyDataBuffer(dataToReceive);
        return customError;
    }
    *data = dataToReceive;

    return SUCCESS_ERROR;
}

Error GetNextSize(CM_SERVER_CLIENT* client, CM_SIZE *size)
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

void CleanUpClient(ClientThreadInformation* clientInfo)
{
    CM_ServerInformation* info = (CM_ServerInformation*)clientInfo->info;
    HANDLE* th = NULL;
    for (int i = 0; i < info->threadCount; i++)
    {
        if (info->clientInfo[i]->client == clientInfo->client)
        {
            th = info->threadHandles[i];
            for (int j = i; j < info->threadCount - 1; j++)
            {
                info->threadHandles[j] = info->threadHandles[j + 1];
                info->clientInfo[j] = info->clientInfo[j + 1];
                info->threadIds[j] = info->threadIds[j + 1];
            }
            info->threadCount--;
            break;
        }
    }
    AbandonClient(clientInfo->client);
    free(clientInfo);
    CloseHandle(th);
}

Error RegisterUser(char* username, char* password) // 100-199 errors related to register
{
    // making sure the username and password are correct
    int i;
    for (i = 0; username[i] != '\0'; i++)
    {
        if ((username[i] < 'a' || username[i] > 'z') && (username[i] < 'A' || username[i] > 'Z') && (username[i] < '0' || username[i] > '9'))
            break;
    }
    if (username[i] != '\0')
        return CreateError(101, 0); // username is not valid error
    int bigLetterCount = 0;
    int symbolCount = 0;
    for (i = 0; password[i] != '\0'; i++)
    {
        if (password[i] == ' ' || password[i] == ',')
            break;

        if ((password[i] < 'a' || password[i] > 'z') && (password[i] < 'A' || password[i] > 'Z') && (password[i] < '0' || password[i] > '9'))
            symbolCount++;

        if (password[i] >= 'A' && password[i] <= 'Z')
            bigLetterCount++;
    }
    if (password[i] != '\0')
        return CreateError(111, 0); // password contains spaces / comma
    if (i < 5)
        return CreateError(112, 0); // password is too short
    if (bigLetterCount == 0 || symbolCount == 0)
        return CreateError(113, 0); // password doesn't contain any big letter / symbol
    FILE* file = fopen("C:/registration.txt", "a+");
    if (file == NULL)
        return CreateError(131, 0); // couldnt open file
    char Line[256] = "";
    char* next;
    char* usernameFromFile;
    int fileContainsUsername = 0;
    while (fgets(Line, 255, file))
    {
        usernameFromFile = strtok_s(Line, ",", &next);
        if (strcmp(usernameFromFile, username) == 0)
        {
            fileContainsUsername = 1;
            break;
        }
    }
    if (fileContainsUsername)
    {
        fclose(file);
        return CreateError(121, 0); // username exists in file
    }
    char NewLine[256] = "";
    sprintf_s(NewLine, 255, "%s,%s\n", username, password);
    fputs(NewLine, file);
    fclose(file);
    return SUCCESS_ERROR;
}

Error SendLastLinesFromFile(char* PathToFile, ClientThreadInformation* clientInfo, int LineCount)
{
    LineCount++;
    char** TemporaryStorage = malloc(sizeof(char*) * LineCount);
    for (int i = 0; i < LineCount; i++)
        TemporaryStorage[i] = malloc(sizeof(char) * 500);
    int Index = 0;
    int Count = 0;
    FILE* file = fopen(PathToFile, "a+");
    if (file == NULL)
        return CreateError(131, 0); // nothing to retrieve..
    char Line[500];
    while (fgets(Line, 500, file))
    {
        if (Line[0] != '\n')
        {
            strcpy_s(TemporaryStorage[Index], 500, Line);
            Index++;
            Count++;
            if (Index == LineCount - 1)
                Index = 0;
        }
    }
    CM_SIZE size;
    Error error;
    int Position;
    int n = Count < LineCount ? Count : LineCount - 1;
    for (int i = 0; i < n; i++)
    {
        if (Count < LineCount) // so Count is max LineCount - 1
        {
            Position = i;
        }
        else 
        {
            Position = Index + i;
            if (Position >= LineCount - 1)
                Position = Position - LineCount + 1;
        }
        TemporaryStorage[Position][strlen(TemporaryStorage[Position]) - 1] = '\0';
        size = (CM_SIZE)(strlen(TemporaryStorage[Position]) + 1) * sizeof(char);
        error = Send(clientInfo->client, &size, sizeof(CM_SIZE));
        if (!SUCCESS(error))
        {
            fclose(file);
            return error; // fatal error
        }
        error = Send(clientInfo->client, TemporaryStorage[Position], size);
        if (!SUCCESS(error))
        {
            fclose(file);
            return error; // fatal error
        }
    }
    for (int i = 0; i < LineCount; i++)
        free(TemporaryStorage[i]);
    free(TemporaryStorage);
    return SUCCESS_ERROR;
}

Error RetrieveOfflineMessages(ClientThreadInformation* clientInfo)
{
    char Filename[280];
    sprintf_s(Filename, 275, "offline_%s.txt", clientInfo->Username);
    char Line[700] = "";
    FILE* file = fopen(Filename, "r");
    if (file == NULL)
        return CreateError(131, 0); // might not be so fatal.
    CM_SIZE size;
    Error error;
    while (fgets(Line, 700, file) != NULL && strlen(Line) > 0)
    {
        Line[strlen(Line) - 1] = 0; // we dont need 2 newlines !
        size = (CM_SIZE)(strlen(Line) + 1) * sizeof(char);
        error = Send(clientInfo->client, &size, sizeof(CM_SIZE));
        if (!SUCCESS(error))
        {
            fclose(file);
            return error; // fatal error
        }
        error = Send(clientInfo->client, Line, size);
        if (!SUCCESS(error))
        {
            fclose(file);
            return error; // fatal error
        }
    }
    fclose(file);
    file = fopen(Filename, "w");
    fputs("", file);
    fclose(file);
    return SUCCESS_ERROR;
}

int CountBytes(unsigned char* chr)
{
    int count = 0;
    while (chr[count])
        count++;
    return count;
}

Error ProcessCommand(ClientThreadInformation* clientInfo, char* command)
{
    // validation for command
    int skipConfirmation = 0;
    char feedbackMessage[256] = "";
    (void)clientInfo;
    char* next = NULL;
    char* token = strtok_s(command, " ", &next);
    if (strcmp(token, "echo") == 0)
    {
        // validation for the second argument (might not exist)
        token = strtok_s(NULL, "\n", &next);
        _tprintf_s(TEXT("%S\n"), token);
        strcpy_s(feedbackMessage, 255, token);
        /*CM_SIZE size = (CM_SIZE)(strlen(token) + 1) * sizeof(char);
        Error error = Send(clientInfo->client, &size, sizeof(CM_SIZE));
        if (!SUCCESS(error))
        {
            return error; // fatal error
        }
        error = Send(clientInfo->client, token, size);
        if (!SUCCESS(error))
        {
            return error; // fatal error
        }*/
    }
    else if (strcmp(token, "register") == 0)
    {
        // validation for the other arguments
        if (clientInfo->isLoggedIn == 1)
        {
            strcpy_s(feedbackMessage, 255, "Error: User already logged in");
        }
        else
        {
            char username[256];
            char password[256];
            token = strtok_s(NULL, " ", &next);
            strcpy_s(username, 255, token);
            token = strtok_s(NULL, "\n", &next);
            strcpy_s(password, 255, token);
            Error error;
            error = RegisterUser(username, password);
            if (!SUCCESS(error))
            {
                if (error.CodeA == 101)
                    strcpy_s(feedbackMessage, 255, "Error: Invalid username");
                else if (error.CodeA == 111)
                    strcpy_s(feedbackMessage, 255, "Error: Invalid password");
                else if (error.CodeA == 112 || error.CodeA == 113)
                    strcpy_s(feedbackMessage, 255, "Error: Password too weak");
                else if (error.CodeA == 121)
                    strcpy_s(feedbackMessage, 255, "Error: Username already registered");
                else if (error.CodeA == 131)
                    return error; // fatal error
            }
            else
            {
                strcpy_s(feedbackMessage, 255, "Success");
            }
        }
    }
    else if (strcmp(token, "login") == 0)
    {
        if (clientInfo->isLoggedIn == 1)
        {
            strcpy_s(feedbackMessage, 255, "Error: Another user already logged in");
        }
        else
        {
            char username[256];
            char password[256];
            token = strtok_s(NULL, " ", &next);
            strcpy_s(username, 255, token);
            token = strtok_s(NULL, "\n", &next);
            strcpy_s(password, 255, token);
            FILE* file = fopen("C:/registration.txt", "r");
            if (file == NULL)
                return CreateError(131, 0); // couldnt open file; fatal error
            char Line[256] = "";
            int fileContainsUser = 0;
            char LineToFind[256] = "";
            sprintf_s(LineToFind, 255, "%s,%s\n", username, password);
            while (fgets(Line, 255, file))
            {
                if (strcmp(LineToFind, Line) == 0)
                {
                    fileContainsUser = 1;
                    break;
                }
            }
            fclose(file);
            if (fileContainsUser)
            {
                CM_ServerInformation* info = (CM_ServerInformation*)clientInfo->info;
                int someoneElseLoggedIn = 0;
                for (int i = 0; i < info->threadCount; i++)
                {
                    if (strcmp(info->clientInfo[i]->Username, username) == 0)
                    {
                        someoneElseLoggedIn = 1;
                        break;
                    }
                }
                if (someoneElseLoggedIn)
                {
                    strcpy_s(feedbackMessage, 255, "Error: User already logged in");
                }
                else
                {
                    // good to login!
                    strcpy_s(feedbackMessage, 255, "Success");
                    clientInfo->isLoggedIn = 1;
                    strcpy_s(clientInfo->Username, 255, username);
                    RetrieveOfflineMessages(clientInfo);
                }
            }
            else
            {
                // bad combination
                strcpy_s(feedbackMessage, 255, "Error: Invalid username/password combination");
            }
        }
        
    }
    else if (strncmp(token, "logout", 6) == 0)
    {
        if (clientInfo->isLoggedIn == 0)
        {
            strcpy_s(feedbackMessage, 255, "Error: No user currently logged in");
        }
        else 
        {
            strcpy_s(feedbackMessage, 255, "Success");
            clientInfo->isLoggedIn = 0;
            clientInfo->Username[0] = 0;
        }
    }
    else if (strcmp(token, "msg") == 0)
    {
        if (clientInfo->isLoggedIn == 0)
        {
            strcpy_s(feedbackMessage, 255, "Error: No user currently logged in");
        }
        else
        {
            char target[256] = "";
            char message[256] = "";
            token = NULL;
            token = strtok_s(NULL, " ", &next);
            strcpy_s(target, 255, token);
            token = strtok_s(NULL, "\n", &next);
            strcpy_s(message, 255, token);
            ClientThreadInformation* targetClientInfo = NULL;
            CM_ServerInformation* info = (CM_ServerInformation*)clientInfo->info;
            for (int i = 0; i < info->threadCount; i++)
            {
                if (strcmp(info->clientInfo[i]->Username, target) == 0)
                {
                    targetClientInfo = info->clientInfo[i];
                    break;
                }
            }
            if (targetClientInfo != NULL)
            {
                // user is logged in
                // easy then, we just send him the message.
                strcpy_s(feedbackMessage, 255, "Success");
                char FormattedMessage[700];
                char FormattedMessageForFile[700];
                sprintf_s(FormattedMessage, 700, "Message from %s: %s", clientInfo->Username, message);
                sprintf_s(FormattedMessageForFile, 700, "From %s: %s\n", clientInfo->Username, message);
                CM_SIZE msgSize = (CM_SIZE)(strlen(FormattedMessage) + 1) * sizeof(char);
                Error error = Send(targetClientInfo->client, &msgSize, sizeof(CM_SIZE));
                if (!SUCCESS(error))
                {
                    return error; // fatal error
                }
                error = Send(targetClientInfo->client, FormattedMessage, msgSize);
                if (!SUCCESS(error))
                {
                    return error; // fatal error
                }
                // let's store the message in the history
                char FileName[500] = "";
                if (strcmp(clientInfo->Username, target) < 0)
                {
                    sprintf_s(FileName, 500, "%s_%s.txt", clientInfo->Username, target);
                }
                else
                {
                    sprintf_s(FileName, 500, "%s_%s.txt", target, clientInfo->Username);
                }
                FILE* file = fopen(FileName, "a");
                fputs(FormattedMessageForFile, file);
                fclose(file);
            }
            else
            {
                // user is not logged in. maybe he doesn't exist?
                FILE* file = fopen("C:/registration.txt", "a+");
                if (file == NULL)
                    return CreateError(131, 0); // couldnt open file
                char Line[256] = "";
                next = NULL;
                char* usernameFromFile;
                int fileContainsUsername = 0;
                while (fgets(Line, 255, file))
                {
                    usernameFromFile = strtok_s(Line, ",", &next);
                    if (strcmp(usernameFromFile, target) == 0)
                    {
                        fileContainsUsername = 1;
                        break;
                    }
                }
                fclose(file);
                if (fileContainsUsername)
                {
                    // user exists but is offline
                    // gotta store his message offline then :(
                    strcpy_s(feedbackMessage, 255, "Success");
                    char Filename[280];
                    sprintf_s(Filename, 275, "offline_%s.txt", target);
                    char FormattedMessage[700];
                    char FormattedMessageForFile[700];
                    sprintf_s(FormattedMessage, 700, "Message from %s: %s\n", clientInfo->Username, message);
                    sprintf_s(FormattedMessageForFile, 700, "From %s: %s\n", clientInfo->Username, message);
                    file = fopen(Filename, "a");
                    if (file == NULL)
                    {
                        return CreateError(131, 0); // fatal error
                    }
                    fputs(FormattedMessage, file);
                    fclose(file);
                    // let's store the message in the history
                    char FileName[500] = "";
                    if (strcmp(clientInfo->Username, target) < 0)
                    {
                        sprintf_s(FileName, 500, "%s_%s.txt", clientInfo->Username, target);
                    }
                    else
                    {
                        sprintf_s(FileName, 500, "%s_%s.txt", target, clientInfo->Username);
                    }
                    file = fopen(FileName, "a");
                    fputs(FormattedMessageForFile, file);
                    fclose(file);
                }
                else
                {
                    // user doesn't exist
                    strcpy_s(feedbackMessage, 255, "Error: No such user");
                }
                // user is not logged in. maybe he doesn't exist?
            }
        }
    }
    else if (strcmp(token, "broadcast") == 0)
    {
        if (clientInfo->isLoggedIn == 0)
        {
            strcpy_s(feedbackMessage, 255, "Error: No user currently logged in");
        }
        else
        {
            char message[256] = "";
            token = NULL;
            token = strtok_s(NULL, " ", &next);
            strcpy_s(message, 255, token);
            strcpy_s(feedbackMessage, 255, "Success");
            char FormattedMessage[700];
            sprintf_s(FormattedMessage, 700, "Broadcast from %s: %s", clientInfo->Username, message);
            CM_SIZE msgSize = (CM_SIZE)(strlen(FormattedMessage) + 1) * sizeof(char);
            CM_ServerInformation* info = (CM_ServerInformation*)clientInfo->info;
            for (int i = 0; i < info->threadCount; i++)
            {
                if (strcmp(info->clientInfo[i]->Username, clientInfo->Username) != 0)
                {
                    Error error = Send(info->clientInfo[i]->client, &msgSize, sizeof(CM_SIZE));
                    if (!SUCCESS(error))
                    {
                        return error; // fatal error
                    }
                    error = Send(info->clientInfo[i]->client, FormattedMessage, msgSize);
                    if (!SUCCESS(error))
                    {
                        return error; // fatal error
                    }
                }
            }
        }
    }
    else if (strncmp(token, "list", 4) == 0)
    {
        CM_ServerInformation* info = (CM_ServerInformation*)clientInfo->info;
        CM_SIZE size = 256 * sizeof(char) * info->threadCount;
        skipConfirmation = 1;
        char* List = malloc((size_t)size);
        List[0] = 0;
        for (int i = 0; i < info->threadCount; i++)
        {
            if (info->clientInfo[i]->isLoggedIn == 1)
            {
                strcat_s(List, size, info->clientInfo[i]->Username);
                strcat_s(List, size, "\n");
            }
        }
        if (List[0] != 0)
        {
            List[strlen(List) - 1] = 0;
        }
        size = (CM_SIZE)(strlen(List) + 1) * sizeof(char);
        Error error = Send(clientInfo->client, &size, sizeof(CM_SIZE));
        if (!SUCCESS(error))
        {
            free(List);
            return error; // fatal error
        }
        error = Send(clientInfo->client, List, size);
        if (!SUCCESS(error))
        {
            free(List);
            return error; // fatal error
        }
        free(List);
    }
    else if (strcmp(token, "history") == 0)
    {
        if (clientInfo->isLoggedIn == 0)
        {
            strcpy_s(feedbackMessage, 255, "Error: No user currently logged in");
        }
        else
        {
            skipConfirmation = 1;
            char target[256] = "";
            token = NULL;
            token = strtok_s(NULL, " ", &next);
            strcpy_s(target, 255, token);
            char numberStr[10] = "";
            token = strtok_s(NULL, "\n", &next);
            strcpy_s(numberStr, 9, token);
            if (StringIsIntClassic(numberStr))
            {
                int MessageCount = atoi(numberStr);
                FILE* file = fopen("C:/registration.txt", "r");
                if (file == NULL)
                    return CreateError(131, 0); // couldnt open file
                char Line[256] = "";
                next = NULL;
                char* usernameFromFile;
                int fileContainsUsername = 0;
                while (fgets(Line, 255, file))
                {
                    usernameFromFile = strtok_s(Line, ",", &next);
                    if (strcmp(usernameFromFile, target) == 0)
                    {
                        fileContainsUsername = 1;
                        break;
                    }
                }
                fclose(file);
                if (fileContainsUsername)
                {
                    char FileName[500] = "";
                    if (strcmp(clientInfo->Username, target) < 0)
                    {
                        sprintf_s(FileName, 500, "%s_%s.txt", clientInfo->Username, target);
                    }
                    else
                    {
                        sprintf_s(FileName, 500, "%s_%s.txt", target, clientInfo->Username);
                    }
                    SendLastLinesFromFile(FileName, clientInfo, MessageCount);

                }
                else
                {
                    strcpy_s(feedbackMessage, 255, "Error: No such user");
                }
            }
            else
            {
                strcpy_s(feedbackMessage, 255, "Error: Invalid count value.");
            }
        }
    }
    else if (strcmp(token, "sendfile") == 0)
    {
        if (clientInfo->isLoggedIn == 0)
        {
            strcpy_s(feedbackMessage, 255, "Error: No user currently logged in");
        }
        else
        {
            char target[256] = "";
            token = NULL;
            token = strtok_s(NULL, " ", &next);
            strcpy_s(target, 255, token);
            char PathToFile[256] = "";
            token = strtok_s(NULL, "\n", &next);
            strcpy_s(PathToFile, 255, token);
            FILE* file = fopen("C:/registration.txt", "r");
            if (file == NULL)
                return CreateError(131, 0); // couldnt open file
            char Line[256] = "";
            next = NULL;
            char* usernameFromFile;
            int fileContainsUsername = 0;
            while (fgets(Line, 255, file))
            {
                usernameFromFile = strtok_s(Line, ",", &next);
                if (strcmp(usernameFromFile, target) == 0)
                {
                    fileContainsUsername = 1;
                    break;
                }
            }
            fclose(file);
            ClientThreadInformation* targetInfo = NULL;
            if (fileContainsUsername)
            {
                CM_ServerInformation* info = (CM_ServerInformation*)clientInfo->info;
                for (int i = 0; i < info->threadCount; i++)
                {
                    if (strcmp(info->clientInfo[i]->Username, target) == 0)
                    {
                        targetInfo = info->clientInfo[i];
                        break;
                    }
                }
                if (targetInfo == NULL)
                {
                    strcpy_s(feedbackMessage, 255, "Error: User not active");
                }
                else
                {
                    file = fopen(PathToFile, "rb");
                    if (file == NULL)
                    {
                        strcpy_s(feedbackMessage, 255, "Error: File not found");
                    }
                    else
                    {
                        CM_SIZE size = sizeof(int);
                        Error error = Send(clientInfo->client, &size, sizeof(CM_SIZE)); // telling the client that we r sending a file...
                        if (!SUCCESS(error))
                        {
                            fclose(file);
                            return error; // fatal error
                        }
                        int code = -1;
                        error = Send(clientInfo->client, &code, sizeof(CM_SIZE)); // telling the client that we r sending a file...
                        if (!SUCCESS(error))
                        {
                            fclose(file);
                            return error; // fatal error
                        }
                        char* FileName = (char*)strrchr(PathToFile, '\\');
                        if (FileName == NULL)
                            FileName = (char*)strrchr(PathToFile, '/');
                        FileName++;
                        size = (CM_SIZE)(strlen(FileName) + 1) * sizeof(char);
                        error = Send(clientInfo->client, &size, sizeof(CM_SIZE)); // sending the name of the file...
                        if (!SUCCESS(error))
                        {
                            fclose(file);
                            return error; // fatal error
                        }
                        error = Send(clientInfo->client, FileName, size);
                        if (!SUCCESS(error))
                        {
                            fclose(file);
                            return error; // fatal error
                        }
                        // beginning to send the file itself
                        unsigned char* buffer = malloc(sizeof(unsigned char)*1024);
                        int Left;
                        do
                        {
                            memset(buffer, 0, 1024);
                            Left = (CM_SIZE)fread(buffer, 1, 1024, file);
                            size = (CM_SIZE)(Left * sizeof(unsigned char));
                            error = Send(clientInfo->client, &size, sizeof(CM_SIZE));
                            if (!SUCCESS(error))
                            {
                                fclose(file);
                                free(buffer);
                                return error; // fatal error
                            }
                            error = Send(clientInfo->client, buffer, size);
                            if (!SUCCESS(error))
                            {
                                free(buffer);
                                fclose(file);
                                return error; // fatal error
                            }
                            //size = (CM_SIZE)fread(buffer, 1024, 1, file);
                        } while (Left);
                        free(buffer);
                        size = sizeof(int);
                        error = Send(clientInfo->client, &size, sizeof(CM_SIZE)); // telling the client that we r sending a file...
                        if (!SUCCESS(error))
                        {
                            fclose(file);
                            return error; // fatal error
                        }
                        code = -1;
                        error = Send(clientInfo->client, &code, sizeof(CM_SIZE)); // telling the client that we r sending a file...
                        if (!SUCCESS(error))
                        {
                            fclose(file);
                            return error; // fatal error
                        }
                        strcpy_s(feedbackMessage, 255, "Success");
                    }
                }
            }
            else
            {
                strcpy_s(feedbackMessage, 255, "Error: No such user");
            }
        }
    }
    if (!skipConfirmation)
    {
        // send the feedback for the command
        CM_SIZE size = (CM_SIZE)(strlen(feedbackMessage) + 1) * sizeof(char);
        Error error = Send(clientInfo->client, &size, sizeof(CM_SIZE));
        if (!SUCCESS(error))
        {
            return error; // fatal error
        }
        error = Send(clientInfo->client, feedbackMessage, size);
        if (!SUCCESS(error))
        {
            return error; // fatal error
        }
    }
    return SUCCESS_ERROR;
}

DWORD WINAPI HandleClient(PVOID server)
{
    ClientThreadInformation *clientInfo = (ClientThreadInformation*) server;
    clientInfo->isLoggedIn = 0;
    clientInfo->Username[0] = '\0';
    (void)clientInfo;
    _tprintf_s(TEXT("New client has connected...\n"));

    char messageToSend[] = "Success!";
    CM_SIZE messageToSendSize = (CM_SIZE)(strlen(messageToSend)+1) * sizeof(char);
    Error error;
    error = Send(clientInfo->client, &messageToSendSize, sizeof(CM_SIZE));
    //error = Send(clientInfo.client, messageToSend, messageToSendSize);
    if (!SUCCESS(error))
    {
        PrintError(error);
        CleanUpClient(clientInfo);
        return 0;
    }
    error = Send(clientInfo->client, messageToSend, messageToSendSize);
    if (!SUCCESS(error))
    {
        PrintError(error);
        CleanUpClient(clientInfo);
        return 0;
    }
    // Preparation is over.
    // Time to handle commands from this client.
    CM_SIZE size = 0;
    CM_DATA_BUFFER* data = NULL;
    while (TRUE)
    {
        error = GetNextSize(clientInfo->client, &size);
        if (!SUCCESS(error))
        {
            PrintError(error);
            break;
        }
        error = Receive(clientInfo->client, &data, size);
        if (!SUCCESS(error))
        {
            PrintError(error);
            break;
        }
        //_tprintf_s(TEXT("%S"), (char*) data->DataBuffer);
        if (strncmp((char*)data->DataBuffer, "exit", 4) == 0)
        {
            DestroyDataBuffer(data);
            break;
        }
        ProcessCommand(clientInfo, (char*)data->DataBuffer);
    }
    CleanUpClient(clientInfo);
    return 0;
}

void CleanUpServerInformation(CM_ServerInformation* serverInfo)
{
    for (int i = 0; i < serverInfo->threadCount; i++)
    {
        CloseHandle(serverInfo->threadHandles[i]);
        AbandonClient(serverInfo->clientInfo[i]->client);
        free(serverInfo->clientInfo[i]);
    }
    free(serverInfo->threadHandles);
    free(serverInfo->threadIds);
    free(serverInfo->clientInfo);
}

int _tmain(int argc, TCHAR* argv[])
{
    (void)argv;
    (void)argc;
    /*
    This main implementation can be used as an initial example.
    You can erase main implementation when is no longer helpful.
    */
    // Checking the command arguments
    //TCHAR* _1 = TEXT("1");
    //if (argc != 1 || StringIsInt(_1) == FALSE)
    if (argc != 2 || StringIsInt(argv[1]) == FALSE)
    {
        _tprintf_s(TEXT("Error: invalid maximum number of connections"));
        return -1;
    }

    EnableCommunicationModuleLogger();

    CM_ERROR error = InitCommunicationModule();
    if (CM_IS_ERROR(error))
    {
        _tprintf_s(TEXT("Unexpected error: InitCommunicationModule failed with err-code=0x%X!\n"), error);
        return -1;
    }

    CM_SERVER* server = NULL;
    error = CreateServer(&server);
    if (CM_IS_ERROR(error))
    {
        _tprintf_s(TEXT("Unexpected error: CreateServer failed with err-code=0x%X!\n"), error);
        UninitCommunicationModule();
        return -1;
    }
    const int MAX_CONNECTIONS = _tstol(argv[1]);
    _tprintf_s(TEXT("Success\n"));

    //
    CM_ServerInformation info;
    info.threadCount = 0;
    info.threadHandles = malloc(sizeof(HANDLE) * MAX_CONNECTIONS);
    info.threadIds = malloc(sizeof(DWORD) * MAX_CONNECTIONS);
    info.clientInfo = malloc(sizeof(ClientThreadInformation*) * MAX_CONNECTIONS);
    while (TRUE)
    {
        ClientThreadInformation* newThreadInfo = malloc(sizeof(ClientThreadInformation));
        newThreadInfo->client = NULL;
        newThreadInfo->server = server;
        newThreadInfo->info = &info;
        error = AwaitNewClient(server, &newThreadInfo->client);
        if (info.threadCount == MAX_CONNECTIONS)
        {
            _tprintf_s(TEXT("Error: The server is full!\n"));
            CM_SIZE space_available = 0;
            Send(newThreadInfo->client, &space_available, sizeof(CM_SIZE));
            AbandonClient(newThreadInfo->client);
            continue;
        }
        if (CM_IS_ERROR(error))
        {
            _tprintf_s(TEXT("Unexpected error: AwaitNewClient failed with err-code=0x%X!\n"), error);
            DestroyServer(server);
            UninitCommunicationModule();
            CleanUpServerInformation(&info);
            return -1;
        }
        CM_SIZE space_available = 1;
        Send(newThreadInfo->client, &space_available, sizeof(CM_SIZE));
        info.threadHandles[info.threadCount] = CreateThread(NULL, 0, HandleClient, newThreadInfo, 0, &info.threadIds[info.threadCount]);
        info.clientInfo[info.threadCount] = newThreadInfo;
        info.threadCount++;
    }

    _tprintf_s(TEXT("Server is shutting down now...\n"));
    CleanUpServerInformation(&info);
    DestroyServer(server);
    UninitCommunicationModule();
    return 0;
}

