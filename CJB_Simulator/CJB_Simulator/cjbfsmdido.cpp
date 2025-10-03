#include "cjbfsmdido.h"


using namespace std;

cjbfsmdido::cjbfsmdido()
{
    cout << "UART Test Application\n";
    printMenu();
    accessMenu();
}

void cjbfsmdido::printMenu()
{
    cout << "? - this help menu\n";
    cout << "g-v - RS232/422 uart0 to uart15 tx test respectively\n";
    cout << "0-f - RS232/422/485 uart0 to uart15 rx test respectively\n";
    cout << "W - Turn on all DO   ,  w - Turn off all DO\n";
    cout << "+xx - DO ON   ,  $xx - DO OFF   ,  #xx - DI READ\n";
    cout << "Q - quit this program\n\n";
}

void cjbfsmdido::accessMenu()
{
    while (true)
    {
        cout << "user_in> ";
        cin >> user_in;

        if (user_in == "Q")   // quit program
        {
            cout << "Exiting program...\n";
            return;
        }
        else if (user_in.size() == 1)
        {
            char cmd = user_in[0];
            switch (cmd)
            {
            case UART_TX:
                handleUartTransmit(user_in);
                break;
            case UART_RX:
                handleUartReceive(user_in);
                break;
            case ALL_DO_ON:
                cout << "All DO's are on" << endl;
                break;
            case ALL_DO_OFF:
                cout << "All DO's are off" << endl;
                break;
            case MENU:
                printMenu();
                break;
            default:
                cout << UNKOWN_CMD_PRINT << user_in << endl;
            }
        }
        else if (user_in.rfind("+", 0) == 0)  // DO ON
        {
            string num = user_in.substr(1);
            cout << "DO " << num << " is ON" << endl;
        }
        else if (user_in.rfind("$", 0) == 0)  // DO OFF
        {
            string num = user_in.substr(1);
            cout << "DO " << num << " is OFF" << endl;
        }
        else if (user_in.rfind("#", 0) == 0)   // DI READ
        {
            string num = user_in.substr(1);
            cout << "DI " << num << " is ON" << endl;
        }
        else
        {
            cout << UNKOWN_CMD_PRINT << user_in << endl;
        }
    }
}



void cjbfsmdido::handleUartTransmit(const string &user_in)
{

    cout << UART_TX_PRINT;
}

void cjbfsmdido::handleUartReceive(const string &user_in)
{
    cout << UART_RX_PRINT;
}

void cjbfsmdido::handleDIStatus()
{
    cout << "Enter DI NO \n";
    string di_in;
    cin >> di_in;
    if (di_in.rfind("#DI", 0) == 0)
    {
        string num = di_in.substr(4);
        cout << "DI " << num << " is ON" << endl;
    }
    else
    {
        cout << DI_PRINT;
    }
}
